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

/**
 * \brief  Set nlbits value
 * \details
 * This function set the nlbits value of CLICCFG register.
 * \param [in]    nlbits    nlbits value
 * \remarks
 * - nlbits is used to set the width of level in the CLICINTCTL[i].
 * \sa
 * - \ref IRQC_GetCfgNlbits
 */
__STATIC_FORCEINLINE void IRQC_SetCfgNlbits(uint32_t nlbits)
{
    IRQC->CFG &= ~CLIC_CLICCFG_NLBIT_Msk;
    IRQC->CFG |= (uint8_t)((nlbits <<CLIC_CLICCFG_NLBIT_Pos) & CLIC_CLICCFG_NLBIT_Msk);
}

/**
 * \brief  Get nlbits value
 * \details
 * This function get the nlbits value of CLICCFG register.
 * \return   nlbits value of CLICCFG register
 * \remarks
 * - nlbits is used to set the width of level in the CLICINTCTL[i].
 * \sa
 * - \ref IRQC_SetCfgNlbits
 */
__STATIC_FORCEINLINE uint32_t IRQC_GetCfgNlbits(void)
{
    return ((uint32_t)((IRQC->CFG & CLIC_CLICCFG_NLBIT_Msk) >> CLIC_CLICCFG_NLBIT_Pos));
}

/**
 * \brief  Get the IRQC version number
 * \details
 * This function gets the hardware version information from CLICINFO register.
 * \return   hardware version number in CLICINFO register.
 * \remarks
 * - This function gets harware version information from CLICINFO register.
 * - Bit 20:17 for architecture version, bit 16:13 for implementation version.
 * \sa
 * - \ref IRQC_GetInfoNum
*/
__STATIC_FORCEINLINE uint32_t IRQC_GetInfoVer(void)
{
    return ((uint32_t)((IRQC->INFO & CLIC_CLICINFO_VER_Msk) >> CLIC_CLICINFO_VER_Pos));
}

/**
 * \brief  Get CLICINTCTLBITS
 * \details
 * This function gets CLICINTCTLBITS from CLICINFO register.
 * \return  CLICINTCTLBITS from CLICINFO register.
 * \remarks
 * - In the CLICINTCTL[i] registers, with 2 <= CLICINTCTLBITS <= 8.
 * - The implemented bits are kept left-justified in the most-significant bits of each 8-bit
 *   CLICINTCTL[I] register, with the lower unimplemented bits treated as hardwired to 1.
 * \sa
 * - \ref IRQC_GetInfoNum
 */
__STATIC_FORCEINLINE uint32_t IRQC_GetInfoCtlbits(void)
{
    return ((uint32_t)((IRQC->INFO & CLIC_CLICINFO_CTLBIT_Msk) >> CLIC_CLICINFO_CTLBIT_Pos));
}

/**
 * \brief  Get number of maximum interrupt inputs supported
 * \details
 * This function gets number of maximum interrupt inputs supported from CLICINFO register.
 * \return  number of maximum interrupt inputs supported from CLICINFO register.
 * \remarks
 * - This function gets number of maximum interrupt inputs supported from CLICINFO register.
 * - The num_interrupt field specifies the actual number of maximum interrupt inputs supported in this implementation.
 * \sa
 * - \ref IRQC_GetInfoCtlbits
 */
__STATIC_FORCEINLINE uint32_t IRQC_GetInfoNum(void)
{
    return ((uint32_t)((IRQC->INFO & CLIC_CLICINFO_NUM_Msk) >> CLIC_CLICINFO_NUM_Pos));
}

/**
 * \brief  Set Machine Mode Interrupt Level Threshold
 * \details
 * This function sets machine mode interrupt level threshold.
 * \param [in]  mth       Interrupt Level Threshold.
 * \sa
 * - \ref IRQC_GetMth
 */
__STATIC_FORCEINLINE void IRQC_SetMth(uint8_t mth)
{
    IRQC->MTH = mth;
}

