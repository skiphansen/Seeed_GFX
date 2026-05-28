#ifndef _DRAW_OWM_H_
#define _DRAW_OWM_H_

#include <TFT_eSPI.h>
#include "api_response.h"
#include "config.h"

// for compatibility with Seeed_GFX
#ifdef EPAPER_ENABLE
   #define _THE_DISPLAY_CLASS EPaper
#else
   #define _THE_DISPLAY_CLASS TFT_eSprite
#endif
typedef enum alignment
{
  LEFT,
  RIGHT,
  CENTER
} alignment_t;

// UNITS - WIND SPEED
//   Metric   : Kilometers per Hour
//   Imperial : Miles per Hour
typedef enum {
   UNITS_SPEED_METERSPERSECOND,
   UNITS_SPEED_FEETPERSECOND,
   UNITS_SPEED_KILOMETERSPERHOUR,
   UNITS_SPEED_MILESPERHOUR,
   UNITS_SPEED_KNOTS,
   UNITS_SPEED_BEAUFORT
} ConfigWindSpeed_t;

// UNITS - VISIBILITY DISTANCE
//   Metric   : Kilometers
//   Imperial : Miles
typedef enum {
   UNITS_DIST_MILES,
   UNITS_DIST_KILOMETERS
} ConfigDistance_t;

// UNITS - PRECIPITATION (DAILY)
// Measure of precipitation.
// This can either be Probability of Precipitation (PoP) or daily volume.
//   Metric   : Millimeters
//   Imperial : Inches
typedef enum {
   UNITS_DAILY_PRECIP_POP,
   UNITS_DAILY_PRECIP_MILLIMETERS,
   UNITS_DAILY_PRECIP_CENTIMETERS,
   UNITS_DAILY_PRECIP_INCHES
} ConfigPrecip_t;

// UNITS - PRECIPITATION (HOURLY)
// Measure of precipitation.
// This can either be Probability of Precipitation (PoP) or hourly volume.
//   Metric   : Millimeters
//   Imperial : Inches
typedef enum {
   UNITS_HOURLY_PRECIP_POP,
   UNITS_HOURLY_PRECIP_MILLIMETERS,
   UNITS_HOURLY_PRECIP_CENTIMETERS,
   UNITS_HOURLY_PRECIP_INCHES
} ConfigPrecipHr_t;

// UNITS - PRESSURE
//   Metric   : Millibars
//   Imperial : Inches of Mercury
typedef enum {
   UNITS_PRES_HECTOPASCALS,
   UNITS_PRES_PASCALS,
   UNITS_PRES_MILLIMETERSOFMERCURY,
   UNITS_PRES_INCHESOFMERCURY,
   UNITS_PRES_MILLIBARS,
   UNITS_PRES_ATMOSPHERES,
   UNITS_PRES_GRAMSPERSQUARECENTIMETER,
   UNITS_PRES_POUNDSPERSQUAREINCH
} ConfigPressure_t;


typedef struct {
   const char *City;
   const char *TimeFormat;
   const char *DateFormat;
   const char *ForecastApiResponse;
   const char *AirPollutionApiResponse;
// bMetric false: Fahrenheit / MPH / inches
// bMetric true: Celcius / Beaufort / millimeters
   bool bHighRes;  // true for 800 x 640, false for 640 x 384 
   bool bMetric;
   ConfigWindSpeed_t WindSpeed;
   ConfigDistance_t DistanceType;
   ConfigPrecip_t PrecipType;
   ConfigPrecipHr_t PrecipHrType;
   ConfigPressure_t PressureType;

// ALERTS
//   The handling of alerts is complex. Each country has a unique national alert
//   system that receives alerts from many different government agencies. This
//   results is huge variance in the formatting of alerts. OpenWeatherMap
//   provides alerts in English only. Any combination of these factors may make
//   it undesirable to display alerts in some regions.
//   Disable alerts by changing the DISPLAY_ALERTS macro to 0.
   bool bDisplayAlerts;

// WIDGET POSITIONS
// Set the order of current condition you want to display
// in the following order
//  0   1
//  2   3
//  4   5
//  6   7
//  8   9
// if a 640 x 384 display is used, then positions 6,7,8,9 are not available
   int8_t PosSunrise;
   int8_t PosSunset;
   int8_t PosWind;
   int8_t PosHumidity;
   int8_t PosUvi;
   int8_t PosPressure;
   int8_t PosAirQuality;
   int8_t PosVisibility;
   int8_t PosIntemp;
   int8_t PosInhumidity;
   int8_t PosMoonrise;
   int8_t PosMoonset;
   int8_t PosMoonphase;
   int8_t PosDewpoint;
   uint16_t DisplayWidth;
   uint16_t DisplayHeight;

   float inTemp;
   float inHumidity;
   uint16_t batteryVoltage;
   int Rssi;
} OwmArgs;

