#ifndef DISPLAYAIRQUALITY_H
#define DISPLAYAIRQUALITY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <DHT.h>
#include <MQ135.h>

class DisplayAirQuality {
  public:
    DisplayAirQuality(Adafruit_ILI9341 &tft, DHT &dhtSensor, MQ135 &_mq135) 
    : _tft(tft), dht(dhtSensor), mq135_sensor(_mq135), PPM1(0), PPM2(0) {}

    void update(bool draw) {
        // khoi tao gia tri cho cac RZero
        if (PPM1 == 0) update_RZERO();

        // Lay PPM cua khi CO2
        PPM1 = mq135_sensor.getPPM_CO2(RZero1);
        // Lay PPM cua khi CO
        PPM2 = mq135_sensor.getPPM_CO(RZero2);
        // Hien thi PPM cua 2 khi
        displayGasLevels(PPM1, PPM2);
    }

    float get_ppmCO2 () {
      return this->mq135_sensor.getPPM_CO2(RZero1);
    }
    float get_ppmCO () {
      return this->mq135_sensor.getPPM_CO(RZero2);
    }

  private:
    Adafruit_ILI9341 &_tft;
    DHT &dht;
    MQ135 &mq135_sensor;
    
    float RZero1, RZero2, PPM1, PPM2;

    void update_RZERO () {
        for (int i = 1; i <= 50; i++) {
          this->RZero1 += mq135_sensor.getRZero_CO2();
          this->RZero2 += mq135_sensor.getRZero_CO();
        }
        this->RZero1 /= 50;
        this->RZero2 /= 50;
    }

    void displayGasLevels(float CO2, float CO) {

        _tft.fillRect(90, 100, 200, 200, ILI9341_BLACK);
        _tft.setTextSize(3);
        _tft.setTextColor(ILI9341_YELLOW);
        _tft.setCursor(0, 30);
        _tft.println("   AIR QUALITY");
        _tft.setCursor(10, 70);
        _tft.println("      (PPM)");
        
        _tft.setTextColor(ILI9341_WHITE);

        _tft.setCursor(10, 110);
        _tft.print("CO2: ");
        _tft.println(CO2);
       // _tft.println(" PPM");

        _tft.setCursor(10, 150);
        _tft.print("CO:  ");
        _tft.println(CO);
       // _tft.println(" PPM");
    }
};

#endif
