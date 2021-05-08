/**
 * Copyright (c) 2015 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup pwm_example_main main.c
 * @{
 * @ingroup pwm_example
 *
 * @brief PWM Example Application main file.
 *
 * This file contains the source code for a sample application using PWM.
 */

#include <stdio.h>
#include <string.h>
#include "nrf_drv_pwm.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define ONBOARD_LED
//#define STRIPE_LED

#ifdef STRIPE_LED
#define ROW                      ( 1u )
#define COL                      ( 17u )
#define SIZE_PIX                 ( 24u )
#define PERIOD                   ( 20u | 0x8000 ) //20
#define WS2812B_1                ( 15u | 0x8000 ) //15
#define WS2812B_0                ( 6u | 0x8000 )  //6
#define WS2812B_PIN              ( 4u ) //16
#define WS2812B_PORT             ( 0u )   //0
#define WS2812B                   NRF_GPIO_PIN_MAP(WS2812B_PORT,WS2812B_PIN)
#define RESET_ZEROS_AT_START     45
#endif

#ifdef ONBOARD_LED
#define ROW                      ( 1u )
#define COL                      ( 1u )
#define SIZE_PIX                 ( 24u )
#define PERIOD                   ( 20u | 0x8000 ) //20
#define WS2812B_1                ( 15u | 0x8000 ) //15
#define WS2812B_0                ( 6u | 0x8000 )  //6
#define WS2812B_PIN              ( 16u ) //16
#define WS2812B_PORT             ( 0u )   //0
#define WS2812B                   NRF_GPIO_PIN_MAP(WS2812B_PORT,WS2812B_PIN)
#define RESET_ZEROS_AT_START     45
#endif

static bool WS2812B_TX_CPLT; 
static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
static nrf_pwm_values_common_t WS2812B_Buffer[RESET_ZEROS_AT_START+ROW*COL*SIZE_PIX]; // 17*1*24=408
nrf_pwm_sequence_t const seq =
{
    .values.p_common = WS2812B_Buffer,
    .length          = NRF_PWM_VALUES_LENGTH(WS2812B_Buffer),
    .repeats         = 0,
    .end_delay       = 1500
};

static void WS2812B_pwmhandler(nrf_drv_pwm_evt_type_t event_type)
{
   if( event_type == NRF_DRV_PWM_EVT_FINISHED )
   {
      WS2812B_TX_CPLT = true;
   }
}

static void init_pwm(void)
{
    /*
     * This demo uses only one channel, which is reflected on LED 1.
     * The LED blinks three times (200 ms on, 200 ms off), then it stays off
     * for one second.
     * This scheme is performed three times before the peripheral is stopped.
     */
  
   nrf_gpio_cfg_output(WS2812B);
   nrf_gpio_pin_clear(WS2812B);
  
   for(uint8_t i=0; i<RESET_ZEROS_AT_START; i++)
   {
      WS2812B_Buffer[i] = 0x8000;
   }

    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            WS2812B | NRF_DRV_PWM_PIN_INVERTED, // channel 0 NRF_DRV_PWM_PIN_INVERTED
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 1
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 2
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_HIGHEST,
        .base_clock   = NRF_PWM_CLK_16MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = PERIOD,
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, WS2812B_pwmhandler));
    
    WS2812B_TX_CPLT = true; 
}

static void WS2812B_sendBuffer( void )
{
   WS2812B_TX_CPLT = false;
  
   nrf_drv_pwm_simple_playback(&m_pwm0, &seq, 1, NRF_DRV_PWM_FLAG_STOP);
    
   while( WS2812B_TX_CPLT != true );
}

static void WS2812B_clearBuffer( void  )
{
   for( uint16_t i=0; i<ROW*COL*SIZE_PIX; i++ )
   {
      WS2812B_Buffer[RESET_ZEROS_AT_START+i] = WS2812B_0;
   }
}

static void WS2812B_setPixel( uint16_t row, uint16_t col, uint8_t red, uint8_t green, uint8_t blue  )
{
  for( uint8_t i=0; i<8; i++)
  {
    if( (0x01 & (red>>i)) == 0x01 )
    {
      WS2812B_Buffer[RESET_ZEROS_AT_START+col*SIZE_PIX+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[RESET_ZEROS_AT_START+col*SIZE_PIX+i] = WS2812B_0;
    }
    if( (0x01 & (green>>i)) == 0x01 )
    {
      WS2812B_Buffer[RESET_ZEROS_AT_START+col*SIZE_PIX+8+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[RESET_ZEROS_AT_START+col*SIZE_PIX+8+i] = WS2812B_0;
    }
    if( (0x01 & (blue>>i)) == 0x01 )
    {
      WS2812B_Buffer[RESET_ZEROS_AT_START+col*SIZE_PIX+16+i] = WS2812B_1;
    }
    else
    {
      WS2812B_Buffer[RESET_ZEROS_AT_START+col*SIZE_PIX+16+i] = WS2812B_0;
    }
  }
}


static void init_bsp()
{
    APP_ERROR_CHECK(nrf_drv_clock_init());
    nrf_drv_clock_lfclk_request(NULL);

    APP_ERROR_CHECK(app_timer_init());
}


void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
}


int main(void)
{
   static uint8_t i;
  
   init_bsp();

   init_pwm();

   for (;;)
   {
        //// Wait for an event.
        //__WFE();
        //
        //// Clear the event register.
        //__SEV();
        //__WFE();
      i++;
      WS2812B_clearBuffer();
      WS2812B_setPixel(0,i%COL,0,i,0);//rand()%0xFF,rand()%0xFF,rand()%0xFF
      WS2812B_sendBuffer();
      nrf_delay_ms(100);
      
   }
}


/** @} */
