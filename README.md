# object-detect-FOMO-stream-Esp32
[ฉบับไทย](https://github.com/San279/object-detect-FOMO-stream-Esp32/blob/main/Readme-th.md)
<br/>
<br/>
This project was intended for streaming [FOMO object detection model](https://edge-impulse.gitbook.io/docs/edge-impulse-studio/learning-blocks/object-detection/fomo-object-detection-for-constrained-devices) by [Edge Impulse](https://edgeimpulse.com/) inference results from Esp32 to webserver. The Hardware that was used were provided by [Wireless Solution Asia](https://wirelesssolution.asia/). To use this script users must obtain a model from Edge Impulse </br> </br>
<strong> - I've provided simple guide to train your own model please visit [train-FOMO-object-detect-esp32](https://github.com/San279/train-FOMO-object-detect-esp32). </strong> <br/>
<br/> <br/>
![alt text](/Images_for_readme/done.PNG)
<br /><br /><br />
## What you'll need
- [Arduino IDE](https://www.arduino.cc/en/software), preferably the latest ones, but older versions will still do the job.
- [NodeJS](https://nodejs.org/en/download/package-manager/current) provide access points for inference results.
- ESP32-S3 is preferable but older version will do just fine.
- OV2640 camera or any OV series.

## Project files descriptions

1. FOMO_object_detect_stream.ino - Containes Arduino codes for streaming camera images to the webserver and posting inference results to NodeJS.
2. index.html - Contains HTML elements for streaming and inferencing.
3. index.js - Javascript functions for HTML element for streaming and inferencing.
4. api.js - NodeJS access point for processing inference results for index.js.
5. styles.css - makes webserver looks more lively.

## How to install and run the project

1.  <strong> Download the zip file and extract it to Arduino directory </strong> <br /><br />
![alt text](/Images_for_readme/folder_directory.PNG)
<br /><br /><br /><br />
2.  <strong> Go to the installation directory and install javascript dependencies. Run the api.js script, NodeJS is required for this process. </strong><br /><br />
  - For users with an IDE, linux, or Mac navigate to the directory and enter "npm install" follow by "node api.js". <br /><br />
  ![alt text](/Images_for_readme/ide_run_api.PNG)
  <br /><br /><br />
- For Window users, navigate to the directory then select the file path and type cmd. New terminal window will be opened in the folder directory. <br /><br />
 ![alt text](/Images_for_readme/window_run_api1.PNG)
 ![alt text](/Images_for_readme/window_run_api2.PNG)
- Enter "npm install" follow by "node api.js" <br /><br />
 ![alt text](/Images_for_readme/window_run_api3.PNG)
  <br /><br /> <br /><br />
3.  <strong> Open FOMO_object_detect_stream.ino and enter WIFI SSID, PASSWORD, and the IPV4 address obtained from your PC. </strong> <br /><br />
![alt text](/Images_for_readme/wifi_ipv4.PNG)
<br /><br /><br />
  - To find IPV4 For Window users, open cmd and enter "ipconfig". <br /><br />
    ![alt text](/Images_for_readme/ipv4_addr.jpg) <br /><br />
  - To find IPV4 For Linux users, open terminal and enter hostname -I <br /><br />
    ![alt text](Images_for_readme/terminal_ipv4.PNG) <br /><br />
<br /><br /><br /><br />
4. <strong> Add zip folder of the model obtained from Edge Impulse to Arduino IDE. </strong>
<br /><br />
![alt text](Images_for_readme/arduino_model_zip.PNG)
<br /><br />
![alt text](Images_for_readme/FOMO_model_zip.PNG)
<br /><br /><br /><br />
5. <strong> Under tools change your Board to "ESP32S3 Dev Module" and PSRAM to "OPI PSRAM". </strong><br /><br />
![alt text](/Images_for_readme/IDE_configure.PNG)
<br /><br /><br /><br />
6. <strong> Upload the code to your ESP32S3. This process may take up to 30 minutes, once completed copy the IP address from the serial monitor to use it for the next process. </strong> <br /><br />
![alt text](/Images_for_readme/arduino_serial_monitor.PNG)
<br /><br /><br /><br />
7. <strong> Go to directory and open index.html, it should open the default browser and ask for IP Addres. Paste the Ip address obtain from Arduino IDE in process 7. </strong> <br /><br />
![alt text](/Images_for_readme/ip_prompt.PNG)
<br /><br /><br /><br />
9. <strong> Done!! the inference results from Esp32 should be shown in the webserver. </strong>  
![alt text](/Images_for_readme/done.PNG)<br /><br />
<br /><br /><br /><br />
## Webserver features
- User can switch languages on the top right corner of the web.<br />
- Adjustable Camera Settings, try to match camera settings with training datasets, to see more details about each setting please visit [
https://heyrick.eu/blog/index.php?diary=20210418&keitai=0](https://heyrick.eu/blog/index.php?diary=20210418&keitai=0).<br />
- User can hide the setting console by clicking on the icon above.<br /><br />
![alt text](/Images_for_readme/settings.PNG)
<br /><br /><br />
- Adjustable Resolutions. <br /><br />
![alt_text](/Images_for_readme/console.PNG)
<br /><br /><br />
- Capture button will start capturing images of detected objects. <br />
- Set Instances to limit the amount of images captured.  <br />
- Once capture button is clicked a timer will start to count number of seconds it took to capture all images.<br /> <br />
![alt_text](/Images_for_readme/console_seconds.PNG)
<br /><br /> <br />
- Download button located on the top left will zip all captured images into a single file and download. The downloaded images will not have labels.<br />
- Clear buttons located on the top right will delete all captured images.<br />
- User can aslo delete each image individually.<br /><br />
![alt_text](/Images_for_readme/gallery_img.PNG)
<br /> <br /><br /> <br />
## Credit
Thanks to [WIRELESS SOLUTION ASIA CO.,LTD](https://wirelesssolution.asia/) for providing AIOT board to support this project. Also thanks to [RandomNerdTutorials](https://RandomNerdTutorials.com/esp32-cam-video-streaming-web-server-camera-home-assistant) for providing essential codes for streaming Esp32 camera to webserver. Scripted used for Esp32 FOMO object detection inferencing were from [Edge Impulse](https://edge-impulse.gitbook.io/docs/edge-impulse-studio/learning-blocks/object-detection/fomo-object-detection-for-constrained-devices). 
