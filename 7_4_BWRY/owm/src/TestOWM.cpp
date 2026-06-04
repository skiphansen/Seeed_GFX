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

void DrawBoundingBox(int16_t xOffset,int16_t yOffset,int16_t Width,int16_t Height);

EPaper epaper;

void setup() 
{
   OwmConfig Config;
   const char *FormatDesc = NULL;

   Serial.begin(115200);
// City name that will be shown in the top-right corner of the display.
   Config.City = "Rancho Palos Verdes";
   Config.TimeFormat = "%l:%M %P";
   Config.DateFormat = "%a, %B %e";
   Config.ForecastApiResponse = OwmForecastTestResponse;
   Config.AirPollutionApiResponse = OwmAirPollutionTestResponse;
   Config.inTemp     = NAN;
   Config.inHumidity = NAN;
   Config.batteryVoltage = 2960;
   Config.Rssi = -59;
   Config.bMetric = false;
   Config.DisplayFormat = FORMAT_400X300;
//   Config.DisplayFormat = FORMAT_640X384;
//   Config.DisplayFormat = FORMAT_800X480;

   setenv("TZ", "PST8PDT", 1);
   tzset();

   while (!Serial);
   delay(250);

   LOG("Owm test\n");

   while (true) {
      LOG("Press a key to continue\n");
      while (!Serial.available());
      while (Serial.available()) {
         Serial.read();
      }
      Config.WindSpeed = Config.bMetric ? UNITS_SPEED_KILOMETERSPERHOUR : 
                     UNITS_SPEED_MILESPERHOUR;
      Config.DistanceType = Config.bMetric ? UNITS_DIST_KILOMETERS : UNITS_DIST_MILES;
      Config.PrecipType = Config.bMetric ? UNITS_DAILY_PRECIP_MILLIMETERS : 
                                       UNITS_DAILY_PRECIP_INCHES;

      Config.PrecipHrType = Config.bMetric ? UNITS_HOURLY_PRECIP_MILLIMETERS :
                                         UNITS_HOURLY_PRECIP_INCHES;
      Config.PressureType = Config.bMetric ? UNITS_PRES_MILLIBARS :
                                         UNITS_PRES_INCHESOFMERCURY;

      Config.bDisplayAlerts = Config.bMetric ? false : true;
      Config.bDisplayAlerts = Config.bMetric ? false : true;
      switch(Config.DisplayFormat) {
         case FORMAT_800X480:
            Config.DisplayWidth    = 800;
            Config.DisplayHeight   = 480;
            Config.PosSunrise      = 0;
            Config.PosSunset       = 1;
            Config.PosWind         = 2;
            Config.PosHumidity     = 3;
            Config.PosUvi          = 4;
            Config.PosPressure     = 5;
            Config.PosAirQuality   = 6;
            Config.PosVisibility   = 7;
            Config.PosIntemp       = 8;
            Config.PosInhumidity   = 9;
            Config.PosMoonrise     = -1;
            Config.PosMoonset      = -1;
            Config.PosMoonphase    = -1;
            Config.PosDewpoint     = -1;
            break;

         case FORMAT_640X384:
     // if a 640 x 384 display is used, then positions 6,7,8,9 are not available
            Config.DisplayWidth    = 640;
            Config.DisplayHeight   = 384;
            Config.PosSunrise      = 0;
            Config.PosSunset       = 1;
            Config.PosWind         = 2;
            Config.PosHumidity     = 3;
            Config.PosVisibility   = 4;
            Config.PosIntemp       = 5;
            Config.PosUvi          = -1;
            Config.PosPressure     = -1;
            Config.PosAirQuality   = -1;
            Config.PosInhumidity   = -1;
            Config.PosMoonrise     = -1;
            Config.PosMoonset      = -1;
            Config.PosMoonphase    = -1;
            Config.PosDewpoint     = -1;
            break;

         case FORMAT_400X300:
      // if a 400 x 300 display is used, then 12 positions available ???
            Config.DisplayWidth    = 400;
            Config.DisplayHeight   = 300;
            Config.PosSunrise      = 0;
            Config.PosSunset       = 1;
            Config.PosWind         = 2;
            Config.PosHumidity     = 3;
            Config.PosUvi          = 4;
            Config.PosPressure     = 5;
            Config.PosAirQuality   = 6;
            Config.PosVisibility   = 7;
            Config.PosIntemp       = 8;
            Config.PosInhumidity   = 9;
            Config.PosMoonrise     = -1;
            Config.PosMoonset      = -1;
            Config.PosMoonphase    = -1;
            Config.PosDewpoint     = -1;
            break;
      }

      epaper.begin();
// Work around bug in Seeed TFT_eSPI library, fillScreen() doesn't handle
// rotation correctly.
      epaper.setRotation(0);
      epaper.fillScreen(TFT_WHITE);
      epaper.setRotation(1);

      int16_t xOffset = 0;
      int16_t yOffset = 0;
      switch(Config.DisplayFormat) {
         case FORMAT_800X480:
            FormatDesc = "800 x 480 ";
            break;

         case FORMAT_640X384:
            FormatDesc = "640 x 384";
            break;

         case FORMAT_400X300:
            FormatDesc = "400 x 300";
            break;
      }
      xOffset = (800 - Config.DisplayWidth) / 2;
      yOffset = (480 - Config.DisplayHeight) / 2;
       
      LOG("Width %d Height %d xOffset %d yOffset %d\n",
          Config.DisplayWidth,Config.DisplayHeight,xOffset,yOffset);
      Config.xOffset = xOffset;
      Config.yOffset = yOffset;

      if (Config.DisplayWidth != 800) {
         DrawBoundingBox(xOffset,yOffset,Config.DisplayWidth,Config.DisplayHeight);
      }

      LOG("Updating %s res display in %s mode.\n",
          FormatDesc,Config.bMetric ? "metric" : "english");

      class DrawOWM *owm = new DrawOWM(epaper,Config);
      owm->DrawIt();
      delete owm;
      epaper.update(); // update the display
  // setup Config for next screen

      switch(Config.DisplayFormat) {
         case FORMAT_400X300:
            Config.DisplayFormat = FORMAT_640X384;
            break;

         case FORMAT_640X384:
            Config.DisplayFormat = FORMAT_800X480;
            break;

         case FORMAT_800X480:
            Config.DisplayFormat = FORMAT_400X300;
            Config.bMetric = !Config.bMetric;
            break;
      }
   }
}

void loop()
{
    // put your main code here, to run repeatedly:
}

// Draw bounding box
void DrawBoundingBox(int16_t xOffset,int16_t yOffset,int16_t Width,int16_t Height)
{
   // left edge
   epaper.drawLine(xOffset - 1,yOffset - 1,
                   xOffset - 1,yOffset + Height - 1,TFT_BLACK);  
   // right edge
   epaper.drawLine(xOffset + Width + 1,yOffset - 1,
                   xOffset + Width + 1,yOffset + Height - 1,TFT_BLACK);  
   // bottom
   epaper.drawLine(xOffset - 1,yOffset + Height - 1,
                   xOffset + Width + 1,yOffset + Height - 1,TFT_BLACK); 
   // top 
   epaper.drawLine(xOffset - 1,yOffset - 1,
                   xOffset + Width + 1,yOffset - 1,TFT_BLACK); 
}
