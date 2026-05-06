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


#ifdef E2741QS0B3
#define FUNCT_END()  0,0

// <num_bytes>
// 
// two possible formats
// if <num_bytes> == zero then the next bytes is the function to call
//    function 0:  end of list
//    function 1: FUNCT_WAIT_BUSY(x,y) were 
//                x wait for busy == x
//                y wait timeout in seconds
// otherwise <num_bytes> is the number of bytes to the EPD, the first byte is 
// sent as a command and the rest are sent as data
const uint8_t epd_E2741Q_init[] PROGMEM = {
   2,0xE6,0x19,
   2,0xE0,0x02,
   1,0xA5,
// (PWR): Power setting Register ?? JD79665AA shows 6 parameters
   2,0x01,0x07,
   3,0x00,  //  (PSR): Panel setting Register
   0x07, // 00 0 0 0 0 1 1 1 
   	   // 00 RES: - 800 x 600
   	   // 0 PST_MODE: Power switching time in the period of frame scanning.
   	   // 0 X
   	   // 0 Ud: 0xScan down
   	   // 1 SHL: shift left
   	   // 1 SHD_N: Booster on
   	   // 1 RST_N: not reset
   0xAB, // 1 0 1 0 1 0 1 1
   	   // 1 LUT_EN: Using LUT from register
   	   // 0 X
   	   // 1 FOPT: No scan after waveform finished and switch the source channel output to Hiz.
   	   // 0 VCMZ 0: VCOM status function: no effect
   	   // 1 TS_AUTO: When RST_N low to high,Temperature Sensor will be activated automatically one time. (
   	   // 0 TIEG: VGN power off status function 0 no effect
   	   // 1 NORG: After refreshing display, VCOM is tied to GND before power off
   	   // 1 VC_LUTZ: After refreshing display, the output of VCOM is set to floating automatically

   5,0x61,     // TRES
   0x01,0xE0,  // 0x1e0: 480
   0x03,0x20,  // 0x320: 800

   3,0x00,	   //PSB
   0x07,
   	//  00 RES: - 800 x 600
   	//  0 PST_MODE: Power switching time in the period of frame scanning.
   	//  0 X
   	//  0 Ud: 0xScan down
   	//  1 SHL: shift left
   	//  1 SHD_N: Booster on
   	//  1 RST_N: not reset

   0x2B,   // 0 0 1 0 1 0 1 1
   	// 0 LUT_EN: Using LUT from MTP
   	// 0 X
   	// 1 FOPT: No scan after waveform finished and switch the source channel output to Hiz.
   	// 0 VCMZ 0: VCOM status function: no effect
   	// 1 TS_AUTO: When RST_N low to high,Temperature Sensor will be activated automatically one time. (
   	// 0 TIEG: VGN power off status function 0 no effect
   	// 1 NORG: After refreshing display, VCOM is tied to GND before power off
   	// 1 VC_LUTZ: After refreshing display, the output of VCOM is set to floating automatically

   4,0x06,  // BTST
   0x40,0x40,0x40,

   4,0x03,   // ?? not defined for JD79665AA perhaps POFS (various JD devices
   0x00,0x00,0x00,

   2,0xE7,  // not defined for JD79665AA perhaps SPI2 enable (ST7789)
   0x3C,

   5,0x65,      //(GSST): Gate/Source Start Setting Register
   0x00,0x00,0x00,0x00,

   2,0x30,   // (PLL): PLL Control Register
   0x08,

   2,0x50,  //  (CDI): VCOM and DATA interval setting Register
   0x37,

   3,0x60,   // ?? not defined for JD79665AA perhaps TCON ? (various controllers
   0x03,0x03,

   2,0xE3,   // (PWS): Power Saving Register ??? JD79665AA shows 2 argments
   0x00,

   2,0xFF,   // ?? not defined for JD79665AA
   0xA5,

   9,0xEF,    // not defined for JD79665AA
   0x01,0x1E,0x06,0x0A,0x0F,0x19,0x0F,0x09,

   2,0xDC,  // ?? not defined for JD79665AA perhaps RDID3 ?
   0x01,

   2,0xDD,  // ?? not defined for JD79665AA 
   0x04,

   2,0xDE,  //?? not defined for JD79665AA  
   0x01,

   2,0xE8,  // ?? not defined for JD79665AA perhaps PWCTRL2 ?
   0x01,

   2,0xDA,  // ?? not defined for JD79665AA perhaps ILI9341_RDID1
   0x3F,

   2,0xFF,  // ?? not defined for JD79665AA
   0xE3,

   2,0xE9,  // ?? not defined for JD79665AA perhaps ST7789_EQCTRL
   0x01,

   FUNCT_END()
};

void RunSequence(const uint8_t *p)
{
   uint8_t Count;
   uint8_t bRun = 1;
   while(bRun) {
      if((Count = *p++) == 0) {
      // function
         switch(*p++) {
            case 0:  // FUNCT_END
               bRun = 0;
               break;
         }
      }
      else {
         epaper.writecommand(*p++);
         while(Count--) {
            epaper.writedata(*p++);
         }
      }
   }
}

void E2741Q_init()
{
   RunSequence(epd_E2741Q_init);
}

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
  
  StartTime =  millis();
  epaper.begin();

  LOG("TFT_SCLK %d, TFT_MISO %d, TFT_MOSI %d, TFT_CS %d, TFT_DC %d, TFT_BUSY %d, TFT_RST %d\n",
      TFT_SCLK,TFT_MISO,TFT_MOSI,TFT_CS,TFT_DC,TFT_BUSY,TFT_RST);
  
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
