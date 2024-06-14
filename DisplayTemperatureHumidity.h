#ifndef DISPLAYTEMPHUMID_H
#define DISPLAYTEMPHUMID_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <DHT.h>

#define COLOR_RED     0x001F
#define COLOR_BLUE    0xF800

class DisplayTemperatureHumidity {
public:
    DisplayTemperatureHumidity(Adafruit_ILI9341 &tftDisplay, DHT &dhtSensor) 
        : tft(tftDisplay), dht(dhtSensor), lastTemp(NAN), lastHumidity(NAN) {}

    void update(bool draw) {
        float temp = dht.readTemperature();
        float humidity = dht.readHumidity();

        if (!isnan(temp) && !isnan(humidity) && (temp != lastTemp || humidity != lastHumidity)) {
            displayTemperatureHumidity(temp, humidity);
            lastTemp = temp;
            lastHumidity = humidity;
        }
        if (draw){
            displayTemperatureHumidity(temp, humidity);
            lastTemp = temp;
            lastHumidity = humidity;
        }
    }

private:
    Adafruit_ILI9341 &tft;
    DHT &dht;
    float lastTemp;
    float lastHumidity;

    void displayTemperatureHumidity(float temp, float humidity) {
  
         tft.fillScreen(ILI9341_BLACK); // Clear the screen
        // Draw temperature icon (thermometer)
        drawThermometerIcon(20, 50, 50);
        // Draw temperature value
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(3);
        tft.setCursor(100, 50);
        tft.print(temp);
        tft.print(" *C");

        // Draw humidity icon (water droplet)
        drawWaterDropletIcon(30, 150, 40);
        // Draw humidity value
        tft.setTextSize(3);
        tft.setCursor(100, 150);
        tft.print(humidity);
        tft.print(" %");
    }

    void drawThermometerIcon(int x, int y, int size) {
        int bulbRadius = size / 3;
        // Draw bulb
        tft.fillCircle(x + size / 2, y + size - bulbRadius, bulbRadius, COLOR_RED);
        // Draw body
        tft.fillRect(x + size / 2 - bulbRadius / 2, y, bulbRadius, size - bulbRadius, COLOR_RED);
        // Draw top of thermometer
        tft.fillCircle(x + size / 2, y, bulbRadius / 2, COLOR_RED);
    }

    void drawWaterDropletIcon(int x, int y, int size) {
        // Draw bottom part of droplet
        tft.fillCircle(x + size / 2, y + size / 2, size / 2, COLOR_BLUE);
        // Draw top part of droplet
        tft.fillTriangle(x, y + size / 2, x + size, y + size / 2, x + size / 2, y - size / 2, COLOR_BLUE);
    }
};

#endif // DISPLAYTEMPHUMID_H