/**
 * \brief  Get Machine Mode Interrupt Level Threshold
 * \details
 * This function gets machine mode interrupt level threshold.
 * \return       Interrupt Level Threshold.
 * \sa
 * - \ref IRQC_SetMth
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetMth(void)
{
    return (IRQC->MTH);
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
    IRQC->CTRL[IRQn].INTIE |= CLIC_INTIE_IE_Msk;
}

/**
 * \brief  Get a specific interrupt enable status
 * \details
 * This function returns the interrupt enable status for the specific interrupt \em IRQn.
 * \param [in]  IRQn  Interrupt number
 * \returns
 * - 0  Interrupt is not enabled
 * - 1  Interrupt is pending
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_EnableIRQ
 * - \ref IRQC_DisableIRQ
 */
__STATIC_FORCEINLINE uint32_t IRQC_GetEnableIRQ(IRQn_Type IRQn)
{
    return ((uint32_t) (IRQC->CTRL[IRQn].INTIE) & CLIC_INTIE_IE_Msk);
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
    IRQC->CTRL[IRQn].INTIE &= ~CLIC_INTIE_IE_Msk;
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
    return ((uint32_t)(IRQC->CTRL[IRQn].INTIP) & CLIC_INTIP_IP_Msk);
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
    IRQC->CTRL[IRQn].INTIP |= CLIC_INTIP_IP_Msk;
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
    IRQC->CTRL[IRQn].INTIP &= ~CLIC_INTIP_IP_Msk;
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
    IRQC->CTRL[IRQn].INTATTR &= ~CLIC_INTATTR_TRIG_Msk;
    IRQC->CTRL[IRQn].INTATTR |= (uint8_t)(trig << CLIC_INTATTR_TRIG_Pos);
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
    return ((uint32_t)(((IRQC->CTRL[IRQn].INTATTR) & CLIC_INTATTR_TRIG_Msk) >> CLIC_INTATTR_TRIG_Pos));
}

/**
 * \brief  Set interrupt working mode for a specific interrupt
 * \details
 * This function set selective hardware vector or non-vector working mode of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      shv
 *                        - 0  non-vector mode, \ref IRQC_NON_VECTOR_INTERRUPT
 *                        - 1  vector mode, \ref IRQC_VECTOR_INTERRUPT
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_GetShvIRQ
 */
__STATIC_FORCEINLINE void IRQC_SetShvIRQ(IRQn_Type IRQn, uint32_t shv)
{
    IRQC->CTRL[IRQn].INTATTR &= ~CLIC_INTATTR_SHV_Msk;
    IRQC->CTRL[IRQn].INTATTR |= (uint8_t)(shv << CLIC_INTATTR_SHV_Pos);
}

/**
 * \brief  Get interrupt working mode for a specific interrupt
 * \details
 * This function get selective hardware vector or non-vector working mode of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return       shv
 *                        - 0  non-vector mode, \ref IRQC_NON_VECTOR_INTERRUPT
 *                        - 1  vector mode, \ref IRQC_VECTOR_INTERRUPT
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_SetShvIRQ
 */
__STATIC_FORCEINLINE uint32_t IRQC_GetShvIRQ(IRQn_Type IRQn)
{
    return ((uint32_t)(((IRQC->CTRL[IRQn].INTATTR) & CLIC_INTATTR_SHV_Msk) >> CLIC_INTATTR_SHV_Pos));
}

/**
 * \brief  Modify IRQC Interrupt Input Control Register for a specific interrupt
 * \details
 * This function modify IRQC Interrupt Input Control(CLICINTCTL[i]) register of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      intctrl  Set value for CLICINTCTL[i] register
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_GetCtrlIRQ
 */
__STATIC_FORCEINLINE void IRQC_SetCtrlIRQ(IRQn_Type IRQn, uint8_t intctrl)
{
    IRQC->CTRL[IRQn].INTCTRL = intctrl;
}

/**
 * \brief  Get IRQC Interrupt Input Control Register value for a specific interrupt
 * \details
 * This function modify IRQC Interrupt Input Control register of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return       value of IRQC Interrupt Input Control register
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_SetCtrlIRQ
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetCtrlIRQ(IRQn_Type IRQn)
{
    return (IRQC->CTRL[IRQn].INTCTRL);
}

/**
 * \brief  Set IRQC Interrupt level of a specific interrupt
 * \details
 * This function set interrupt level of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      lvl_abs   Interrupt level
 * \remarks
 * - IRQn must not be negative.
 * - If lvl_abs to be set is larger than the max level allowed, it will be force to be max level.
 * - When you set level value you need use clciinfo.nlbits to get the width of level.
 *   Then we could know the maximum of level. CLICINTCTLBITS is how many total bits are
 *   present in the CLICINTCTL register.
 * \sa
 * - \ref IRQC_GetLevelIRQ
 */