class DrawOWM {
public:
   DrawOWM(_THE_DISPLAY_CLASS &spr,OwmArgs &Args);
   void DrawIt();

private:
      _THE_DISPLAY_CLASS &display;
      OwmArgs &config;

   // too large to allocate locally on stack

      uint16_t getStringWidth(const String &text);
      uint16_t getStringHeight(const String &text);
      void drawString(int16_t x, int16_t y, const String &text,
                      alignment_t alignment,uint16_t color=TFT_BLACK);
      void drawMultiLnString(int16_t x, int16_t y, const String &text,
                       alignment_t alignment, uint16_t max_width,
                       uint16_t max_lines, int16_t line_spacing,
                       uint16_t color=TFT_BLACK);
      void initDisplay();
      void powerOffDisplay();
      void drawCurrentSunrise(const owm_current_t &current);
      void drawCurrentWind(const owm_current_t &current);
      void drawCurrentUVI(const owm_current_t &current);
      void drawCurrentAirQuality(const owm_resp_air_pollution_t &owm_air_pollution);
      void drawCurrentInTemp(float inTemp);
      void drawCurrentSunset(const owm_current_t &current);
      void drawCurrentHumidity(const owm_current_t &current);
      void drawCurrentPressure(const owm_current_t &current);
      void drawCurrentVisibility(const owm_current_t &current);
      void drawCurrentInHumidity(float inHumidity);
      void drawCurrentMoonrise(const owm_daily_t &today);
      void drawCurrentMoonset(const owm_daily_t &today);
      void drawCurrentMoonphase(const owm_daily_t &daily);
      void drawCurrentDewpoint(const owm_current_t &current);
      void drawCurrentConditions(const owm_current_t &current,
                                 const owm_daily_t &today,
                                 const owm_resp_air_pollution_t &owm_air_pollution,
                                 float inTemp, float inHumidity);
      void drawForecast(const owm_daily_t *daily, tm timeInfo);
      void drawAlerts(std::vector<owm_alerts_t> & alerts,const String &city,
                      const String &date);
      void drawLocationDate(const String &city, const String &date);
      void drawOutlookGraph(const owm_hourly_t *hourly,
                            const owm_daily_t *daily,tm timeInfo);
      void drawStatusBar(const String &statusStr, const String &refreshTimeStr,
                         int rssi, uint32_t batVoltage);
      void drawError(const uint8_t *bitmap_196x196,const String &errMsgLn1,
                     const String &errMsgLn2);
      void drawInvertedBitmap(int16_t x, int16_t y, const uint8_t bitmap[], 
                              int16_t w, int16_t h, uint16_t color);
      void getDateStr(String &s, tm *timeInfo);
      void getTextBounds(const String &str,int16_t x,int16_t y,int16_t *x1,
                         int16_t *y1,uint16_t *w,uint16_t *h);
      int kelvin_to_plot_y(float kelvin, int tempBoundMin, float yPxPerUnit,
                           int yBoundMin);

      owm_resp_onecall_t       owm_onecall;
      owm_resp_air_pollution_t owm_air_pollution;
};

#undef _THE_DISPLAY_CLASS
#endif   // _DRAW_OWM_H_
