/* Edge Impulse Arduino examples
 * Copyright (c) 2022 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Includes ---------------------------------------------------------------- */
#include <pvcColorCubes_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include <WiFi.h>
#include <HTTPClient.h>
//#include <SPI.h>
//#include <TFT_eSPI.h>
#include "esp_camera.h"

#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "esp_http_server.h"
#include <string>


// Select camera model - find more camera models in camera_pins.h file here
// https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/Camera/CameraWebServer/camera_pins.h

#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 15
#define SIOD_GPIO_NUM 4
#define SIOC_GPIO_NUM 5

#define Y9_GPIO_NUM 16
#define Y8_GPIO_NUM 17
#define Y7_GPIO_NUM 18
#define Y6_GPIO_NUM 12
#define Y5_GPIO_NUM 10
#define Y4_GPIO_NUM 8
#define Y3_GPIO_NUM 9
#define Y2_GPIO_NUM 11
#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM 7
#define PCLK_GPIO_NUM 13


const char *ssid = "San 2.4G";
const char *password = "0611244569";
String ipv4_address = "192.168.1.2";
String path = "http://" + ipv4_address + ":3000/api/postResult";

#define PART_BOUNDARY "123456789000000000000987654321"

static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;

/* Constant defines -------------------------------------------------------- */
int EI_CAMERA_RAW_FRAME_BUFFER_COLS = 240;  //96
int EI_CAMERA_RAW_FRAME_BUFFER_ROWS = 240;  //96
int EI_CAMERA_FRAME_BYTE_SIZE = 3;

uint32_t model_size; 

/* Private variables ------------------------------------------------------- */
static bool debug_nn = false;  // Set this to true to see e.g. features generated from the raw signal
static bool is_initialised = false;
uint8_t *snapshot_buf;  //points to the output of the capture
sensor_t *s = NULL;
float inferXMult;
float inferYMult;

/* Function definitions ------------------------------------------------------- */
bool camera_setup(int resolution);
bool camera_setting(String setting, int value);
void camera_deinit(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf, httpd_req_t *req, esp_err_t res);

static esp_err_t stream_handler(httpd_req_t *req) {
  esp_err_t res = ESP_OK;
  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  unsigned long previousMillis = 0;
  int fps = 0;
  if (is_initialised == false) {
    if (camera_setup(240240) == false) {
      Serial.println("camera initialised failed");
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "failed to initialized camera from esp");
      return ESP_OK;
    }
  }
  Serial.println("camera ready");
  // instead of wait_ms, we'll wait on the signal, this allows threads to cancel us...
  while (true) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 1000) {
      Serial.println(fps);
      previousMillis = currentMillis;
      fps = 0;
    }
    Serial.println(currentMillis);
    if (ei_sleep(5) != EI_IMPULSE_OK) {
      break;
    }
    snapshot_buf = (uint8_t *)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);

    // check if allocation was successful
    if (snapshot_buf == nullptr) {
      ei_printf("ERR: Failed to allocate snapshot buffer!\n");
      break;
    }

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH, (size_t)EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf, req, res) == false) {
      ei_printf("Failed to capture image\r\n");
      free(snapshot_buf);
      break;
    }

    // Run the classifier
    ei_impulse_result_t result = { 0 };

    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);
    if (err != EI_IMPULSE_OK) {
      ei_printf("ERR: Failed to run classifier (%d)\n", err);
      break;
    }

    // print the predictions
    ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
              result.timing.dsp, result.timing.classification, result.timing.anomaly);