__STATIC_FORCEINLINE void IRQC_SetLevelIRQ(IRQn_Type IRQn, uint8_t lvl_abs)
{
    uint8_t nlbits = IRQC_GetCfgNlbits();
    uint8_t intctlbits = (uint8_t)IRQC_INTCTLBITS;

    if (nlbits == 0) {
        return;
    }

    if (nlbits > intctlbits) {
        nlbits = intctlbits;
    }
    uint8_t maxlvl = ((1 << nlbits) - 1);
    if (lvl_abs > maxlvl) {
        lvl_abs = maxlvl;
    }
    uint8_t lvl = lvl_abs << (IRQC_MAX_NLBITS - nlbits);
    uint8_t cur_ctrl = IRQC_GetCtrlIRQ(IRQn);
    cur_ctrl = cur_ctrl << nlbits;
    cur_ctrl = cur_ctrl >> nlbits;
    IRQC_SetCtrlIRQ(IRQn, (cur_ctrl | lvl));
}

/**
 * \brief  Get IRQC Interrupt level of a specific interrupt
 * \details
 * This function get interrupt level of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return         Interrupt level
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_SetLevelIRQ
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetLevelIRQ(IRQn_Type IRQn)
{
    uint8_t nlbits = IRQC_GetCfgNlbits();
    uint8_t intctlbits = (uint8_t)IRQC_INTCTLBITS;

    if (nlbits == 0) {
        return 0;
    }

    if (nlbits > intctlbits) {
        nlbits = intctlbits;
    }
    uint8_t intctrl = IRQC_GetCtrlIRQ(IRQn);
    uint8_t lvl_abs = intctrl >> (IRQC_MAX_NLBITS - nlbits);
    return lvl_abs;
}

/**
 * \brief  Get IRQC Interrupt priority of a specific interrupt
 * \details
 * This function get interrupt priority of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      pri   Interrupt priority
 * \remarks
 * - IRQn must not be negative.
 * - If pri to be set is larger than the max priority allowed, it will be force to be max priority.
 * - Priority width is CLICINTCTLBITS minus clciinfo.nlbits if clciinfo.nlbits
 *   is less than CLICINTCTLBITS. Otherwise priority width is 0.
 * \sa
 * - \ref IRQC_GetPriorityIRQ
 */
__STATIC_FORCEINLINE void IRQC_SetPriorityIRQ(IRQn_Type IRQn, uint8_t pri)
{
    uint8_t nlbits = IRQC_GetCfgNlbits();
    uint8_t intctlbits = (uint8_t)IRQC_INTCTLBITS;
    if (nlbits < intctlbits) {
        uint8_t maxpri = ((1 << (intctlbits - nlbits)) - 1);
        if (pri > maxpri) {
            pri = maxpri;
        }
        pri = pri << (IRQC_MAX_NLBITS - intctlbits);
        uint8_t mask = ((uint8_t)(-1)) >> intctlbits;
        pri = pri | mask;
        uint8_t cur_ctrl = IRQC_GetCtrlIRQ(IRQn);
        cur_ctrl = cur_ctrl >> (IRQC_MAX_NLBITS - nlbits);
        cur_ctrl = cur_ctrl << (IRQC_MAX_NLBITS - nlbits);
        IRQC_SetCtrlIRQ(IRQn, (cur_ctrl | pri));
    }
}

