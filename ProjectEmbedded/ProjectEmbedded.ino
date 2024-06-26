#include <SPI.h>
#include <MQ135.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <DHT.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <HTTPClient.h>
#include "DisplayDateTime.h"
#include "DisplayTemperatureHumidity.h"
#include "DisplayAirQuality.h"

#define TFT_CS   5
#define TFT_DC   16
#define TFT_MOSI 23
#define TFT_CLK  18
#define TFT_RST  17
#define TFT_MISO 19
#define TFT_LED  4  
// khai bao script cua sheet
String Web_App_URL = "https://script.google.com/macros/s/AKfycbw-ZUc-ImTcnh2BJs3tarCLp6IZjrRtQ5MNsYA_QNRi_avJeU7mTJJujBQSQAmpBtcZ4g/exec";

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#define PIN_MQ135 32
#define DHT_PIN 15
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

MQ135 mq135_sensor(PIN_MQ135);

RTC_DS1307 rtc;

// Đặt thông tin WiFi của bạn
const char *ssid     = "iPhone của Sơn";
const char *password = "20222022";

const char* serverIP = "172.20.10.11";  // Địa chỉ IP của server
const uint16_t serverPort = 8000;           // Cổng của server

// Đặt múi giờ cho Hà Nội (GMT+7)
const long utcOffsetInSeconds = 7 * 3600;

// Khởi tạo UDP
WiFiUDP ntpUDP;

// Đối tượng WiFiClient để kết nối tới server -> gửi mail 
WiFiClient client;

// Khởi tạo NTPClient
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//// Khởi tạo đối tượng DisplayDateTime
DisplayDateTime displayDateTime(tft, rtc);


// Khởi tạo đối tượng DisplayTemperatureHumidity
DisplayTemperatureHumidity displayTempHumidity(tft, dht);


// Khởi tạo đối tượng DisplayAirQuality
DisplayAirQuality displayAirQuality(tft, dht, mq135_sensor);


unsigned long lastDateTimeUpdate = 0;
unsigned long lastTempHumidityUpdate = 0;
unsigned long lastAirQualityUpdate = 0;
unsigned long lastMail = -60000*30;
unsigned long lastUpdate = 0;
const unsigned long switchInterval = 10000; // Chuyển đổi màn hình mỗi 10 giây
const unsigned long dateTimeUpdateInterval = 10000; // Cập nhật thời gian mỗi 10 giây
const unsigned long tempHumidityUpdateInterval = 10000; // Thời gian cập nhật nhiệt độ độ ẩm
const unsigned long updateInterval = 30000;
const unsigned long airQualityUpdateInterval = 5000;
int mode = 0; // chế độ hiển thị
int recent_mode = -1; // chế độ hiển thị hiện tại
int lastmode = 0;
#define BUTTON_PIN 12
#define BUZZER_PIN 13
#define MQ135_PIN 32

//Chống nhảy phím
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

String Status_Read_Sensor = "Success";

void setup() {
  Serial.begin(115200);
  Serial.println("ILI9341 Test!"); 

  tft.begin();

  pinMode(TFT_LED, OUTPUT);
  backlighting(true);

  // Thiết lập chân nút bấm
  pinMode(BUTTON_PIN, INPUT);

  // Thiết lập chân buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Tắt buzzer ban đầu

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  tft.setRotation(2); // Landscape orientation

  dht.begin();
  
  tft.fillScreen(ILI9341_BLACK);

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  tft.setCursor(20, 100);
  tft.setTextSize(2);
  tft.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(20, 100);
  tft.setTextColor(ILI9341_GREEN);
  tft.println("Connected to WiFi...");
  
  tft.fillScreen(ILI9341_BLACK);
  Serial.println("Connected to WiFi");
  delay(5000);

  // Khởi động NTPClient
  //timeClient.begin();

  // Cập nhật thời gian từ NTP
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    DateTime now = DateTime(epochTime);
    rtc.adjust(now);
  }
}

