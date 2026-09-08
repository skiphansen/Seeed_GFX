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
#include <PNGdec.h>
#include <LittleFS.h>

#include "driver.h"
#include "TFT_eSPI.h"

#define ENABLE_LOGGING  1
#if ENABLE_LOGGING && __has_include("logging.h") 
#include "logging.h"
#else
#define LOG(format, ...)
#define LOG_RAW(format, ...)
#endif

EPaper epaper;
PNG png; // statically allocate the PNG structure (about 50K of RAM)
// File handle for the open file
File pngFile;

void ClearScreen(void);
// ==========================================
// 1. MANDATORY FILE SYSTEM HELPER FUNCTIONS
// ==========================================
void * myOpen(const char *filename, int32_t *size) {
    pngFile = LittleFS.open(filename, "r");
    if (!pngFile) return NULL;
    *size = pngFile.size();
    return &pngFile;
}

void myClose(void *handle) {
}

int32_t myRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
    if (!pngFile) return 0;
    return pngFile.read(buffer, length);
}

int32_t mySeek(PNGFILE *handle, int32_t position) {
    if (!pngFile) return 0;
    return pngFile.seek(position);
}

// ==========================================
// 2. DISPLAY DRAWING CALLBACK
// ==========================================
// Change return type from 'void' to 'int'
int pngDrawCallback(PNGDRAW *pDraw) 
{
#if 0
    uint16_t usPixels[pDraw->iWidth]; 
    
    // Convert line data to RGB565
    png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
    
    // Push the line to the screen
    tft.pushImage(xOffset, yOffset + pDraw->y, pDraw->iWidth, 1, usPixels);
#endif
    
    return 1; // FIX: Return 1 to tell PNGdec to continue decoding the next line
}

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
      while (!Serial.available());
      while (Serial.available()) {
         c = Serial.read();
      }
      LOG("xkcd test\n");
#if 0
      ClearScreen();
      epaper.update(); // update the display
#endif
      do {
         int x, y, w, h, bpp;
         const char *Filename = "/airport_meeting.png";
         int err;

         // Open the PNG file from flash storage
         File file = LittleFS.open(Filename,"r");
         if(!file) {
             LOG("Failed to open %s\n",Filename);
             break;
         }

         err = png.open(Filename,myOpen,myClose,myRead,mySeek,pngDrawCallback);

         if(err != PNG_SUCCESS) {
            LOG("png.open of %s failed %d\n",Filename,err);
         }
         else {
             LOG("%s: %dx%d, %d bpp, color type: %d\n",Filename,png.getWidth(),
                 png.getHeight(),png.getBpp(),png.getPixelType());
         }

      } while(false);
   }
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
