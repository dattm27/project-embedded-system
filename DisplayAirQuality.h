#ifndef DISPLAYAIRQUALITY_H
#define DISPLAYAIRQUALITY_H

#include <Adafruit_ILI9341.h>

class DisplayAirQuality {
  public:
    DisplayAirQuality(Adafruit_ILI9341 &tft) : _tft(tft) {}
    void displayGasLevels(float CO2, float NH4, float CO, float ALC, float TOL, float ACE) {
        _tft.fillScreen(ILI9341_BLACK);
        _tft.setTextSize(2);
        _tft.setTextColor(ILI9341_WHITE);

        _tft.setCursor(10, 10);
        _tft.print("CO2: ");
        _tft.println(CO2);

        _tft.setCursor(10, 30);
        _tft.print("NH4: ");
        _tft.println(NH4);

        _tft.setCursor(10, 50);
        _tft.print("CO: ");
        _tft.println(CO);

        _tft.setCursor(10, 70);
        _tft.print("Alcohol: ");
        _tft.println(ALC);

        _tft.setCursor(10, 90);
        _tft.print("Toluene: ");
        _tft.println(TOL);

        _tft.setCursor(10, 110);
        _tft.print("Acetone: ");
        _tft.println(ACE);
    }

  private:
    Adafruit_ILI9341 &_tft;

   
};

#endif