/**
 * \brief  Get IRQC Interrupt priority of a specific interrupt
 * \details
 * This function get interrupt priority of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return   Interrupt priority
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_SetPriorityIRQ
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetPriorityIRQ(IRQn_Type IRQn)
{
    uint8_t nlbits = IRQC_GetCfgNlbits();
    uint8_t intctlbits = (uint8_t)IRQC_INTCTLBITS;
    if (nlbits < intctlbits) {
        uint8_t cur_ctrl = IRQC_GetCtrlIRQ(IRQn);
        uint8_t pri = cur_ctrl << nlbits;
        pri = pri >> nlbits;
        pri = pri >> (IRQC_MAX_NLBITS - intctlbits);
        return pri;
    } else {
        return 0;
    }
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
#if (defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1))
#if (defined(__CCM_PRESENT) && (__CCM_PRESENT == 1))
    MFlushDCacheLine((unsigned long)vec_base);
#endif
#endif
#if (defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1))
#if (defined(__CCM_PRESENT) && (__CCM_PRESENT == 1))
    MInvalICacheLine((unsigned long)vec_base);
#else
    __FENCE_I();
#endif
#endif
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
#if __RISCV_XLEN == 32
    return (*(uint32_t *)(__RV_CSR_READ(CSR_MTVT) + IRQn * 4));
#elif __RISCV_XLEN == 64
    return (*(uint64_t *)(__RV_CSR_READ(CSR_MTVT) + IRQn * 8));
#else // TODO Need cover for XLEN=128 case in future
    return (*(uint64_t *)(__RV_CSR_READ(CSR_MTVT) + IRQn * 8));
#endif
}

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
/**
 * \brief  Set privilege mode of a specific interrupt
 * \details
 * This function set in which privilege mode the interrupts \em IRQn should be taken.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      mode   Privilege mode
 * \remarks
 * - IRQn must not be negative.
 * - mode must be 1(Supervisor Mode) or 3(Machine Mode), other values are ignored.
 * - M-mode can R/W this field, but S-mode can only read.And IRQC with TEE does not
 *   reply on CSR mideleg to delegate interrupts.
 * - Mode of S-mode IRQC region's clicintattr can be omitted to set, which is mirror to M-mode IRQC region's.
 *   Only the low 6 bits of clicintattr [i] can be written via the S-mode memory region.
 */
__STATIC_FORCEINLINE void IRQC_SetModeIRQ(IRQn_Type IRQn, uint32_t mode)
{
    /*
     * only 1 or 3 can be assigned to mode in one step.the default value of mode is 3,
     * which can't be clear to 0 firstly, then OR it to 1
     */
    IRQC->CTRL[IRQn].INTATTR = (uint8_t)(mode << CLIC_INTATTR_MODE_Pos) + \
        (IRQC->SCTRL[IRQn].INTATTR & (~CLIC_INTATTR_MODE_Msk));
}

/**
 * \brief  Set supervisor-mode Interrupt Level Threshold in supervisor mode
 * \details
 * This function sets supervisor-mode interrupt level threshold.
 * \param [in]  sth       Interrupt Level Threshold.
 * \remarks
 * - S-mode IRQC region sintthresh'sth is a mirror to M-mode IRQC region's mintthresh.sth,
 *   and will be updated synchronously, here operate on mintthresh.sth.
 * \sa
 * - \ref IRQC_GetSth
 */
__STATIC_FORCEINLINE void IRQC_SetSth(uint8_t sth)
{
    IRQC->STH = sth;
}

/**
 * \brief  Get supervisor-mode Interrupt Level Threshold in supervisor mode
 * \details
 * This function gets supervisor mode interrupt level threshold.
 * \return       Interrupt Level Threshold.
 * \remarks
 * - S-mode IRQC region sintthresh'sth is a mirror to M-mode IRQC region's mintthresh.sth,
 *   and will be updated synchronously, here operate on mintthresh.sth.
 * \sa
 * - \ref IRQC_SetSth
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetSth(void)
{
    return (IRQC->STH);
}

/**
 * \brief  Set trigger mode and polarity for a specific interrupt in supervisor mode
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
 * - \ref IRQC_GetTrigIRQ_S
 */
__STATIC_FORCEINLINE void IRQC_SetTrigIRQ_S(IRQn_Type IRQn, uint32_t trig)
{
    IRQC->SCTRL[IRQn].INTATTR &= ~CLIC_INTATTR_TRIG_Msk;
    IRQC->SCTRL[IRQn].INTATTR |= (uint8_t)(trig << CLIC_INTATTR_TRIG_Pos);
}

