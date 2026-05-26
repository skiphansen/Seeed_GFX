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
#include <DrawOWM.h>
#include "owm_response.h"

#define ENABLE_LOGGING  1
#if ENABLE_LOGGING && __has_include("logging.h") 
#include "logging.h"
#else
#define LOG(format, ...)
#define LOG_RAW(format, ...)
#endif

EPaper epaper;

void setup() {
   Serial.begin(115200);

   while (!Serial);
   delay(250);

   LOG("Owm test\n");

   while (true) {
      LOG("Press a key to continue");
      while (!Serial.available());
      int incomingByte = Serial.read();

      epaper.begin();
      epaper.setRotation(1);
      epaper.fillScreen(TFT_WHITE);
      class DrawOWM owm = DrawOWM(epaper);

      owm.DrawIt(OwmForecastTestResponse,OwmAirPollutionTestResponse);
      epaper.update(); // update the display
   }
}

void loop()
{
    // put your main code here, to run repeatedly:
}
