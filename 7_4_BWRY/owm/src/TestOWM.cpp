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
#include <ArduinoJson.h>

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

extern const char LanguagesJson[];

typedef struct {
   const char *Name;
   const char **Value;
} LookupTbl_t;

LocaleStrings_t Strings;
const LookupTbl_t LookupTbl[] = {
#if 0
   {"days",LC_DAY},
   {"daysShort",LC_ABDAY},
   {"months",LC_MON},
   {"monthsShort",LC_ABMON},
#endif
   {"LC_D_T_FMT",&Strings.LC_D_T_FMT},
   {"LC_D_FMT",&Strings.LC_D_FMT},
   {"LC_T_FMT",&Strings.LC_T_FMT},
   {"LC_T_FMT_AMPM",&Strings.LC_T_FMT_AMPM},
   {"LC_AM_STR",&Strings.LC_AM_STR},
   {"LC_PM_STR",&Strings.LC_PM_STR},
   {"LC_ERA",&Strings.LC_ERA},
   {"LC_ERA_D_FMT",&Strings.LC_ERA_D_FMT},
   {"LC_ERA_D_T_FMT",&Strings.LC_ERA_D_T_FMT},
   {"LC_ERA_T_FMT",&Strings.LC_ERA_T_FMT},
   {"TXT_UNKNOWN",&Strings.TXT_UNKNOWN},
   {"TXT_FEELS_LIKE",&Strings.TXT_FEELS_LIKE},
   {"TXT_SUNRISE",&Strings.TXT_SUNRISE},
   {"TXT_SUNSET",&Strings.TXT_SUNSET},
   {"TXT_MOONRISE",&Strings.TXT_MOONRISE},
   {"TXT_MOONSET",&Strings.TXT_MOONSET},
   {"TXT_WIND",&Strings.TXT_WIND},
   {"TXT_HUMIDITY",&Strings.TXT_HUMIDITY},
   {"TXT_UV_INDEX",&Strings.TXT_UV_INDEX},
   {"TXT_PRESSURE",&Strings.TXT_PRESSURE},
   {"TXT_AIR_QUALITY",&Strings.TXT_AIR_QUALITY},
   {"TXT_AIR_POLLUTION",&Strings.TXT_AIR_POLLUTION},
   {"TXT_VISIBILITY",&Strings.TXT_VISIBILITY},
   {"TXT_INDOOR_TEMPERATURE",&Strings.TXT_INDOOR_TEMPERATURE},
   {"TXT_INDOOR_HUMIDITY",&Strings.TXT_INDOOR_HUMIDITY},
   {"TXT_DEWPOINT",&Strings.TXT_DEWPOINT},
   {"TXT_MOONPHASE",&Strings.TXT_MOONPHASE},
   {"TXT_NEW_MOON",&Strings.TXT_NEW_MOON},
   {"TXT_WAXING_CRESCENT",&Strings.TXT_WAXING_CRESCENT},
   {"TXT_FIRST_QUARTER",&Strings.TXT_FIRST_QUARTER},
   {"TXT_WAXING_GIBBOUS",&Strings.TXT_WAXING_GIBBOUS},
   {"TXT_FULL_MOON",&Strings.TXT_FULL_MOON},
   {"TXT_WANING_GIBBOUS",&Strings.TXT_WANING_GIBBOUS},
   {"TXT_THIRD_QUARTER",&Strings.TXT_THIRD_QUARTER},
   {"TXT_WANING_CRESCENT",&Strings.TXT_WANING_CRESCENT},
   {"TXT_UV_LOW",&Strings.TXT_UV_LOW},
   {"TXT_UV_MODERATE",&Strings.TXT_UV_MODERATE},
   {"TXT_UV_HIGH",&Strings.TXT_UV_HIGH},
   {"TXT_UV_VERY_HIGH",&Strings.TXT_UV_VERY_HIGH},
   {"TXT_UV_EXTREME",&Strings.TXT_UV_EXTREME},
   {"TXT_WIFI_EXCELLENT",&Strings.TXT_WIFI_EXCELLENT},
   {"TXT_WIFI_GOOD",&Strings.TXT_WIFI_GOOD},
   {"TXT_WIFI_FAIR",&Strings.TXT_WIFI_FAIR},
   {"TXT_WIFI_WEAK",&Strings.TXT_WIFI_WEAK},
   {"TXT_WIFI_NO_CONNECTION",&Strings.TXT_WIFI_NO_CONNECTION},
   {"TXT_UNITS_TEMP_KELVIN",&Strings.TXT_UNITS_TEMP_KELVIN},
   {"TXT_UNITS_TEMP_CELSIUS",&Strings.TXT_UNITS_TEMP_CELSIUS},
   {"TXT_UNITS_TEMP_FAHRENHEIT",&Strings.TXT_UNITS_TEMP_FAHRENHEIT},
   {"TXT_UNITS_SPEED_METERSPERSECOND",&Strings.TXT_UNITS_SPEED_METERSPERSECOND},
   {"TXT_UNITS_SPEED_FEETPERSECOND",&Strings.TXT_UNITS_SPEED_FEETPERSECOND},
   {"TXT_UNITS_SPEED_KILOMETERSPERHOUR",&Strings.TXT_UNITS_SPEED_KILOMETERSPERHOUR},
   {"TXT_UNITS_SPEED_MILESPERHOUR",&Strings.TXT_UNITS_SPEED_MILESPERHOUR},
   {"TXT_UNITS_SPEED_KNOTS",&Strings.TXT_UNITS_SPEED_KNOTS},
   {"TXT_UNITS_SPEED_BEAUFORT",&Strings.TXT_UNITS_SPEED_BEAUFORT},
   {"TXT_UNITS_PRES_HECTOPASCALS",&Strings.TXT_UNITS_PRES_HECTOPASCALS},
   {"TXT_UNITS_PRES_PASCALS",&Strings.TXT_UNITS_PRES_PASCALS},
   {"TXT_UNITS_PRES_MILLIMETERSOFMERCURY",&Strings.TXT_UNITS_PRES_MILLIMETERSOFMERCURY},
   {"TXT_UNITS_PRES_INCHESOFMERCURY",&Strings.TXT_UNITS_PRES_INCHESOFMERCURY},
   {"TXT_UNITS_PRES_MILLIBARS",&Strings.TXT_UNITS_PRES_MILLIBARS},
   {"TXT_UNITS_PRES_ATMOSPHERES",&Strings.TXT_UNITS_PRES_ATMOSPHERES},
   {"TXT_UNITS_PRES_GRAMSPERSQUARECENTIMETER",&Strings.TXT_UNITS_PRES_GRAMSPERSQUARECENTIMETER},
   {"TXT_UNITS_PRES_POUNDSPERSQUAREINCH",&Strings.TXT_UNITS_PRES_POUNDSPERSQUAREINCH},
   {"TXT_UNITS_DIST_KILOMETERS",&Strings.TXT_UNITS_DIST_KILOMETERS},
   {"TXT_UNITS_DIST_MILES",&Strings.TXT_UNITS_DIST_MILES},
   {"TXT_UNITS_PRECIP_MILLIMETERS",&Strings.TXT_UNITS_PRECIP_MILLIMETERS},
   {"TXT_UNITS_PRECIP_CENTIMETERS",&Strings.TXT_UNITS_PRECIP_CENTIMETERS},
   {"TXT_UNITS_PRECIP_INCHES",&Strings.TXT_UNITS_PRECIP_INCHES},
   {NULL}
};


