#ifndef DISPLAYAIRQUALITY_H
#define DISPLAYAIRQUALITY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <DHT.h>
#include <MQ135.h>

class DisplayAirQuality {
  public:
    DisplayAirQuality(Adafruit_ILI9341 &tft, DHT &dhtSensor, MQ135 &_mq135) 
    : _tft(tft), dht(dhtSensor), mq135_sensor(_mq135), correctedPPM1(0) {}

    void update(bool draw) {

        if (correctedPPM1 < 350) update_RZERO();

        temperature = dht.readTemperature(); // Assume current temperature. Recommended to measure with DHT22
        humidity = dht.readHumidity(); // Assume current humidity. Recommended to measure with DHT22

        correctedPPM1 = mq135_sensor.getCorrectedPPM_CO2(temperature, humidity, correctedRZero1);
        correctedPPM2 = mq135_sensor.getCorrectedPPM_CO(temperature, humidity, correctedRZero2);
        correctedPPM3 = mq135_sensor.getCorrectedPPM_NH4(temperature, humidity, correctedRZero3);

        /*Serial.print("\t Temperature: ");
        Serial.print(temperature);
        Serial.println("'C");
        Serial.print("\t Humidity: ");
        Serial.println(humidity);
        Serial.println("----------------------------");
        Serial.print("\t Corrected RZero: ");
        Serial.print(correctedRZero1);
        Serial.print("\t Corrected PPM: ");
        Serial.print(correctedPPM1);
        Serial.println("ppm");
        Serial.println("");

        Serial.print("\t Corrected RZero: ");
        Serial.print(correctedRZero2);
        Serial.print("\t Corrected PPM: ");
        Serial.print(correctedPPM2);
        Serial.println("ppm");
        Serial.println("");

        Serial.print("\t Corrected RZero: ");
        Serial.print(correctedRZero3);
        Serial.print("\t Corrected PPM: ");
        Serial.print(correctedPPM3);
        Serial.println("ppm");
        Serial.println("");

        Serial.println("");*/

        displayGasLevels(correctedPPM1, correctedPPM2, correctedPPM3);
    }

  private:
    Adafruit_ILI9341 &_tft;
    DHT &dht;
    MQ135 &mq135_sensor;
    
    float correctedRZero1, correctedRZero2, correctedRZero3, temperature, humidity, correctedPPM1, correctedPPM2, correctedPPM3;

    void update_RZERO () {
        temperature = dht.readTemperature(); // Assume current temperature. Recommended to measure with DHT22
        humidity = dht.readHumidity(); // Assume current humidity. Recommended to measure with DHT22
        for (int i = 1; i <= 50; i++) {
          this->correctedRZero1 += mq135_sensor.getCorrectedRZero_CO2(temperature, humidity);
          this->correctedRZero2 += mq135_sensor.getCorrectedRZero_CO(temperature, humidity);
          this->correctedRZero3 += mq135_sensor.getCorrectedRZero_NH4(temperature, humidity);
        }
        this->correctedRZero1 /= 50;
        this->correctedRZero2 /= 50;
        this->correctedRZero3 /= 50;
    }

    void displayGasLevels(float CO2, float CO, float NH4) {

        _tft.fillRect(90, 100, 200, 200, ILI9341_BLACK);
        _tft.setTextSize(3);
        _tft.setTextColor(ILI9341_YELLOW);
        _tft.setCursor(0, 30);
        _tft.println("   GAS DETECTOR");
        _tft.setCursor(10, 70);
        _tft.println("      (PPM)");
        
        _tft.setTextColor(ILI9341_WHITE);

        _tft.setCursor(10, 110);
        _tft.print("CO2: ");
        _tft.println(CO2);
       // _tft.println(" PPM");

        _tft.setCursor(10, 150);
        _tft.print("NH4: ");
        _tft.println(NH4);
       // _tft.println(" PPM");

        _tft.setCursor(10, 190);
        _tft.print("CO:  ");
        _tft.println(CO);
       // _tft.println(" PPM");
    }

   
};

#endif
