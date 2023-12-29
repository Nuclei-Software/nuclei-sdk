/*!
    \file    gd32vw553h_eval.h
    \brief   definitions for GD32VW553H_EVAL's leds, keys and COM ports hardware resources

    \version 2023-08-08, V1.0.0, demo for GD32VW55x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

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

#ifndef GD32VW553H_EVAL_H
#define GD32VW553H_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "gd32vw55x.h"

/* exported types */
typedef enum
{
    LED1 = 0U,
    LED2 = 1,
    LED3 = 2,
    LED_MAX
}led_typedef_enum;

typedef enum
{
    KEY_TAMPER_WAKEUP = 0,
    KEY_MAX
}key_typedef_enum;

typedef enum
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
}keymode_typedef_enum;

/* eval board low layer led */
#define LED1_PIN                         GPIO_PIN_4
#define LED1_GPIO_PORT                   GPIOA
#define LED1_GPIO_CLK                    RCU_GPIOA

#define LED2_PIN                         GPIO_PIN_5
#define LED2_GPIO_PORT                   GPIOA
#define LED2_GPIO_CLK                    RCU_GPIOA

#define LED3_PIN                         GPIO_PIN_6
#define LED3_GPIO_PORT                   GPIOA
#define LED3_GPIO_CLK                    RCU_GPIOA

#define COMn                             1U
#define EVAL_COM0                        USART0
#define EVAL_COM0_CLK                    RCU_USART0

#define EVAL_COM0_TX_PIN                 GPIO_PIN_15
#define EVAL_COM0_RX_PIN                 GPIO_PIN_8

#define EVAL_COM0_TX_GPIO_PORT           GPIOB
#define EVAL_COM0_TX_GPIO_CLK            RCU_GPIOB
#define EVAL_COM0_TX_AF                  GPIO_AF_8

#define EVAL_COM0_RX_GPIO_PORT           GPIOA
#define EVAL_COM0_RX_GPIO_CLK            RCU_GPIOA
#define EVAL_COM0_RX_AF                  GPIO_AF_2

/* tamper push-button */
#define TAMPER_WAKEUP_KEY_PIN                   GPIO_PIN_0
#define TAMPER_WAKEUP_KEY_GPIO_PORT             GPIOA
#define TAMPER_WAKEUP_KEY_GPIO_CLK              RCU_GPIOA
#define TAMPER_WAKEUP_KEY_EXTI_LINE             EXTI_0
#define TAMPER_WAKEUP_KEY_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOA
#define TAMPER_WAKEUP_KEY_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN0
#define TAMPER_WAKEUP_KEY_EXTI_IRQn             EXTI0_IRQn

/* function declarations */
/* configure led GPIO */
void gd_eval_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_eval_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_eval_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_eval_led_toggle(led_typedef_enum lednum);
/* configure key */
void gd_eval_key_init(key_typedef_enum key_num, keymode_typedef_enum key_mode);
/* return the selected button state */
uint8_t gd_eval_key_state_get(key_typedef_enum button);
/* configure COM port */
void gd_eval_com_init(uint32_t com);


#ifdef __cplusplus
}
#endif

#endif /* GD32VW553H_EVAL_H */
