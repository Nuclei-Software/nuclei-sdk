/******************************************************************************
 * @file     system_gd32vw55x.c
 * @brief    NMSIS Nuclei Core Device Peripheral Access Layer Source File for
 *           Device gd32vw55x
 * @version  V1.00
 * @date     25. Dec 2023
 ******************************************************************************/
/*
 * Copyright (c) 2023, GigaDevice Semiconductor Inc.
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <inttypes.h>
#include <stdio.h>
#include "nuclei_sdk_hal.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
/* ToDo: add here your necessary defines for device initialization
         following is an example for different system frequencies */

// When using nuclei-sdk build system, refer to SoC/gd32vw55x/build.mk
// SYSTEM_CLOCK is defined via SYSCLK make variable
// SYSCLK_USING_IRC16M will be defined when CLKSRC=irc16m
// SYSCLK_USING_HXTAL will be defined when CLKSRC=hxtal

#ifndef SYSTEM_CLOCK
#define SYSTEM_CLOCK    160000000
#endif

/**
 * \defgroup  NMSIS_Core_SystemAndClock   System and Clock Configuration
 * \brief Functions for system and clock setup available in system_<device>.c.
 * \details
 * Nuclei provides a template file **system_Device.c** that must be adapted by
 * the silicon vendor to match their actual device. As a <b>minimum requirement</b>,
 * this file must provide:
 *  -  A device-specific system configuration function, \ref SystemInit().
 *  -  A global variable that contains the system frequency, \ref SystemCoreClock.
 *
 * The file configures the device and, typically, initializes the oscillator (PLL) that is part
 * of the microcontroller device. This file might export other functions or variables that provide
 * a more flexible configuration of the microcontroller system.
 *
 * \note Please pay special attention to the static variable \c SystemCoreClock. This variable might be
 * used throughout the whole system initialization and runtime to calculate frequency/time related values.
 * Thus one must assure that the variable always reflects the actual system clock speed.
 *
 * \attention
 * Be aware that a value stored to \c SystemCoreClock during low level initialization (i.e. \c SystemInit()) might get
 * overwritten by C libray startup code and/or .bss section initialization.
 * Thus its highly recommended to call \ref SystemCoreClockUpdate at the beginning of the user \c main() routine.
 *
 * @{
 */

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
/* ToDo: initialize SystemCoreClock with the system core clock frequency value
         achieved after system intitialization.
         This means system core clock frequency after call to SystemInit() */
/**
 * \brief      Variable to hold the system core clock value
 * \details
 * Holds the system core clock, which is the system clock frequency supplied to the SysTick
 * timer and the processor core clock. This variable can be used by debuggers to query the
 * frequency of the debug timer or to configure the trace clock speed.
 *
 * \attention
 * Compilers must be configured to avoid removing this variable in case the application
 * program is not using it. Debugging systems require the variable to be physically
 * present in memory so that it can be examined to configure the debugger.
 */

/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/

/* system frequency define */
#define __IRC16M          (IRC16M_VALUE)            /* internal 16 MHz RC oscillator frequency */
#define __HXTAL           (HXTAL_VALUE)             /* high speed crystal oscillator frequency */
#define __SYS_OSC_CLK     (__IRC16M)                /* main oscillator frequency */

/* select a system clock by uncommenting the following line */
#ifdef SYSCLK_USING_IRC16M
/* use IRC16M */
#if SYSTEM_CLOCK == 16000000
#define __SYSTEM_CLOCK_IRC16M                           (uint32_t)(__IRC16M)
#elif SYSTEM_CLOCK == 48000000
#define __SYSTEM_CLOCK_48M_PLLDIG_IRC16M                (uint32_t)(48000000)
#elif SYSTEM_CLOCK == 160000000
#define __SYSTEM_CLOCK_160M_PLLDIG_IRC16M               (uint32_t)(160000000)
#else
#warning "unsupported SYSTEM_CLOCK, use default 160MHz using IRC16M"
#define __SYSTEM_CLOCK_160M_PLLDIG_IRC16M               (uint32_t)(160000000)
#endif
#else /* SYSCLK_USING_IRC16M */
/* use HXTAL */
#if SYSTEM_CLOCK == 48000000
#define __SYSTEM_CLOCK_48M_PLLDIG_HXTAL                 (uint32_t)(48000000)
#elif SYSTEM_CLOCK == 160000000
#define __SYSTEM_CLOCK_160M_PLLDIG_HXTAL                (uint32_t)(160000000)
#else
#warning "using HXTAL_VALUE as __SYSTEM_CLOCK_HXTAL for using HXTAL"
#define __SYSTEM_CLOCK_HXTAL                            (uint32_t)(__HXTAL)
#endif
#endif

