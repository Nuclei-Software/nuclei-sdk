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
#define SysTimer_MSTOP_TIMESTOP_Pos         0U                                          /*!< SysTick Timer MSTOP: TIMESTOP bit Position */
#define SysTimer_MSTOP_TIMESTOP_Msk         (1UL << SysTimer_MSTOP_TIMESTOP_Pos)        /*!< SysTick Timer MSTOP: TIMESTOP Mask */
#define SysTimer_MSTOP_CMPCLREN_Pos         1U                                          /*!< SysTick Timer MSTOP: CMPCLREN bit Position, introduced in core version 1.4 */
#define SysTimer_MSTOP_CMPCLREN_Msk         (1UL << SysTimer_MSTOP_CMPCLREN_Pos)        /*!< SysTick Timer MSTOP: CMPCLREN Mask, introduced in core version 1.4 */
#define SysTimer_MSTOP_CLKSRC_Pos           2U                                          /*!< SysTick Timer MSTOP: CLKSRC bit Position, introduced in core version 1.4 */
#define SysTimer_MSTOP_CLKSRC_Msk           (1UL << SysTimer_MSTOP_CLKSRC_Pos)          /*!< SysTick Timer MSTOP: CLKSRC Mask, introduced in core version 1.4 */

#define SysTimer_MSIP_MSIP_Pos              0U                                          /*!< SysTick Timer MSIP: MSIP bit Position */
#define SysTimer_MSIP_MSIP_Msk              (1UL << SysTimer_MSIP_MSIP_Pos)             /*!< SysTick Timer MSIP: MSIP Mask */

#define SysTimer_MTIMER_Msk                 (0xFFFFFFFFFFFFFFFFULL)                     /*!< SysTick Timer MTIMER value Mask */
#define SysTimer_MTIMERCMP_Msk              (0xFFFFFFFFFFFFFFFFULL)                     /*!< SysTick Timer MTIMERCMP value Mask */
#define SysTimer_MSTOP_Msk                  (0xFFFFFFFFUL)                              /*!< SysTick Timer MSTOP  value Mask */
#define SysTimer_MSIP_Msk                   (0xFFFFFFFFUL)                              /*!< SysTick Timer MSIP   value Mask */
#define SysTimer_MSFTRST_Msk                (0xFFFFFFFFUL)                              /*!< SysTick Timer MSFTRST value Mask */

#define SysTimer_MSFRST_KEY                 (0x80000A5FUL)                              /*!< SysTick Timer Software Reset Request Key */

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
__STATIC_FORCEINLINE void SysTimer_SetLoadValue(uint64_t value)
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
__STATIC_FORCEINLINE uint64_t SysTimer_GetLoadValue(void)
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
__STATIC_FORCEINLINE void SysTimer_SetCompareValue(uint64_t value)
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
__STATIC_FORCEINLINE uint64_t SysTimer_GetCompareValue(void)
{
    return SysTimer->MTIMERCMP;
}

/**
 * \brief  Enable System Timer Counter Running
 * \details
 * Enable system timer counter running.
 */
__STATIC_FORCEINLINE void SysTimer_Start(void)
{
    SysTimer->MSTOP &= ~(SysTimer_MSTOP_TIMESTOP_Msk);
}

/**
 * \brief  Stop System Timer Counter Running
 * \details
 * Stop system timer counter running.
 */
__STATIC_FORCEINLINE void SysTimer_Stop(void)
{
    SysTimer->MSTOP |= SysTimer_MSTOP_TIMESTOP_Msk;
}

/**
 * \brief  Set system Timer MSTOP value
 * \details
 * This function set the system Timer MSTOP value.
 * \param [in]       mstop   set mstop value to MSTOP value
 * \remarks
 * - Bit TIMESTOP is used to start and stop timer.
 *   Clear TIMESTOP bit to 0 to start timer, otherwise to stop timer.
 * - Bit CMPCLREN is used to enable auto MTIMER clear to zero when MTIMER >= MTIMERCMP.
 *   Clear CMPCLREN bit to 0 to stop auto clear MTIMER feature, otherwise to enable it.
 * - Bit CLKSRC is used to select timer clock source.
 *   Clear CLKSRC bit to 0 to use mtime_toggle_a, otherwise use core_clk_aon
 * - \ref SysTimer_SetMstopValue
 */
__STATIC_FORCEINLINE void SysTimer_SetMstopValue(uint32_t mstop)
{
    SysTimer->MSTOP = (mstop & SysTimer_MSTOP_Msk);
}

/**
 * \brief  Get system Timer MSTOP value
 * \details
 * This function get the system Timer MSTOP register value.
 * \return  MSTOP register value
 * \remarks
 * - \ref SysTimer_SetMstopValue
 */
__STATIC_FORCEINLINE uint32_t SysTimer_GetMstopValue(void)
{
    return (SysTimer->MSTOP & SysTimer_MSTOP_Msk);
}

/**
 * \brief  Set system Timer MSIP to trigger SW interrupt
 * \details
 * This function set the system Timer MSIP bit.
 * \remarks
 * - Set system timer MSIP bit and generate a SW interrupt.
 * - \ref SysTimer_ClearSWIRQ
 * - \ref SysTimer_GetMsipValue
 */
