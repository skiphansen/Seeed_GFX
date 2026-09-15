// This code is cut and pasted from the OpenEPaperLink project:
// https://github.com/OpenEPaperLink/OpenEPaperLink/blob/master/ESP32_AP-Flasher/src/makeimage.cpp

#include <Arduino.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include "dither.h"

#define ENABLE_LOGGING  0
#if ENABLE_LOGGING && __has_include("logging.h") 
#include "logging.h"
#else
#define LOG(format, ...)
#define ELOG(format, ...)
#define LOG_RAW(format, ...)
#endif

struct Error {
    int32_t r;
    int32_t g;
    int32_t b;
};

uint32_t colorDistance(const Color &c1, const Color &c2, const Error &e1) {
    int32_t r_diff = c1.r + e1.r - c2.r;
    int32_t g_diff = c1.g + e1.g - c2.g;
    int32_t b_diff = c1.b + e1.b - c2.b;
    if (abs(c1.r - c1.g) < 20 && abs(c1.b - c1.g) < 20) {
        if (abs(c2.r - c2.g) > 20 || abs(c2.b - c2.g) > 20) return 4294967295;  // don't select color pixels on black and white
    }
    return 3 * r_diff * r_diff + 5.47 * g_diff * g_diff + 1.53 * b_diff * b_diff;
}

std::tuple<int, int, float, float> findClosestColors(const Color &pixel, const std::vector<Color> &palette) {
    int closestIndex = -1, secondClosestIndex = -1;
    float closestDist = std::numeric_limits<float>::max();
    float secondClosestDist = std::numeric_limits<float>::max();
    for (size_t i = 0; i < palette.size(); ++i) {
        float dist = colorDistance(pixel, palette[i], (Error){0, 0, 0});
        if (dist < closestDist) {
            secondClosestIndex = closestIndex;
            secondClosestDist = closestDist;
            closestIndex = i;
            closestDist = dist;
        } else if (dist < secondClosestDist) {
            secondClosestIndex = i;
            secondClosestDist = dist;
        }
    }
    if (closestIndex != -1 && secondClosestIndex != -1) {
        auto rgbValue = [](const Color &color) {
            return (color.r << 16) | (color.g << 8) | color.b;
        };

        if (rgbValue(palette[secondClosestIndex]) > rgbValue(palette[closestIndex])) {
            std::swap(closestIndex, secondClosestIndex);
            std::swap(closestDist, secondClosestDist);
        }
    }
    return { closestIndex, secondClosestIndex, closestDist, secondClosestDist};
}

