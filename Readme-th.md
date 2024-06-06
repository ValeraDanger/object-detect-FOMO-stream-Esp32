## object-detect-FOMO-stream-Esp32
 [For English version](https://github.com/San279/object-detect-FOMO-stream-Esp32)
 <br/>
 <br/>
 โปรเจ็คนี้ถูกออกแบบมาใช้กับ [FOMO](https://docs.edgeimpulse.com/docs/edge-impulse-studio/learning-blocks/object-detection/fomo-object-detection-for-constrained-devices) AI ตรวจจับวัตถุ ในส่วนของการสตรีมผล AI บน AIoT บอร์ด หรือ Esp32Cam ที่มี PSRAM เท่านั้น
 <br/>
 <br/>
 <strong> - ก่อนใช้ไฟล์นี้ต้องมีไฟล์โมเดล [FOMO](https://github.com/San279/train-FOMO-object-detect-esp32) ก่อน</strong> 
<br/>
## สิงที่ต้องมี
 - [AIoT](https://wirelesssolution.asia/) บอร์ด Esp32-S3 หรือ Esp32 ที่มี PSRAM
 - กล้อง OV 2640
 - [Arduino IDE](https://www.arduino.cc/en/software) อันเก่าหรือใหม่ก้ได้
 - [NodeJS](https://nodejs.org/en/download/package-manager/current) สำหรับการส่งผลของ AI ไปประมวลผล
## โครงสร้าง
1. FOMO_object_detect_stream.ino - โค้ด Arduino สำหรับส่งผลของ AI ไปที่ NodeJS และ stream รูปภาพกล้อง
2. index.html - โค้ด html สำหรับส่วนหน้าบ้านของเว็ปไซค์
3. index.js - โค้ด Javascript สำหรับฟังชึ้นต่างๆ ของส่วนหน้าบ้านเว็ปไซค์.
4. api.js - โค้ด Javascript คำนวนผลของ AI และเป็นตัวเชื่อมเว็ปไซค์กับโค้ด Arduino
5. styles.css - ทำให้เว็ปไซค์ดูสวยงสาม.
## วิธีรันโปรเจ็ค
<strong> 1. ดาวน์โหลดแฟ้มเป็น zip และแตกไฟล์ในแฟ้ม Arduino. </strong>
<br /><br />
![alt_text](/Images_for_readme/folder_directory.PNG)
<br /><br /><br /><br />
<strong> 2. ไปที่แฟ้มที่แตกไฟล์แล้วลง javascript dependencies เพื่อรันไฟล์ api.js จะต้องมี NodeJS ในขั้นตอนนี้</strong> <br /><br />
  - สำหรับ IDE linux หรือ Mac ให้เราเปิด terminal และไปที่ไฟล์ หลังจากนั้นให้พิมพ์ npm install ตามไปด้วย node api.js บน terminal <br /><br />
  ![alt text](/Images_for_readme/ide_run_api.PNG)
  <br /><br /><br />
- สำหรับวินโดว์(windows) ให้ไปที่ไฟล์และพิมพ์ cmd ตรงช่องด้านบน จอ terminal ก้จะเปิดขึ้นมา<br /><br />
 ![alt text](/Images_for_readme/window_run_api1.PNG)
 ![alt text](/Images_for_readme/window_run_api2.PNG)
- พิมพ์ npm install ตามด้วย node api.js บนจอ terminal<br /><br />
 ![alt text](/Images_for_readme/window_run_api3.PNG)
  <br /><br /> <br /><br />
 <strong> 3. เปิดไฟล์ FOMO_object_detect_stream.ino และใส่(ชื่ออินเตอร์เน็ตที่คอมพิวเตอร์เราต่ออยู่) SSID  รหัส(PASSWORD) และ IPV4 address </strong> <br /><br />
![alt text](/Images_for_readme/wifi_ipv4.PNG)
<br /><br /><br />
  - สำหรับวินโดว์ เปิด cmd แล้วพิมพ์ ipconfig. <br /><br />
    ![alt text](/Images_for_readme/cmd_ipv4.PNG) <br /><br />
  - สำหรับ Linux เปิด terminal และพิม hostname -I <br /><br />
    ![alt text](Images_for_readme/terminal_ipv4.PNG) <br /><br />
<br /><br /><br /><br />
<strong> 4. ใน Arduino กดไปที่ sketch และ Add .Zip library หลังจากนั้นให้เลือกไฟล์โมเดล</strong>
<br /><br />
![alt text](Images_for_readme/arduino_model_zip.PNG)
<br /><br />
![alt text](Images_for_readme/FOMO_model_zip.PNG)
<br /><br /><br /><br />
<strong> 5. เปลี่ยนชื่อไฟล์บรรทัดที่ 24 ให้ตรงกับชื่อโปรเจ็คใน Edge Impulse </strong> 
<br /><br />
![alt_text](/Images_for_readme/match_name.PNG)
<br /><br /><br /><br />
<strong> 6. กดไปที่ tools ตรงตัวเลือกด้านบนและเปลี่ยน Board เป็น "ESP32S3 Dev Module" และเปลี่ยน PSRAM เป็น "OPI PSRAM".  </strong>
<br /><br />
![alt_text](/Images_for_readme/IDE_configure.PNG)
<br /><br /><br /><br />
<strong> 7. อัพโหลดโค้ดขึ้นบน ESP32-S3 ขั้นตอนนี่อาจจะใช้ 20 - 30 นาที เสร็จแล้วให้คัดลอก ip address</strong>
<br/> <br/>
![alt text](/Images_for_readme/arduino_serial_monitor.PNG)
<br /><br /><br /><br />
<strong> 8. เปิด index.html และนำ ip address ที่คัดลอกมาวางใว้ในกล่องและกด ok</strong>
<br/> <br/>
![alt text](/Images_for_readme/ip_prompt.PNG)
<br /><br /><br /><br />
9. <strong> เสร็จสิ้น </strong>  
![alt text](/Images_for_readme/done.PNG)<br /><br />
<br /><br /><br /><br />
## เครดิต 
ต้องขอขอบคุณ [WIRELESS SOLUTION ASIA CO.,LTD](https://wirelesssolution.asia/) สำหรับการสนับสนุนโปรเจ็คนี้ และ [RandomNerdTutorials](https://RandomNerdTutorials.com/esp32-cam-video-streaming-web-server-camera-home-assistant) สำหรับโค้ดส่วนสตรีมรูปภาพขึ้นบนเว็ปเซอร์เวอร์ และ [Edge Impulse](https://edge-impulse.gitbook.io/docs/edge-impulse-studio/learning-blocks/object-detection/fomo-object-detection-for-constrained-devices) สำหรับโค้ดส่วนรันโมเดล FOMO