//#define __SYSTEM_CLOCK_IRC16M                       (uint32_t)(__IRC16M)
//#define __SYSTEM_CLOCK_HXTAL                        (uint32_t)(__HXTAL)
//#define __SYSTEM_CLOCK_48M_PLLDIG_IRC16M             (uint32_t)(48000000)
//#define __SYSTEM_CLOCK_160M_PLLDIG_IRC16M             (uint32_t)(160000000)
//#define __SYSTEM_CLOCK_48M_PLLDIG_HXTAL              (uint32_t)(48000000)
//#define __SYSTEM_CLOCK_160M_PLLDIG_HXTAL              (uint32_t)(160000000)

#define HXTALSTB_DELAY     {                                  \
                              volatile uint32_t i;           \
                              for(i=0; i<0x2000; i++){       \
                              }                              \
                          }

/* set the system clock frequency and declare the system clock configuration function */
#ifdef __SYSTEM_CLOCK_IRC16M
uint32_t SystemCoreClock = __SYSTEM_CLOCK_IRC16M;
static void system_clock_16m_irc16m(void);
#elif defined (__SYSTEM_CLOCK_HXTAL)
uint32_t SystemCoreClock = __SYSTEM_CLOCK_HXTAL;
static void system_clock_hxtal(void);
#elif defined (__SYSTEM_CLOCK_48M_PLLDIG_IRC16M)
uint32_t SystemCoreClock = __SYSTEM_CLOCK_48M_PLLDIG_IRC16M;
static void system_clock_48m_irc16m(void);
#elif defined (__SYSTEM_CLOCK_160M_PLLDIG_IRC16M)
uint32_t SystemCoreClock = __SYSTEM_CLOCK_160M_PLLDIG_IRC16M;
static void system_clock_160m_irc16m(void);
#elif defined (__SYSTEM_CLOCK_48M_PLLDIG_HXTAL)
uint32_t SystemCoreClock = __SYSTEM_CLOCK_48M_PLLDIG_HXTAL;
static void system_clock_48m_hxtal(void);
#elif defined (__SYSTEM_CLOCK_160M_PLLDIG_HXTAL)
uint32_t SystemCoreClock = __SYSTEM_CLOCK_160M_PLLDIG_HXTAL;
static void system_clock_160m_hxtal(void);
#endif /* __SYSTEM_CLOCK_IRC16M */

/* configure the system clock */
static void system_clock_config(void);

/*!
    \brief      setup the microcontroller system, initialize the system
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SystemInit (void)
{
    /* reset the RCU clock configuration to the default reset state */
    /* set IRC16MEN bit */
    RCU_CTL |= RCU_CTL_IRC16MEN;
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    /* reset CFG0 register */
    RCU_CFG0 = 0x00000000U;

    /* reset HXTALEN, RFCKMEN and PLLEN bits */
    RCU_CTL &= ~(RCU_CTL_PLLDIGEN | RCU_CTL_RFCKMEN | RCU_CTL_HXTALEN);

    /* reset PLLCFGR register */
    RCU_PLL = 0x00000000U;
    RCU_PLLDIGCFG0 = 0x00000000U;
    RCU_PLLDIGCFG1 = 0x07800000U;

    /* disable all interrupts */
    RCU_INT = 0x00000000U;

    /* configure the System clock source, PLL Multiplier and Divider factors,
       AHB/APBx prescalers and Flash settings */
    system_clock_config();
}
/*!
    \brief      configure the system clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_config(void)
{
#ifdef __SYSTEM_CLOCK_IRC16M
    system_clock_16m_irc16m();
#elif defined (__SYSTEM_CLOCK_HXTAL)
    system_clock_hxtal();
#elif defined (__SYSTEM_CLOCK_48M_PLLDIG_IRC16M)
    system_clock_48m_irc16m();
#elif defined (__SYSTEM_CLOCK_160M_PLLDIG_IRC16M)
    system_clock_160m_irc16m();
#elif defined (__SYSTEM_CLOCK_160M_PLLDIG_HXTAL)
    system_clock_160m_hxtal();
#elif defined (__SYSTEM_CLOCK_48M_PLLDIG_HXTAL)
    system_clock_48m_hxtal();
#endif /* __SYSTEM_CLOCK_IRC16M */
}

