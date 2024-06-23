#ifndef DISPLAYDATETIME_H
#define DISPLAYDATETIME_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <RTClib.h>

class DisplayDateTime {
public:
    DisplayDateTime(Adafruit_ILI9341 &tftDisplay, RTC_DS1307 &rtc_ds1307)
    : tft(tftDisplay), rtc(rtc_ds1307), lastDateTime(2000, 1, 1, 0, 0, 0) {}

    void update(bool modeChanged) {
       DateTime now = rtc.now();
      // So sánh với thời điểm hiển thị lần cuối để tránh hiển thị không cần thiết
      if (modeChanged)
      {
         currentDateTime = now; 
        
         displayTime();
         displayDate();
         lastDateTime = now;
        
      }
      else {
          if (now.minute() != currentDateTime.minute()) { 
            currentDateTime = now;  
            displayTime();
            
            if (now.day() != lastDateTime.day() ) {
              displayDate();
              
            }
            lastDateTime = now;
                       
        }
      }
      
    }

private:
    Adafruit_ILI9341 &tft;
    RTC_DS1307 &rtc;
    DateTime currentDateTime; // Thời điểm hiển thị lần cuối
    DateTime lastDateTime; // datetime của lần hiển thị trước đó
    void displayTime(){
       //tft.fillScreen(ILI9341_BLACK); // Clear the screen
      // Set the text color and size for hours and minutes
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(7);
  
      //Clear màn hình giờ
      if (currentDateTime.hour() != lastDateTime.hour()) tft.fillRect(40, 50, 250, 50, ILI9341_BLACK);
      else tft.fillRect(140, 50, 250, 50, ILI9341_BLACK);
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
  
      
    }

    void displayDate() {
      tft.fillRect(60, 150, 200, 50, ILI9341_BLACK);
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
   
};




#endif // DISPLAYDATETIME_H
