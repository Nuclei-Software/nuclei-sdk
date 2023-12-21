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

/**\brief IRQC Trigger Enum for different Trigger Type */
typedef enum IRQC_TRIGGER {
    IRQC_LEVEL_TRIGGER = 0x0,          /*!< Level Triggerred, trig[0] = 0 */
    IRQC_POSTIVE_EDGE_TRIGGER = 0x1,   /*!< Postive/Rising Edge Triggered, trig[0] = 1, trig[1] = 0 */
    IRQC_NEGTIVE_EDGE_TRIGGER = 0x3,   /*!< Negtive/Falling Edge Triggered, trig[0] = 1, trig[1] = 1 */
    IRQC_MAX_TRIGGER = 0x3             /*!< MAX Supported Trigger Mode */
} IRQC_TRIGGER_Type;

/**
 * \brief  Get the IRQC Info
 * \details
 * This function gets the hardware information from IRQCINFO CSR register.
 * \return   IRQCINFO CSR register content.
 * \remarks
 * - Bit 5:0 for irq source number
*/
__STATIC_FORCEINLINE uint32_t IRQC_GetInfo(void)
{
    return __RV_CSR_READ(CSR_IRQCINFO);
}

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
 * \sa
 * - \ref IRQC_SetPendingIRQ
 * - \ref IRQC_ClearPendingIRQ
 */
__STATIC_FORCEINLINE int32_t IRQC_GetPendingIRQ(IRQn_Type IRQn)
{
    return (uint32_t) ((__RV_CSR_READ(CSR_IRQCIP)) >> (IRQn) & 0x1);
}

/**
 * \brief  Set a specific interrupt to pending
 * \details
 * This function sets the pending bit for the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_GetPendingIRQ
 * - \ref IRQC_ClearPendingIRQ
 */
__STATIC_FORCEINLINE void IRQC_SetPendingIRQ(IRQn_Type IRQn)
{
    __RV_CSR_SET(CSR_IRQCIP, (1<<IRQn));
}

/**
 * \brief  Clear a specific interrupt from pending
 * \details
 * This function removes the pending state of the specific interrupt \em IRQn.
 * \em IRQn cannot be a negative number.
 * \param [in]      IRQn  Interrupt number
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_SetPendingIRQ
 * - \ref IRQC_GetPendingIRQ
 */
__STATIC_FORCEINLINE void IRQC_ClearPendingIRQ(IRQn_Type IRQn)
{
    __RV_CSR_CLEAR(CSR_IRQCIP, (1<<IRQn));
}

/**
 * \brief  Set trigger mode and polarity for a specific interrupt
 * \details
 * This function set trigger mode and polarity of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      trig
 *                   - 00  level trigger, \ref IRQC_LEVEL_TRIGGER
 *                   - 01  positive edge trigger, \ref IRQC_POSTIVE_EDGE_TRIGGER
 *                   - 02  level trigger, \ref IRQC_LEVEL_TRIGGER
 *                   - 03  negative edge trigger, \ref IRQC_NEGTIVE_EDGE_TRIGGER
 * \remarks
 * - IRQn must not be negative.
 *
 * \sa
 * - \ref IRQC_GetTrigIRQ
 */
__STATIC_FORCEINLINE void IRQC_SetTrigIRQ(IRQn_Type IRQn, uint32_t trig)
{
    if (trig & 0x1) { // level trigger
        __RV_CSR_SET(CSR_IRQCLVL, (1<<IRQn));
    } else {
        __RV_CSR_CLEAR(CSR_IRQCLVL, (1<<IRQn));
        if (trig & 0x2) { // negative edge
            __RV_CSR_SET(CSR_IRQCEDGE, (1<<IRQn));
        } else {
            __RV_CSR_CLEAR(CSR_IRQCEDGE, (1<<IRQn));
        }
    }
}

/**
 * \brief  Get trigger mode and polarity for a specific interrupt
 * \details
 * This function get trigger mode and polarity of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return
 *                 - 00  level trigger, \ref IRQC_LEVEL_TRIGGER
 *                 - 01  positive edge trigger, \ref IRQC_POSTIVE_EDGE_TRIGGER
 *                 - 02  level trigger, \ref IRQC_LEVEL_TRIGGER
 *                 - 03  negative edge trigger, \ref IRQC_NEGTIVE_EDGE_TRIGGER
 * \remarks
 *     - IRQn must not be negative.
 * \sa
 *     - \ref IRQC_SetTrigIRQ
 */
__STATIC_FORCEINLINE uint32_t IRQC_GetTrigIRQ(IRQn_Type IRQn)
{
    unsigned long tmp;
    tmp = __RV_CSR_READ(CSR_IRQCLVL);

    if (tmp & 0x1) {
        return IRQC_LEVEL_TRIGGER;
    }
    tmp = __RV_CSR_READ(CSR_IRQCEDGE);
    if (tmp & 0x1) {
        return IRQC_NEGTIVE_EDGE_TRIGGER;
    }
    return IRQC_POSTIVE_EDGE_TRIGGER;
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


/**
 * \brief   Save necessary CSRs into variables for vector interrupt nesting
 * \details
 * This macro is used to declare variables which are used for saving
 * CSRs(MCAUSE, MEPC), and it will read these CSR content into
 * these variables, it need to be used in a vector-interrupt if nesting
 * is required.
 * \remarks
 * - Interrupt will be enabled after this macro is called
 * - It need to be used together with \ref RESTORE_IRQ_CSR_CONTEXT
 * - Don't use variable names __mcause, __mpec, __msubm in your ISR code
 * - If you want to enable interrupt nesting feature for vector interrupt,
 * you can do it like this:
 * \code
 * // __INTERRUPT attribute will generates function entry and exit sequences suitable
 * // for use in an interrupt handler when this attribute is present
 * __INTERRUPT void eclic_mtip_handler(void)
 * {
 *     // Must call this to save CSRs
 *     SAVE_IRQ_CSR_CONTEXT();
 *     // !!!Interrupt is enabled here!!!
 *     // !!!Higher priority interrupt could nest it!!!
 *
 *     // put you own interrupt handling code here
 *
 *     // Must call this to restore CSRs
 *     RESTORE_IRQ_CSR_CONTEXT();
 * }
 * \endcode
 */
#define SAVE_IRQ_CSR_CONTEXT()                                              \
        rv_csr_t __mcause = __RV_CSR_READ(CSR_MCAUSE);                      \
        rv_csr_t __mepc = __RV_CSR_READ(CSR_MEPC);                          \
        __enable_irq();


/**
 * \brief   Restore necessary CSRs from variables for vector interrupt nesting
 * \details
 * This macro is used restore CSRs(MCAUSE, MEPC) from pre-defined variables
 * in \ref SAVE_IRQ_CSR_CONTEXT macro.
 * \remarks
 * - Interrupt will be disabled after this macro is called
 * - It need to be used together with \ref SAVE_IRQ_CSR_CONTEXT
 */
#define RESTORE_IRQ_CSR_CONTEXT()                                           \
        __disable_irq();                                                    \
        __RV_CSR_WRITE(CSR_MEPC, __mepc);                                   \
        __RV_CSR_WRITE(CSR_MCAUSE, __mcause);

/** @} */ /* End of Doxygen Group NMSIS_Core_IntExc */

#endif /* defined(__IRQC_PRESENT) && (__IRQC_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_IRQC__ */