#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    bool bb_found = result.bounding_boxes[0].value > 0;
    HTTPClient http;
    //String jsonBody = "{\"test\":\"101\"}";
    String jsonBody = "{";
    for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
      auto bb = result.bounding_boxes[ix];
      if (bb.value == 0) {
        continue;
      }
      if (ix != 0) {
        jsonBody += ",";
      }
      //ei_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
      jsonBody += "\"" + String(bb.label) + "\":[" + String(bb.value * 100) + "," + String(bb.x) + "," + String(bb.y) + "," + String(bb.width) + "," + String(bb.height) + "]";
      //Serial.println(ix);
      //Serial.println(result.bounding_boxes_count);
      //tft.drawCircle(bb.x * 2.5, bb.y * 2.5, 15 , TFT_GREEN);
      //tft.setCursor(bb.x*2.5, bb.y * 2.5);
      //tft.print(bb.label);
    }
    if (jsonBody != "{") {
      String jsonModelProfile = ",\"size\":" + String(model_size) + ",";
      jsonBody += "}";
      Serial.println(jsonBody);
      http.begin(path);
      http.addHeader("Content-type", "application/json");
      int httpCode = http.POST(jsonBody);
      if (httpCode > 0) {
        Serial.printf("http code %d", httpCode);
      }
    }
    if (!bb_found) {
      ei_printf("    No objects found\n");
    }
#else
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
      ei_printf("    %s: %.5f\n", result.classification[ix].label,
                result.classification[ix].value);
    }
#endif

#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
    free(snapshot_buf);
    fps++;
  }
  return res;
}


//TFT_eSPI tft = TFT_eSPI();

/**
* @brief      Arduino setup function
*/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //comment out the below line to start inference immediately after upload
  while (!Serial)
    ;
  Serial.println("Edge Impulse Inferencing Demo");
  if (camera_setup(240240) == false) {
    ei_printf("Failed to initialize Camera!\r\n");
  } else {
    ei_printf("Camera initialized\r\n");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    ei_sleep(500);
    ei_printf(".");
  }

  ei_printf("Wifi connected      ");
  Serial.println(WiFi.localIP());
  ei_printf("/");
  /*
    tft.init();
    tft.setSwapBytes(true);
    tft.setRotation(2);
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED);
    */

  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  httpd_uri_t stream_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = stream_handler,
    .user_ctx = NULL
  };

  httpd_uri_t setting_uri = {
    .uri = "/setting",
    .method = HTTP_GET,
    .handler = setting_handler,
    .user_ctx = NULL
  };


  Serial.println("Copy and paste this in first line of index.js");
  Serial.print(WiFi.localIP());
  Serial.print("/");
  Serial.println();
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
    httpd_register_uri_handler(stream_httpd, &setting_uri);
  }

  Serial.println("open index.html file to start inferencing");
  //ei_printf("\nStarting continious inference in 2 seconds...\n");
  ei_sleep(1000);
}

/**
* @brief      Get data and run inferencing
*
* @param[in]  debug  Get debug info if true
*/

static esp_err_t setting_handler(httpd_req_t *req) {
  char buffer[100];
  String bufVal = "";
  bool startCap = false;
  String param = "";
  int value = 0;
  Serial.println("setting handler accessed");
  if (httpd_req_get_url_query_str(req, buffer, sizeof(buffer)) == ESP_OK) {
    for (int i = 0; i < buffer[i] != '\0'; i++) {
      if (buffer[i] == '=') {
        startCap = true;
        continue;
      }
      if (startCap == true) {
        bufVal += buffer[i];
      } else {
        param += buffer[i];
      }
      if (buffer[i] == '&' || buffer[i + 1] == '\0') {
        startCap = false;
        value = bufVal.toInt();
      }
    }
    //Serial.println(param);
    //Serial.println(value);
  }
  if (param == "resolution") {
    camera_deinit();
    if (!camera_setup(value)) {
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "failed to run setup from esp camera");
      return ESP_OK;
    }
  } else {
    if (!camera_setting(param, value)) {
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "failed to change setting from esp");
      return ESP_OK;
    }
  }
  char *response = "setting changed successfully";
  httpd_resp_send(req, response, strlen(response));
  return ESP_OK;
}


/**
 * @brief   Setup image sensor & start streaming
 *
 * @retval  false if initialisation failed
 */