void DrawBoundingBox(int16_t xOffset,int16_t yOffset,int16_t Width,int16_t Height);

EPaper epaper;

void setup() 
{
   OwmConfig Config;
   JsonDocument doc;
   JsonObject languageObject;
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
   Config.bMetric = true;
   Config.bLiPo = false;
   Config.DisplayFormat = FORMAT_400X300;
//   Config.DisplayFormat = FORMAT_640X384;
   Config.DisplayFormat = FORMAT_800X480;

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
      if(Config.bMetric) {
         JsonDocument filter;
         int Language = 2;
         const char *Name;
         const LookupTbl_t *p = LookupTbl;

         filter[String(Language)] = true;
         memset(&Strings,0,sizeof(Strings));

         do {
            DeserializationError error;
            error = deserializeJson(doc,LanguagesJson,
                                    DeserializationOption::Filter(filter));
            if(error) {
                LOG("Failed to parse JSON:\n%s\n",error.c_str());
                break;
            }
            languageObject = doc[String(Language)];
            for(int i = 0; i < 7; i++) {
               Strings.LC_ABDAY[i] = languageObject["daysShort"][i].as<const char *>();
               Strings.LC_DAY[i] = languageObject["days"][i].as<const char *>();
            }
            for(int i = 0; i < 12; i++) {
               Strings.LC_MON[i] = languageObject["months"][i].as<const char *>();
            }
            while((Name = p->Name) != NULL) {
               if(languageObject[Name]) {
                  *p->Value = languageObject[Name].as<const char *>();
               }
               p++;
            }
            owm->SetLocale(&Strings);
         } while(false);
      }
      owm->DrawIt();
      delete owm;
      epaper.update(); // update the display
  // setup Config for next screen
      Config.bMetric = !Config.bMetric;

      if(Config.bMetric) {
         switch(Config.DisplayFormat) {
            case FORMAT_400X300:
               Config.DisplayFormat = FORMAT_640X384;
               break;

            case FORMAT_640X384:
               Config.DisplayFormat = FORMAT_800X480;
               break;

            case FORMAT_800X480:
               Config.DisplayFormat = FORMAT_400X300;
               break;
         }
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
