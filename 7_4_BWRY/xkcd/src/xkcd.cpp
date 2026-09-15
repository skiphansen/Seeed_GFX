#include <Arduino.h>

#include <LittleFS.h>

#include "driver.h"
#include "TFT_eSPI.h"
#include "dither.h"

#define ENABLE_LOGGING  1
#if ENABLE_LOGGING && __has_include("logging.h") 
#include "logging.h"
#else
#define LOG(format, ...)
#define LOG_RAW(format, ...)
#endif

#include <PNGdec.h>

EPaper epaper;
PNG png; // statically allocate the PNG structure (about 50K of RAM)
// File handle for the open file
File pngFile;

void ClearScreen(void);
void CopyRaw2Epaper(uint8_t *p0,uint8_t *p1,int w,int h,int x_off = 0,int y_offset = 0);
void DrawTestPattern(int w,int h,int x_off = 0,int y_offset = 0);
uint32_t Value2Color(int Value);
void RotateSprite(TFT_eSprite &spr,TFT_eSprite &spr1);

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
   TFT_eSPI *pSPR = static_cast<TFT_eSPI *>(pDraw->pUser);
   int y = pDraw->y;
   int iWidth = pDraw->iWidth;

    uint16_t usPixels[iWidth]; 
    
    // Convert line data to RGB565
    if(y == 0) {
       LOG("y %d w %d iPitch %d iPixelType %d bpp %d\n",
           y,iWidth,pDraw->iPitch,pDraw->iPixelType,pDraw->iBpp);
    }
    png.getLineAsRGB565(pDraw,usPixels,PNG_RGB565_LITTLE_ENDIAN,0xffffffff);
#if 0
    if(y < 2) {
       LOG("After getLineAsRGB565\n");
       Color color;
       for(int i = 0; i < iWidth;i++) {
          color = Color(usPixels[i]);
          if((i % 8) == 0) {
             LOG_RAW("\n%d,%d: ",i,y);
          //   DUMP_HEX(&usPixels[i],16);
          }
//          LOG_RAW("%d:%d:%d, ",color.r,color.g,color.b);
          LOG_RAW("0x%02x, ",usPixels[i]);
       }
       LOG_RAW("\n");
    }
#endif

    for(int i = 0; i < iWidth; i++) {
       pSPR->drawPixel(i,y,usPixels[i]);
    }

#if 0
    if(y == 0) {
       LOG("Readback\n");
       Color color;
       for(int i = 0; i < iWidth;i++) {
          color = pSPR->readPixel(i,y);
          if((i % 8) == 0) {
             LOG_RAW("\n%d: ",i);
          }
          LOG_RAW("%d:%d:%d, ",color.r,color.g,color.b);
       }
       LOG_RAW("\n");
    }
#endif
    
    return 1;
}

