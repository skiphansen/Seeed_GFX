#ifndef _DRAW_OWM_H_
#define _DRAW_OWM_H_

#include <TFT_eSPI.h>
#include "api_response.h"
#include "config.h"

#define DISP_WIDTH  800
#define DISP_HEIGHT 480

typedef enum alignment
{
  LEFT,
  RIGHT,
  CENTER
} alignment_t;

class DrawOWM {
public:
   DrawOWM(EPaper &spr);
   void DrawIt(const char *JsonDocument,const char *Pollution);

private:
      EPaper &display;
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

};
#endif   // _DRAW_OWM_H_
