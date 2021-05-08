// ****************************************************************************
/// \file      main.c
///
/// \brief     Main C Source File
///
/// \details   Example code for using the ws2812b library for the nrf52840
///            Microcontroller. The ws2812b leds are controlled by the pwm
///            peripheral. Depending on the dutycicle the ws2812b interprets
///            a 1 or 0 bit. A ws2812b needs 24 bits to get all co
///            colorinformation it needs. The 24 bits are splittet into R
///            red (8b), green (8) and blue (8b). ABits are being shiftet
///            through all leds. After finishing the shifting, the line needs
///            to be set low for at least 50 0 us so the leds accept their
///            shift registers thus emitting their colours.
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
#include "nrf_drv_pwm.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"
#include "ws2812b.h"

// Private define *************************************************************

// Private types     **********************************************************

// Private variables **********************************************************

// Private function prototypes ************************************************
static void init_clk( void );

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
   static uint8_t i;
  
   init_bsp();

   WS2812B_init();

   for (;;)
   {
      i++;
      WS2812B_clearBuffer();
      WS2812B_setPixel(i%PIXEL_COUNT,i,0,0);//rand()%0xFF,rand()%0xFF,rand()%0xFF
      WS2812B_sendBuffer();
      nrf_delay_ms(10); 
   }
}

// ----------------------------------------------------------------------------
/// \brief     Init the clocks.
///
/// \param     none
///
/// \return    none
static void init_clk( void )
{
    APP_ERROR_CHECK(nrf_drv_clock_init());
    nrf_drv_clock_lfclk_request(NULL);
    APP_ERROR_CHECK(app_timer_init());
}