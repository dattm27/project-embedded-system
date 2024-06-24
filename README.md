# IT4210 - Embedded Systems - Project: Weather Station
## 1. Member
| No. |  Full name      | Student ID|
|-----|-----------------|-----------|
|  1  |  Trần Mạnh Đạt  | 20210159  |
|  2  | Phan Việt Hoàng | 20210372  |
|  3  | Trần Danh Linh  | 20210527  |
|  4  | Trần Hoàng Sơn  | 20210744  |

## 2. Project Introduction 
Name: Building a Weather Station \
Project objective: Develope a mulifunctional clock with indoor weather and air quality monitoring on a embedded system.

## 3. Building the system
- Devices: ESP32,  [LCD TFT 2.4](https://shopee.vn/M%C3%A0n-h%C3%ACnh-LCD-TFT-c%E1%BA%A3m-%E1%BB%A9ng-1.8-2.4-2.8-3.2-3.5-inch-SPI-cho-Arduino-ESP-i.330801854.24158446698?xptdk=23a411b6-4434-4119-84d3-810c1bae9b1d&fbclid=IwAR0mosN6kOy84kZnLfknSB1vfLk_Y4u6Vb0YZEppFElZ2gUGtfXslXeMl2A), DHT22, [MQ-135](https://shopee.vn/Module-Cảm-Biến-Chất-Lượng-Không-Kh%C3%AD-MQ135-MQ2-i.117503124.24350246624?sp_atk=fa0bcde8-68ca-4b26-8fcb-83077713ad5b&xptdk=fa0bcde8-68ca-4b26-8fcb-83077713ad5b), Breadboard, [Jumper Wire](https://shopee.vn/-L%E1%BA%ADp-Tr%C3%ACnh-Nh%C3%BAng-A-Z-G16-H%E1%BB%99p-G%E1%BB%93m-140-D%C3%A2y-C%E1%BA%AFm-Board-Test-i.107147748.22006884825?xptdk=a976812f-be33-4fd1-9cee-66e137d4bbf4&fbclid=IwAR05qScDSY-8r-3U31J760PkESmJaxe7Y5clcl2oxWoQluyZ_-GbGgx9h94), [Button](https://shopee.vn/-L%E1%BA%ADp-Tr%C3%ACnh-Nh%C3%BAng-A-Z-G16-H%E1%BB%99p-G%E1%BB%93m-140-D%C3%A2y-C%E1%BA%AFm-Board-Test-i.107147748.22006884825?xptdk=a976812f-be33-4fd1-9cee-66e137d4bbf4&fbclid=IwAR05qScDSY-8r-3U31J760PkESmJaxe7Y5clcl2oxWoQluyZ_-GbGgx9h94), Buzzer, Resistor (Purchase link).
- Development environment:  Arduino IDE, Visual Studio Code.
- Programming language: C/C++.
- Libraries Required: Adafruit GFX Library, DHT Sensor Libary, Adafruit ILI9431, TFT e_SPI, Wifi, NTCClient.
## 4. Implementation instructions
1. Clone this repository 
2. Install Arduino and launch the project
3. Install ESP32 Board Support (Arduino ESP32 Boards)
3. Install Required Libraries
4. Connect the hardware components to the ESP as the design. 
5. Upload the code the ESP32
6. Compile the server code (**server.c**) with the below cmd: \
`gcc server.c -o server -lpthread -lcurl`

    > Note: Replace your app password in EMAIL_PASSWORD before compiling.
7. Start the server \
`./server`

## 4. Demo
### Device
<img src="/Images/Device.jpg" alt="Device" width="200"/>

### Mode 0: Date Time Screen
<img src="/Images/DateTimeScreen.jpg" alt="Date - Time Screen" width="200"/>

### Mdoe 1: Temperature - Humidity Screen
<img src="/Images/Temp_HumidityScreen.jpg" alt="Temperature - Humidity Screen" width="200"/>

### Mdoe 2: Air Quality Screen
<img src="/Images/AirQuality.jpg" alt="Air Quality Screen" width="200"/>

### Gas Dectected Screen
<img src="/Images/Gas_Dectected.jpg" alt="Gas Dectected Screen" width="200"/>

### Notify user by email
<div style="display: flex;">
    <img src="/Images/Email_1.jpg" alt="Email to notify user" width="200"/> 
    <img src="/Images/Email_2.jpeg" alt="Email to notify user" width="200"/>
</div>
