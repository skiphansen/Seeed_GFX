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

void setup() 
{
   OwmArgs Args;

   Serial.begin(115200);
// City name that will be shown in the top-right corner of the display.
   Args.City = "Rancho Palos Verdes";
   Args.TimeFormat = "%l:%M %P";
   Args.DateFormat = "%a, %B %e";
   Args.ForecastApiResponse = OwmForecastTestResponse;
   Args.AirPollutionApiResponse = OwmAirPollutionTestResponse;
   Args.inTemp     = NAN;
   Args.inHumidity = NAN;
   Args.batteryVoltage = 2960;
   Args.Rssi = -59;
   Args.bMetric = true;
   Args.bHighRes = true;
   setenv("TZ", "PST8PDT", 1);
   tzset();

   while (!Serial);
   delay(250);

   LOG("Owm test\n");

   while (true) {
      LOG("Press a key to continue\n");
      while (!Serial.available());
      int incomingByte = Serial.read();
      Args.WindSpeed = Args.bMetric ? UNITS_SPEED_KILOMETERSPERHOUR : 
                     UNITS_SPEED_MILESPERHOUR;
      Args.DistanceType = Args.bMetric ? UNITS_DIST_KILOMETERS : UNITS_DIST_MILES;
      Args.PrecipType = Args.bMetric ? UNITS_DAILY_PRECIP_MILLIMETERS : 
                                       UNITS_DAILY_PRECIP_INCHES;

      Args.PrecipHrType = Args.bMetric ? UNITS_HOURLY_PRECIP_MILLIMETERS :
                                         UNITS_HOURLY_PRECIP_INCHES;
      Args.PressureType = Args.bMetric ? UNITS_PRES_MILLIBARS :
                                         UNITS_PRES_INCHESOFMERCURY;
      Args.bDisplayAlerts = Args.bMetric ? false : true;

      epaper.begin();
// Work around bug in Seeed TFT_eSPI library, fillScreen() doesn't handle
// rotation correctly.
      epaper.setRotation(0);
      epaper.fillScreen(TFT_WHITE);
      epaper.setRotation(1);
      LOG("Updating display in %s mode.\n",
          Args.bMetric ? "metric" : "english");
      class DrawOWM *owm = new DrawOWM(epaper,Args);
      owm->DrawIt();
      delete owm;
      epaper.update(); // update the display
      Args.bMetric = !Args.bMetric;
   }
}

void loop()
{
    // put your main code here, to run repeatedly:
}
