/*
 Font generation:

 This sketch uses a ttf (TrueType Font) file that has been converted to a binary
 format in a byte array. See NotoSans_Bold.h tab of this sketch.

    https://en.wikipedia.org/wiki/TrueType

 The font used in this sketch is free to use and from Google:

    https://fonts.google.com/
 
 TTF font files can be VERY large, fortunately there are python and online tools that can
 be used to convert a ttf font to a new ttf font file with a subset of the characters
 (called "font subsetting") as needed by the sketch. For example maybe only 0-9 and : are
 needed to display the time. It’s important to check the licence a particular font before
 use and subsetting. Here is a simple subsetting online tool:

    https://products.aspose.app/font/generator/ttf-to-ttf

 To use this tool:
   1. Drag and drop file
   2. Copy and paste the "Font symbols" you want, here are ASCII characters 20 (space) and 33-126:
       !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
   3. Make sure you include the "space" character if you are going to use it (renders as a box otherwise)
   4. Click "Generate"
   5. Save the file and convert to an array

 There is a more sophisticated font subsetting tool here:

    https://www.fontsquirrel.com/tools/webfont-generator

 The font must be in ttf format, it can then be converted to a binary array using the tool here:
    
    https://notisrac.github.io/FileToCArray/

 To use this tool:
   1. Drag and drop the ttf file on "Browse..." button
   2. Untick box "static"
   3. Click "Convert"
   4. Click "Save as file" and move the header file to sketch folder
   5. Open the sketch in IDE
   6. Include the header file containing the array (NotoSans_Bold.h in this example)

 ttf font files can be very large, there are python and online tools that can
 be used to convert a ttf font to a new font file with a subset of the characters
 that will be used by the sketch. For example maybe only 0-9 and : are needed to
 display the time.

 Note that translating a ttf font character to a glyph that can be rendered on
 screen is a processor intensive operation. This means the rendering speed will
 be much lower than bitmap encoded fonts which just need to be copied to the
 display.

 TrueType font characters are scalable, so different font files are not needed to
 render different size characters on the TFT screen. This is very convenient, just
 use the setFontSize(font_size) function (see below).
*/

#include "TFT_eSPI.h"
#include "OpenFontRender.h"

#define ENABLE_LOGGING  1
#if ENABLE_LOGGING && __has_include("logging.h") 
#include "logging.h"
#else
#define LOG(format, ...)
#define LOG_RAW(format, ...)
#endif


// The font is referenced with the array name
#if 0
   #include "NotoSans_Bold.h"
   #define TTF_FONT NotoSans_Bold
#else
   #define TTF_FONT FreeSans_utf8
   #include "FreeSans-utf8.h"
#endif
OpenFontRender ofr;

TFT_eSPI tft;
EPaper epaper;

void setup() 
{
   int c = 0;

  Serial.begin(115200);
  while (!Serial);
  delay(250);

  while (!Serial.available());
  while (Serial.available()) {
     c = Serial.read();
  }

  LOG("Calling epaper.begin()\n");

  epaper.begin();
// Work around bug in Seeed TFT_eSPI library, fillScreen() doesn't handle
// rotation correctly.
  epaper.setRotation(0);
  epaper.fillScreen(TFT_WHITE);
  epaper.setRotation(1);

  ofr.setDrawer(epaper); // Link drawing object to epaper instance (so font will be rendered on TFT)
  ofr.setSerial(Serial);	  // Need to print render library message
  ofr.showFreeTypeVersion(); // print FreeType version
  ofr.setDebugLevel(OFR_DEBUG);
}

void loop() {
  // epaper.fillScreen(TFT_BLACK);
   FT_Error Err;
  
  ofr.setFontColor(TFT_BLACK,TFT_WHITE);

  do {
     // Load the font and check it can be read OK
     if(( Err = ofr.loadFont(TTF_FONT, sizeof(TTF_FONT)))) {
       LOG("Initialise error %d\n",Err);
       break;
     }

     // Set the cursor to top left
     int32_t y = 0;
     ofr.setCursor(0,y);

     // A neat feature is that line spacing can be tightend up (by a factor of 0.7 here)
     ofr.setLineSpaceRatio(0.7);

     for (uint16_t font_size = 5; font_size < 60; font_size += 5) {
        ofr.setCursor(0,y);
        ofr.setFontSize(font_size);
        LOG("Cursor @ %d,%d font_size %d\n",ofr.getCursorX(),ofr.getCursorY(),
            font_size);
       ofr.printf("Hello World\n");
       LOG("Cursor now @ %d,%d\n",ofr.getCursorX(),ofr.getCursorY());
       y += font_size;
     }

     // Unload font
     ofr.unloadFont();
     epaper.update(); // update the display

  } while(false);
  while(1)  delay(5000);
}