/**
 * \brief  Get trigger mode and polarity for a specific interrupt in supervisor mode
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
 *     - \ref IRQC_SetTrigIRQ_S
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetTrigIRQ_S(IRQn_Type IRQn)
{
    return ((uint8_t)(((IRQC->SCTRL[IRQn].INTATTR) & CLIC_INTATTR_TRIG_Msk) >> CLIC_INTATTR_TRIG_Pos));
}


/**
 * \brief  Set interrupt working mode for a specific interrupt in supervisor mode
 * \details
 * This function set selective hardware vector or non-vector working mode of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      shv
 *                        - 0  non-vector mode, \ref IRQC_NON_VECTOR_INTERRUPT
 *                        - 1  vector mode, \ref IRQC_VECTOR_INTERRUPT
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_GetShvIRQ_S
 */
__STATIC_FORCEINLINE void IRQC_SetShvIRQ_S(IRQn_Type IRQn, uint32_t shv)
{
    IRQC->SCTRL[IRQn].INTATTR &= ~CLIC_INTATTR_SHV_Msk;
    IRQC->SCTRL[IRQn].INTATTR |= (uint8_t)(shv << CLIC_INTATTR_SHV_Pos);
}

/**
 * \brief  Get interrupt working mode for a specific interrupt in supervisor mode
 * \details
 * This function get selective hardware vector or non-vector working mode of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return       shv
 *                        - 0  non-vector mode, \ref IRQC_NON_VECTOR_INTERRUPT
 *                        - 1  vector mode, \ref IRQC_VECTOR_INTERRUPT
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_SMODE_SetShvIRQ
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetShvIRQ_S(IRQn_Type IRQn)
{
    return ((uint8_t)(((IRQC->SCTRL[IRQn].INTATTR) & CLIC_INTATTR_SHV_Msk) >> CLIC_INTATTR_SHV_Pos));
}

/**
 * \brief  Modify IRQC Interrupt Input Control Register for a specific interrupt in supervisor mode
 * \details
 * This function modify IRQC Interrupt Input Control(CLICINTCTL[i]) register of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      intctrl  Set value for CLICINTCTL[i] register
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_GetCtrlIRQ_S
 */
__STATIC_FORCEINLINE void IRQC_SetCtrlIRQ_S(IRQn_Type IRQn, uint8_t intctrl)
{
    IRQC->SCTRL[IRQn].INTCTRL = intctrl;
}

/**
 * \brief  Get IRQC Interrupt Input Control Register value for a specific interrupt in supervisor mode
 * \details
 * This function modify IRQC Interrupt Input Control register of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return       value of IRQC Interrupt Input Control register
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_SetCtrlIRQ_S
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetCtrlIRQ_S(IRQn_Type IRQn)
{
    return (IRQC->SCTRL[IRQn].INTCTRL);
}

/**
 * \brief  Set IRQC Interrupt level of a specific interrupt in supervisor mode
 * \details
 * This function set interrupt level of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      lvl_abs   Interrupt level
 * \remarks
 * - IRQn must not be negative.
 * - If lvl_abs to be set is larger than the max level allowed, it will be force to be max level.
 * - When you set level value you need use clciinfo.nlbits to get the width of level.
 *   Then we could know the maximum of level. CLICINTCTLBITS is how many total bits are
 *   present in the CLICINTCTL register.
 * \sa
 * - \ref IRQC_GetLevelIRQ_S
 */
__STATIC_FORCEINLINE void IRQC_SetLevelIRQ_S(IRQn_Type IRQn, uint8_t lvl_abs)
{
    uint8_t nlbits = IRQC_GetCfgNlbits();
    uint8_t intctlbits = (uint8_t)IRQC_INTCTLBITS;

    if (nlbits == 0) {
        return;
    }

    if (nlbits > intctlbits) {
        nlbits = intctlbits;
    }
    uint8_t maxlvl = ((1 << nlbits) - 1);
    if (lvl_abs > maxlvl) {
        lvl_abs = maxlvl;
    }
    uint8_t lvl = lvl_abs << (IRQC_MAX_NLBITS - nlbits);
    uint8_t cur_ctrl = IRQC_GetCtrlIRQ_S(IRQn);
    cur_ctrl = cur_ctrl << nlbits;
    cur_ctrl = cur_ctrl >> nlbits;
    IRQC_SetCtrlIRQ_S(IRQn, (cur_ctrl | lvl));
}


