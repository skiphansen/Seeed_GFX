#include <Arduino.h>
#include <ArduinoJson.h>

#include "TFT_eSPI.h"
#include "config.h"
#include <DrawOWM.h>

#define ENABLE_LOGGING  1
#if ENABLE_LOGGING && __has_include("logging.h") 
#include "logging.h"
#else
#define LOG(format, ...)
#define LOG_RAW(format, ...)
#endif

owm_resp_onecall_t       owm_onecall;
owm_resp_air_pollution_t owm_air_pollution;

DrawOWM::DrawOWM(EPaper &epaper) : display(epaper)
{
}

void DrawOWM::DrawIt(const char *ForecastResponse,const char *Pollution)
{
   float inTemp     = NAN;
   float inHumidity = NAN;
   tm timeInfo = {};
   String dateStr = "date";

   deserializeOneCall(ForecastResponse,owm_onecall);
   deserializeAirQuality(Pollution,owm_air_pollution);
   drawCurrentConditions(owm_onecall.current, owm_onecall.daily[0],
                         owm_air_pollution, inTemp, inHumidity);
   drawOutlookGraph(owm_onecall.hourly, owm_onecall.daily, timeInfo);
   drawForecast(owm_onecall.daily, timeInfo);
   drawLocationDate(CITY_STRING, dateStr);
#if DISPLAY_ALERTS
   drawAlerts(owm_onecall.alerts, CITY_STRING, dateStr);
#endif
//   drawStatusBar(statusStr, refreshTimeStr, wifiRSSI, batteryVoltage);
}


