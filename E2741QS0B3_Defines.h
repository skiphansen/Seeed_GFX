// PDI 7.4" BWRY EPD (E2741QS0B3)
#include "TFT_eSPI.h"

void E2741Q_init();
void E2741Q_wakeup(void);

#ifdef JD79667_DRIVER
#undef JD79667_DRIVER
#endif

#ifndef EPD_WIDTH
#define EPD_WIDTH 480
#endif

#ifndef EPD_HEIGHT
#define EPD_HEIGHT 800
#endif

#ifndef TFT_WIDTH
#define TFT_WIDTH EPD_WIDTH
#endif

#ifndef TFT_HEIGHT
#define TFT_HEIGHT EPD_HEIGHT
#endif

#define EPD_COLOR_DEPTH 4

// It is unknown if these are correct, they were cut and pasted and don't 
// appear to be needed
#if 0 
#define EPD_NOP 0xFF     // No operation command (not supported)
#define EPD_PNLSET 0x00  // Panel setting (R00H PSR)
#define EPD_DISPON 0x04  // Power on (R04H PON)
#define EPD_DISPOFF 0x02 // Power off (R02H POF)
#define EPD_SLPIN 0x07   // Enter deep sleep (R07H DSLP)
#define EPD_SLPOUT 0xFF  // Exit sleep (not supported, requires wake-up)
#define EPD_PTLIN 0x91   // Partial display in (R91H PTIN)
#define EPD_PTLOUT 0x92  // Partial display out (R92H PTOUT)
#define EPD_PTLW 0x90    // Partial display window setting (R90H PTL)
#endif

#define TFT_SWRST 0xFF   // Software reset (not supported)
#define TFT_CASET 0xFF   // Column address setting (not supported)
#define TFT_PASET 0xFF   // Page address setting (not supported)
#define TFT_RAMWR 0x13   // Write RAM (R13H DTM2, red data)
#define TFT_RAMRD 0xFF   // Read RAM (not supported)
#define TFT_INVON 0xFF   // Display inversion on (not supported)
#define TFT_INVOFF 0xFF  // Display inversion off (not supported)
#define TFT_INIT_DELAY 0 // Initialization delay (none)


#ifdef TFT_BUSY
#define CHECK_BUSY()                   \
    do                                 \
    {                                  \
        while (!digitalRead(TFT_BUSY)) \
            ;                          \
    } while (0)
#else
#define CHECK_BUSY()
#endif

#define EPD_UPDATE()        \
    do                      \
    {                       \
        LOG("C: 04\n");   \
        writecommand(0x04); \
        LOG("call CHECK_BUSY\n");   \
        CHECK_BUSY();       \
        LOG("C: 12\n");   \
        writecommand(0x12); \
        LOG("D: 00\n");   \
        writedata(0);    \
        LOG("call CHECK_BUSY\n");   \
        CHECK_BUSY();       \
    } while (0)

#define EPD_SLEEP()         \
    do                      \
    {                       \
        writecommand(0x02); \
        CHECK_BUSY();       \
        writecommand(0x07); \
        writedata(0xA5);    \
    } while (0)

#define EPD_WAKEUP() E2741Q_wakeup()

#define EPD_SET_WINDOW(x1, y1, x2, y2)

#if 0

#define EPD_PUSH_NEW_COLORS(w, h, colors)       \
    do                                          \
    {                                           \
       int BytesWritten = 0; \
       LOG("C: 13\n"); \
        writecommand(0x13);                     \
        for (int i = 0; i < (w) * (h) / 8; i++) \
        {                                       \
            writedata(colors[i]);               \
               LOG("D: %2X\n",colors[i]); \
               BytesWritten++; \
        }                                       \
        LOG("BytesWritten %d\n",BytesWritten); \
    } while (0)
#else
#define COLOR_GET(color) ( \
    (color) == 0x00 ? 0x01 : \
    (color) == 0x0B ? 0x02 : \
    (color) == 0x06 ? 0x03 : \
    (color) == 0x0F ? 0x00 : \
    0x00 \
)

#define EPD_PUSH_NEW_COLORS(w, h, colors)   \
    do                                      \
    {                                       \
       LOG("Custom EPD_PUSH_NEW_COLORS called, %d / %d \n",w,h); \
        uint16_t bytes_per_row = (w) / 2;   \
        uint8_t temp1, temp2, temp3, temp4;               \
        int BytesWritten = 0; \
        LOG("C: 10\n"); \
        writecommand(0x10);                 \
        for (uint16_t row = 0; row < (h) ; row++)        \
        {                                   \
            for(uint16_t col = 0; col < bytes_per_row; col+=2)   \
            {                               \
                uint8_t b = (colors[bytes_per_row *row+col ]) ;   \
                uint8_t c = (colors[bytes_per_row *row+col + 1]) ;   \
                temp1 =  (b >> 4) & 0x0F;\
                temp2 =   b & 0x0F;\
                temp3 =  (c >> 4) & 0x0F;\
                temp4 =   c & 0x0F;\
                uint8_t x = (COLOR_GET(temp1) <<6)|( COLOR_GET(temp2) << 4 ) |( COLOR_GET(temp3) << 2 ) |( COLOR_GET(temp4) << 0 );\
                LOG("D: %02X\n",x); \
                writedata(x); \
                BytesWritten++; \
            }                               \
        }                                   \
        LOG("BytesWritten %d!\n",BytesWritten); \
    } while (0)
#endif

#define EPD_PUSH_NEW_COLORS_FLIP(w, h, colors)                         \
    do                                                                 \
    {                                                                  \
       int BytesWritten = 0; \
       LOG("C: 13\n"); \
        writecommand(0x13);                                            \
        uint16_t bytes_per_row = (w) / 8;                              \
        for (uint16_t row = 0; row < (h); row++)                       \
        {                                                              \
            uint16_t start = row * bytes_per_row;                      \
            for (uint16_t col = 0; col < bytes_per_row; col++)         \
            {                                                          \
                uint8_t b = colors[start + (bytes_per_row - 1 - col)]; \
                b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);             \
                b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);             \
                b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);             \
                writedata(b);                                          \
               LOG("D: %2X\n",b); \
               BytesWritten++; \
            }                                                          \
        }                                                              \
        LOG("BytesWritten %d\n",BytesWritten); \
    } while (0)

#define EPD_PUSH_OLD_COLORS(w, h, colors)
#define EPD_PUSH_OLD_COLORS_FLIP(w, h, colors)
#define EPD_SET_TEMP(temp)