/**
 * \brief  Get IRQC Interrupt level of a specific interrupt
 * \details
 * This function get interrupt level of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return         Interrupt level
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_SetLevelIRQ_S
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetLevelIRQ_S(IRQn_Type IRQn)
{
    uint8_t nlbits = IRQC_GetCfgNlbits();
    uint8_t intctlbits = (uint8_t)IRQC_INTCTLBITS;

    if (nlbits == 0) {
        return 0;
    }

    if (nlbits > intctlbits) {
        nlbits = intctlbits;
    }
    uint8_t intctrl = IRQC_GetCtrlIRQ_S(IRQn);
    uint8_t lvl_abs = intctrl >> (IRQC_MAX_NLBITS - nlbits);
    return lvl_abs;
}

/**
 * \brief  Set IRQC Interrupt priority of a specific interrupt in supervisor mode
 * \details
 * This function get interrupt priority of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \param [in]      pri   Interrupt priority
 * \remarks
 * - IRQn must not be negative.
 * - If pri to be set is larger than the max priority allowed, it will be force to be max priority.
 * - Priority width is CLICINTCTLBITS minus clciinfo.nlbits if clciinfo.nlbits
 *   is less than CLICINTCTLBITS. Otherwise priority width is 0.
 * \sa
 * - \ref IRQC_GetPriorityIRQ_S
 */
__STATIC_FORCEINLINE void IRQC_SetPriorityIRQ_S(IRQn_Type IRQn, uint8_t pri)
{
    uint8_t nlbits = IRQC_GetCfgNlbits();
    uint8_t intctlbits = (uint8_t)IRQC_INTCTLBITS;
    if (nlbits < intctlbits) {
        uint8_t maxpri = ((1 << (intctlbits - nlbits)) - 1);
        if (pri > maxpri) {
            pri = maxpri;
        }
        pri = pri << (IRQC_MAX_NLBITS - intctlbits);
        uint8_t mask = ((uint8_t)(-1)) >> intctlbits;
        pri = pri | mask;
        uint8_t cur_ctrl = IRQC_GetCtrlIRQ_S(IRQn);
        cur_ctrl = cur_ctrl >> (IRQC_MAX_NLBITS - nlbits);
        cur_ctrl = cur_ctrl << (IRQC_MAX_NLBITS - nlbits);
        IRQC_SetCtrlIRQ_S(IRQn, (cur_ctrl | pri));
    }
}

/**
 * \brief  Get IRQC Interrupt priority of a specific interrupt in supervisor mode
 * \details
 * This function get interrupt priority of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return   Interrupt priority
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_SetPriorityIRQ_S
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetPriorityIRQ_S(IRQn_Type IRQn)
{
    uint8_t nlbits = IRQC_GetCfgNlbits();
    uint8_t intctlbits = (uint8_t)IRQC_INTCTLBITS;
    if (nlbits < intctlbits) {
        uint8_t cur_ctrl = IRQC_GetCtrlIRQ_S(IRQn);
        uint8_t pri = cur_ctrl << nlbits;
        pri = pri >> nlbits;
        pri = pri >> (IRQC_MAX_NLBITS - intctlbits);
        return pri;
    } else {
        return 0;
    }
}

/**
 * \brief  Enable a specific interrupt in supervisor mode
 * \details
 * This function enables the specific interrupt \em IRQn.
 * \param [in]  IRQn  Interrupt number
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_DisableIRQ
 */
__STATIC_FORCEINLINE void IRQC_EnableIRQ_S(IRQn_Type IRQn)
{
    IRQC->SCTRL[IRQn].INTIE |= CLIC_INTIE_IE_Msk;
}

/**
 * \brief  Get a specific interrupt enable status in supervisor mode
 * \details
 * This function returns the interrupt enable status for the specific interrupt \em IRQn in S MODE.
 * \param [in]  IRQn  Interrupt number
 * \returns
 * - 0  Interrupt is not masked
 * - 1  Interrupt is enabled
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_EnableIRQ_S
 * - \ref IRQC_DisableIRQ_S
 */
