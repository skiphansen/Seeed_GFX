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
#include <DrawOWM.h>

#include "driver.h"
#include "TFT_eSPI.h"

#include <FS.h>
using namespace fs;
#include <truetype.h>
#include "LittleFS.h"
#include "config.h"
#include "owm_response.h"
#include "TestData.h"

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
bool bTestMoonSupport;
bool bMetric;
bool bEmbedded;
int gRssi = -59;
uint16_t gBattV = 2960;

void DrawBoundingBox(int16_t xOffset,int16_t yOffset,int16_t Width,int16_t Height);
void drawInvertedBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
void OwmDrawTest(int c);
void TestTrueType(int c);
void ClearScreen(void);
void DrawBB(uint16_t x,uint16_t y,uint16_t Size,int color);

EPaper epaper;

void setup() 
{
   Serial.begin(115200);
   LittleFS.begin();

   setenv("TZ", "PST8PDT", 1);
   tzset();

   while (!Serial);
   delay(250);

   bool bDisplayMenu = true;
   while (true) {
      int c = 0;
      if(bDisplayMenu) {
         LOG_RAW(" 1: 400 x 300\n");
         LOG_RAW(" 2: 640 x 384\n");
         LOG_RAW(" 3: 800 x 480\n");
         LOG_RAW(" a: Arrow Icon test\n");
         LOG_RAW(" b: Battery Icon test\n");
         LOG_RAW(" f: Toggle between file based and embedded TrueType data\n");
         LOG_RAW(" l: Toggle language\n");
         LOG_RAW(" m: Toggle moon data testing\n");
         LOG_RAW(" o: owm_icons.ttf test\n");
         LOG_RAW(" r: change RSSI\n");
         LOG_RAW(" t: TrueType test\n");
         LOG_RAW(" T: 196 x 196 Icon test\n");
         LOG_RAW(" v: change Battery voltage\n");
         LOG_RAW(" w: Wifi icon test\n");
         LOG_RAW("Press a key to start a test\n");
      }
      bDisplayMenu = true;
      while (!Serial.available());
      while (Serial.available()) {
         c = Serial.read();
      }

      switch(c) {
         case '1':
         case '2':
         case '3':
            OwmDrawTest(c);
            break;

         case 'a':
         case 'A':
         case 'b':
         case 'o':
         case 't':
         case 'T':
         case 'w':
            TestTrueType(c);
            break;

         case 'v':
            switch(gBattV) {
               case 3100:
                  gBattV = 3025;
                  break;

               case 3025:
                  gBattV = 2950;
                  break;

               case 2950:
                  gBattV = 2850;
                  break;

               case 2850:
                  gBattV = 2400;
                  break;

               case 2400:
                  gBattV = 1500;
                  break;

               default:
                  gBattV = 3100;
                  break;
            }
            LOG_RAW("Battery V %d mv\n",gBattV);
            bDisplayMenu = false;
            continue;

         case 'f':
            bEmbedded = !bEmbedded;
            LOG_RAW("Using TrueType data from %s\n",bEmbedded ? "flash" : "file");
            bDisplayMenu = false;
            continue;

         case 'l':
            bMetric = !bMetric;
            LOG_RAW("%s selected\n",bMetric ? "German" : "English");
            bDisplayMenu = false;
            continue;

         case 'm':
            bTestMoonSupport = !bTestMoonSupport;
            LOG_RAW("%s moon support\n",bTestMoonSupport ? "Testing" : "Not testing");
            bDisplayMenu = false;
            continue;

         case 'r':
            if(gRssi == 0) {
               gRssi = -50;
            }
            else if(gRssi >= -70) {
               gRssi -= 10;
            }
            else {
               gRssi = 0;
            }
            bDisplayMenu = false;
            LOG_RAW("RSSI %d\n",gRssi);
            continue;

         default:
            LOG_RAW("Invalid option\n\n");
            bDisplayMenu = true;
            continue;
      }
      epaper.update(); // update the display
   }
}