__STATIC_FORCEINLINE void SysTimer_SetSWIRQ(void)
{
    SysTimer->MSIP |= SysTimer_MSIP_MSIP_Msk;
}

/**
 * \brief  Clear system Timer MSIP to clear SW interrupt
 * \details
 * This function set the system Timer MSIP bit.
 * \remarks
 * - Clear system timer MSIP bit and Clear the SW interrupt.
 * - \ref SysTimer_SetSWIRQ
 * - \ref SysTimer_GetMsipValue
 */
__STATIC_FORCEINLINE void SysTimer_ClearSWIRQ(void)
{
    SysTimer->MSIP &= ~SysTimer_MSIP_MSIP_Msk;
}

/**
 * \brief  Get system Timer MSIP
 * \details
 * This function get the system Timer MSIP register value.
 * \return    Value of Timer MSIP register.
 * \remarks
 * - Bit0 is SW interrupt flag.
 *   Bit0 is 1 then SW interrupt set. Bit0 is 0 then SW interrupt clear.
 * - \ref SysTimer_SetSWIRQ
 * - \ref SysTimer_ClearSWIRQ
 */
__STATIC_FORCEINLINE uint32_t SysTimer_GetMsipValue(void)
{
    return (uint32_t)(SysTimer->MSIP & SysTimer_MSIP_Msk);
}

/**
 * \brief  Set system Timer MSIP
 * \details
 * This function set the system Timer MSIP register value.
 */
__STATIC_FORCEINLINE void SysTimer_SetMsipValue(uint32_t msip)
{
    SysTimer->MSIP = (msip & SysTimer_MSIP_Msk);
}

/**
 * \brief  Do software reset request
 * \details
 * This function will do software reset request through MTIMER
 * - Software need to write \ref SysTimer_MSFRST_KEY to generate software reset request
 * - The software request flag can be cleared by reset operation to clear
 * \remarks
 * - The software reset is sent to SoC, SoC need to generate reset signal and send back to Core
 * - This function will not return, it will do while(1) to wait the Core reset happened
 */
__STATIC_FORCEINLINE void SysTimer_SoftwareReset(void)
{
    SysTimer->MSFTRST = SysTimer_MSFRST_KEY;
    while(1);
}

#if defined (__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U) && defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
/**
 * \brief   System Tick Configuration
 * \details Initializes the System Timer and its non-vector interrupt, and starts the System Tick Timer.
 *
 *  In our default implementation, the timer counter will be set to zero, and it will start a timer compare non-vector interrupt
 *  when it matchs the ticks user set, during the timer interrupt user should reload the system tick using \ref SysTick_Reload function
 *  or similar function written by user, so it can produce period timer interrupt.
 * \param [in]  ticks  Number of ticks between two interrupts.
 * \return          0  Function succeeded.
 * \return          1  Function failed.
 * \remarks
 * - For \ref __NUCLEI_N_REV >= 0x0104, the CMPCLREN bit in MSTOP is introduced,
 *   so we will use this feature to let Nuclei Core automatically reload the MTIMER register value to zero.
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
#if defined(__NUCLEI_N_REV) && (__NUCLEI_N_REV < 0x0104)
#else
    SysTimer_SetMstopValue(SysTimer_GetMstopValue() | SysTimer_MSTOP_CMPCLREN_Msk);
#endif
    SysTimer_SetLoadValue(0);
    SysTimer_SetCompareValue(ticks);
    ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ(SysTimer_IRQn, 1);
    ECLIC_EnableIRQ(SysTimer_IRQn);
    return (0UL);
}

/**
 * \brief   System Tick Reload
 * \details Reload the System Timer Tick when the MTIMECMP reached TIME value
 *
 * \param [in]  ticks  Number of ticks between two interrupts.
 * \return          0  Function succeeded.
 * \return          1  Function failed.
 * \remarks
 * - For \ref __NUCLEI_N_REV >= 0x0104, the CMPCLREN bit in MSTOP is introduced,
 *   so we will use this feature to let Nuclei Core automatically reload the MTIMER register value to zero.
 *   Otherwise, we will set the (ticks + MTIMER) value to MTIMERCMP register, and the MTIMER register will not be modified
 * - When the variable \ref __Vendor_SysTickConfig is set to 1, then the
 *   function \ref SysTick_Reload is not included.
 * - In this case, the file **<Device>.h** must contain a vendor-specific implementation
 *   of this function.
 * - This function only available when __SYSTIMER_PRESENT == 1 and __ECLIC_PRESENT == 1 and __Vendor_SysTickConfig == 0
 * \sa
 * - \ref SysTimer_SetCompareValue
 * - \ref SysTimer_SetLoadValue
 */
__STATIC_FORCEINLINE uint32_t SysTick_Reload(uint64_t ticks)
{
#if defined(__NUCLEI_N_REV) && (__NUCLEI_N_REV < 0x0104)
    SysTimer->MTIMERCMP = ticks + SysTimer->MTIMER;
#endif
    return (0UL);
}

#endif /* defined(__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U) */
/** @} */ /* End of Doxygen Group NMSIS_Core_SysTimer */

#endif /* defined(__SYSTIMER_PRESENT) && (__SYSTIMER_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /** __CORE_FEATURE_TIMER_H__  */