void spr2color(TFT_eSprite &spr, imgParam &imageParams, uint8_t *buffer, size_t buffer_size, bool is_red) 
{
   uint8_t rotate = imageParams.rotate;
   long bufw = spr.width(), bufh = spr.height();

   if (imageParams.rotatebuffer % 2) {
      // turn the image 90 or 270
      rotate = (rotate + 3) % 4;
      rotate = (rotate + (imageParams.rotatebuffer - 1)) % 4;
      bufw = spr.height();
      bufh = spr.width();
   }
   else {
      // rotate 180
      rotate = (rotate + (imageParams.rotatebuffer)) % 4;
   }
   LOG("bufw %ld bufh %ld rotatebuffer %d\n",bufw,bufh,imageParams.rotatebuffer);

   memset(buffer, 0, buffer_size);

   std::vector<Color> palette = imageParams.hwdata.colortable;
   if (imageParams.invert == 1) {
      std::swap(palette[0], palette[1]);
   }
   Color color;
   int num_colors = palette.size();
   if (imageParams.bufferbpp == 1) num_colors = 2;
   Error *error_bufferold = new Error[bufw + 4];
   Error *error_buffernew = new Error[bufw + 4];

   size_t bitOffset = 0;

   memset(error_bufferold, 0, bufw * sizeof(Error));
   for (uint16_t y = 0; y < bufh; y++) {
      memset(error_buffernew, 0, bufw * sizeof(Error));
      for (uint16_t x = 0; x < bufw; x++) {
         uint16_t Value = 0;
         switch (rotate) {
         case 0:
            Value = spr.readPixel(x, y);
            break;
         case 1:
            Value = spr.readPixel(y, bufw - 1 - x);
            break;
         case 2:
            Value = spr.readPixel(bufw - 1 - x, bufh - 1 - y);
            break;
         case 3:
            Value = spr.readPixel(bufh - 1 - y, x);
            break;
         }
         color = Color(Value);

         int best_color_index = 0;
         if (imageParams.dither == 2) {
            // special ordered dithering
            auto [c1Index, c2Index, distC1, distC2] = findClosestColors(color, palette);
#if 0
            Color c1 = palette[c1Index];
            Color c2 = palette[c2Index];
#endif
            float weight = distC1 / (distC1 + distC2);
            if (weight <= 0.03) {
               best_color_index = c1Index;
            }
            else if (weight < 0.30) {
               best_color_index = ((y % 2 && ((y / 2 + x) % 2)) ? c2Index : c1Index);
            }
            else if (weight < 0.70) {
               best_color_index = ((x + y) % 2 ? c2Index : c1Index);
            }
            else if (weight < 0.97) {
               best_color_index = ((y % 2 && ((y / 2 + x) % 2)) % 2 ? c1Index : c2Index);
            }
            else {
               best_color_index = c2Index;
            }
         }

         if (imageParams.dither == 1 || imageParams.dither == 0) {
            uint32_t best_color_distance = colorDistance(color, palette[0], error_bufferold[x]);

            for (int i = 1; i < num_colors; i++) {
               if (best_color_distance == 0) break;
               uint32_t distance = colorDistance(color, palette[i], error_bufferold[x]);
               if (distance < best_color_distance) {
                  best_color_distance = distance;
                  best_color_index = i;
               }
            }
         }

         if (imageParams.bpp == BPP_PACKED_2BIT) {
         // Packed palette indices, four pixels per byte, MSB first. The
         // tagtype colortable order defines the on-wire colour codes.
            const size_t byteIndex = bitOffset / 8;
            const uint8_t bitIndex = bitOffset % 8;
            buffer[byteIndex] |= best_color_index << (6 - bitIndex);
            bitOffset += 2;
            if (best_color_index >= 2) imageParams.hasRed = true;
         }
         else if (imageParams.bpp == 3 || imageParams.bpp == 4) {
            size_t byteIndex = bitOffset / 8;
            uint8_t bitIndex = bitOffset % 8;

            if (bitIndex + imageParams.bpp <= 8) {
               buffer[byteIndex] |= best_color_index << (8 - bitIndex - imageParams.bpp);
            }
            else {
               uint8_t highPart = best_color_index >> (bitIndex + imageParams.bpp - 8);
               uint8_t lowPart = best_color_index & ((1 << (bitIndex + imageParams.bpp - 8)) - 1);
               buffer[byteIndex] |= highPart;
               buffer[byteIndex + 1] |= lowPart << (8 - (bitIndex + imageParams.bpp - 8));
            }
            bitOffset += imageParams.bpp;
         }
         else if (imageParams.bpp != 2) {
            ELOG("Invalid bbp %d\n",imageParams.bpp);
         }
         else {
         // bbp == 2
            uint8_t bitIndex = 7 - (x % 8);
            uint32_t byteIndex = (y * bufw + x) / 8;

            // this looks a bit ugly, but it's performing better than shorter notations
            switch (best_color_index) {
            case 1:
               if (!is_red)
                  buffer[byteIndex] |= (1 << bitIndex);
               break;
            case 2:
               imageParams.hasRed = true;
               if (is_red)
                  buffer[byteIndex] |= (1 << bitIndex);
               break;
            case 3:
               imageParams.hasRed = true;
               buffer[byteIndex] |= (1 << bitIndex);
               break;
            }
#if 0
            if(y == 0) {
               LOG("best_color_index %d bitIndex %d, data 0x%02x\n",
                   best_color_index,bitIndex,buffer[byteIndex]);
            }
#endif
            if(y < 2) {
   //            LOG_RAW("%d:%d:%d, ",color.r,color.g,color.b);
               LOG_RAW("%d,%d: %d/%d\n",x,y,Value,best_color_index);
            }
         }

         if (imageParams.dither == 1) {
            // Burkes Dithering

            Error error = {
               color.r + error_bufferold[x].r - palette[best_color_index].r,
               color.g + error_bufferold[x].g - palette[best_color_index].g,
               color.b + error_bufferold[x].b - palette[best_color_index].b};

            float scaling_factor = 255.0f / std::max(std::abs(error.r), std::max(std::abs(error.g), std::abs(error.b)));
            if (scaling_factor < 1.0f) {
               error.r *= scaling_factor;
               error.g *= scaling_factor;
               error.b *= scaling_factor;
            }

            error_buffernew[x].r += error.r / 4;
            error_buffernew[x].g += error.g / 4;
            error_buffernew[x].b += error.b / 4;

            if (x > 0) {
               error_buffernew[x - 1].r += error.r / 8;
               error_buffernew[x - 1].g += error.g / 8;
               error_buffernew[x - 1].b += error.b / 8;
            }

            if (x > 1) {
               error_buffernew[x - 2].r += error.r / 16;
               error_buffernew[x - 2].g += error.g / 16;
               error_buffernew[x - 2].b += error.b / 16;
            }

            error_buffernew[x + 1].r += error.r / 8;
            error_buffernew[x + 1].g += error.g / 8;
            error_buffernew[x + 1].b += error.b / 8;

            error_bufferold[x + 1].r += error.r / 4;
            error_bufferold[x + 1].g += error.g / 4;
            error_bufferold[x + 1].b += error.b / 4;

            error_buffernew[x + 2].r += error.r / 16;
            error_buffernew[x + 2].g += error.g / 16;
            error_buffernew[x + 2].b += error.b / 16;

            error_bufferold[x + 2].r += error.r / 8;
            error_bufferold[x + 2].g += error.g / 8;
            error_bufferold[x + 2].b += error.b / 8;
         }
      }
      memcpy(error_bufferold, error_buffernew, bufw * sizeof(Error));
   }

   delete[] error_buffernew;
   delete[] error_bufferold;

   return;
}