#ifdef __SYSTEM_CLOCK_IRC16M
/*!
    \brief      configure the system clock to 16M by IRC16M
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_16m_irc16m(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* enable IRC16M */
    RCU_CTL |= RCU_CTL_IRC16MEN;

    /* wait until IRC16M is stable or the startup time is longer than IRC16M_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_IRC16MSTB);
    }while((0U == stab_flag) && (IRC16M_STARTUP_TIMEOUT != timeout));

    if(0U == (RCU_CTL & RCU_CTL_IRC16MSTB)){
        /* if fail */
        while(1){
        }
    }

    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;

    /* select IRC16M as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_IRC16M;

    /* wait until IRC16M is selected as system clock */
    while(RCU_SCSS_IRC16M != (RCU_CFG0 & RCU_CFG0_SCSS)){
    }
}

#elif defined (__SYSTEM_CLOCK_HXTAL)
/*!
    \brief      configure the system clock to HXTAL
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* power up the HXTAL */
    RCU_CTL |= RCU_CTL_HXTALPU;
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;
    HXTALSTB_DELAY
    RCU_CTL |= RCU_CTL_HXTALREADY;
    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)){
        /* if fail */
        while(1){
        }
    }

    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;

    /* select HXTAL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_HXTAL;

    /* wait until HXTAL is selected as system clock */
    while(RCU_SCSS_HXTAL != (RCU_CFG0 & RCU_CFG0_SCSS)){
    }
}
#elif defined (__SYSTEM_CLOCK_48M_PLLDIG_IRC16M)
/*!
    \brief      configure the system clock to 48M by PLL which selects IRC16M as its clock source
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_48m_irc16m(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* enable IRC16M */
    RCU_CTL |= (RCU_CTL_IRC16MEN | RCU_CTL_IRC16MRFON);

    /* wait until IRC16M is stable or the startup time is longer than IRC16M_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_IRC16MSTB);
    }while((0U == stab_flag) && (IRC16M_STARTUP_TIMEOUT != timeout));

    if(0U == (RCU_CTL & RCU_CTL_IRC16MSTB)){
        /* if fail */
        while(1){
        }
    }

    /* IRC16M is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/1 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;

    RCU_PLL |= RCU_PLLSRC_IRC16M;

    /* 960M = 16MHz*60 */
    RCU_PLLDIGCFG1 = ((960 << 21) / 16) & 0x7FFFFFFF;
    /* PLLDIG OUT = 480M */
    RCU_PLLDIGCFG0 |= ( RCU_PLLDIG_480M );
    /* SYS clock = 48M */
    RCU_PLLDIGCFG0 |= ( RCU_PLLDIG_SYS_DIV10 );

    /* enable PLLDIG */
    RCU_CFG1 |= (RCU_CFG1_BGPU);
    RCU_CTL |= (RCU_CTL_PLLDIGEN | RCU_CTL_PLLDIGPU);

    /* wait until PLLDIG is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLDIGSTB)){
    }

    /* select PLLDIG as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLDIG;

    /* wait until PLLDIG is selected as system clock */
    while(RCU_SCSS_PLLDIG != (RCU_CFG0 & RCU_CFG0_SCSS)){
    }
}
#elif defined (__SYSTEM_CLOCK_160M_PLLDIG_IRC16M)
/*!
    \brief      configure the system clock to 160M by PLL which selects IRC16M as its clock source
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_160m_irc16m(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* enable IRC16M */
    RCU_CTL |= (RCU_CTL_IRC16MEN | RCU_CTL_IRC16MRFON);

    /* wait until IRC16M is stable or the startup time is longer than IRC16M_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_IRC16MSTB);
    }while((0U == stab_flag) && (IRC16M_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_IRC16MSTB)){
        while(1){
        }
    }

    /* IRC16M is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    RCU_PLL |= RCU_PLLSRC_IRC16M;

    /* 960M = 16MHz*60 */
    RCU_PLLDIGCFG1 = ((960 << 21) / 16) & 0x7FFFFFFF;
    /* PLLDIG OUT = 480M */
    RCU_PLLDIGCFG0 |= ( RCU_PLLDIG_480M );
    /* SYS clock = 160M */
    RCU_PLLDIGCFG0 |= ( RCU_PLLDIG_SYS_DIV3 );

    /* enable PLLDIG */
    RCU_CFG1 |= (RCU_CFG1_BGPU);
    RCU_CTL |= (RCU_CTL_PLLDIGEN | RCU_CTL_PLLDIGPU);

    /* wait until PLLDIG is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLDIGSTB)){
    }

    /* select PLLDIG as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLDIG;

    /* wait until PLLDIG is selected as system clock */
    while(RCU_SCSS_PLLDIG != (RCU_CFG0 & RCU_CFG0_SCSS)){
    }
}
#elif defined (__SYSTEM_CLOCK_160M_PLLDIG_HXTAL)
/*!
    \brief      configure the system clock to 160M by PLL which selects HXTAL as its clock source
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_160m_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* power up HXTAL */
    RCU_CTL |= RCU_CTL_HXTALPU;
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;
    HXTALSTB_DELAY
    RCU_CTL |= RCU_CTL_HXTALREADY;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)){
        /* if fail */
        while(1){
        }
    }

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    RCU_PLL |= RCU_PLLSRC_HXTAL;

    /* PLLDIG = 960/HXTAL */
    RCU_PLLDIGCFG1 = ((960 << 21) / (HXTAL_VALUE/1000000)) & 0x7FFFFFFF;

    /* PLLDIG OUT = 480M */
    RCU_PLLDIGCFG0 |= ( RCU_PLLDIG_480M );
    /* SYS clock = 160M */
    RCU_PLLDIGCFG0 |= ( RCU_PLLDIG_SYS_DIV3 );
    /* enable PLLDIG */
    RCU_CFG1 |= (RCU_CFG1_BGPU);
    RCU_CTL |= (RCU_CTL_PLLDIGEN | RCU_CTL_PLLDIGPU);
    /* wait until PLLDIG is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLDIGSTB)){
    }
    /* select PLLDIG as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLDIG;

    /* wait until PLLDIG is selected as system clock */
    while(RCU_SCSS_PLLDIG != (RCU_CFG0 & RCU_CFG0_SCSS)){
    }
} /* __SYSTEM_CLOCK_IRC16M */
#elif defined (__SYSTEM_CLOCK_48M_PLLDIG_HXTAL)
/*!
    \brief      configure the system clock to 48M by PLL which selects HXTAL as its clock source
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_48m_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* power up HXTAL */
    RCU_CTL |= RCU_CTL_HXTALPU;
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;
    HXTALSTB_DELAY
    RCU_CTL |= RCU_CTL_HXTALREADY;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)){
        /* if fail */
        while(1){
        }
    }

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    RCU_PLL |= RCU_PLLSRC_HXTAL;

    /* PLLDIG = 960/HXTAL */
    RCU_PLLDIGCFG1 = ((960 << 21) / (HXTAL_VALUE/1000000)) & 0x7FFFFFFF;

    /* PLLDIG OUT = 480M */
    RCU_PLLDIGCFG0 |= ( RCU_PLLDIG_480M );
    /* SYS clock = 48M */
    RCU_PLLDIGCFG0 |= ( RCU_PLLDIG_SYS_DIV10 );
    /* enable PLL */
    RCU_CFG1 |= (RCU_CFG1_BGPU);
    RCU_CTL |= (RCU_CTL_PLLDIGEN | RCU_CTL_PLLDIGPU);
    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLDIGSTB)){
    }
    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLDIG;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLLDIG != (RCU_CFG0 & RCU_CFG0_SCSS)){
    }
}
#endif

