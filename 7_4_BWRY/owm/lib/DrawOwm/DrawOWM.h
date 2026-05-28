#ifndef _DRAW_OWM_H_
#define _DRAW_OWM_H_

#include <TFT_eSPI.h>
#include "api_response.h"
#include "config.h"

#define DISP_WIDTH  800
#define DISP_HEIGHT 480

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
#ifdef POS_SUNRISE
      void drawCurrentSunrise(const owm_current_t &current);
#endif
#ifdef POS_WIND
      void drawCurrentWind(const owm_current_t &current);
#endif
#ifdef POS_UVI
      void drawCurrentUVI(const owm_current_t &current);
#endif
#ifdef POS_AIR_QULITY
      void drawCurrentAirQuality(const owm_resp_air_pollution_t &owm_air_pollution);
#endif
#ifdef POS_INTEMP
      void drawCurrentInTemp(float inTemp);
#endif
#ifdef POS_SUNSET
      void drawCurrentSunset(const owm_current_t &current);
#endif
#ifdef POS_HUMIDITY
      void drawCurrentHumidity(const owm_current_t &current);
#endif
#ifdef POS_PRESSURE
      void drawCurrentPressure(const owm_current_t &current);
#endif
#ifdef POS_VISIBILITY
      void drawCurrentVisibility(const owm_current_t &current);
#endif
#ifdef POS_INHUMIDITY
      void drawCurrentInHumidity(float inHumidity);
#endif
#ifdef POS_MOONRISE
      void drawCurrentMoonrise(const owm_daily_t &today);
#endif
#ifdef POS_MOONSET
      void drawCurrentMoonset(const owm_daily_t &today);
#endif
#ifdef POS_MOONPHASE
      void drawCurrentMoonphase(const owm_daily_t &daily);
#endif
#ifdef POS_DEWPOINT
      void drawCurrentDewpoint(const owm_current_t &current);
#endif
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
