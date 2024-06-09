/***************************************************
 Modified graphics demo for the ESP-WROVER-KIT and equivalent
 projects using Ardiono-ESP32 with an ILI9341 LCD display
 By Martin Falatic
 ****************************************************/
/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
 
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
 
 
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "DHT.h"
#include "RTClib.h"
#include "WiFi.h"
#include <HTTPClient.h>
//#include <MQ135.h>
 
// For the ESP-WROVER_KIT, these are the default.
#define TFT_CS   5
#define TFT_DC   16
#define TFT_MOSI 23
#define TFT_CLK  18
#define TFT_RST  17
#define TFT_MISO 19
#define TFT_LED   4  // GPIO not managed by library
#define On_Board_LED_PIN  4
#define PPM_PIN 32
 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
//Thêm cài đặt cho moudule nhiệt độ, độ ẩm
// DHT setup
#define DHT_PIN 15
#define DHTTYPE DHT22 // Change to DHT22 if you're using DHT22
DHT dht(DHT_PIN, DHTTYPE);
 
//Thêm cài đặt cho moudule thời gian thực
// RTC setup
RTC_DS1307 rtc;
 
//----------------------------------------SSID and PASSWORD of your WiFi network.
const char* ssid = "THANG_2G";  //--> Your wifi name
const char* password = "0967240219"; //--> Your wifi password
//----------------------------------------
 
// Google script Web_App_URL.
String Web_App_URL = "https://script.google.com/macros/s/AKfycbyNtS-RJRaIMleQVuK3IGBLFW7f7eGk9mTiNNe_-K5y0UDsPGEVJPOAkLzXE0XfnZ4shQ/exec";
 
String Status_Read_Sensor = "Success";
 
int ppm = 1000;
//MQ135 gasSensor = MQ135(PPM_PIN);
 
void backlighting(bool state) {
    if (state) {
        digitalWrite(TFT_LED, LOW);
    }
    else {
        digitalWrite(TFT_LED, HIGH);
    }
}
 
void setup() {
  Serial.begin(115200);
  Serial.println("ILI9341 Test!");
 
  tft.begin();
  pinMode(TFT_LED, OUTPUT);
  backlighting(true);
  //Khởi động hai module mới
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  if (!rtc.isrunning()) {
    Serial.println("RTC lost power, let's set the time!");
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
 
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
   //----------------------------------------Set Wifi to STA mode
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.println("-------------");
  //----------------------------------------
 
  //----------------------------------------Connect to Wi-Fi (STA).
  Serial.println();
  Serial.println("------------");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
 
  //:::::::::::::::::: The process of connecting ESP32 with WiFi Hotspot / WiFi Router.
  // The process timeout of connecting ESP32 with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 has not been successfully connected to WiFi, the ESP32 will restart.
  // I made this condition because on my ESP32, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.
 
  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(On_Board_LED_PIN, HIGH);
    delay(250);
    digitalWrite(On_Board_LED_PIN, LOW);
    delay(250);
    if (connecting_process_timed_out > 0) connecting_process_timed_out--;
    if (connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }
 
  digitalWrite(On_Board_LED_PIN, LOW);
 
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("------------");
  //////////////////////////////////////////////////////////////////////////////
 
  dht.begin();
  //----------------------------
 
  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
}
 
void loop(void) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1); // Landscape orientation
  displayDateTime();
  displayTemperatureHumidity();
 
 
  // CODE SON
 
  //----------------------------------------Conditions that are executed when WiFi is connected.
  // This condition is the condition for sending or writing data to Google Sheets.
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(On_Board_LED_PIN, HIGH);
 
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    //ppm = gasSensor.getPPM();
    ppm = analogRead(PPM_PIN);
    Serial.println(ppm);
 
    tft.setCursor(10, 130);
    tft.print("PPM: ");
    tft.print(ppm);
 
    // Create a URL for sending or writing data to Google Sheets.
    String Send_Data_URL = Web_App_URL + "?sts=write";
    Send_Data_URL += "&srs=" + Status_Read_Sensor;
    Send_Data_URL += "&temp=" + String(t);
    Send_Data_URL += "&humd=" + String(h);
    Send_Data_URL += "&ppm=" + String(ppm);
 
    Serial.println();
    Serial.println("-------------");
    Serial.println("Send data to Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Send_Data_URL);
 
    //::::::::::::::::::The process of sending or writing data to Google Sheets.
      // Initialize HTTPClient as "http".
      HTTPClient http;
 
      // HTTP GET Request.
      http.begin(Send_Data_URL.c_str());
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
 
      // Gets the HTTP status code.
      int httpCode = http.GET();
      Serial.print("HTTP Status Code : ");
      Serial.println(httpCode);
 
      // Getting response from google sheets.
      String payload;
      if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload : " + payload);    
      }
     
      http.end();
    //::::::::::::::::::
   
    digitalWrite(On_Board_LED_PIN, LOW);
    Serial.println("-------------");
  }
  //----------------------------------------
 
 
  delay(10000); // Update every 10 seconds
}
 
 
 
void displayDateTime() {
  DateTime now = rtc.now();
 
  tft.setTextColor(ILI9341_WHITE);  
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print(now.hour(), DEC);
  tft.print(':');
  tft.print(now.minute(), DEC);
  tft.print(':');
  tft.print(now.second(), DEC);
 
  tft.setCursor(10, 40);
  tft.print(now.day(), DEC);
  tft.print('/');
  tft.print(now.month(), DEC);
  tft.print('/');
  tft.print(now.year(), DEC);
}
 
void displayTemperatureHumidity() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
 
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 70);
  tft.print("Temp: ");
  tft.print(t);
  tft.print(" *C");
 
  tft.setCursor(10, 100);
  tft.print("Humidity: ");
  tft.print(h);
  tft.print(" %");
}