/*!
    \brief      update the SystemCoreClock with current core clock retrieved from cpu registers
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SystemCoreClockUpdate (void)
{
    uint32_t sws;
    uint32_t fresel, plldigdiv, outputfre, idx, clk_exp;

    /* exponent of AHB, APB1 and APB2 clock divider */
    const uint8_t ahb_exp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

    sws = GET_BITS(RCU_CFG0, 2, 3);
    switch(sws){
    /* IRC16M is selected as CK_SYS */
    case 0:
        SystemCoreClock = IRC16M_VALUE;
        break;
    /* HXTAL is selected as CK_SYS */
    case 1:
        SystemCoreClock = HXTAL_VALUE;
        break;
    /* PLLDIG is selected as CK_SYS */
    case 2:
        /* get the value of PLLDIGOSEL[1:0] */
        fresel = GET_BITS(RCU_PLLDIGCFG0, 24U, 25U);
        switch(fresel){
        case 0:
            outputfre = 192000000;
            break;
        case 1:
            outputfre = 240000000;
            break;
        case 2:
            outputfre = 320000000;
            break;
        case 3:
            outputfre = 480000000;
            break;
        default:
            outputfre = 192000000;
            break;
        }
        plldigdiv = (GET_BITS(RCU_PLLDIGCFG0, 26U, 31U) + 1U);
        SystemCoreClock = outputfre/plldigdiv;
        break;
    /* IRC16M is selected as CK_SYS */
    default:
        SystemCoreClock = IRC16M_VALUE;
        break;
    }
    /* calculate AHB clock frequency */
    idx = GET_BITS(RCU_CFG0, 4, 7);
    clk_exp = ahb_exp[idx];
    SystemCoreClock = SystemCoreClock >> clk_exp;
}

