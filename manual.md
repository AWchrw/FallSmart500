# คู่มือการใช้งาน
## 1. Mqtt broker (Mosquito)
ติดตั้ง [mosquitto](https://mosquitto.org/download/) (ขึ้นอยู่กับ OS)

สร้างไฟล์ mosquitto.conf
```bash
listener 1883 0.0.0.0
allow_anonymous true
```
เเล้วรัน
```bash
mosquitto -c mosquitto.conf -v
```
เสร็จเเล้ว mosquitto จะรันอยู่บน localhost ของเราที่ port 1883
สามารถเชื่อมต่อผ่าน Mqttx หรือ Node Red ด้วย localhost:1883

หากเชื่อมต่อจากเครื่องอื่นให้ใช้ ip เเทน ตามตัวอย่างข้างล่าง

## 2. Node Red
ติดตั้ง [Nodejs](https://nodejs.org/en/download)

ติดตั้ง Node Red
```bash
npm i -g node-red
```
เสร็จเเล้วรัน
```bash
node-red
```
Node Red จะรันบน localhost ของเรา ให้เปิด web browser เเล้วไปที่ http://localhost:1880ฟ

หากเข้าจากเครื่องอื่นบน network


## เชื่อมต่อด้วย ip (Mosquitto เเละ Node Red)
อย่างเเรกอุปกรณ์จะต้องอยู่ใน network เดียวกัน ให้รันคำสั่งดังนี้บนเครื่องที่เป็น host

Window
```bash
ipconfig
```
Mac / Linux
```bash
ifconfig
```
เเล้วนำ ip ไปใช้งาน ตย. http://192.168.1.40:1883

## 3. Google Sheets
สร้าง Service account [ที่นี้](https://console.cloud.google.com/iam-admin/serviceaccounts) 

ตั้ง Role ให้เป็น Editor

Export credential เป็น Json เเละก็อป email service ไว้

เพิ่ม Credential (json) ใน Node red

เพิ่ม email share ใน spreadsheet

ใน Node Red ให้ใส่ spreadsheet id ใน node ด้วย

## 4. Arduino IDE
ใส่ต่าใน config.h ของเเต่ละโฟลเดอร์

ติดตั้ง Library ดังนี้
- [LiquidCrystal](https://docs.arduino.cc/libraries/liquidcrystal/)
- [PubSubClient](http://pubsubclient.knolleary.net)
- [MPU6050](https://github.com/electroniccats/mpu6050)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)