#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <DHT.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define TFT_CS   5
#define TFT_DC   16
#define TFT_MOSI 23
#define TFT_CLK  18
#define TFT_RST  17
#define TFT_MISO 19
#define TFT_LED  4  

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#define DHT_PIN 15
#define DHTTYPE DHT22 
DHT dht(DHT_PIN, DHTTYPE);

RTC_DS1307 rtc;

DateTime lastDateTime;
float lastTemp = 0;
float lastHumidity = 0;

// Đặt thông tin WiFi của bạn
const char *ssid     = "THANG_2G";
const char *password = "0967240219";

// Đặt múi giờ cho Hà Nội (GMT+7)
const long utcOffsetInSeconds = 7 * 3600;

// Khởi tạo UDP
WiFiUDP ntpUDP;

// Khởi tạo NTPClient
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(115200);
  Serial.println("ILI9341 Test!"); 

  tft.begin();
  pinMode(TFT_LED, OUTPUT);
  backlighting(true);
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  dht.begin();
  tft.setRotation(1); // Landscape orientation

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Khởi động NTPClient
  timeClient.begin();
}

void loop(void) {
  // Cập nhật thời gian từ NTP mỗi giờ
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    DateTime now = DateTime(epochTime);
    rtc.adjust(now);
  }

  DateTime now = rtc.now();

  // Clear the top section before displaying date and time
  clearSection(0, 0, tft.width(), tft.height() / 2);

  // Display date and time in the top section of the screen
  displayDateTime(now);

  // Cập nhật nhiệt độ và độ ẩm
  updateTemperatureHumidity();

  // Chờ 10 giây trước khi cập nhật lại
  delay(10000); // Update every 10 seconds
}

void backlighting(bool state) {
  if (state) {
    digitalWrite(TFT_LED, LOW);
  } else {
    digitalWrite(TFT_LED, HIGH);
  }
}

void displayDateTime(DateTime lastDateTime) {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);

  // Display date and time in the top section of the screen
  tft.setCursor(10, 10);
  tft.print(lastDateTime.hour(), DEC);
  tft.print(':');
  tft.print(lastDateTime.minute(), DEC);

  // Display the full date in the top section
  tft.setTextSize(2);
  tft.setCursor(10, 50);
  tft.print(lastDateTime.day(), DEC);
  tft.print('/');
  tft.print(lastDateTime.month(), DEC);
  tft.print('/');
  tft.print(lastDateTime.year(), DEC);
}

void updateTemperatureHumidity() {
  float newTemp = dht.readTemperature();
  float newHumidity = dht.readHumidity();

  // Kiểm tra xem dữ liệu mới có thay đổi không
  if (newTemp != lastTemp || newHumidity != lastHumidity) {
    // Cập nhật dữ liệu hiển thị nếu có thay đổi
    lastTemp = newTemp;
    lastHumidity = newHumidity;
    displayTemperatureHumidity();
  }
}

void displayTemperatureHumidity() {
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);

  // Clear the bottom section before displaying temperature and humidity
  clearSection(0, tft.height() / 2, tft.width(), tft.height() / 2);

  // Display temperature on the left side of the bottom section
  tft.setCursor(10, tft.height() / 2 + 10);
  tft.print("Temp: ");
  tft.setCursor(10, tft.height() / 2 + 40);
  tft.print(lastTemp);
  tft.print(" *C");

  // Display humidity on the right side of the bottom section
  tft.setCursor(tft.width() / 2 + 10, tft.height() / 2 + 10);
  tft.print("Humidity: ");
  tft.setCursor(tft.width() / 2 + 10, tft.height() / 2 + 40);
  tft.print(lastHumidity);
  tft.print(" %");
}

void clearSection(int x, int y, int width, int height){
  tft.fillRect(x, y, width, height, ILI9341_BLACK);
}
