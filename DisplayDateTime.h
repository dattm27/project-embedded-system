#ifndef DISPLAYDATETIME_H
#define DISPLAYDATETIME_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <RTClib.h>

class DisplayDateTime {
public:
    DisplayDateTime(Adafruit_ILI9341 &tftDisplay, RTC_DS1307 &rtc_ds1307);

    void update(bool draw);

private:
    Adafruit_ILI9341 &tft;
    RTC_DS1307 &rtc;
    DateTime lastDisplayTime; // Thời điểm hiển thị lần cuối

    void displayDateTime(DateTime currentDateTime);
};

DisplayDateTime::DisplayDateTime(Adafruit_ILI9341 &tftDisplay, RTC_DS1307 &rtc_ds1307)
    : tft(tftDisplay), rtc(rtc_ds1307), lastDisplayTime(2000, 1, 1, 0, 0, 0) {}

void DisplayDateTime::update(bool draw) {
    
    DateTime now = rtc.now();
   
    // So sánh với thời điểm hiển thị lần cuối để tránh hiển thị không cần thiết
    if ((now.minute() != lastDisplayTime.minute()) ||draw) {
        displayDateTime(now);
        lastDisplayTime = now;
    }
   
    
}

void DisplayDateTime::displayDateTime(DateTime currentDateTime) {
   
     tft.fillScreen(ILI9341_BLACK); // Clear the screen
    // Set the text color and size for hours and minutes
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(7);

    // Display hours
    tft.setCursor(40, 50);
    if (currentDateTime.hour() < 10) {
        tft.print('0');
    }
    tft.print(currentDateTime.hour(), DEC);

    tft.print(':');

    // Display minutes
    if (currentDateTime.minute() < 10) {
        tft.print('0');
    }
    tft.print(currentDateTime.minute(), DEC);

    // Set the text size for date
    tft.setTextSize(3);
    tft.setCursor(60, 150);

    // Display date
    if (currentDateTime.day() < 10) {
        tft.print('0');
    }
    tft.print(currentDateTime.day(), DEC);
    tft.print('/');
    if (currentDateTime.month() < 10) {
        tft.print('0');
    }
    tft.print(currentDateTime.month(), DEC);
    tft.print('/');
    tft.print(currentDateTime.year(), DEC);
}

#endif // DISPLAYDATETIME_H