bool camera_setup(int resolution) {
  if (is_initialised) return true;
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 30000000;
  config.pixel_format = PIXFORMAT_JPEG;
  if (resolution == 160120) {
    config.frame_size = FRAMESIZE_QQVGA;
    EI_CAMERA_RAW_FRAME_BUFFER_COLS = 160;
    EI_CAMERA_RAW_FRAME_BUFFER_ROWS = 120;
  } else if (resolution == 240240) {
    config.frame_size = FRAMESIZE_240X240;
    EI_CAMERA_RAW_FRAME_BUFFER_COLS = 240;
    EI_CAMERA_RAW_FRAME_BUFFER_ROWS = 240;
  } else if (resolution == 320240) {
    config.frame_size = FRAMESIZE_QVGA;
    EI_CAMERA_RAW_FRAME_BUFFER_COLS = 320;
    EI_CAMERA_RAW_FRAME_BUFFER_ROWS = 240;
  } else if (resolution == 640480) {
    config.frame_size = FRAMESIZE_VGA;
    EI_CAMERA_RAW_FRAME_BUFFER_COLS = 640;
    EI_CAMERA_RAW_FRAME_BUFFER_ROWS = 480;
  } else if (resolution == 800600) {
    config.frame_size = FRAMESIZE_SVGA;
    EI_CAMERA_RAW_FRAME_BUFFER_COLS = 800;
    EI_CAMERA_RAW_FRAME_BUFFER_ROWS = 600;
  } else if (resolution == 1024768) {
    config.frame_size = FRAMESIZE_XGA;
    EI_CAMERA_RAW_FRAME_BUFFER_COLS = 1024;
    EI_CAMERA_RAW_FRAME_BUFFER_ROWS = 768;
  } else if (resolution == 12801024) {
    config.frame_size = FRAMESIZE_SXGA;
    EI_CAMERA_RAW_FRAME_BUFFER_COLS = 1280;
    EI_CAMERA_RAW_FRAME_BUFFER_ROWS = 1024;
  } else if (resolution == 16001200) {
    config.frame_size = FRAMESIZE_UXGA;
    EI_CAMERA_RAW_FRAME_BUFFER_COLS = 1600;
    EI_CAMERA_RAW_FRAME_BUFFER_ROWS = 1200;
  } else {
    config.frame_size = FRAMESIZE_240X240;
    EI_CAMERA_RAW_FRAME_BUFFER_COLS = 240;
    EI_CAMERA_RAW_FRAME_BUFFER_ROWS = 240;
  }
  config.jpeg_quality = 12;
  config.fb_count = 2;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }
  s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  is_initialised = true;
  return true;
}


bool camera_setting(String setting, int value) {
  if (setting == "bright") {
    s->set_brightness(s, value);
  }
  if (setting == "contrast") {
    s->set_contrast(s, value);
  }
  if (setting == "saturation") {
    s->set_saturation(s, value);
  }
    if (setting == "sharpness"){
    s->set_sharpness(s, value);
  }
  if (setting == "effect") {
    s->set_special_effect(s, value); 
  }
  if (setting == "whiteball") {
    s->set_whitebal(s, value);
  }
  if (setting == "awb") {
    s->set_awb_gain(s, value);
  }
  if (setting == "mode") {
    s->set_awb_gain(s, 1);
    s->set_wb_mode(s, value);
  }
  if (setting == "exposure"){
    s->set_exposure_ctrl(s,value);
  }
  if (setting == "aec2"){
    s->set_aec2(s, value);
  }
  if (setting == "ae") {
    s->set_ae_level(s, value);
  }
  if (setting == "aec"){
    s->set_aec_value(s, value);
  }
  if (setting == "gain"){
    s->set_gain_ctrl(s, value);
  }
  if (setting == "agc"){
    s->set_agc_gain(s, value);
  }
  if (setting == "gainCeiling"){
    s->set_gainceiling(s, (gainceiling_t)value);
  }
  if (setting == "bpc"){
    s->set_bpc(s, value);
  }
  if (setting == "wpc"){
    s->set_wpc(s, value);
  }
  if (setting == "rawGma"){
    s->set_raw_gma(s, value);
  }
  s->set_vflip(s, 1);
  return true;
}

