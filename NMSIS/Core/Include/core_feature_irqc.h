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
#ifndef __CORE_FEATURE_IRQC__
#define __CORE_FEATURE_IRQC__
/*!
 * @file     core_feature_irqc.h
 * @brief    IRQC feature API header file for Nuclei N100 Core
 */
/*
 * IRQC Feature Configuration Macro:
 * 1. __IRQC_PRESENT:  Define whether Enhanced Core Local Interrupt Controller (IRQC) Unit is present or not
 *   * 0: Not present
 *   * 1: Present
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "core_feature_base.h"

#if defined(__IRQC_PRESENT) && (__IRQC_PRESENT == 1)

/* ##########################   IRQC functions  #################################### */
/**
 * \defgroup   NMSIS_Core_IntExc        Interrupts and Exceptions
 * \brief Functions that manage interrupts and exceptions via the IRQC.
 *
 * @{
 */

// IRQC only support vector interrupt
#define IRQC_VECTOR_INTERRUPT                 0x1                                      /*!< Vector Interrupt Mode of IRQC */

/**
 * \brief  Enable a specific interrupt
 * \details
 * This function enables the specific interrupt \em IRQn.
 * \param [in]  IRQn  Interrupt number
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_DisableIRQ
 */
__STATIC_FORCEINLINE void IRQC_EnableIRQ(IRQn_Type IRQn)
{
    __RV_CSR_SET(CSR_IRQCIE, (1<<IRQn));
}

/**
 * \brief  Get a specific interrupt enable status
 * \details
 * This function returns the interrupt enable status for the specific interrupt \em IRQn.
 * \param [in]  IRQn  Interrupt number
 * \returns
 * - 0  Interrupt is not enabled
 * - 1  Interrupt is enabled
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_EnableIRQ
 * - \ref IRQC_DisableIRQ
 */
__STATIC_FORCEINLINE uint32_t IRQC_GetEnableIRQ(IRQn_Type IRQn)
{
    return (uint32_t) ((__RV_CSR_READ(CSR_IRQCIE)) >> (IRQn) & 0x1);
}

/**
 * \brief  Disable a specific interrupt
 * \details
 * This function disables the specific interrupt \em IRQn.
 * \param [in]  IRQn  Number of the external interrupt to disable
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_EnableIRQ
 */
__STATIC_FORCEINLINE void IRQC_DisableIRQ(IRQn_Type IRQn)
{
    __RV_CSR_CLEAR(CSR_IRQCIE, (1<<IRQn));
}

/**
 * \brief  Get the pending specific interrupt
 * \details
 * This function returns the pending status of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \returns
 * - 0  Interrupt is not pending
 * - 1  Interrupt is pending
 * \remarks
 * - IRQn must not be negative.
 * - Currently only level interrupt supported.
 */
__STATIC_FORCEINLINE int32_t IRQC_GetPendingIRQ(IRQn_Type IRQn)
{
    return (uint32_t) ((__RV_CSR_READ(CSR_IRQCIP)) >> (IRQn) & 0x1);
}

/**
 * \brief  Set Interrupt Vector of a specific interrupt
 * \details
 * This function set interrupt handler address of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      vector   Interrupt handler address
 * \remarks
 * - IRQn must not be negative.
 * - You can set the \ref CSR_CSR_MTVT to set interrupt vector table entry address.
 * - If your vector table is placed in readonly section, the vector for IRQn will not be modified.
 *   For this case, you need to use the correct irq handler name defined in your vector table as
 *   your irq handler function name.
 * - This function will only work correctly when the vector table is placed in an read-write enabled section.
 * \sa
 * - \ref IRQC_GetVector
 */
__STATIC_FORCEINLINE void IRQC_SetVector(IRQn_Type IRQn, rv_csr_t vector)
{
    volatile unsigned long vec_base;
    vec_base = ((unsigned long)__RV_CSR_READ(CSR_MTVT));
    vec_base += ((unsigned long)IRQn) * sizeof(unsigned long);
    (* (unsigned long *) vec_base) = vector;
    __FENCE_I();
}

/**
 * \brief  Get Interrupt Vector of a specific interrupt
 * \details
 * This function get interrupt handler address of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return        Interrupt handler address
 * \remarks
 * - IRQn must not be negative.
 * - You can read \ref CSR_CSR_MTVT to get interrupt vector table entry address.
 * \sa
 *     - \ref IRQC_SetVector
 */
__STATIC_FORCEINLINE rv_csr_t IRQC_GetVector(IRQn_Type IRQn)
{
    return (*(uint32_t *)(__RV_CSR_READ(CSR_MTVT) + IRQn * 4));
}

/**
 * \brief  Get Exception entry address
 * \details
 * This function get exception handler address from 'CSR_MTVEC'.
 * \return       Exception handler address
 * \remarks
 * - This function use to get exception handler address from 'CSR_MTVEC'.
 *   Address need to be aligned to 64 bytes.
 * \sa
 * - \ref __set_exc_entry
 */
__STATIC_FORCEINLINE rv_csr_t __get_exc_entry(void)
{
    unsigned long addr = __RV_CSR_READ(CSR_MTVEC);
    return (addr & ~(0x2F));
}

/** @} */ /* End of Doxygen Group NMSIS_Core_IntExc */

#endif /* defined(__IRQC_PRESENT) && (__IRQC_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_IRQC__ */
