// PDI 7.4" BWRY EPD (E2741QS0B3)

#ifndef EPD_WIDTH
#define EPD_WIDTH 800
#endif

#ifndef EPD_HEIGHT
#define EPD_HEIGHT 480
#endif

#ifndef TFT_WIDTH
#define TFT_WIDTH EPD_WIDTH
#endif

#ifndef TFT_HEIGHT
#define TFT_HEIGHT EPD_HEIGHT
#endif

#define EPD_COLOR_DEPTH 1

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

   FUNCT_END();
};

static void RunSequence(const uint8_t *p)
{
   uint8_t Count;
   for( ; ; ) {
      if((Count = *p++) == 0) {
      // function
         switch(*p++) {
            case 0;  // FUNCT_END
               break;
         }
         else {
            writecommand(*p++);
            while(Count--) {
               writedata(*p++);
            }
         }
      }
   }
}


static void EpdWakeup()
{
   RunSequence(epd_E2741Q_init);
}

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
        writecommand(0x04); \
        CHECK_BUSY();       \
        writecommand(0x12); \
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

#define EPD_WAKEUP() EpdWakeup()

#define EPD_SET_WINDOW(x1, y1, x2, y2)

#define EPD_PUSH_NEW_COLORS(w, h, colors)       \
    do                                          \
    {                                           \
        writecommand(0x13);                     \
        for (int i = 0; i < (w) * (h) / 8; i++) \
        {                                       \
            writedata(colors[i]);               \
        }                                       \
    } while (0)

#define EPD_PUSH_NEW_COLORS_FLIP(w, h, colors)                         \
    do                                                                 \
    {                                                                  \
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
            }                                                          \
        }                                                              \
    } while (0)

#define EPD_PUSH_OLD_COLORS(w, h, colors)
#define EPD_PUSH_OLD_COLORS_FLIP(w, h, colors)
#define EPD_SET_TEMP(temp)
