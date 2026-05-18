/*This is a 6-color electronic ink screen, which can only display 6 colors. 
Any other colors will be mapped to similar ones.

Here is the 6 colors you can display:
1.TFT_WHITE
2.TFT_BLACK
3.TFT_YELLOW
4.TFT_GREEN
5.TFT_BLUE
6.TFT_RED
*/

#include <Arduino.h>

#include "driver.h"
#include "TFT_eSPI.h"
#include "logging.h"

#ifdef EPAPER_ENABLE // Only compile this code if the EPAPER_ENABLE is defined in User_Setup.h
EPaper epaper;
#endif

void setup()
{
    Serial.begin(115200);

    while(!Serial);
    delay(250);

#ifdef E2741QS0B3
  LOG("7.4\" BWRY E-Paper HelloWold Example\n");
#else
  LOG("2.9\" BWRY E-Paper HelloWold Example\n");
#endif
  while(true) {
     LOG("Press a key to continue");
     while(!Serial.available());
     int incomingByte = Serial.read();

   #ifdef EPAPER_ENABLE
       epaper.begin();
       epaper.fillScreen(TFT_WHITE);

       epaper.fillCircle(25, 25, 15, TFT_RED);
       epaper.fillRect(epaper.width() - 40,  10, 30, 30, TFT_YELLOW);

       for (int i = 0; i < epaper.height() / 80; i++)
       {
           epaper.setTextColor(TFT_BLACK);
           epaper.setTextSize(i + 1);
           epaper.drawLine(10, 70 + 60 * i, epaper.width() - 10, 70 + 60 * i, TFT_BLACK);
           epaper.drawString("Hello ePaper", 10, 80 + 60 * i);
       }

       epaper.update(); // update the display

   #endif
  }
}

void loop()
{
    // put your main code here, to run repeatedly:
}
