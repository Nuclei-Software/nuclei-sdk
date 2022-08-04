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
#ifndef __CORE_FEATURE_SPMP_H__
#define __CORE_FEATURE_SPMP_H__
/*!
 * @file     core_feature_spmp.h
 * @brief    sPMP feature API header file for Nuclei N/NX Core
 */
/*
 * sPMP Feature Configuration Macro:
 * 1. __SPMP_PRESENT:  Define whether supervisor Physical Memory Protection(sPMP) is present or not
 *   * 0: Not present
 *   * 1: Present
 * 2. __SPMP_ENTRY_NUM:  Define the number of sPMP entries, only 8 or 16 is configurable.
 */
#ifdef __cplusplus
 extern "C" {
#endif

#include "core_feature_base.h"

#if defined(__SPMP_PRESENT) && (__SPMP_PRESENT == 1)
/* ===== sPMP Operations ===== */
/**
 * \defgroup NMSIS_Core_SPMP_Functions sPMP Functions
 * \ingroup  NMSIS_Core
 * \brief    Functions that related to the RISCV supervisor-mode Phyiscal Memory Protection.
 * \details
 * Optional superviosr physical memory protection (sPMP) unit provides per-hart supervisor-mode
 * control registers to allow physical memory access privileges (read, write, execute)
 * to be specified for each physical memory region. The sPMP values are checked after the physical 
 * address to be accessed pass PMP checks described in the RISC-V privileged spec.
 *
 * Like PMP, the sPMP can supports region access control settings as small as four bytes.
 *
 *   @{
 */
#ifndef __SPMP_ENTRY_NUM
/* numbers of sPMP entries(__SPMP_ENTRY_NUM) should be defined in <Device.h> */
#error "__SPMP_ENTRY_NUM is not defined, please check!"
#endif

/**
 * \brief   Get sPMPCFGx Register by csr index
 * \details Return the content of the sPMPCFGx Register.
 * \param [in]    csr_idx    sPMPCFG CSR index(0-3)
 * \return        sPMPCFGx Register value
 * \remark
 * - For RV64, only csr_idx = 0 and csr_idx = 2 is allowed.
 *   spmpcfg0 and spmpcfg2 hold the configurations
 *   for the 16 sPMP entries, spmpcfg1 and spmpcfg3 are illegal
 * - For RV32, spmpcfg0–spmpcfg3, hold the configurations
 *   spmp0cfg–spmp15cfg for the 16 sPMP entries
 */
__STATIC_INLINE rv_csr_t __get_sPMPCFGx(uint32_t csr_idx)
{
    switch (csr_idx) {
        case 0: return __RV_CSR_READ(CSR_SPMPCFG0);
        case 1: return __RV_CSR_READ(CSR_SPMPCFG1);
        case 2: return __RV_CSR_READ(CSR_SPMPCFG2);
        case 3: return __RV_CSR_READ(CSR_SPMPCFG3);
        default: return 0;
    }
}

/**
 * \brief   Set sPMPCFGx by csr index
 * \details Write the given value to the sPMPCFGx Register.
 * \param [in]    csr_idx      sPMPCFG CSR index(0-3)
 * \param [in]    spmpcfg      sPMPCFGx Register value to set
 * \remark
 * - For RV64, only csr_idx = 0 and csr_idx = 2 is allowed.
 *   spmpcfg0 and spmpcfg2 hold the configurations
 *   for the 16 sPMP entries, spmpcfg1 and spmpcfg3 are illegal
 * - For RV32, spmpcfg0–spmpcfg3, hold the configurations
 *   spmp0cfg–spmp15cfg for the 16 sPMP entries
 */
__STATIC_INLINE void __set_sPMPCFGx(uint32_t csr_idx, rv_csr_t spmpcfg)
{
    switch (csr_idx) {
        case 0: __RV_CSR_WRITE(CSR_SPMPCFG0, spmpcfg); break;
        case 1: __RV_CSR_WRITE(CSR_SPMPCFG1, spmpcfg); break;
        case 2: __RV_CSR_WRITE(CSR_SPMPCFG2, spmpcfg); break;
        case 3: __RV_CSR_WRITE(CSR_SPMPCFG3, spmpcfg); break;
        default: return;
    }
}

/**
 * \brief   Get 8bit sPMPxCFG Register by sPMP entry index
 * \details Return the content of the sPMPxCFG Register.
 * \param [in]    idx    sPMP region index(0-15)
 * \return        sPMPxCFG Register value
 */
__STATIC_INLINE uint8_t __get_sPMPxCFG(uint32_t entry_idx)
{
    rv_csr_t spmpcfgx = 0;
    uint8_t csr_cfg_num = 0;
    uint16_t csr_idx = 0;
    uint16_t cfg_shift = 0;

    if (entry_idx >= __SPMP_ENTRY_NUM) return 0;

#if __RISCV_XLEN == 32
    csr_cfg_num = 4;
    csr_idx = entry_idx >> 2;
#elif __RISCV_XLEN == 64
    csr_cfg_num = 8;
    /* For RV64, spmpcfg0 and spmpcfg2 each hold 8 PMP entries, align by 2 */
    csr_idx = (entry_idx >> 2)  & ~1;
#else
    // TODO Add RV128 Handling
    return 0;
#endif
    spmpcfgx = __get_sPMPCFGx(csr_idx);
    /* 
     * first get specific pmpxcfg's order in one CSR composed of csr_cfg_num spmpxcfgs,
     * then get spmpxcfg's bit position in one CSR by left shift 3, each spmpxcfg size is one byte
     */
    cfg_shift = (entry_idx & (csr_cfg_num - 1)) << 3;
    
    /* read specific pmpxcfg register value */
    return (uint8_t)(__RV_EXTRACT_FIELD(spmpcfgx, 0xFF << cfg_shift));
}

/**
 * \brief   Set 8bit sPMPxCFG by spmp entry index
 * \details Set the given spmpxcfg value to the sPMPxCFG Register.
 * \param [in]    idx       sPMPx region index(0-15)
 * \param [in]    spmpxcfg  sPMPxCFG register value to set
 * - For RV32, 4 spmpxcfgs are densely packed into one CSR in order
 *   For RV64, 8 spmpxcfgs are densely packed into one CSR in order
 */
__STATIC_INLINE void __set_sPMPxCFG(uint32_t entry_idx, uint8_t spmpxcfg)
{
    rv_csr_t spmpcfgx = 0;
    uint8_t csr_cfg_num = 0;
    uint16_t csr_idx = 0;
    uint16_t cfg_shift = 0;
    if (entry_idx >= __SPMP_ENTRY_NUM) return;

#if __RISCV_XLEN == 32
    csr_cfg_num = 4;
    csr_idx = entry_idx >> 2;
#elif __RISCV_XLEN == 64
    csr_cfg_num = 8;
    /* For RV64, spmpcfg0 and spmpcfg2 each hold 8 PMP entries, align by 2 */
    csr_idx = (entry_idx >> 2)  & ~1;
#else
    // TODO Add RV128 Handling
    return;
#endif
    /* read specific spmpcfgx register value */
    spmpcfgx = __get_sPMPCFGx(csr_idx);
    /* 
     * first get specific pmpxcfg's order in one CSR composed of csr_cfg_num spmpxcfgs,
     * then get spmpxcfg's bit position in one CSR by left shift 3, each spmpxcfg size is one byte
     */
    cfg_shift = (entry_idx & (csr_cfg_num - 1)) << 3;

    spmpcfgx = __RV_INSERT_FIELD(spmpcfgx, 0xFFUL << cfg_shift, spmpxcfg);
    __set_sPMPCFGx(csr_idx, spmpcfgx);
}

/**
 * \brief   Get sPMPADDRx Register by CSR index
 * \details Return the content of the sPMPADDRx Register.
 * \param [in]    csr_idx    sPMP region CSR index(0-15)
 * \return        sPMPADDRx Register value
 */
__STATIC_INLINE rv_csr_t __get_sPMPADDRx(uint32_t csr_idx)
{
    switch (csr_idx) {
        case 0: return __RV_CSR_READ(CSR_SPMPADDR0);
        case 1: return __RV_CSR_READ(CSR_SPMPADDR1);
        case 2: return __RV_CSR_READ(CSR_SPMPADDR2);
        case 3: return __RV_CSR_READ(CSR_SPMPADDR3);
        case 4: return __RV_CSR_READ(CSR_SPMPADDR4);
        case 5: return __RV_CSR_READ(CSR_SPMPADDR5);
        case 6: return __RV_CSR_READ(CSR_SPMPADDR6);
        case 7: return __RV_CSR_READ(CSR_SPMPADDR7);
        case 8: return __RV_CSR_READ(CSR_SPMPADDR8);
        case 9: return __RV_CSR_READ(CSR_SPMPADDR9);
        case 10: return __RV_CSR_READ(CSR_SPMPADDR10);
        case 11: return __RV_CSR_READ(CSR_SPMPADDR11);
        case 12: return __RV_CSR_READ(CSR_SPMPADDR12);
        case 13: return __RV_CSR_READ(CSR_SPMPADDR13);
        case 14: return __RV_CSR_READ(CSR_SPMPADDR14);
        case 15: return __RV_CSR_READ(CSR_SPMPADDR15);
        default: return 0;
    }
}

/**
 * \brief   Set sPMPADDRx by CSR index
 * \details Write the given value to the sPMPADDRx Register.
 * \param [in]    csr_idx    sPMP region CSR index(0-15)
 * \param [in]    spmpaddr   sPMPADDRx Register value to set
 */
__STATIC_INLINE void __set_sPMPADDRx(uint32_t csr_idx, rv_csr_t spmpaddr)
{
    switch (csr_idx) {
        case 0: __RV_CSR_WRITE(CSR_SPMPADDR0, spmpaddr); break;
        case 1: __RV_CSR_WRITE(CSR_SPMPADDR1, spmpaddr); break;
        case 2: __RV_CSR_WRITE(CSR_SPMPADDR2, spmpaddr); break;
        case 3: __RV_CSR_WRITE(CSR_SPMPADDR3, spmpaddr); break;
        case 4: __RV_CSR_WRITE(CSR_SPMPADDR4, spmpaddr); break;
        case 5: __RV_CSR_WRITE(CSR_SPMPADDR5, spmpaddr); break;
        case 6: __RV_CSR_WRITE(CSR_SPMPADDR6, spmpaddr); break;
        case 7: __RV_CSR_WRITE(CSR_SPMPADDR7, spmpaddr); break;
        case 8: __RV_CSR_WRITE(CSR_SPMPADDR8, spmpaddr); break;
        case 9: __RV_CSR_WRITE(CSR_SPMPADDR9, spmpaddr); break;
        case 10: __RV_CSR_WRITE(CSR_SPMPADDR10, spmpaddr); break;
        case 11: __RV_CSR_WRITE(CSR_SPMPADDR11, spmpaddr); break;
        case 12: __RV_CSR_WRITE(CSR_SPMPADDR12, spmpaddr); break;
        case 13: __RV_CSR_WRITE(CSR_SPMPADDR13, spmpaddr); break;
        case 14: __RV_CSR_WRITE(CSR_SPMPADDR14, spmpaddr); break;
        case 15: __RV_CSR_WRITE(CSR_SPMPADDR15, spmpaddr); break;
        default: return;
    }
}

/**
 * \brief   Set sPMP entry by entry idx
 * \details Write the given value to the sPMPxCFG Register and sPMPADDRx.
 * \param [in]    entry_idx    sPMP entry index(0-15)
 * \param [in]    protection   L,X,W,R field of sPMP configuration register
 * \param [in]    addr         the sPMP memory region base address, must be 2^log2len aligned
 * \param [in]    log2len      size of memory region as power of 2
 * \remark
 * - If the sPMP granularity is 2^12(4KB) NAPOT range, log2len makes 12, and the like.
 * - TOR of A field in sPMP configuration register is not considered here.
 */
__STATIC_INLINE void __set_sPMPENTRYx(uint32_t entry_idx, uint32_t protection, unsigned long addr,
    unsigned long log2len)
{
    unsigned int cfg_shift, cfg_csr_idx, addr_csr_idx = 0;
    unsigned long cfgmask, addrmask = 0;
    unsigned long spmpcfg, spmpaddr = 0;
    uint8_t csr_cfg_num = 0;
    /* check parameters */
    if (entry_idx >= __SPMP_ENTRY_NUM || log2len > __RISCV_XLEN || log2len < SPMP_SHIFT) return;

    /* calculate PMP register and offset */
#if __RISCV_XLEN == 32
    csr_cfg_num = 4;
    cfg_csr_idx = (entry_idx >> 2);
#elif __RISCV_XLEN == 64
    csr_cfg_num = 8;
    cfg_csr_idx = ((entry_idx >> 2)) & ~1;
#else
    // TODO Add RV128 Handling
    return;
#endif
    /* 
     * first get specific pmpxcfg's order in one CSR composed of csr_cfg_num spmpxcfgs,
     * then get spmpxcfg's bit position in one CSR by left shift 3, each spmpxcfg size is one byte
     */
    cfg_shift = (entry_idx & (csr_cfg_num - 1)) << 3;
    addr_csr_idx = entry_idx;

    if (cfg_csr_idx < 0 || cfg_shift < 0) return;

    /* encode PMP config */
    protection |= (log2len == SPMP_SHIFT) ? SPMP_A_NA4 : SPMP_A_NAPOT;
    cfgmask = ~(0xFFUL << cfg_shift);
    spmpcfg = (__get_sPMPCFGx(cfg_csr_idx) & cfgmask);
    spmpcfg |= ((protection << cfg_shift) & ~cfgmask);

    /* encode PMP address */
    if (log2len == SPMP_SHIFT) {
        spmpaddr = (addr >> SPMP_SHIFT);
    } else {
        if (log2len == __RISCV_XLEN) {
            spmpaddr = -1UL;
        } else {
            addrmask = (1UL << (log2len - SPMP_SHIFT)) - 1;
            spmpaddr = ((addr >> SPMP_SHIFT) & ~addrmask);
            spmpaddr |= (addrmask >> 1);
        }
    }

    /* write csrs */
    __set_sPMPADDRx(addr_csr_idx, spmpaddr);
    __set_sPMPCFGx(cfg_csr_idx, spmpcfg);
}
/** @} */ /* End of Doxygen Group NMSIS_Core_SPMP_Functions */
#endif /* defined(__SPMP_PRESENT) && (__SPMP_PRESENT == 1) */

#ifdef __cplusplus
}
#endif 
#endif /** __CORE_FEATURE_SPMP_H__  */