/**
 * @brief      Stop streaming of sensor data
 */
void camera_deinit(void) {

  //deinitialize the camera
  esp_err_t err = esp_camera_deinit();

  if (err != ESP_OK) {
    ei_printf("Camera deinit failed\n");
    return;
  }

  is_initialised = false;
  return;
}


/**
 * @brief      Capture, rescale and crop image
 *
 * @param[in]  img_width     width of output image
 * @param[in]  img_height    height of output image
 * @param[in]  out_buf       pointer to store output image, NULL may be used
 *                           if ei_camera_frame_buffer is to be used for capture and resize/cropping.
 *
 * @retval     false if not initialised, image captured, rescaled or cropped failed
 *
 */
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf, httpd_req_t *req, esp_err_t res) {
  bool do_resize = false;

  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;
  char *part_buf[64];

  if (!is_initialised) {
    ei_printf("ERR: Camera is not initialized\r\n");
    return false;
  }

  camera_fb_t *fb = esp_camera_fb_get();

  if (!fb) {
    ei_printf("Camera capture failed\n");
    return false;
  } else {
    //if(fb->width > 400){
    if (fb->format != PIXFORMAT_JPEG) {
      bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
      esp_camera_fb_return(fb);
      fb = NULL;
      if (!jpeg_converted) {
        Serial.println("JPEG compression failed");
        res = ESP_FAIL;
      }
    } else {
      _jpg_buf_len = fb->len;
      _jpg_buf = fb->buf;
    }
    //}
  }
  if (res == ESP_OK) {
    size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
    res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
  }
  if (res == ESP_OK) {
    res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
  }
  if (res == ESP_OK) {
    res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
  }
  bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);
  if (!converted) {
    ei_printf("Conversion failed\n");
    return false;
  }
  if (fb) {
    esp_camera_fb_return(fb);
    fb = NULL;
    _jpg_buf = NULL;
  } else if (_jpg_buf) {
    free(_jpg_buf);
    _jpg_buf = NULL;
  }
  if (res != ESP_OK) {
    return false;
  }

  //uint8_t *rgb565 = (uint8_t *) malloc(240 * 240 * 3);
  //jpg2rgb565(fb->buf, fb->len, rgb565, JPG_SCALE_NONE);
  //tft.pushImage(0,0,240,240,(uint16_t*)rgb565);
  //esp_camera_fb_return(fb);
  //free(rgb565);


  if ((img_width != EI_CAMERA_RAW_FRAME_BUFFER_COLS)
      || (img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS)) {
    do_resize = true;
  }

  if (do_resize) {
    ei::image::processing::crop_and_interpolate_rgb888(
      out_buf,
      EI_CAMERA_RAW_FRAME_BUFFER_COLS,
      EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
      out_buf,
      img_width,
      img_height);
  }
  model_size = img_width;
  float inferXMult = float(EI_CAMERA_RAW_FRAME_BUFFER_COLS)/(float)model_size;
  float inferYMult = float(EI_CAMERA_RAW_FRAME_BUFFER_ROWS)/(float)model_size;  
  //Serial.println(img_width);
  //Serial.println(img_height);
  return true;
}

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr) {
  // we already have a RGB888 buffer, so recalculate offset into pixel index
  size_t pixel_ix = offset * 3;
  size_t pixels_left = length;
  size_t out_ptr_ix = 0;

  while (pixels_left != 0) {
    out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix + 2] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix];

    // go to the next pixel
    out_ptr_ix++;
    pixel_ix += 3;
    pixels_left--;
  }
  // and done!
  return 0;
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_CAMERA
#error "Invalid model for current sensor"
#endif

void loop() {
  delay(1);
}