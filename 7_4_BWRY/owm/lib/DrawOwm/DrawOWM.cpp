#include <Arduino.h>
#include <ArduinoJson.h>

#include "TFT_eSPI.h"
#include "config.h"
#include <DrawOWM.h>
#include "display_utils.h"

#define ENABLE_LOGGING  1
#if ENABLE_LOGGING && __has_include("logging.h") 
#include "logging.h"
#else
#define LOG(format, ...)
#define LOG_RAW(format, ...)
#endif

#ifdef SEEED_GFX
DrawOWM::DrawOWM(EPaper &epaper,OwmConfig &Config) : display(epaper), config(Config){}
#else
DrawOWM::DrawOWM(TFT_eSprite &epaper,OwmConfig &Config) : display(epaper), config(Config){}
#endif

void DrawOWM::DrawIt()
{
   tm timeInfo = {};
   String dateStr;
   String refreshTimeStr;
   time_t CurrentTime;
   String statusStr = {};

   deserializeOneCall(config.ForecastApiResponse,owm_onecall,config.bDisplayAlerts);
   CurrentTime = (time_t) owm_onecall.current.dt;
   localtime_r(&CurrentTime, &timeInfo);
   getRefreshTimeStr(refreshTimeStr,true,&timeInfo);
   getDateStr(dateStr, &timeInfo);
   deserializeAirQuality(config.AirPollutionApiResponse,owm_air_pollution);
   drawInit();
   drawCurrentConditions(owm_onecall.current, owm_onecall.daily[0],
                         owm_air_pollution, config.inTemp,config.inHumidity);
   drawOutlookGraph(owm_onecall.hourly, owm_onecall.daily, timeInfo);
   drawForecast(owm_onecall.daily, timeInfo);
   drawLocationDate(config.City,dateStr);
   if(config.bDisplayAlerts ) {
      drawAlerts(owm_onecall.alerts,config.City,dateStr);
   }
   drawStatusBar(statusStr,refreshTimeStr,config.Rssi,config.batteryVoltage);
}


