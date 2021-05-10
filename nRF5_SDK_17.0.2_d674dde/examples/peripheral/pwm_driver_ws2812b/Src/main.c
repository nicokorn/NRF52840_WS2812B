// ****************************************************************************
/// \file      main.c
///
/// \brief     Main C Source File
///
/// \details   Example code for using Nico's ws2812b library for the nrf52840
///            Microcontroller. This example code works out of the box with the
///            IAR workbench, J-Link debugger/programmer and the Adafruit
///            Featherboard nrf52840 with the integrated ws2812b led.
///            Amount of leds and the pins on which they are connected to can be
///            set in the ws2812b library. The ws2812b leds are controlled by 
///            the pwm peripheral. Depending on the dutycycle the ws2812b leds 
///            interprets a 1 or 0 bit and holds a 24 bit shiftregister. The 24 
///            bits are splittet into red (8b), green (8) and blue (8b). 
///            Bits are being shiftet through all leds connected in serial. 
///            After finishing the shifting, the line needs
///            to be set low for at least 50 us, so the leds accept their
///            shift registers thus emitting their colours.
///            The library is heavily ram consuming because the pwm sequence
///            arrays are defined as 16bit uints and 1 ws2812b bit is used as
///            array entity. A future version will solve this issue.
///
/// \author    Nico Korn
///
/// \version   1.0.0.0
///
/// \date      24032021
/// 
/// \copyright Copyright (c) 2021 Nico Korn
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///
/// \pre       
///
/// \bug       
///
/// \warning   
///
/// \todo      
///
// ****************************************************************************

// Include ********************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nrf_delay.h"
#include "ws2812b.h"

// Private define *************************************************************

// Private types     **********************************************************

// Private variables **********************************************************

// Private function prototypes ************************************************

// Global variables ***********************************************************

// Functions ******************************************************************
// ----------------------------------------------------------------------------
/// \brief     Entry point of the application.
///
/// \param     none
///
/// \return    none
int main( void )
{
   static uint32_t i;

   WS2812B_init();

   for (;;)
   {
      WS2812B_clearBuffer();
      WS2812B_setPixel(++i%PIXEL_COUNT, 0x00,0x00,0xff);//rand()%0xFF,rand()%0xFF,rand()%0xFF
      WS2812B_sendBuffer();
      nrf_delay_ms(100); 
   }
}