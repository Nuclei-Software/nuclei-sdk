/*
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
#ifndef __CORE_FEATURE_TIMER_H__
#define __CORE_FEATURE_TIMER_H__
/*!
 * @file     core_feature_timer.h
 * @brief    System Timer feature API header file for Nuclei N/NX Core
 */
/*
 * System Timer Feature Configuration Macro:
 * 1. __SYSTIMER_PRESENT:  Define whether Private System Timer is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 2. __SYSTIMER_BASEADDR:  Define the base address of the System Timer.
 */
#ifdef __cplusplus
 extern "C" {
#endif

#if defined(__SYSTIMER_PRESENT) && (__SYSTIMER_PRESENT == 1)
/**
 * \defgroup NMSIS_Core_SysTimer_Registers     Register Define and Type Definitions Of System Timer
 * \ingroup NMSIS_Core_Registers
 * \brief   Type definitions and defines for system timer registers.
 *
 * @{
 */
/**
 * \brief  Structure type to access the System Timer (SysTimer).
 */
typedef struct {
    __IOM uint64_t MTIMER;                  /*!< Offset: 0x000 (R/W)  System Timer current value 64bits Register */
    __IOM uint64_t MTIMERCMP;               /*!< Offset: 0x008 (R/W)  System Timer compare Value 64bits Register */
    __IOM uint32_t RESERVED0[0x3F8];        /*!< Offset: 0x010 - 0xFEC Reserved */
    __IOM uint32_t MSFTRST;                 /*!< Offset: 0xFF0 (R/W)  System Timer Software Core Reset Register */
    __IOM uint32_t RESERVED1;               /*!< Offset: 0xFF4 Reserved */
    __IOM uint32_t MSTOP;                   /*!< Offset: 0xFF8 (R/W)  System Timer Stop Register */
    __IOM uint32_t MSIP;                    /*!< Offset: 0xFFC (R/W)  System Timer SW interrupt Register */
} SysTimer_Type;

/* Timer Control / Status Register Definitions */
#define SysTimer_TIMER_MSTOP_Pos            0U                                          /*!< SysTick TIMER STOP: MSTOP Position */
#define SysTimer_TIMER_MSTOP_Msk            (1UL << SysTimer_TIMER_MSTOP_Pos)           /*!< SysTick TIMER STOP: MSTOP Mask */

#define SysTimer_TIMER_MSIP_Pos             0U                                          /*!< SysTick TIMER MSIP: MSIP Position */
#define SysTimer_TIMER_MSIP_Msk             (1UL << SysTimer_TIMER_MSIP_Pos)            /*!< SysTick TIMER MSIP: MSIP Mask */
#define SysTimer_VALUE_Msk                  0xFFFFFFFFFFFFFFFFULL                       /*!< SysTick Timer Value Mask */

#define SysTimer_TIMER_MSFRST_KEY           (0x80000A5F)                                /*!< SysTick TIMER Software Reset Request Key */

#ifndef __SYSTIMER_BASEADDR
/* Base address of SYSTIMER(__SYSTIMER_BASEADDR) should be defined in <Device.h> */
#error "__SYSTIMER_BASEADDR is not defined, please check!"
#endif
/* System Timer Memory mapping of Device  */
#define SysTimer_BASE                       __SYSTIMER_BASEADDR                         /*!< SysTick Base Address */
#define SysTimer                            ((SysTimer_Type *) SysTimer_BASE)           /*!< SysTick configuration struct */
/** @} */ /* end of group NMSIS_Core_SysTimer_Registers */

/* ##################################    SysTimer function  ############################################ */
/**
 * \defgroup NMSIS_Core_SysTimer SysTimer Functions
 * \brief    Functions that configure the System Timer.
 * @{
 */
/**
 * \brief  Set system Timer load value
 * \details
 * This function set the system Timer load value.
 * \param [in]      value  init value for system timer.
 * \remarks
 * - Load value is 64bits wide.
 * - \ref SysTimer_GetLoadValue
 */
__STATIC_INLINE void SysTimer_SetLoadValue(uint64_t value)
{
    SysTimer->MTIMER = value;
}

/**
 * \brief  Get system Timer load value
 * \details
 * This function get the system Timer current value.
 * \return       current value for system timer.
 * \remarks
 * - Load value is 64bits wide.
 * - \ref SysTimer_SetLoadValue
 */
__STATIC_INLINE uint64_t SysTimer_GetLoadValue(void)
{
    return SysTimer->MTIMER;
}

/**
 * \brief  Set system Timer compare value
 * \details
 * This function set the system Timer compare value.
 * \param [in]      value  compare value for system timer.
 * \remarks
 * - Compare value is 64bits wide. 
 * - If compare value is larger than current value Timer interrupt generate.
 * - Modify the load value or compare value less to clear the interrupt.
 * - \ref SysTimer_GetCompareValue
 */
__STATIC_INLINE void SysTimer_SetCompareValue(uint64_t value)
{
    SysTimer->MTIMERCMP = value;
}

/**
 * \brief  Get system Timer compare value
 * \details
 * This function get the system Timer compare value.
 * \return       compare value for system timer.
 * \remarks
 * - Compare value is 64bits wide.
 * - \ref SysTimer_SetCompareValue
 */
__STATIC_INLINE uint64_t SysTimer_GetCompareValue(void)
{
    return SysTimer->MTIMERCMP;
}

/**
 * \brief  Set system Timer MSTOP
 * \details
 * This function set the system Timer MSTOP bit.
 * \param [in]       mstop   0 stop timer and 1 start timer.
 * \remarks
 * - Bit0 is used to start and stop. Set bit0 timer start. Clear Bit0 Timer stop.
 * - \ref SysTimer_SetMstopValue
 */
__STATIC_INLINE void SysTimer_SetMstopValue(uint32_t mstop)
{
    SysTimer->MSTOP = (mstop & SysTimer_TIMER_MSTOP_Msk);
}

/**
 * \brief  Set system Timer MSTOP
 * \details
 * This function set the system Timer MSTOP bit.
 * \return         0  timer stop and  1 timer is running.
 * \remarks
 * - \ref SysTimer_SetMstopValue
 */
__STATIC_INLINE uint32_t SysTimer_GetMstopValue(void)
{
    return (SysTimer->MSTOP & SysTimer_TIMER_MSTOP_Msk);
}

/**
 * \brief  Set system Timer MSIP to trigger SW interrupt
 * \details
 * This function set the system Timer MSIP bit.
 * \remarks
 * - Set system timer MSIP bit and generate a SW interrupt.
 * - \ref SysTimer_ClearSWIRQ;SysTimer_GetMsipValue;
 */
__STATIC_INLINE void SysTimer_SetSWIRQ(void)
{
    SysTimer->MSIP |= SysTimer_TIMER_MSIP_Msk;
}

/**
 * \brief  Clear system Timer MSIP to clear SW interrupt
 * \details
 * This function set the system Timer MSIP bit.
 * \remarks
 * - Clear system timer MSIP bit and Clear the SW interrupt.
 * - \ref SysTimer_SetSWIRQ;SysTimer_GetMsipValue;
 */
__STATIC_INLINE void SysTimer_ClearSWIRQ(void)
{
    SysTimer->MSIP &= ~SysTimer_TIMER_MSIP_Msk;
}

/**
 * \brief  Get system Timer MSIP
 * \details
 * This function get the system Timer MSIP bit.
 * \return         0  SW interrupt set and 1 SW interrupt clear.
 * \remarks
 * - Bit0 is SW interrupt flag. Bit0 is 1 then SW interrupt set. Bit0 is 0 then SW interrupt clear.
 * - \ref SysTimer_SetSWIRQ; SysTimer_ClearSWIRQ;
 * - \ref Set_MTVEC
 */
__STATIC_INLINE uint32_t SysTimer_GetMsipValue(void)
{
    return (uint32_t)(SysTimer->MSIP & SysTimer_TIMER_MSIP_Msk);
}

/**
 * \brief  Do software reset request
 * \details
 * This function will do software reset request through TIMER/
 * - Software need to write \ref SysTimer_TIMER_MSFRST_KEY to generate software reset request
 * - The software request flag can be cleared by reset operation to clear
 * \remarks
 * - The software reset is sent to SoC, SoC need to generate reset signal and send back to Core
 * - This function will not return, it will do while(1) to wait the Core reset happened
 */
__STATIC_INLINE void SysTimer_SoftwareReset(void)
{
    SysTimer->MSFTRST = SysTimer_TIMER_MSFRST_KEY;
    while(1);
}

#if defined (__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U) && defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
/**
 * \brief   System Tick Configuration
 * \details Initializes the System Timer and its non-vector interrupt, and starts the System Tick Timer.
 *
 *  In our default implementation, the timer counter will be set to zero, and it will start a timer compare non-vector interrupt
 *  when it matchs the ticks user set, during the timer interrupt user should reload the timer counter to zero, so it
 *  can produce period timer interrupt.
 * \param [in]  ticks  Number of ticks between two interrupts.
 * \return          0  Function succeeded.
 * \return          1  Function failed.
 * \remarks
 * - When the variable \ref __Vendor_SysTickConfig is set to 1, then the
 *   function \ref SysTick_Config is not included.
 * - In this case, the file **<Device>.h** must contain a vendor-specific implementation
 *   of this function.
 * - If user need this function to start a period timer interrupt, then in timer interrupt handler
 *   routine code, user should call \ref SysTimer_SetLoadValue to set counter as 0 to reload the timer.
 * - This function only available when __SYSTIMER_PRESENT == 1 and __ECLIC_PRESENT == 1 and __Vendor_SysTickConfig == 0
 * \sa
 * - \ref SysTimer_SetCompareValue; SysTimer_SetLoadValue
 */
__STATIC_INLINE uint32_t SysTick_Config(uint64_t ticks)
{
    SysTimer_SetLoadValue(0);
    SysTimer_SetCompareValue(ticks);
    ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ(SysTimer_IRQn, 1);
    ECLIC_EnableIRQ(SysTimer_IRQn);
    return (0UL);
}
#endif /* defined(__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U) */
/** @} */ /* End of Doxygen Group NMSIS_Core_SysTimer */

#endif /* defined(__SYSTIMER_PRESENT) && (__SYSTIMER_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /** __CORE_FEATURE_TIMER_H__  */

