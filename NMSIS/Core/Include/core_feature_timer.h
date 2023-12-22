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
 * @brief    Timer feature API header file for Nuclei N100 Core
 */
/*
 * System Timer Feature Configuration Macro:
 * 1. __TIMER_PRESENT: Must, Define whether Private System Timer is present or not.
 *   * 0: Not present
 *   * 1: Present
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "core_feature_base.h"

#if defined(__TIMER_PRESENT) && (__TIMER_PRESENT == 1)
/* ##################################    SysTimer function  ############################################ */
/**
 * \defgroup NMSIS_Core_SysTimer N100 SysTimer Functions
 * \brief    Functions that configure the Core System Timer.
 * @{
 */

#define SysTimer_MTIME_Msk                 (0x00FFFFFF)                     /*!< SysTick Timer MTIME value Mask */
#define SysTimer_MTIMECMP_Msk              (0x00FFFFFF)                     /*!< SysTick Timer MTIMECMP value Mask */

/**
 * \brief  Set system timer load value
 * \details
 * This function set the system timer load value in MTIME CSR register.
 * \param [in]  value   value to set system timer MTIME register.
 * \remarks
 * - Load value is 32bits wide.
 * - \ref SysTimer_GetLoadValue
 */
__STATIC_FORCEINLINE void SysTimer_SetLoadValue(uint32_t value)
{
    __RV_CSR_WRITE(CSR_MTIME, value);
}

/**
 * \brief  Get system timer load value
 * \details
 * This function get the system timer current value in MTIMER register.
 * \return  current value(32bit) of system timer MTIMER register.
 * \remarks
 * - Load value is 32bits wide.
 * - \ref SysTimer_SetLoadValue
 */
__STATIC_FORCEINLINE uint32_t SysTimer_GetLoadValue(void)
{
    return __RV_CSR_READ(CSR_MTIME);
}


/**
 * \brief  Set system timer compare value in machine mode
 * \details
 * This function set the system Timer compare value in MTIMECMP CSR register.
 * \param [in]  value   compare value to set system timer MTIMECMP CSR register.
 * \remarks
 * - Compare value is 32bits wide.
 * - If compare value is larger than current value timer interrupt generate.
 * - Modify the load value or compare value less to clear the interrupt.
 * - \ref SysTimer_GetCompareValue
 */
__STATIC_FORCEINLINE void SysTimer_SetCompareValue(uint32_t value)
{
    __RV_CSR_WRITE(CSR_MTIMECMP, value);
}

/**
 * \brief  Get system timer compare value in machine mode
 * \details
 * This function get the system timer compare value in MTIMECMP CSR register.
 * \return  compare value of system timer MTIMECMP CSR register.
 * \remarks
 * - Compare value is 32bits wide.
 * - \ref SysTimer_SetCompareValue
 */
__STATIC_FORCEINLINE uint32_t SysTimer_GetCompareValue(void)
{
    return __RV_CSR_READ(CSR_MTIMECMP);
}

/**
 * \brief  Enable system timer counter running
 * \details
 * Enable system timer counter running by clear
 * TIMESTOP bit in MSTOP CSR register.
 */
__STATIC_FORCEINLINE void SysTimer_Start(void)
{
    //__RV_CSR_CLEAR(CSR_MSTOP, 0x1);
}

/**
 * \brief  Stop system timer counter running
 * \details
 * Stop system timer counter running by set
 * TIMESTOP bit in MSTOP CSR register.
 */
__STATIC_FORCEINLINE void SysTimer_Stop(void)
{
    //__RV_CSR_SET(CSR_MSTOP, 0x1);
}
/**
 * \brief  Trigger or set software interrupt via system timer in machine mode
 * \details
 * This function set the system timer MSIP bit in MSIP CSR register.
 * \remarks
 * - Set system timer MSIP bit and generate a SW interrupt.
 * - \ref SysTimer_ClearSWIRQ
 */
__STATIC_FORCEINLINE void SysTimer_SetSWIRQ(void)
{
    __RV_CSR_SET(CSR_MSIP, 0x1);
}

/**
 * \brief  Clear system timer software interrupt pending request in machine mode
 * \details
 * This function clear the system timer MSIP bit in MSIP CSR register.
 * \remarks
 * - Clear system timer MSIP bit in MSIP register to clear the software interrupt pending.
 * - \ref SysTimer_SetSWIRQ
 */
__STATIC_FORCEINLINE void SysTimer_ClearSWIRQ(void)
{
    __RV_CSR_CLEAR(CSR_MSIP, 0x1);
}

#if defined (__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U) && defined(__IRQC_PRESENT) && (__IRQC_PRESENT == 1)
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
 * - When the variable \ref __Vendor_SysTickConfig is set to 1, then the
 *   function \ref SysTick_Config is not included.
 * - In this case, the file <b><Device>.h</b> must contain a vendor-specific implementation
 *   of this function.
 * - If user need this function to start a period timer interrupt, then in timer interrupt handler
 *   routine code, user should call \ref SysTick_Reload with ticks to reload the timer.
 * - This function only available when __TIMER_PRESENT == 1 and __IRQC_PRESENT == 1 and __Vendor_SysTickConfig == 0
 * \sa
 * - \ref SysTimer_SetCompareValue; SysTimer_SetLoadValue
 */
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)
{
    uint32_t loadticks = SysTimer_GetLoadValue();
    SysTimer_SetCompareValue(ticks + loadticks);
    SysTimer_Start();
    // TODO add IRQC related API
    IRQC_EnableIRQ(SysTimer_IRQn);
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
 * - When the variable \ref __Vendor_SysTickConfig is set to 1, then the
 *   function \ref SysTick_Reload is not included.
 * - In this case, the file <b><Device>.h</b> must contain a vendor-specific implementation
 *   of this function.
 * - This function only available when __TIMER_PRESENT == 1 and __IRQC_PRESENT == 1 and __Vendor_SysTickConfig == 0
 * - Since the MTIMECMP CSR value might overflow, if overflowed, MTIME CSR will be set to 0, and MTIMECMP CSR set to ticks
 * \sa
 * - \ref SysTimer_SetCompareValue
 * - \ref SysTimer_SetLoadValue
 */
__STATIC_FORCEINLINE uint32_t SysTick_Reload(uint32_t ticks)
{
    uint32_t cur_ticks = SysTimer_GetLoadValue();
    uint32_t reload_ticks = ticks + cur_ticks;

    // TODO: Need to consider mtimecmp  and mtime are 24bit long
    if (reload_ticks > SysTimer_MTIMECMP_Msk) { // overflow 24bits
        reload_ticks = reload_ticks - SysTimer_MTIMECMP_Msk - 1;
    }
    SysTimer_SetCompareValue(reload_ticks);

    return (0UL);
}

#endif /* defined(__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U) */
/** @} */ /* End of Doxygen Group NMSIS_Core_SysTimer */

#endif /* defined(__TIMER_PRESENT) && (__TIMER_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_TIMER_H__ */
