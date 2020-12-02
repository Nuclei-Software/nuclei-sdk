/*!
 *  \file    gd32vf103c_longan_nano.c
 *  \brief   firmware functions to manage leds, keys, COM ports
 *  
 *  \version 2020-11-25, V1.0.0, sipeed longan nano board functions for GD32VF103
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

#include "gd32vf103c_longan_nano.h"

/* private variables */
static const uint32_t GPIO_PORT[LEDn]       = {LEDG_GPIO_PORT,LEDB_GPIO_PORT,LEDR_GPIO_PORT};

static const uint32_t GPIO_PIN[LEDn]        = {LEDG_PIN,LEDB_PIN,LEDR_PIN};

static const rcu_periph_enum GPIO_CLK[LEDn] = {LEDG_GPIO_CLK,LEDB_GPIO_CLK,LEDR_GPIO_CLK};

/* eval board low layer private functions */
/*!
 *  \brief      configure led GPIO
 *  \param[in]  lednum: specify the led to be configured
 *  \arg        LED1
 *  \param[out] none
 *  \retval     none
 */
void gd_led_init(led_typedef_enum lednum)
{
    /* enable the led clock */
    rcu_periph_clock_enable(GPIO_CLK[lednum]);
    /* configure led GPIO port */ 
    gpio_init(GPIO_PORT[lednum], GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN[lednum]);
    GPIO_BOP(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
 *  \brief      turn on selected led
 *  \param[in]  lednum: specify the led to be turned on
 *  \arg        LED1
 *  \param[out] none
 *  \retval     none
 */
void gd_led_on(led_typedef_enum lednum)
{
    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
 *  \brief      turn off selected led
 *  \param[in]  lednum: specify the led to be turned off
 *  \arg        LED1
 *  \param[out] none
 *  \retval     none
 */
void gd_led_off(led_typedef_enum lednum)
{
    GPIO_BOP(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
 *  \brief      toggle selected led
 *  \param[in]  lednum: specify the led to be toggled
 *  \arg        LED1
 *  \param[out] none
 *  \retval     none
 */
void gd_led_toggle(led_typedef_enum lednum)
{
    gpio_bit_write(GPIO_PORT[lednum], GPIO_PIN[lednum],
        (bit_status)(1-gpio_input_bit_get(GPIO_PORT[lednum], GPIO_PIN[lednum])));
}

/*!
 *  \brief      configure COM port
 *  \param[in]  com: COM on the board
 *  \arg        GD32_COM0: COM0 on the board
 *  \param[out] none
 *  \retval     none
 */
void gd_com_init(uint32_t usart_periph)
{    
    /* enable GPIO TX and RX clock */
    rcu_periph_clock_enable(GD32_COM_TX_GPIO_CLK);
    rcu_periph_clock_enable(GD32_COM_RX_GPIO_CLK);

    /* enable USART clock */
    rcu_periph_clock_enable(GD32_COM_CLK);

    /* connect port to USARTx_Tx */
    gpio_init(GD32_COM_TX_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GD32_COM_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_init(GD32_COM_RX_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GD32_COM_RX_PIN);

    /* USART configure */
    usart_deinit(usart_periph);
    usart_baudrate_set(usart_periph, 115200U);
    usart_word_length_set(usart_periph, USART_WL_8BIT);
    usart_stop_bit_set(usart_periph, USART_STB_1BIT);
    usart_parity_config(usart_periph, USART_PM_NONE);
    usart_hardware_flow_rts_config(usart_periph, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(usart_periph, USART_CTS_DISABLE);
    usart_receive_config(usart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(usart_periph, USART_TRANSMIT_ENABLE);
    usart_enable(usart_periph);
}
