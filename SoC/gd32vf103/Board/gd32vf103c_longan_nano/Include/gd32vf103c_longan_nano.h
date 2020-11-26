/*!
    \file    gd32vf103c_longan_nano.h
    \brief   definitions for GD32VF103C_longan_nano's leds, keys and COM ports hardware resources

    \version 2020-11-25, V1.0.0, demo for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.
    Copyright (c) 2020, Roman Buchert (roman_dot_buchert_at_googlemail_dot_com)

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef GD32VF103C_LONGAN_NANO_H
#define GD32VF103C_LONGAN_NANO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "nuclei_sdk_soc.h"

/* exported types */
typedef enum
{
    LED1 = 0,
    LED2 = 1,
    LED3 = 2,
}led_typedef_enum;

typedef enum
{
    KEY_WAKEUP = 0,
}key_typedef_enum;

typedef enum
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
}keymode_typedef_enum;


/* sipeed longan nano board low layer led */
#define LEDn                             (3U)

#define LEDG_PIN                         GPIO_PIN_1
#define LEDG_GPIO_PORT                   GPIOA
#define LEDG_GPIO_CLK                    RCU_GPIOA

#define LEDB_PIN                         GPIO_PIN_2
#define LEDB_GPIO_PORT                   GPIOA
#define LEDB_GPIO_CLK                    RCU_GPIOA

#define LEDR_PIN                         GPIO_PIN_13
#define LEDR_GPIO_PORT                   GPIOC
#define LEDR_GPIO_CLK                    RCU_GPIOC

/* sipeed longan nano board UART com port */
#define GD32_COM0                        USART0
#define GD32_COM_CLK                     RCU_USART0
#define GD32_COM_TX_PIN                  GPIO_PIN_9
#define GD32_COM_RX_PIN                  GPIO_PIN_10
#define GD32_COM_TX_GPIO_PORT            GPIOA
#define GD32_COM_RX_GPIO_PORT            GPIOA
#define GD32_COM_TX_GPIO_CLK             RCU_GPIOA
#define GD32_COM_RX_GPIO_CLK             RCU_GPIOA

/* sipeed longan nano board low layer button (none)*/
#define KEYn                             (0U)

/* function declarations */
/* configure led GPIO */
void gd_longan_nano_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_longan_nano_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_longan_nano_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_longan_nano_led_toggle(led_typedef_enum lednum);
/* configure COM port */
void gd_com_init(uint32_t usart_periph);
#ifdef __cplusplus
}
#endif

#endif /* GD32VF103V_LONGAN_NANO_H */

