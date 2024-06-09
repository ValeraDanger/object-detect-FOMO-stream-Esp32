## object-detect-FOMO-stream-Esp32
 [For English version](https://github.com/San279/object-detect-FOMO-stream-Esp32)
 <br/>
 <br/>
 โปรเจ็คนี้ถูกออกแบบมาใช้กับ [FOMO](https://docs.edgeimpulse.com/docs/edge-impulse-studio/learning-blocks/object-detection/fomo-object-detection-for-constrained-devices) AI ตรวจจับวัตถุ ในส่วนของการสตรีมผล AI จาก AIoT บอร์ด หรือ Esp32Cam ขึ้นบนเว็ปเซอร์เวอร์ <br/>
 <br/>
 <strong> - ก่อนใช้ไฟล์นี้ต้องมีไฟล์โมเดล [FOMO](https://github.com/San279/train-FOMO-object-detect-esp32) ก่อน</strong> 
 <br/>
 <br/>
 ![alt text](/Images_for_readme/done_th.PNG)<br /><br />
<br /><br />
## สิงที่ต้องมี
 - [AIoT](https://wirelesssolution.asia/) บอร์ด Esp32-S3 หรือ Esp32 ที่มี PSRAM
 - กล้อง OV 2640
 - [Arduino IDE](https://www.arduino.cc/en/software) อันเก่าหรือใหม่ก้ได้
 - [NodeJS](https://nodejs.org/en/download/package-manager/current) สำหรับการส่งผลของ AI ไปประมวลผล
## โครงสร้าง
1. FOMO_object_detect_stream.ino - Arduino สำหรับส่งผลของ AI ไปที่ NodeJS และ stream รูปภาพกล้อง
2. index.html - html สำหรับส่วนหน้าเว็ปไซค์
3. index.js - ไฟล์ Javascript สำหรับฟังชึ้นต่างๆ ของหน้าเว็ปไซค์
4. api.js - ไฟล์ Javascript สำหรับ Acess point ในส่วนของการเชื่อมผล Ai จาก Arduino กับ หน้าเว็ปไซค์ 
5. styles.css - ทำให้เว็ปไซค์ดูสวยงสาม
## วิธีรันโปรเจ็ค
<strong> 1. ดาวน์โหลดแฟ้มเป็น zip และแตกไฟล์ในแฟ้ม Arduino บนคอมพิวเตอร์ของเรา </strong>
<br /><br />
![alt_text](/Images_for_readme/folder_directory.PNG)
<br /><br /><br /><br />
<strong> 2. ไปที่แฟ้มแล้วลง javascript dependencies เพื่อรันไฟล์ api.js จะต้องมี NodeJS ในขั้นตอนนี้</strong> <br /><br />
  - สำหรับ IDE linux หรือ Mac ให้เราเปิด terminal และไปที่ไฟล์ หลังจากนั้นให้ก็อบ command เหล่านี่ไปใว้ใน terminal <br /><br />
  ```text1
hostname -I
npm install
node api.js
```
  ![alt text](/Images_for_readme/ide_run_api.PNG)
  <br /><br /><br />
- สำหรับวินโดว์(windows) ให้ไปที่ไฟล์และพิมพ์ cmd ตรงช่องด้านบนไฟล์ของเราและให้กด enter จอ cmd จะเปิดขึ้น<br /><br />
 ![alt text](/Images_for_readme/window_run_api1.PNG)
 ![alt text](/Images_for_readme/window_run_api2.PNG)
- พิมพ์หรือก็อบ command เหล่านี่ไปใว้ที่ cmd และกดปุ้ม enter
```text1
ipconfig
npm install
node api.js
```
 ![alt text](/Images_for_readme/cmd_commands.jpg)
  <br /><br /> <br /><br /><br />
 <strong> 3. เปิดไฟล์ FOMO_object_detect_stream.ino และใส่ ssid(ชื่ออินเตอร์เน็ต)  รหัส(password) และ ipv4_address </strong> <br /><br />
![alt text](/Images_for_readme/wifi_ipv4.PNG)
<br /><br /><br />
  - เพื่อหา ipv4_address ให้ไปที่ cmd หรือ terminal ที่เราเปิดในขั้นตอนที่แล้วและหารหัสของเครื่องเรา <br /><br />
    ![alt text](/Images_for_readme/ipv4_addr.jpg) <br /><br />
<br /><br /><br /><br />
<strong> 4. ใน Arduino กดไปที่ sketch และ Add .Zip library หลังจากนั้นให้เลือกไฟล์โมเดล</strong>
<br /><br />
![alt text](Images_for_readme/arduino_model_zip.PNG)
<br /><br />
![alt text](Images_for_readme/FOMO_model_zip.PNG)
<br /><br /><br /><br /><br />
<strong> 5. เปลี่ยนชื่อไฟล์บรรทัดที่ 24 ให้ตรงกับชื่อโปรเจ็คใน Edge Impulse </strong> 
<br /><br />
![alt_text](/Images_for_readme/match_name.PNG)
<br /><br /><br /><br /><br />
<strong> 6. กดไปที่ tools ตรงตัวเลือกด้านบนและเปลี่ยน Board เป็น "ESP32S3 Dev Module" และเปลี่ยน PSRAM เป็น "OPI PSRAM".  </strong>
<br /><br />
![alt_text](/Images_for_readme/IDE_configure.PNG)
<br /><br /><br /><br /><br />
<strong> 7. อัพโหลดโค้ดขึ้นบน ESP32-S3 ขั้นตอนนี่อาจจะใช้ 20 - 30 นาที เสร็จแล้วให้คัดลอก ip address</strong>
<br/> <br/>
![alt text](/Images_for_readme/arduino_serial_monitor.PNG)
<br /><br /><br /><br />
<strong> 8. เปิด index.html และนำ ip address ที่คัดลอกมาวางใว้ในกล่องและกด ok</strong>
<br/> <br/>
![alt text](/Images_for_readme/ip_prompt.PNG)
<br /><br /><br /><br /><br />
9. <strong> เสร็จสิ้น </strong>  
![alt text](/Images_for_readme/done_th.PNG)<br /><br />
<br /><br /><br /><br /><br /><br />
## ฟีเจอร์ของโปรเจ็ต
- ยูเซอร์สามารถเปลี่ยนภาษา ตรงด้านขวาบนของหน้าเว็ป<br />
- เปลี่ยนการตั้งค่าต่างๆ ของกล้องได้ ดูเพิ่มเติมได้ที่[
https://heyrick.eu/blog/index.php?diary=20210418&keitai=0](https://heyrick.eu/blog/index.php?diary=20210418&keitai=0)<br />
- ยูเซอร์เลือกที่จะแอบการตั้งค่าของกล้องได้ กดตรงปุ่ม icon ด้านบน <br /><br />
![alt text](/Images_for_readme/settings_th.PNG)
<br /><br /><br />
- เปลี่ยนความละเอียดหรือมิติกล้อง. <br /><br />
![alt_text](/Images_for_readme/console_th.PNG)
<br /><br /><br />
- เมื่อกดปุ้มกล้อง เว็บเซิร์ฟเวอร์จะเริ่มการบันทึกรูปที่มีผลของวัตถุที่เราใช้ฝึก AI <br />
- เซ็ทจำนวนรูปที่อยากบันทึกต่อครั้ง  <br />
- Once capture button is clicked a timer will start to count number of seconds it took to capture all images.<br /> <br />
![alt_text](/Images_for_readme/console_seconds_th.PNG)
<br /><br /> <br />
- Dเมื่อกดปุ้ม Download ตรงซ้ายบนของ gallery จะรวมรูปภาพทั้งหมดใว้ในแฟ้มเดียวและดาวโหลดเป็น zip รูปที่ดาวโหลดจะไม่มีผลของ AI วาดใว้<br />
- ปุ้ม Clear ตรงขวาบนจะลบรูปที่บันทึกใว้ทั้งหมด<br />
- ยูเซอร์สามารถเลือกลบรูปที่ไม่ต้องการได้ <br /><br />
![alt_text](/Images_for_readme/gallery_img.PNG)
<br /> <br /><br /> <br />

## เครดิต 
ต้องขอขอบคุณ [WIRELESS SOLUTION ASIA CO.,LTD](https://wirelesssolution.asia/) สำหรับการสนับสนุนโปรเจ็คนี้ และ [RandomNerdTutorials](https://RandomNerdTutorials.com/esp32-cam-video-streaming-web-server-camera-home-assistant) สำหรับโค้ดส่วนสตรีมรูปภาพขึ้นบนเว็ปเซอร์เวอร์ และ [Edge Impulse](https://edge-impulse.gitbook.io/docs/edge-impulse-studio/learning-blocks/object-detection/fomo-object-detection-for-constrained-devices) สำหรับโค้ดส่วนรันโมเดล FOMO