/**
 * \defgroup  NMSIS_Core_IntExcNMI_Handling   Interrupt and Exception and NMI Handling
 * \brief Functions for interrupt, exception and nmi handle available in system_<device>.c.
 * \details
 * Nuclei provide a template for interrupt, exception and NMI handling. Silicon Vendor could adapat according
 * to their requirement. Silicon vendor could implement interface for different exception code and
 * replace current implementation.
 *
 * @{
 */
/** \brief Max exception handler number, don't include the NMI(0xFFF) one */
#define MAX_SYSTEM_EXCEPTION_NUM        12
/**
 * \brief      Store the exception handlers for each exception ID
 * \note
 * - This SystemExceptionHandlers are used to store all the handlers for all
 * the exception codes Nuclei N/NX core provided.
 * - Exception code 0 - 11, totally 12 exceptions are mapped to SystemExceptionHandlers[0:11]
 * - Exception for NMI is also re-routed to exception handling(exception code 0xFFF) in startup code configuration, the handler itself is mapped to SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM]
 */
static unsigned long SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM + 1];

/**
 * \brief      Exception Handler Function Typedef
 * \note
 * This typedef is only used internal in this system_gd32vw55x.c file.
 * It is used to do type conversion for registered exception handler before calling it.
 */
typedef void (*EXC_HANDLER)(unsigned long mcause, unsigned long sp);

/**
 * \brief      System Default Exception Handler
 * \details
 * This function provided a default exception and NMI handling code for all exception ids.
 * By default, It will just print some information for debug, Vendor can customize it according to its requirements.
 */
static void system_default_exception_handler(unsigned long mcause, unsigned long sp)
{
    /* TODO: Uncomment this if you have implement printf function */
    printf("MCAUSE: 0x%lx\r\n", mcause);
    printf("MEPC  : 0x%lx\r\n", __RV_CSR_READ(CSR_MEPC));
    printf("MTVAL : 0x%lx\r\n", __RV_CSR_READ(CSR_MTVAL));
    Exception_DumpFrame(sp);
    while (1);
}

/**
 * \brief      Initialize all the default core exception handlers
 * \details
 * The core exception handler for each exception id will be initialized to \ref system_default_exception_handler.
 * \note
 * Called in \ref _init function, used to initialize default exception handlers for all exception IDs
 */
static void Exception_Init(void)
{
    for (int i = 0; i < MAX_SYSTEM_EXCEPTION_NUM + 1; i++) {
        SystemExceptionHandlers[i] = (unsigned long)system_default_exception_handler;
    }
}

/**
 * \brief       Register an exception handler for exception code EXCn
 * \details
 * * For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will be registered into SystemExceptionHandlers[EXCn-1].
 * * For EXCn == NMI_EXCn, it will be registered into SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM].
 * \param   EXCn    See \ref EXCn_Type
 * \param   exc_handler     The exception handler for this exception code EXCn
 */