__STATIC_FORCEINLINE uint8_t IRQC_GetEnableIRQ_S(IRQn_Type IRQn)
{
    return ((uint8_t) (IRQC->SCTRL[IRQn].INTIE) & CLIC_INTIE_IE_Msk);
}

/**
 * \brief  Disable a specific interrupt in supervisor mode
 * \details
 * This function disables the specific interrupt \em IRQn.
 * \param [in]  IRQn  Number of the external interrupt to disable
 * \remarks
 * - IRQn must not be negative.
 * \sa
 * - \ref IRQC_EnableIRQ
 */
__STATIC_FORCEINLINE void IRQC_DisableIRQ_S(IRQn_Type IRQn)
{
    IRQC->SCTRL[IRQn].INTIE &= ~CLIC_INTIE_IE_Msk;
}

/**
 * \brief  Set Interrupt Vector of a specific interrupt in supervisor mode
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
 * - \ref IRQC_GetVector_S
 */
__STATIC_FORCEINLINE void IRQC_SetVector_S(IRQn_Type IRQn, rv_csr_t vector)
{
    volatile unsigned long vec_base;
    vec_base = ((unsigned long)__RV_CSR_READ(CSR_STVT));
    vec_base += ((unsigned long)IRQn) * sizeof(unsigned long);
    (* (unsigned long *) vec_base) = vector;
#if (defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1))
#if (defined(__CCM_PRESENT) && (__CCM_PRESENT == 1))
    SFlushDCacheLine((unsigned long)vec_base);
#endif
#endif
#if (defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1))
#if (defined(__CCM_PRESENT) && (__CCM_PRESENT == 1))
    SInvalICacheLine((unsigned long)vec_base);
#else
    __FENCE_I();
#endif
#endif
}

/**
 * \brief  Get Interrupt Vector of a specific interrupt in supervisor mode
 * \details
 * This function get interrupt handler address of the specific interrupt \em IRQn.
 * \param [in]      IRQn  Interrupt number
 * \return        Interrupt handler address
 * \remarks
 * - IRQn must not be negative.
 * - You can read \ref CSR_CSR_MTVT to get interrupt vector table entry address.
 * \sa
 *     - \ref IRQC_SMODE_SetVector
 */
__STATIC_FORCEINLINE rv_csr_t IRQC_GetVector_S(IRQn_Type IRQn)
{
#if __RISCV_XLEN == 32
    return (*(uint32_t *)(__RV_CSR_READ(CSR_STVT) + IRQn * 4));
#elif __RISCV_XLEN == 64
    return (*(uint64_t *)(__RV_CSR_READ(CSR_STVT) + IRQn * 8));
#else // TODO Need cover for XLEN=128 case in future
    return (*(uint64_t *)(__RV_CSR_READ(CSR_STVT) + IRQn * 8));
#endif
}

#endif /* defined(__TEE_PRESENT) && (__TEE_PRESENT == 1) */

/**
 * \brief  Set Exception entry address
 * \details
 * This function set exception handler address to 'CSR_MTVEC'.
 * \param [in]      addr  Exception handler address
 * \remarks
 * - This function use to set exception handler address to 'CSR_MTVEC'.
 *   Address need to be aligned to 64 bytes.
 * \sa
 * - \ref __get_exc_entry
 */
__STATIC_FORCEINLINE void __set_exc_entry(rv_csr_t addr)
{
    addr &= (rv_csr_t)(~0x3F);
    addr |= IRQC_MODE_MTVEC_Msk;
    __RV_CSR_WRITE(CSR_MTVEC, addr);
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
    return (addr & ~IRQC_MODE_MTVEC_Msk);
}

/**
 * \brief  Set Non-vector interrupt entry address
 * \details
 * This function set Non-vector interrupt address.
 * \param [in]      addr  Non-vector interrupt entry address
 * \remarks
 * - This function use to set non-vector interrupt entry address to 'CSR_MTVT2' if
 * - CSR_MTVT2 bit0 is 1. If 'CSR_MTVT2' bit0 is 0 then set address to 'CSR_MTVEC'
 * \sa
 * - \ref __get_nonvec_entry
 */