void setup() 
{
   Serial.begin(115200);
   LittleFS.begin();

   setenv("TZ", "PST8PDT", 1);
   tzset();

   while (!Serial);
   delay(250);

   while (true) {
      int c = 0;
      while (!Serial.available());
      while (Serial.available()) {
         c = Serial.read();
      }
      LOG("xkcd test\n");
      ClearScreen();
      TFT_eSPI *tft = NULL;
      TFT_eSPI *tft1 = NULL;
      uint8_t *Plane0 = NULL;
      uint8_t *Plane1 = NULL;
      int PngHeight;
      int PngWidth;
      int DisplayHeight;
      int DisplayWidth;

      do {
//#define FILENAME "/airport_meeting.png";
#define FILENAME "/color_test_1.png";
       const char *Filename = FILENAME;
         int err;

         err = png.open(Filename,myOpen,myClose,myRead,mySeek,pngDrawCallback);

         if(err != PNG_SUCCESS) {
            LOG("png.open of %s failed %d\n",Filename,err);
            break;
         }
         PngHeight = png.getHeight();
         PngWidth = png.getWidth();
         LOG("%s: %dx%d, %d bpp, color type: %d\n",
             Filename,PngWidth,PngHeight,png.getBpp(),png.getPixelType());
         DisplayWidth = epaper.width();
         DisplayHeight = epaper.height();
      // Create an SPR to receive the image
         tft = new TFT_eSPI();
         TFT_eSprite spr = TFT_eSprite(tft);
         spr.setColorDepth(16);
         LOG("Create %dx%d spr\n",DisplayWidth,DisplayHeight);
         spr.createSprite(DisplayWidth,DisplayHeight);

         if(spr.getPointer() == nullptr) {
            ELOG("Failed to create sprite\n");
            break;
         }
         spr.fillScreen(TFT_WHITE);

      // Decode PNG file into SPR
         err = png.decode(&spr,0);
         if(err != PNG_SUCCESS) {
            LOG("png failed %d\n",err);
         }
         png.close();

      // convert 8bbp SPR into bit planes for display
         struct imgParam imageParams;
   // from working AP's log:
   // drawForecast: dither 2 bufferbpp 8 rotate 0 rotatebuffer 3 bpp 2 invert 0
         imageParams.bufferbpp = 8;
         imageParams.rotate = 0;
         imageParams.rotatebuffer = 0;
         imageParams.bpp = 2;
         imageParams.invert = 0;

         imageParams.dither = 0; // set dynamically eventually
         imageParams.hasRed = false;   // set by spr2color()
/* 
   from A3.json: 
         "colortable": {
                 "white": [ 255, 255, 255 ],
                 "black": [ 0, 0, 0 ],
                 "red": [ 255, 0, 0 ],
                 "yellow": [ 255, 255, 0 ]
         },
*/
         imageParams.hwdata.colortable.push_back(Color(255,255,255));   // white
         imageParams.hwdata.colortable.push_back(Color(0,0,0));         // black
         imageParams.hwdata.colortable.push_back(Color(255,0,0));       // red
         imageParams.hwdata.colortable.push_back(Color(255,255,0));     // yellow

         size_t buffer_size = (DisplayWidth * DisplayHeight) /8;

         Plane0 = (uint8_t *) malloc(buffer_size);
         if(Plane0 == NULL) {
            ELOG("Malloc of %d bytes failed\n",buffer_size);
            break;
         }
         Plane1 = (uint8_t *) malloc(buffer_size);
         if(Plane1 == NULL) {
            ELOG("Malloc of %d bytes failed\n",buffer_size);
            break;
         }
         spr2color(spr,imageParams,Plane0,buffer_size,false);
         spr2color(spr,imageParams,Plane1,buffer_size,true);

#if 0
         LOG("Plane0:\n",Plane0);
         DUMP_HEX(Plane0,800/4);

         LOG("Plane1:\n",Plane0);
         DUMP_HEX(Plane1,800/4);
#endif

#if 1
       CopyRaw2Epaper(Plane0,Plane1,DisplayWidth,DisplayHeight);
#else
       DrawTestPattern(DisplayWidth,DisplayHeight);
#endif
         epaper.update(); // update the display
      } while(false);

      if(tft != NULL) {
         delete tft;
      }

      if(tft1 != NULL) {
         delete tft1;
      }

      if(Plane0 != NULL) {
         free(Plane0 );
      }
      if(Plane1!= NULL) {
         free(Plane1);
      }
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

void DumpHex(void *AdrIn,int Len)
{
   unsigned char *Adr = (unsigned char *) AdrIn;
   int i = 0;
   int j;

   while(i < Len) {
      for(j = 0; j < 16; j++) {
         if((i + j) == Len) {
            break;
         }
         LOG_RAW("%02x ",Adr[i+j]);
      }

      LOG_RAW(" ");
      for(j = 0; j < 16; j++) {
         if((i + j) == Len) {
            break;
         }
         if(isprint(Adr[i+j])) {
            LOG_RAW("%c",Adr[i+j]);
         }
         else {
            LOG_RAW(".");
         }
      }
      i += 16;
      LOG_RAW("\n");
   }
}


// Scaler for 16-bit RGB565 images
void scaleImageRGB565(const uint16_t* src, int srcW, int srcH, 
                      uint16_t* dst, int dstW, int dstH) {
    // Calculate scale factors
    uint32_t x_ratio = ((srcW << 16) / dstW) + 1;
    uint32_t y_ratio = ((srcH << 16) / dstH) + 1;

    for (int y = 0; y < dstH; y++) {
        uint32_t srcY = (y * y_ratio) >> 16;
        if (srcY >= srcH) srcY = srcH - 1; // Bound check
        
        const uint16_t* srcRow = src + (srcY * srcW);
        uint16_t* dstRow = dst + (y * dstW);

        for (int x = 0; x < dstW; x++) {
            uint32_t srcX = (x * x_ratio) >> 16;
            if (srcX >= srcW) srcX = srcW - 1; // Bound check
            
            dstRow[x] = srcRow[srcX];
        }
    }
}

uint32_t Value2Color(int Value) 
{
   uint32_t Color = TFT_WHITE;

// 0x0 = black
// 0x1 = white
// 0x2 = red
// 0x3 = yellow

   switch (Value) {
      case 0:
         Color = TFT_WHITE;
         break;

      case 1:
         Color = TFT_BLACK;
         break;

      case 3:
         Color = TFT_YELLOW;
         break;

      case 2:
         Color = TFT_RED;
         break;

      default:
         ELOG("Value = %d, WTF ?\n",Value);
   }

   return Color;
}

/*
   White = 255:255:255 -> 0x0
   Black = 0:0:0       -> 0x1
   Red = 255:0:0       -> 0x2
   Yellow = 255:255:0  -> 0x3
 
lsb first 
 
 
*/
void CopyRaw2Epaper(uint8_t *p0,uint8_t *p1,int w,int h,int x_off,int y_offset)
{  
#if 0
   LOG("%dx%d x_off %d y_offset %d\n",w,h,x_off,y_offset);
//   for(int y = 0; y < h; y++) {
   LOG("Plane 0 line 0:\n");
   DUMP_HEX(p0,800/4);
   LOG("Plane 0 line 1:\n");
   DUMP_HEX(&p0[800/4],800/4);

   LOG("Plane 1 line 0:\n");
   DUMP_HEX(&p1,800/4);
   LOG("Plane 1 line 1:\n");
   DUMP_HEX(&p1[800/4],800/4);
#endif

   uint8_t Value0;
   uint8_t Value1;
   uint8_t Value;
   uint8_t InShift = 8;
   for(int y = 0; y < h; y++) {
#if 0
      if(y == 1) {
         LOG("In loop Plane 0, line 1:\n");
         DUMP_HEX(p0,800/4);
         LOG("In loop Plane 1, line 1:\n");
         DUMP_HEX(p1,800/4);
      }
#endif
      for(int x = 0; x < w; x++) {
         if(InShift == 8) {
            InShift = 0;
            Value0 = *p0++;
            Value1 = *p1++;
         }
         Value = 0;
         if(Value0 & (0x80 >> InShift)) {
            Value = 1;
         }
         if(Value1 & (0x80 >> InShift)) {
            Value |= 2;
         }
#if 0
         if(y < 2) {
            LOG_RAW("%d,%d: %d InShift %d\n",x,y,Value,InShift);
         }
#endif
         InShift++;
         epaper.drawPixel(x + x_off,y + y_offset,Value2Color(Value));
         // epaper.drawPixel(y + y_offset,x + x_off,Color);
      }
   }
}


#define PATTERN_SIZE 64
void DrawTestPattern(int w,int h,int x_off,int y_offset)
{
   LOG("%dx%d x_off %d y_offset %d\n",w,h,x_off,y_offset);
   for(int y = 0; y < h; y++) {
      for(int x = 0; x < w; x++) {
         uint32_t Value = ((x / PATTERN_SIZE) + (y / PATTERN_SIZE)) & 0x3;
         epaper.drawPixel(x + x_off,y + y_offset,Value2Color(Value));
      }
   }
}