void Exception_Register_EXC(uint32_t EXCn, unsigned long exc_handler)
{
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        SystemExceptionHandlers[EXCn] = exc_handler;
    } else if (EXCn == NMI_EXCn) {
        SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM] = exc_handler;
    }
}

/**
 * \brief       Get current exception handler for exception code EXCn
 * \details
 * * For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will return SystemExceptionHandlers[EXCn-1].
 * * For EXCn == NMI_EXCn, it will return SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM].
 * \param   EXCn    See \ref EXCn_Type
 * \return  Current exception handler for exception code EXCn, if not found, return 0.
 */
unsigned long Exception_Get_EXC(uint32_t EXCn)
{
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        return SystemExceptionHandlers[EXCn];
    } else if (EXCn == NMI_EXCn) {
        return SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM];
    } else {
        return 0;
    }
}

/**
 * \brief      Common NMI and Exception handler entry
 * \details
 * This function provided a command entry for NMI and exception. Silicon Vendor could modify
 * this template implementation according to requirement.
 * \remarks
 * - RISCV provided common entry for all types of exception. This is proposed code template
 *   for exception entry function, Silicon Vendor could modify the implementation.
 * - For the core_exception_handler template, we provided exception register function \ref Exception_Register_EXC
 *   which can help developer to register your exception handler for specific exception number.
 */
uint32_t core_exception_handler(unsigned long mcause, unsigned long sp)
{
    uint32_t EXCn = (uint32_t)(mcause & 0X00000fff);
    EXC_HANDLER exc_handler;

    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        exc_handler = (EXC_HANDLER)SystemExceptionHandlers[EXCn];
    } else if (EXCn == NMI_EXCn) {
        exc_handler = (EXC_HANDLER)SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM];
    } else {
        exc_handler = (EXC_HANDLER)system_default_exception_handler;
    }
    if (exc_handler != NULL) {
        exc_handler(mcause, sp);
    }
    return 0;
}
/** @} */ /* End of Doxygen Group NMSIS_Core_ExceptionAndNMI */

void SystemBannerPrint(void)
{
#if defined(NUCLEI_BANNER) && (NUCLEI_BANNER == 1)
    printf("Nuclei SDK Build Time: %s, %s\r\n", __DATE__, __TIME__);
#ifdef DOWNLOAD_MODE_STRING
    printf("Download Mode: %s\r\n", DOWNLOAD_MODE_STRING);
#endif
    printf("CPU Frequency %" PRIu32 " Hz\r\n", SystemCoreClock);
#endif
}

/**
 * \brief initialize eclic config
 * \details
 * Eclic need initialize after boot up, Vendor could also change the initialization
 * configuration.
 */
void ECLIC_Init(void)
{
    /* TODO: Add your own initialization code here. This function will be called by main */
    ECLIC_SetMth(0);
    ECLIC_SetCfgNlbits(__ECLIC_INTCTLBITS);
}

/**
 * \brief      Dump Exception Frame
 * \details
 * This function provided feature to dump exception frame stored in stack.
 */
void Exception_DumpFrame(unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;

#ifndef __riscv_32e
    printf("ra: 0x%lx, tp: 0x%lx, t0: 0x%lx, t1: 0x%lx, t2: 0x%lx, t3: 0x%lx, t4: 0x%lx, t5: 0x%lx, t6: 0x%lx\n" \
           "a0: 0x%lx, a1: 0x%lx, a2: 0x%lx, a3: 0x%lx, a4: 0x%lx, a5: 0x%lx, a6: 0x%lx, a7: 0x%lx\n" \
           "mcause: 0x%lx, mepc: 0x%lx, msubm: 0x%lx\n", exc_frame->ra, exc_frame->tp, exc_frame->t0, \
           exc_frame->t1, exc_frame->t2, exc_frame->t3, exc_frame->t4, exc_frame->t5, exc_frame->t6, \
           exc_frame->a0, exc_frame->a1, exc_frame->a2, exc_frame->a3, exc_frame->a4, exc_frame->a5, \
           exc_frame->a6, exc_frame->a7, exc_frame->cause, exc_frame->epc, exc_frame->msubm);
#else
    printf("ra: 0x%lx, tp: 0x%lx, t0: 0x%lx, t1: 0x%lx, t2: 0x%lx\n" \
           "a0: 0x%lx, a1: 0x%lx, a2: 0x%lx, a3: 0x%lx, a4: 0x%lx, a5: 0x%lx\n" \
           "mcause: 0x%lx, mepc: 0x%lx, msubm: 0x%lx\n", exc_frame->ra, exc_frame->tp, exc_frame->t0, \
           exc_frame->t1, exc_frame->t2, exc_frame->a0, exc_frame->a1, exc_frame->a2, exc_frame->a3, \
           exc_frame->a4, exc_frame->a5, exc_frame->cause, exc_frame->epc, exc_frame->msubm);
#endif
}