void OwmDrawTest(int c)
{
   OwmConfig Config;
   JsonDocument doc;
   JsonObject languageObject;
   const char *FormatDesc = NULL;

// City name that will be shown in the top-right corner of the display.
   Config.City = "Rancho Palos Verdes";
   Config.TimeFormat = "%l:%M %P";
   Config.DateFormat = "%a, %B %e";
   Config.ForecastApiResponse = OwmForecastTestResponse;
   Config.AirPollutionApiResponse = OwmAirPollutionTestResponse;
   Config.inTemp     = NAN;
   Config.inHumidity = NAN;
   Config.batteryVoltage = gBattV;
   Config.Rssi = gRssi;
   Config.bMetric = bMetric; // also selects language
   Config.bLiPo = false;
   Config.DisplayFormat = FORMAT_400X300;
//   Config.DisplayFormat = FORMAT_640X384;
   Config.DisplayFormat = FORMAT_800X480;

   Config.bDisplayAlerts = false;
   switch (c) {
      case '1':
         Config.DisplayFormat = FORMAT_400X300;
         Config.bMetric = false;
         break;

      case '2':
         Config.DisplayFormat = FORMAT_640X384;
         Config.bMetric = false;
         Config.bDisplayAlerts = true;
         break;

      case '3':
         Config.DisplayFormat = FORMAT_800X480;
         Config.bDisplayAlerts = true;
         break;

      default:
         break;
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

// First 4 positions when not in moon mode
   Config.PosSunrise    = !bTestMoonSupport ? 0 : -1;
   Config.PosSunset     = !bTestMoonSupport ? 1 : -1;
   Config.PosWind       = !bTestMoonSupport ? 2 : -1;
   Config.PosHumidity   = !bTestMoonSupport ? 3 : -1;

// First 4 positions when in moon mode
   Config.PosMoonrise   = bTestMoonSupport ? 0 : -1;
   Config.PosMoonset    = bTestMoonSupport ? 1 : -1;
   Config.PosMoonphase  = bTestMoonSupport ? 2 : -1;
   Config.PosInhumidity = bTestMoonSupport ? 3 : -1;

   Config.PosUvi        = 4;
   Config.PosPressure   = 5;
   Config.PosAirQuality = 6;
   Config.PosVisibility = 7;
   Config.PosIntemp     = 8;
   Config.PosDewpoint   = 9;

   switch(Config.DisplayFormat) {
      case FORMAT_800X480:
         Config.DisplayWidth  = 800;
         Config.DisplayHeight = 480;
         break;

      case FORMAT_640X384:
      // if a 640 x 384 display is used, then positions 6,7,8,9 are not available
         Config.DisplayWidth  = 640;
         Config.DisplayHeight = 384;
         Config.PosVisibility = 4;  // display visibility instead of UVI
         Config.PosIntemp     = 5;  // display inside temp instead of pressure
         Config.PosUvi        = -1;
         Config.PosPressure   = -1;
         Config.PosAirQuality = -1;
         Config.PosDewpoint   = -1;
         break;

      case FORMAT_400X300:
         Config.DisplayWidth  = 400;
         Config.DisplayHeight = 300;
         break;
   }

   int16_t xOffset = 0;
   int16_t yOffset = 0;
   switch (Config.DisplayFormat) {
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

   ClearScreen();
   if (Config.DisplayWidth != 800) {
      DrawBoundingBox(xOffset,yOffset,Config.DisplayWidth,Config.DisplayHeight);
   }
   LOG("Updating %s res display.\n",FormatDesc);

   class DrawOWM *owm = new DrawOWM(epaper,Config);
   if (Config.bMetric) {
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
         if (error) {
            LOG("Failed to parse JSON:\n%s\n",error.c_str());
            break;
         }
         languageObject = doc[String(Language)];
         for (int i = 0; i < 7; i++) {
            Strings.LC_ABDAY[i] = languageObject["daysShort"][i].as<const char *>();
            Strings.LC_DAY[i] = languageObject["days"][i].as<const char *>();
         }
         for (int i = 0; i < 12; i++) {
            Strings.LC_MON[i] = languageObject["months"][i].as<const char *>();
         }
         while ((Name = p->Name) != NULL) {
            if (languageObject[Name]) {
               *p->Value = languageObject[Name].as<const char *>();
            }
            p++;
         }
         owm->SetLocale(&Strings);
      } while (false);
   }
   owm->DrawIt();
   delete owm;
}


void TestTrueType(int c)
{
   uint8_t Err;
   int16_t y = 0;
   int16_t x = 0;
   int16_t y1;
   int16_t x1;

   ClearScreen();
   int16_t height = epaper.width();
   int16_t width = epaper.height();
   truetypeClass truetype = truetypeClass();
   void *framebuffer = epaper.getPointer();
   truetype.setFramebuffer(width,height,epaper.getColorDepth(),
                           static_cast<uint8_t *>(framebuffer));

   File fontFile = LittleFS.open("/fonts/FreeSans-utf8.ttf","r");

   if((Err = truetype.setTtfFile(fontFile)) == 0) {
      LOG("setTtfFile returned %d\n",Err);
   }
   truetype.setCharacterSize(32);
   truetype.setCharacterSpacing(0,0);
   truetype.setTextColor(TFT_WHITE,TFT_BLACK);
   truetype.setTextRotation(90);
   truetype.setTextBoundary(0,height,width);
   truetype.textDraw(x,y,"TrueType Test");
   y += 40;

   const char* IconPath = NULL;
   if(!bEmbedded) switch(c) {
      case 'b':
      case 'a':
      case 'A':
      case 'o':
      case 'w':
         IconPath = "/fonts/owm_icons.ttf";
         break;

      case 't':
         IconPath = "/fonts/weathericons.ttf";
         break;
   }

   if(IconPath != NULL) {
      fontFile = LittleFS.open(IconPath,"r");
      if((Err = truetype.setTtfFile(fontFile)) == 0) {
         LOG("setTtfFile returned %d\n",Err);
      }
   }
   else {
      if(!truetype.setTtfPointer((uint8_t*)owm_icons,sizeof(owm_icons),1)) {
         LOG("setTtfPointer failed\n");
      }
   }

   wchar_t Icons[33];
   if(c == 't') {
      wchar_t Icon = 0xf000;
      int i = 0;

      LOG("TrueType font test\n");

      while(Icon <= 0xf0eb && y < (480 - 32)) {
         for(i = 0; i < 16; i++) {
            Icons[i] = Icon++;
         }
         Icons[i] = 0;
         truetype.textDraw(x,y,Icons);
         y += 32;
      }
      if(i > 0) {
         Icons[i] = 0;
         truetype.textDraw(x,y,Icons);
         y += 32;
      }
   }
   else if(c == 'T') {
      LOG("196 x 196 TrueType weather icon test\n");
      Icons[0] = 0xf00c;
      Icons[1] = 0;
      uint16_t Size = 196;
      uint16_t ScalledSize = (Size * 1000) / 1241;
      truetype.setCharacterSize(ScalledSize);
      truetype.textDraw(200,y,Icons);
      epaper.drawLine(200,y,200 + Size,y,TFT_RED); // top
      epaper.drawLine(200,32+Size,200 + Size,y + Size,TFT_RED); // bottom
      epaper.drawLine(200,32,200,y + Size,TFT_RED); // left
      epaper.drawLine(200 + Size,y,200 + Size, y + Size,TFT_RED);   // right
      LOG("Size %d, ScalledSize %d, getStringWidth %d\n",
          Size,ScalledSize,truetype.getStringWidth(Icons));
   }
   else if(c == 'b') {
      LOG_RAW("Battery Icon test\n");
      wchar_t BattIcons[] = {
      // battery_*
         0xebdc,0xebd9,0xebe0,0xebdd,0xebe2,0xebd4,0xebd2,0xe1a4, 
         0
      };
      wchar_t Temp[2] = {0,0};

   // natural rotation, just send all icons at once
      truetype.setTextRotation(90);
      truetype.setCharacterSize(64);
      truetype.textDraw(0,y,BattIcons);
      y += 65; // one pixel space between lines

   // natural rotation rotated by 180 degrees, x & y positions
   // must be adjusted for the rotation and x must be advanced
   // for each character
      truetype.setTextRotation(90 + 180);
      y1 = 479 - 64 - y;
      Temp[0] = BattIcons[0];
      x1 = 799 - truetype.getStringWidth(Temp);
   // not natural rotation, must icons one at a time 
      for(int i = 0; BattIcons[i] != 0; i++) {
         Temp[0] = BattIcons[i];
         truetype.textDraw(x1,y1,Temp);
         x1 -= truetype.getStringWidth(Temp);
      }
      y += 65; // one pixel space between lines

   // natural rotation rotated by 90 degrees, x & y positions
   // must be adjusted for the rotation and x must be advanced
   // for each character.  Note x & y are swapped 
      truetype.setTextRotation(90 + 90);
      Temp[0] = BattIcons[0];
      x1 = 799 - truetype.getStringWidth(Temp);
      truetype.setTextBoundary(0,width,height);
      for(int i = 0; BattIcons[i] != 0; i++) {
         Temp[0] = BattIcons[i];
         truetype.textDraw(y,x1,Temp);
         x1 -= truetype.getStringWidth(Temp);
      }
      y += 65; // one pixel space between lines

   // natural rotation rotated by 270 degrees, x & y positions
   // must be adjusted for the rotation and x must be advanced
   // for each character.  Note x & y are swapped 

      truetype.setTextRotation(0);
      Temp[0] = BattIcons[0];
      x1 = 0;
      y1 = 479 - 64 - y;
      truetype.setTextBoundary(0,width,height);
      for(int i = 0; BattIcons[i] != 0; i++) {
         Temp[0] = BattIcons[i];
         truetype.textDraw(y1,x1,Temp);
         x1 += truetype.getStringWidth(Temp);
      }
   }
   else if(c == 'a' || c == 'A') {
      LOG_RAW("Arrow Icon test, TTF data from %s\n",
              IconPath == NULL ? "flash" : "file");

      if(c == 'A') {
      // dump bitmaped version of wind_direction_meteorological_0deg_24x24
      // as ASCII
         int index  = 0;
         for(int i = 0; i < 24; i++) {
            for(int j = 0; j < 3; j++) {
               uint8_t Mask = 0x80;
               uint8_t Value = wind_direction_meteorological_0deg_24x24[index++];
               for(int k = 0; k < 8; k++) {
                  printf("%c",(Value & Mask) == 0 ? '*' : ' ');
                  Mask >>= 1;
               }
            }
            printf("| line %d\n",i + 1);
         }
      }
      for(int i = 0; i < WIND_VALUES; i++) {
         drawInvertedBitmap(x,y,wind_direction_icon_arr[i],24,24,TFT_BLACK);
         x += 24;
         if(x > 799) {
            y += 25;
            x = 0;
         }
      }
      x = 0;
      y += 30;
      wchar_t ArrowIcons[] = {
         0xf0fe,  // wind_direction_meteorological_0deg
         0xf105,  // wind_direction_meteorological_22_5deg
         0xf10b,  // wind_direction_meteorological_45deg
         0xf10c,  // wind_direction_meteorological_67_5deg
         0xf10d,  // wind_direction_meteorological_90deg
         0xf0ff,  // wind_direction_meteorological_112_5deg
         0xf100,  // wind_direction_meteorological_135deg
         0xf101,  // wind_direction_meteorological_157_5deg
         0xf102,  // wind_direction_meteorological_180deg
         0xf103,  // wind_direction_meteorological_202_5deg
         0xf104,  // wind_direction_meteorological_225deg
         0xf106,  // wind_direction_meteorological_247_5deg
         0xf107,  // wind_direction_meteorological_270deg
         0xf108,  // wind_direction_meteorological_292_5deg
         0xf109,  // wind_direction_meteorological_315deg
         0xf10a,  // wind_direction_meteorological_337_5deg
         0
      };
      wchar_t Temp[2] = {0,0};

      truetype.setCharacterSize(24);
      for(int i = 0; ArrowIcons[i] != 0; i++) {
         Temp[0] = ArrowIcons[i];
         truetype.textDraw(x,y,Temp);
         x += 24;
         if(x > 799) {
            y += 25;
            x = 0;
         }
      }
   }
   else if(c == 'o') {
      LOG_RAW("testing owm icons in %s\n",bEmbedded ? "flash" : "file");
      wchar_t OwmIcons[] = {
         0xebdc,  // battery_0_bar_90deg
         0xebd9,  // battery_1_bar_90deg
         0xebe0,  // battery_2_bar_90deg
         0xebdd,  // battery_3_bar_90deg
         0xebe2,  // battery_4_bar_90deg
         0xebd4,  // battery_5_bar_90deg
         0xebd2,  // battery_6_bar_90deg
         0xe1a4,  // battery_full_90deg
         0xe4ca,  // wifi_1_bar
         0xe4d9,  // wifi_2_bar
         0xebe1,  // wifi_3_bar
         0xe63e,  // wifi
         0xf0f0,  // wifi_x
         0xf0f1,  // air_filter
         0xf0fa,  // error_icon
         0xf0fb,  // house_humidity
         0xf0fc,  // house_thermometer
         0xf10e,  // biological_hazard_symbol
         0xf10f,  // ionizing_radiation_symbol
         0xf110,  // warning_icon
         0xe8f4,  // visibility
         0xf0fe,  // wind_direction_meteorological_0deg
         0xf0ff,  // wind_direction_meteorological_112_5deg
         0xf100,  // wind_direction_meteorological_135deg
         0xf101,  // wind_direction_meteorological_157_5deg
         0xf102,  // wind_direction_meteorological_180deg
         0xf103,  // wind_direction_meteorological_202_5deg
         0xf104,  // wind_direction_meteorological_225deg
         0xf105,  // wind_direction_meteorological_22_5deg
         0xf106,  // wind_direction_meteorological_247_5deg
         0xf107,  // wind_direction_meteorological_270deg
         0xf108,  // wind_direction_meteorological_292_5deg
         0xf109,  // wind_direction_meteorological_315deg
         0xf10a,  // wind_direction_meteorological_337_5deg
         0xf10b,  // wind_direction_meteorological_45deg
         0xf10c,  // wind_direction_meteorological_67_5deg
         0xf10d,  // wind_direction_meteorological_90deg
         0xf111,  // tide_down_arrow_water
         0xf112,  // tide_up_arrow_water
         0
      };
      wchar_t Temp[2] = {0,0};
      wchar_t CodePoint;

      uint16_t Size = 48;
      x = 0;
      truetype.setCharacterSize(Size);
      for(int i = 0; OwmIcons[i] != 0; i++) {
         CodePoint = OwmIcons[i];
         Temp[0] = CodePoint;
         if(CodePoint == 0xf0fe || CodePoint == 0xe4ca || CodePoint == 0xf111) {
         // new line
            y += Size + 4;
            x = 0;
         }
         DrawBB(x,y,Size,TFT_RED);
         x += 2;
         if(CodePoint == 0xf112) {
            truetype.bLogTTF = true;
            truetype.textDraw(x,y,Temp);
            truetype.bLogTTF = false;
         }
         else {
            truetype.textDraw(x,y,Temp);
         }
         x += Size + 2;
         if(x > (799 - Size)) {
            y += Size + 1;
            x = 0;
         }
      }
   }
   else if(c == 'w') {
      LOG_RAW("testing wifi icons\n");
      wchar_t OwmIcons[] = {
         0xe4ca,  // wifi_1_bar
         0xe4d9,  // wifi_2_bar
         0xebe1,  // wifi_3_bar
         0xe63e,  // wifi
         0xf0f0,  // wifi_x
         0
      };
      wchar_t Temp[2] = {0,0};
      wchar_t CodePoint;

      for(uint16_t Size = 16; Size < 128; Size += 16) {
         x = 0;
         truetype.setCharacterSize(Size);
         for(int i = 0; OwmIcons[i] != 0; i++) {
            CodePoint = OwmIcons[i];
            Temp[0] = CodePoint;
            DrawBB(x,y,Size,TFT_RED);
            x += 2;
            truetype.bLogTTF = true;
            truetype.textDraw(x,y,Temp);
            truetype.bLogTTF = false;
            x += Size + 2;
            if(x > (799 - Size)) {
            }
         }

         if(x !=  0) {
            y += Size + 1;
            x = 0;
         }
      }
   }

   truetype.end();
}

void ClearScreen()
{
// Work around bug in Seeed TFT_eSPI library, fillScreen() doesn't handle
// rotation correctly.
   epaper.begin();
   epaper.setRotation(0);
   epaper.fillScreen(TFT_WHITE);
   epaper.setRotation(1);
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

void drawInvertedBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) 
{
// taken from Adafruit_GFX.cpp, modified
   int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
   uint8_t byte = 0;
   for (int16_t j = 0; j < h; j++) {
      for (int16_t i = 0; i < w; i++) {
         if (i & 7) byte <<= 1;
         else {
            byte = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
         }
         if (!(byte & 0x80)) {
            epaper.drawPixel(x + i, y + j, color);
         }
      }
   }
}

void DrawBB(uint16_t x,uint16_t y,uint16_t Size,int color)
{
// top
   epaper.drawLine(x,y,x + Size + 2,y,color); 
// bottom
   epaper.drawLine(x,y+Size+2,x + Size + 2,y + Size + 2,color); 
// left
   epaper.drawLine(x,y,x,y + Size+2,color); 
   // right
   epaper.drawLine(x + Size + 2,y,x + Size + 2, y + Size + 2,color);   
}

