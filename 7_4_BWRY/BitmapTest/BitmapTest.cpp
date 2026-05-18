 /*
 * Supported Colors:
 * - TFT_WHITE  (White)
 * - TFT_BLACK  (Black)
 * - TFT_RED    (Red)
 * - TFT_YELLOW (Yellow)
 */

#include <Arduino.h>
#include "driver.h"
#include "TFT_eSPI.h"
#include "logging.h"

#ifdef E2741QS0B3
#include "741E5SE_testimage.h"
#else
#include "image.h"
#endif


#ifdef EPAPER_ENABLE
EPaper epaper;
#endif



void setup()
{
   unsigned long StartTime;
   unsigned long DeltaTime;

#ifdef EPAPER_ENABLE
  Serial.begin(115200);
  while(!Serial);
  delay(250);
#ifdef E2741QS0B3
  LOG("7.4\" BWRY E-Paper Bitmap Display Example\n");
#else
  LOG("2.9\" BWRY E-Paper Bitmap Display Example\n");
#endif
  LOG("Press a key to continue");
  while(!Serial.available());
  int incomingByte = Serial.read();

  LOG("TFT_SCLK %d, TFT_MISO %d, TFT_MOSI %d, TFT_CS %d, TFT_DC %d, TFT_BUSY %d, TFT_RST %d\n",
      TFT_SCLK,TFT_MISO,TFT_MOSI,TFT_CS,TFT_DC,TFT_BUSY,TFT_RST);
  
  StartTime =  millis();
  epaper.begin();
  
  // Display 4-color bitmap image using pushImage API
  // pushImage(x, y, width, height, image_data)
#ifdef E2741QS0B3
  epaper.pushImage(0, 0, 480, 800, (uint16_t *)testImage);
#else
  epaper.pushImage(0, 0, 128, 296, (uint16_t *)gImage_2inch9_BWRY);
#endif
  epaper.update();
  
  DeltaTime = millis() - StartTime;

  LOG("4-color bitmap displayed successfully in %d.%03d seconds\n",
      DeltaTime/1000, DeltaTime % 1000);
  delay(1000);
  Serial.println("Clearing screen");
  
  // Clear screen to white
  StartTime =  millis();
  epaper.fillScreen(TFT_WHITE);
  epaper.update();
  DeltaTime = millis() - StartTime;
  LOG("Screen clear took %d.%03d seconds\n",DeltaTime/1000, DeltaTime % 1000);
    
  // Put display to sleep to save power
  epaper.sleep();
#else
  Serial.begin(115200);
  Serial.println("EPAPER_ENABLE not defined. Please select the correct setup file.");
#endif
}

void loop()
{
  // Nothing to do here
}