void loop(void) {
  // Kiểm tra trạng thái nút bấm
  int reading = digitalRead(BUTTON_PIN);
  if (reading == HIGH && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();
    Serial.print("Mode: ");
    mode = (mode + 1) % 3; // Chuyển đổi mode
    lastmode = mode;
    Serial.println(mode);

    // Buzzer kêu
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100); // Kêu trong 100ms
    digitalWrite(BUZZER_PIN, LOW);
  } 
  if (millis() - lastUpdate >= updateInterval) {

    lastUpdate = millis(); 
    // lay cac tham so de dua len sheet
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float ppm_CO2 = displayAirQuality.get_ppmCO2();
    float ppm_CO = displayAirQuality.get_ppmCO();

    String Send_Data_URL = Web_App_URL + "?sts=write";
    // dinh kem cac tham so cho URL de cap nhat tren google sheet
    Send_Data_URL += "&srs=" + Status_Read_Sensor;
    Send_Data_URL += "&temp=" + String(t);
    Send_Data_URL += "&humd=" + String(h);
    Send_Data_URL += "&CO2=" + String(ppm_CO2);
    Send_Data_URL += "&CO=" + String(ppm_CO);

    Serial.println();
    Serial.println("-------------");
    Serial.println("Send data to Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Send_Data_URL);

    // Khởi tạo HTTPClient
    HTTPClient http;

    // HTTP GET Request.
    http.begin(Send_Data_URL.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); 
    // Gets the HTTP status code.
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);

   
    http.end();
    
    //::::::::::::::::::
  }

  // Hiển thị màn hình ngày giờ hoặc nhiệt độ và độ ẩm
  if (mode != recent_mode) {
    Serial.print("Mode changed ");
    recent_mode = mode;
    tft.fillScreen(ILI9341_BLACK);
    switch (mode) {
      case 0:
        displayDateTime.update(1);
        lastDateTimeUpdate = millis();
        break;
      case 1:
        displayTempHumidity.update(1);
        lastTempHumidityUpdate = millis();
        break;
      case 2:
        // Chỗ này có thể thêm mã cho màn hình khác nếu cần
        displayAirQuality.update(1);
        lastAirQualityUpdate = millis();
        break;
    }
  }

  // Cập nhật màn hình ngày giờ mỗi 10 giây
  if (mode == 0 && millis() - lastDateTimeUpdate >= dateTimeUpdateInterval) {
    displayDateTime.update(0);
    lastDateTimeUpdate = millis();
  }

  // Cập nhật màn hình nhiệt độ và độ ẩm mỗi 60 giây
  if (mode == 1 && millis() - lastTempHumidityUpdate >= tempHumidityUpdateInterval) {
    displayTempHumidity.update(0);
    lastTempHumidityUpdate = millis();

  }

  if (mode == 2 && millis() - lastAirQualityUpdate >= airQualityUpdateInterval){
    displayAirQuality.update(0);
    lastAirQualityUpdate = millis();
  }
  
  if (true) {
    //Serial.print("smoke: ") ;
    //lastAirQualityUpdate = millis();
    float ppm_CO2 = displayAirQuality.get_ppmCO2();
    float ppm_CO = displayAirQuality.get_ppmCO();
   // Serial.println(ppm_CO2);
   // Serial.println(ppm_CO);
      if (ppm_CO2 > 1000 || ppm_CO > 9) {
        if (mode < 4) tft.fillScreen(ILI9341_BLACK);
        if (mode < 4) lastmode = mode;
        mode = 4;
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100); // Kêu trong 100ms
        digitalWrite(BUZZER_PIN, LOW);
        tft.setTextSize(3);
        tft.setTextColor( 0x001F);
        tft.setCursor(20, 100);
        
        tft.println("GAS DETECTED...");

        if (millis() - lastMail > 60000*30 ) {
         if( client.connect(serverIP, serverPort)){
           client.print("GAS_DETECTED");
           lastMail = millis();
           client.stop();
         }
         else {
            Serial.println("Failed to connect to server");
          }
        } 
    }
    else mode = lastmode;
  }
  
  delay(10); // Cập nhật mỗi 10ms một lần
  
}

void backlighting(bool state) {
  if (state) {
    digitalWrite(TFT_LED, LOW);
  } else {
    digitalWrite(TFT_LED, HIGH);
  }
}
