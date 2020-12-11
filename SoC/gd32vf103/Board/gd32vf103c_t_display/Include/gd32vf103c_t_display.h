/*!
    \file    gd32vf103c_t_display.h
    \brief   definitions for Lilygo T-Display-GD32's leds and COM ports hardware resources

    \version 2020-12-10, V1.0.0
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

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

#ifndef GD32VF103C_T_DISPLAY_H
#define GD32VF103C_T_DISPLAY_H

#ifdef cplusplus
 extern "C" {
#endif

#include "nuclei_sdk_soc.h"

/* exported types */
typedef enum
{
    LED_1 = 0,
    LED_2 = 1,
    LED_3 = 2,
    LED_G = 0,
    LED_B = 1,
    LED_R = 2
} led_typedef_enum;

typedef enum
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} keymode_typedef_enum;

/* leds on bottomside of the board */
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

#define COMn                             (1U)

#define GD32_COM0                        USART0
#define GD32_COM0_CLK                    RCU_USART0
#define GD32_COM0_TX_PIN                 GPIO_PIN_9
#define GD32_COM0_RX_PIN                 GPIO_PIN_10
#define GD32_COM0_GPIO_PORT              GPIOA
#define GD32_COM0_GPIO_CLK               RCU_GPIOA

/* function declarations */
/* configure led GPIO */
void gd_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_led_toggle(led_typedef_enum lednum);
/* configure COM port */
void gd_com_init(uint32_t com);

#ifdef cplusplus
}
#endif

#endif /* GD32VF103C_T_DISPLAY_H */
