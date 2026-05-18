/*
 *  Copyright (C) 2023  Skip Hansen
 * 
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms and conditions of the GNU General Public License,
 *  version 2, as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 */
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include "logging.h"

#ifndef GCC_PACKED
#if defined(__GNUC__)
#define GCC_PACKED __attribute__ ((packed))
#else
#define GCC_PACKED
#endif
#endif

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

void DumpHexAdr(void *AdrIn,int Len,int AdrValue)
{
   unsigned char *Adr = (unsigned char *) AdrIn;
   int DumpLen;

   for(int i = 0; i < Len; i += 16) {
      LOG_RAW("%04X ",AdrValue);
      DumpLen = Len - i;
      if(DumpLen > 16) {
         DumpLen = 16;
      }
      DumpHex(Adr,DumpLen);
      AdrValue += 16;
      Adr += 16;
   }
}

void DumpHexSrc(void *AdrIn,int Len)
{
   unsigned char *Adr = (unsigned char *) AdrIn;
   int i = 0;
   int j;

   while(i < Len) {
      for(j = 0; j < 8; j++) {
         if((i + j) == Len) {
            break;
         }
         LOG_RAW("0x%02x,",Adr[i+j]);
      }
      i += 8;
      LOG_RAW("\n");
   }
}


#ifdef E2741QS0B3

#include "driver.h"
#include "TFT_eSPI.h"
extern EPaper epaper;

void WaitBusy(uint8_t State);

#define FUNCT_END()  0,0
#define FUNCT_WAIT_BUSY(x)  0,1,x

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

   FUNCT_WAIT_BUSY(1),
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

            case 1:  // FUNCT_WAIT_BUSY(state)
               LOG("Calling WaitBusy\n");
               WaitBusy(*p++);
               break;
         }
      }
      else {
         LOG_RAW("\nC: %02X\n",*p);
         epaper.writecommand(*p++);
         while(--Count) {
            LOG_RAW("D: %02X\n",*p);
            epaper.writedata(*p++);
         }
      }
   }
}

void WaitBusy(uint8_t State)
{
   volatile int Busy;
   const char *DesiredState = State ? "high" : "low";

   pinMode(4,INPUT);
   Busy = digitalRead(4);

   LOG("Waiting for busy %s, starting with %d\n",DesiredState,Busy);
   do {
      Busy = digitalRead(4);
   } while(Busy != State);
   LOG("Busy went %s\n",DesiredState);
}

void E2741Q_init()
{
   RunSequence(epd_E2741Q_init);
}

void E2741Q_wakeup()
{
   E2741Q_init();

}

#endif