__STATIC_FORCEINLINE void __set_nonvec_entry(rv_csr_t addr)
{
    if (__RV_CSR_READ(CSR_MTVT2) & 0x1) {
        __RV_CSR_WRITE(CSR_MTVT2, addr | 0x01);
    } else {
        addr &= (rv_csr_t)(~0x3F);
        addr |= IRQC_MODE_MTVEC_Msk;
        __RV_CSR_WRITE(CSR_MTVEC, addr);
    }
}

/**
 * \brief  Get Non-vector interrupt entry address
 * \details
 * This function get Non-vector interrupt address.
 * \return      Non-vector interrupt handler address
 * \remarks
 * - This function use to get non-vector interrupt entry address from 'CSR_MTVT2' if
 * - CSR_MTVT2 bit0 is 1. If 'CSR_MTVT2' bit0 is 0 then get address from 'CSR_MTVEC'.
 * \sa
 * - \ref __set_nonvec_entry
 */
__STATIC_FORCEINLINE rv_csr_t __get_nonvec_entry(void)
{
    if (__RV_CSR_READ(CSR_MTVT2) & 0x1) {
        return __RV_CSR_READ(CSR_MTVT2) & (~(rv_csr_t)(0x1));
    } else {
        rv_csr_t addr = __RV_CSR_READ(CSR_MTVEC);
        return (addr & ~IRQC_MODE_MTVEC_Msk);
    }
}

/**
 * \brief  Get NMI interrupt entry from 'CSR_MNVEC'
 * \details
 * This function get NMI interrupt address from 'CSR_MNVEC'.
 * \return      NMI interrupt handler address
 * \remarks
 * - This function use to get NMI interrupt handler address from 'CSR_MNVEC'. If CSR_MMISC_CTL[9] = 1 'CSR_MNVEC'
 * - will be equal as mtvec. If CSR_MMISC_CTL[9] = 0 'CSR_MNVEC' will be equal as reset vector.
 * - NMI entry is defined via \ref CSR_MMISC_CTL, writing to \ref CSR_MNVEC will be ignored.
 */
__STATIC_FORCEINLINE rv_csr_t __get_nmi_entry(void)
{
    return __RV_CSR_READ(CSR_MNVEC);
}

/**
 * \brief   Save necessary CSRs into variables for vector interrupt nesting
 * \details
 * This macro is used to declare variables which are used for saving
 * CSRs(MCAUSE, MEPC, MSUB), and it will read these CSR content into
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
        rv_csr_t __msubm = __RV_CSR_READ(CSR_MSUBM);                        \
        __enable_irq();

/*! Save necessary CSRs into variables for vector interrupt nesting in supervisor mode */
#define SAVE_IRQ_CSR_CONTEXT_S()                                            \
        rv_csr_t __scause = __RV_CSR_READ(CSR_SCAUSE);                      \
        rv_csr_t __sepc = __RV_CSR_READ(CSR_SEPC);                          \
        __enable_irq_s();


/**
 * \brief   Restore necessary CSRs from variables for vector interrupt nesting
 * \details
 * This macro is used restore CSRs(MCAUSE, MEPC, MSUB) from pre-defined variables
 * in \ref SAVE_IRQ_CSR_CONTEXT macro.
 * \remarks
 * - Interrupt will be disabled after this macro is called
 * - It need to be used together with \ref SAVE_IRQ_CSR_CONTEXT
 */
#define RESTORE_IRQ_CSR_CONTEXT()                                           \
        __disable_irq();                                                    \
        __RV_CSR_WRITE(CSR_MSUBM, __msubm);                                 \
        __RV_CSR_WRITE(CSR_MEPC, __mepc);                                   \
        __RV_CSR_WRITE(CSR_MCAUSE, __mcause);

/*! Restore necessary CSRs from variables for vector interrupt nesting in supervisor mode */
#define RESTORE_IRQ_CSR_CONTEXT_S()                                         \
        __disable_irq_s();                                                  \
        __RV_CSR_WRITE(CSR_SEPC, __sepc);                                   \
        __RV_CSR_WRITE(CSR_SCAUSE, __scause);
/** @} */ /* End of Doxygen Group NMSIS_Core_IntExc */

#endif /* defined(__IRQC_PRESENT) && (__IRQC_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_IRQC__ */