/**
 * \brief  Initialize a specific IRQ and register the handler
 * \details
 * This function set vector mode, trigger mode and polarity, interrupt level and priority,
 * assign handler for specific IRQn.
 * \param [in]  IRQn        NMI interrupt handler address
 * \param [in]  shv         \ref ECLIC_NON_VECTOR_INTERRUPT means non-vector mode, and \ref ECLIC_VECTOR_INTERRUPT is vector mode
 * \param [in]  trig_mode   see \ref ECLIC_TRIGGER_Type
 * \param [in]  lvl         interupt level
 * \param [in]  priority    interrupt priority
 * \param [in]  handler     interrupt handler, if NULL, handler will not be installed
 * \return       -1 means invalid input parameter. 0 means successful.
 * \remarks
 * - This function use to configure specific eclic interrupt and register its interrupt handler and enable its interrupt.
 * - If the vector table is placed in read-only section(FLASHXIP mode), handler could not be installed
 */
int32_t ECLIC_Register_IRQ(IRQn_Type IRQn, uint8_t shv, ECLIC_TRIGGER_Type trig_mode, uint8_t lvl, uint8_t priority, void* handler)
{
    if ((IRQn > SOC_INT_MAX) || (shv > ECLIC_VECTOR_INTERRUPT) \
        || (trig_mode > ECLIC_NEGTIVE_EDGE_TRIGGER)) {
        return -1;
    }

    /* set interrupt vector mode */
    ECLIC_SetShvIRQ(IRQn, shv);
    /* set interrupt trigger mode and polarity */
    ECLIC_SetTrigIRQ(IRQn, trig_mode);
    /* set interrupt level */
    ECLIC_SetLevelIRQ(IRQn, lvl);
    /* set interrupt priority */
    ECLIC_SetPriorityIRQ(IRQn, priority);

    if (handler != NULL) {
        /* set interrupt handler entry to vector table */
        ECLIC_SetVector(IRQn, (rv_csr_t)handler);
    }
    /* enable interrupt */
    ECLIC_EnableIRQ(IRQn);
    return 0;
}
/** @} */ /* End of Doxygen Group NMSIS_Core_ExceptionAndNMI */

/**
 * \brief early init function before main
 * \details
 * This function is executed right before main function.
 * For RISC-V gnu toolchain, _init function might not be called
 * by __libc_init_array function, so we defined a new function
 * to do initialization
 */
void _premain_init(void)
{
    /* TODO: Add your own initialization code here, called before main */
#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1)
    if (ICachePresent()) { // Check whether icache real present or not
        EnableICache();
    }
#endif

    // SystemCoreClock = get_cpu_freq();
    /* configure USART */
    gd_eval_com_init(SOC_DEBUG_UART);
    /* Display banner after UART initialized */
    SystemBannerPrint();
    /* Initialize exception default handlers */
    Exception_Init();
    /* ECLIC initialization, mainly MTH and NLBIT */
    ECLIC_Init();
}

/**
 * \brief finish function after main
 * \param [in]  status     status code return from main
 * \details
 * This function is executed right after main function.
 * For RISC-V gnu toolchain, _fini function might not be called
 * by __libc_fini_array function, so we defined a new function
 * to do initialization
 */
void _postmain_fini(int status)
{
    /* TODO: Add your own finishing code here, called after main */

}


/** @} */ /* End of Doxygen Group NMSIS_Core_SystemAndClock */
