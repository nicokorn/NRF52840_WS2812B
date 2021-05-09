// ****************************************************************************
/// \file      ws2812b.c
///
/// \brief     WS2812B C Source File
///
/// \details   The ws2812b leds are controlled by the pwm peripheral. Depending 
///            on the dutycycle the ws2812b leds 
///            interprets a 1 or 0 bit and holds a 24 bit shiftregister. The 24 
///            bits are splittet into red (8b), green (8) and blue (8b). 
///            Bits are being shiftet through all leds connected in serial. 
///            After finishing the shifting, the line needs
///            to be set low for at least 50 us, so the leds accept their
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
#include <string.h>
#include "nrf_drv_pwm.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "ws2812b.h"

// Private define *************************************************************
#define PIXEL_BIT_SIZE           ( 24u )
#define WS2812B_PWM_T            ( 20u ) // 20 ticks @ 16 MHz equals 1250 ns
#define WS2812B_1                ( 15u | 0x8000 ) // 15 ticks @ 16 MHz equals 937.5 ns, the MSB defines the PWM polarity during the sequence
#define WS2812B_0                ( 6u | 0x8000 )  // 6 ticks @ 16 MHz equals 375 ns, the MSB defines the PWM polarity during the sequence
#define WS2812B                  NRF_GPIO_PIN_MAP(WS2812B_PORT,WS2812B_PIN)
#define WS2812B_RESET_LOW        45

// Private types     **********************************************************

// Private variables **********************************************************
static WS2812B_StatusTypeDef WS2812B_State = WS2812B_RESET; 
static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
static nrf_pwm_values_common_t WS2812B_Buffer[WS2812B_RESET_LOW+PIXEL_COUNT*PIXEL_BIT_SIZE];
nrf_pwm_sequence_t const seq =
{
    .values.p_common = WS2812B_Buffer,
    .length          = NRF_PWM_VALUES_LENGTH(WS2812B_Buffer),
    .repeats         = 0,
    .end_delay       = 0
};

// Private function prototypes ************************************************

// Global variables ***********************************************************

// Functions ******************************************************************
// ----------------------------------------------------------------------------
/// \brief     PWM handler callback function. Used to reset the transmission
///            complete  boolean.
///
/// \param     none
///
/// \return    none
void WS2812B_pwmhandler( nrf_drv_pwm_evt_type_t event_type )
{
   if( event_type == NRF_DRV_PWM_EVT_FINISHED )
   {
      WS2812B_State = WS2812B_READY;
   }
}

// ----------------------------------------------------------------------------
/// \brief     Initialisation of the periphherals for using the ws2812b leds.
///
/// \param     none
///
/// \return    none
WS2812B_StatusTypeDef WS2812B_init( void )
{
   // init pins
   nrf_gpio_cfg_output(WS2812B);
   nrf_gpio_pin_clear(WS2812B);
  
   // the end of the data buffer needs to set the output low for at least 50 us,
   // so the leds will accept their bit shiftregisters. This is done by 0 duty
   // cycles for WS2812B_RESET_LOW times.
   for(uint8_t i=0; i<WS2812B_RESET_LOW; i++)
   {
      WS2812B_Buffer[PIXEL_COUNT*PIXEL_BIT_SIZE+i] = 0x8000;
   }

    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            WS2812B, // channel 0, if you would OR with "NRF_DRV_PWM_PIN_INVERTED", the pwm idle state would be high
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 1
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 2
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_HIGHEST,
        .base_clock   = NRF_PWM_CLK_16MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = WS2812B_PWM_T,
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, WS2812B_pwmhandler));
    
    WS2812B_State = WS2812B_READY;
    
    return WS2812B_OK;
}

// ----------------------------------------------------------------------------
/// \brief     Send buffer to the ws2812b leds.
///
/// \param     none
///
/// \return    none
void WS2812B_sendBuffer( void )
{
   WS2812B_State = WS2812B_BUSY;
  
   nrf_drv_pwm_simple_playback(&m_pwm0, &seq, 1, NRF_DRV_PWM_FLAG_STOP);
    
   while( WS2812B_State != WS2812B_READY );
}

// ----------------------------------------------------------------------------
/// \brief      Clear ws2812b buffer. All pixels are black after sending.
///
/// \param     none
///
/// \return    none
void WS2812B_clearBuffer( void )
{
   for( uint16_t i=0; i<PIXEL_COUNT*PIXEL_BIT_SIZE; i++ )
   {
      WS2812B_Buffer[i] = WS2812B_0;
   }
}

// ----------------------------------------------------------------------------
/// \brief      This function sets the color of a single pixel.
///
/// \param      [in]    uint16_t pixel_pos
/// \param      [in]    uint8_t red
/// \param      [in]    uint8_t green
/// \param      [in]    uint8_t blue
///
/// \return     none
void WS2812B_setPixel( uint16_t pixel_pos, uint8_t red, uint8_t green, uint8_t blue )
{
  for( uint8_t i=0; i<8; i++)
  {
    if( (0x80 & (red<<i)) == 0x80 )
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+8+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+8+i] = WS2812B_0;
    }
    if( (0x80 & (green<<i)) == 0x80 )
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+i] = WS2812B_0;
    }
    if( (0x80 & (blue<<i)) == 0x80 )
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+16+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[pixel_pos*PIXEL_BIT_SIZE+16+i] = WS2812B_0;
    }
  }
}