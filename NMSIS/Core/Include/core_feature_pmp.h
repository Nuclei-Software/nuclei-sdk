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
#ifndef __CORE_FEATURE_PMP_H__
#define __CORE_FEATURE_PMP_H__
/*!
 * @file     core_feature_pmp.h
 * @brief    PMP feature API header file for Nuclei N/NX Core
 */
/*
 * PMP Feature Configuration Macro:
 * 1. __PMP_PRESENT:  Define whether Physical Memory Protection(PMP) is present or not
 *   * 0: Not present
 *   * 1: Present
 * 2. __PMP_ENTRY_NUM:  Define the number of PMP entries, only 8 or 16 is configurable.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "core_feature_base.h"
#include "core_compatiable.h"

#if defined(__PMP_PRESENT) && (__PMP_PRESENT == 1)
/* ===== PMP Operations ===== */
/**
 * \defgroup NMSIS_Core_PMP_Functions   PMP Functions
 * \ingroup  NMSIS_Core
 * \brief    Functions that related to the RISCV Phyiscal Memory Protection.
 * \details
 * Optional physical memory protection (PMP) unit provides per-hart machine-mode
 * control registers to allow physical memory access privileges (read, write, execute)
 * to be specified for each physical memory region.
 *
 * The PMP can supports region access control settings as small as four bytes.
 *
 *   @{
 */
#ifndef __PMP_ENTRY_NUM
/* numbers of PMP entries(__PMP_ENTRY_NUM) should be defined in <Device.h> */
#error "__PMP_ENTRY_NUM is not defined, please check!"
#endif

typedef struct PMP_CONFIG {
    /**
     * set locking bit, addressing mode, read, write, and instruction execution permissions,
     * see \ref PMP_L, \ref PMP_R, \ref PMP_W, \ref PMP_X, .etc in <riscv_encoding.h>
     */
    unsigned int protection;
    /**
     * Size of memory region as power of 2, it has to be minimum 2 and maxium \ref __RISCV_XLEN according to the
     * hard-wired granularity 2^N bytes, if N = 12, then order has to be at least 12; if not, the order read out
     * is N though you configure less than N.
     */
    unsigned long order;
    /**
     * Base address of memory region
     * It must be 2^order aligned address
     */
    unsigned long base_addr;
} pmp_config;

/**
 * @brief PMPCFG list for RV32 (indices 0-7)
 *
 * For RV32, each PMPCFG register holds 4 PMP entries.
 * This list covers the first 32 entries (registers 0-7).
 */
#define PMPCFG_LIST_RV32_0_7 \
    X(0) X(1) X(2) X(3) X(4) X(5) X(6) X(7)

/**
 * @brief PMPCFG list for RV32 (indices 8-15)
 *
 * For RV32, each PMPCFG register holds 4 PMP entries.
 * This list covers entries 33-64 (registers 8-15).
 */
#define PMPCFG_LIST_RV32_8_15 \
    X(8) X(9) X(10) X(11) X(12) X(13) X(14) X(15)

/**
 * @brief PMPCFG list for RV64 (even indices 0-6)
 *
 * For RV64, each PMPCFG register holds 8 PMP entries.
 * This list covers the first 32 entries (registers 0,2,4,6).
 */
#define PMPCFG_LIST_RV64_0_6 \
    X(0) X(2) X(4) X(6)

/**
 * @brief PMPCFG list for RV64 (even indices 8-14)
 *
 * For RV64, each PMPCFG register holds 8 PMP entries.
 * This list covers entries 33-64 (registers 8,10,12,14).
 */
#define PMPCFG_LIST_RV64_8_14 \
    X(8) X(10) X(12) X(14)

/**
 * @brief Select appropriate PMPCFG list based on architecture and PMP entry count
 *
 * This macro defines the complete PMPCFG list according to:
 * - RISC-V architecture (RV32/RV64)
 * - Number of PMP entries (__PMP_ENTRY_NUM)
 *
 * @note Valid indices depend on configuration:
 * - RV32:
 *   - <=32 entries: indices 0-7
 *   - <=64 entries: indices 0-15
 * - RV64:
 *   - <=32 entries: indices 0,2,4,6
 *   - <=64 entries: indices 0,2,4,6,8,10,12,14
 */
#if __RISCV_XLEN == 32
    #if __PMP_ENTRY_NUM <= 32
        #define PMPCFG_LIST PMPCFG_LIST_RV32_0_7
    #elif __PMP_ENTRY_NUM <= 64
        #define PMPCFG_LIST PMPCFG_LIST_RV32_0_7 PMPCFG_LIST_RV32_8_15
    #else
        #error "Unsupported PMP_ENTRY_NUM value for RV32"
    #endif
#elif __RISCV_XLEN == 64
    #if __PMP_ENTRY_NUM <= 32
        #define PMPCFG_LIST PMPCFG_LIST_RV64_0_6
    #elif __PMP_ENTRY_NUM <= 64
        #define PMPCFG_LIST PMPCFG_LIST_RV64_0_6 PMPCFG_LIST_RV64_8_14
    #else
        #error "Unsupported PMP_ENTRY_NUM value for RV64"
    #endif
#else
    #error "Unsupported RISC-V architecture"
#endif

/**
 * @brief   Get PMPCFGx Register by CSR index
 * @details Return the content of the PMPCFGx Register.
 * @param [in]    csr_idx    PMPCFG CSR index (0-15 for RV32, 0,2,4,6,8,10,12,14 for RV64)
 * @return        PMPCFGx Register value
 *
 * @note Architecture-specific behavior:
 * - RV32:
 *   - Each register holds 4 PMP entries
 *   - Valid indices: 0 to ceil(__PMP_ENTRY_NUM/4)-1
 *   - 64 entries require indices 0-15
 * - RV64:
 *   - Each register holds 8 PMP entries
 *   - Only even indices are valid
 *   - 64 entries require indices 0,2,4,6,8,10,12,14
 *
 * @remark The function returns 0 for invalid indices to prevent illegal accesses.
 */
__STATIC_INLINE rv_csr_t __get_PMPCFGx(uint32_t csr_idx)
{
    switch (csr_idx) {
    #define X(n) case n: return __RV_CSR_READ(CSR_PMPCFG##n);
        PMPCFG_LIST
    #undef X
        default: return 0;
    }
}

/**
 * @brief   Set PMPCFGx by CSR index
 * @details Write the given value to the PMPCFGx Register.
 * @param [in]    csr_idx    PMPCFG CSR index (0-15 for RV32, 0,2,4,6,8,10,12,14 for RV64)
 * @param [in]    pmpcfg     PMPCFGx Register value to set
 *
 * @note Architecture-specific behavior:
 * - RV32:
 *   - Each register holds 4 PMP entries
 *   - Valid indices: 0 to ceil(__PMP_ENTRY_NUM/4)-1
 *   - 64 entries require indices 0-15
 * - RV64:
 *   - Each register holds 8 PMP entries
 *   - Only even indices are valid
 *   - 64 entries require indices 0,2,4,6,8,10,12,14
 *
 * @remark The function does nothing for invalid indices to prevent illegal accesses.
 */
__STATIC_INLINE void __set_PMPCFGx(uint32_t csr_idx, rv_csr_t pmpcfg)
{
    switch (csr_idx) {
    #define X(n) case n: __RV_CSR_WRITE(CSR_PMPCFG##n, pmpcfg); break;
        PMPCFG_LIST
    #undef X
        default: return;
    }
}

/**
 * \brief   Get 8bit PMPxCFG Register by PMP entry index
 * \details Return the content of the PMPxCFG Register.
 * \param [in]    entry_idx    PMP region index(0-63)
 * \return               PMPxCFG Register value
 */
__STATIC_INLINE uint8_t __get_PMPxCFG(uint32_t entry_idx)
{
    rv_csr_t pmpcfgx = 0;
    uint8_t csr_cfg_num = 0;
    uint16_t csr_idx = 0;
    uint16_t cfg_shift = 0;

    if (entry_idx >= __PMP_ENTRY_NUM) return 0;

#if __RISCV_XLEN == 32
    csr_cfg_num = 4;
    csr_idx = entry_idx >> 2;
#elif __RISCV_XLEN == 64
    csr_cfg_num = 8;
    /* For RV64, each PMPCFG register (pmpcfg0, pmpcfg2, etc.) holds 8 PMP entries */
    /* Only even-numbered CSRs are used, so we align the index by clearing the LSB */
    csr_idx = (entry_idx >> 2) & ~1;
#else
    // TODO Add RV128 Handling
    return 0;
#endif
    pmpcfgx = __get_PMPCFGx(csr_idx);
    /*
     * first get specific pmpxcfg's order in one CSR composed of csr_cfg_num pmpxcfgs,
     * then get pmpxcfg's bit position in one CSR by left shift 3(each pmpxcfg size is one byte)
     */
    cfg_shift = (entry_idx & (csr_cfg_num - 1)) << 3;

    /* read specific pmpxcfg register value */
    return (uint8_t)(__RV_EXTRACT_FIELD(pmpcfgx, 0xFF << cfg_shift));
}

/**
 * \brief   Set 8bit PMPxCFG by pmp entry index
 * \details Set the given pmpxcfg value to the PMPxCFG Register.
 * \param [in]    entry_idx      PMPx region index(0-63)
 * \param [in]    pmpxcfg  PMPxCFG register value to set
 * \remark
 * - For RV32, 4 pmpxcfgs are densely packed into one CSR in order
 *   For RV64, 8 pmpxcfgs are densely packed into one CSR in order
 */
__STATIC_INLINE void __set_PMPxCFG(uint32_t entry_idx, uint8_t pmpxcfg)
{
    rv_csr_t pmpcfgx = 0;
    uint8_t csr_cfg_num = 0;
    uint16_t csr_idx = 0;
    uint16_t cfg_shift = 0;
    if (entry_idx >= __PMP_ENTRY_NUM) return;

#if __RISCV_XLEN == 32
    csr_cfg_num = 4;
    csr_idx = entry_idx >> 2;
#elif __RISCV_XLEN == 64
    csr_cfg_num = 8;
    /* For RV64, pmpcfg0 and pmpcfg2 each hold 8 PMP entries, align by 2 */
    csr_idx = (entry_idx >> 2) & ~1;
#else
    // TODO Add RV128 Handling
    return;
#endif
    /* read specific pmpcfgx register value */
    pmpcfgx = __get_PMPCFGx(csr_idx);
    /*
     * first get specific pmpxcfg's order in one CSR composed of csr_cfg_num pmpxcfgs,
     * then get pmpxcfg's bit position in one CSR by left shift 3(each pmpxcfg size is one byte)
     */
    cfg_shift = (entry_idx & (csr_cfg_num - 1)) << 3;

    pmpcfgx = __RV_INSERT_FIELD(pmpcfgx, 0xFFUL << cfg_shift, pmpxcfg);
    __set_PMPCFGx(csr_idx, pmpcfgx);
}

/**
 * @brief Base PMPADDR list (indices 0-7)
 */
#define PMPADDR_LIST_BASE \
    X(0) X(1) X(2) X(3) X(4) X(5) X(6) X(7)

/**
 * @brief Extended PMPADDR list (indices 8-15)
 */
#define PMPADDR_LIST_8_15 \
    X(8) X(9) X(10) X(11) X(12) X(13) X(14) X(15)

/**
 * @brief Extended PMPADDR list (indices 16-31)
 */
#define PMPADDR_LIST_16_31 \
    X(16) X(17) X(18) X(19) X(20) X(21) X(22) X(23) \
    X(24) X(25) X(26) X(27) X(28) X(29) X(30) X(31)

/**
 * @brief Extended PMPADDR list (indices 32-63)
 */
#define PMPADDR_LIST_32_63 \
    X(32) X(33) X(34) X(35) X(36) X(37) X(38) X(39) \
    X(40) X(41) X(42) X(43) X(44) X(45) X(46) X(47) \
    X(48) X(49) X(50) X(51) X(52) X(53) X(54) X(55) \
    X(56) X(57) X(58) X(59) X(60) X(61) X(62) X(63)

/**
 * @brief Select appropriate PMPADDR list based on PMP_ENTRY_NUM
 *
 * This macro defines the complete PMPADDR list by combining base and
 * extended lists according to the configured number of PMP entries.
 *
 * @note The actual list is determined by the __PMP_ENTRY_NUM configuration:
 * - <=8: Only base list (0-7)
 * - <=16: Base + 8-15
 * - <=32: Base + 8-15 + 16-31
 * - <=64: Base + 8-15 + 16-31 + 32-63
 */
#if __PMP_ENTRY_NUM <= 8
#define PMPADDR_LIST PMPADDR_LIST_BASE
#elif __PMP_ENTRY_NUM <= 16
#define PMPADDR_LIST PMPADDR_LIST_BASE PMPADDR_LIST_8_15
#elif __PMP_ENTRY_NUM <= 32
#define PMPADDR_LIST PMPADDR_LIST_BASE PMPADDR_LIST_8_15 PMPADDR_LIST_16_31
#elif __PMP_ENTRY_NUM <= 64
#define PMPADDR_LIST PMPADDR_LIST_BASE PMPADDR_LIST_8_15 PMPADDR_LIST_16_31 PMPADDR_LIST_32_63
#else
#error "Unsupported PMP_ENTRY_NUM value"
#endif

/**
 * \brief   Get PMPADDRx Register by CSR index
 * \details Return the content of the PMPADDRx Register.
 * \param [in]    csr_idx    PMP region CSR index(0-63)
 * \return        PMPADDRx Register value
 */
__STATIC_INLINE rv_csr_t __get_PMPADDRx(uint32_t csr_idx)
{
    switch (csr_idx) {
    #define X(n) case n: return __RV_CSR_READ(CSR_PMPADDR##n);
        PMPADDR_LIST
    #undef X
        default: return 0;
    }
}

/**
 * \brief   Set PMPADDRx by CSR index
 * \details Write the given value to the PMPADDRx Register.
 * \param [in]    csr_idx  PMP region CSR index(0-63)
 * \param [in]    pmpaddr  PMPADDRx Register value to set
 */
__STATIC_INLINE void __set_PMPADDRx(uint32_t csr_idx, rv_csr_t pmpaddr)
{
    switch (csr_idx) {
    #define X(n) case n: __RV_CSR_WRITE(CSR_PMPADDR##n, pmpaddr); break;
        PMPADDR_LIST
    #undef X
        default: return;
    }
}

/**
 * \brief   Set PMP entry by entry idx
 * \details Write the given value to the PMPxCFG Register and PMPADDRx.
 * \param [in]    entry_idx    PMP entry index(0-63)
 * \param [in]    pmp_cfg      structure of L, X, W, R field of PMP configuration register, memory region base address
 *                and size of memory region as power of 2
 * \remark
 * - If the size of memory region is 2^12(4KB) range, pmp_cfg->order makes 12, and the like.
 * - Suppose the size of memory region is 2^X bytes range, if X >=3, the NA4 mode is not selectable, NAPOT is selected.
 * - TOR of A field in PMP configuration register is not considered here.
 */
__STATIC_INLINE void __set_PMPENTRYx(uint32_t entry_idx, const pmp_config *pmp_cfg)
{
    unsigned int cfg_shift, cfg_csr_idx, addr_csr_idx = 0;
    unsigned long cfgmask, addrmask = 0;
    unsigned long pmpcfg, pmpaddr = 0;
    unsigned long protection, csr_cfg_num = 0;
    /* check parameters */
    if (entry_idx >= __PMP_ENTRY_NUM || pmp_cfg->order > __RISCV_XLEN || pmp_cfg->order < PMP_SHIFT) return;

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
     * first get specific pmpxcfg's order in one CSR composed of csr_cfg_num pmpxcfgs,
     * then get pmpxcfg's bit position in one CSR by left shift 3, each pmpxcfg size is one byte
     */
    cfg_shift = (entry_idx & (csr_cfg_num - 1)) << 3;
    addr_csr_idx = entry_idx;

    /* encode PMP config */
    protection = (unsigned long)pmp_cfg->protection;
    protection |= (PMP_SHIFT == pmp_cfg->order) ? PMP_A_NA4 : PMP_A_NAPOT;
    cfgmask = ~(0xFFUL << cfg_shift);
    pmpcfg = (__get_PMPCFGx(cfg_csr_idx) & cfgmask);
    pmpcfg |= ((protection << cfg_shift) & ~cfgmask);

    /* encode PMP address */
    if (PMP_SHIFT == pmp_cfg->order) { /* NA4 */
        pmpaddr = (pmp_cfg->base_addr >> PMP_SHIFT);
    } else { /* NAPOT */
        addrmask = (1UL << (pmp_cfg->order - PMP_SHIFT)) - 1;
        pmpaddr = ((pmp_cfg->base_addr >> PMP_SHIFT) & ~addrmask);
        pmpaddr |= (addrmask >> 1);
    }
    /*
     * write csrs, update the address first, in case the entry is locked that
     * we won't be able to modify it after we set the config csr.
     */
    __set_PMPADDRx(addr_csr_idx, pmpaddr);
    __set_PMPCFGx(cfg_csr_idx, pmpcfg);
}

/**
 * \brief   Get PMP entry by entry idx
 * \details Write the given value to the PMPxCFG Register and PMPADDRx.
 * \param [in]     entry_idx     PMP entry index(0-63)
 * \param [out]    pmp_cfg       structure of L, X, W, R, A field of PMP configuration register, memory region base
 *                 address and size of memory region as power of 2
 * \return  -1 failure, else 0 success
 * \remark
 * - If the size of memory region is 2^12(4KB) range, pmp_cfg->order makes 12, and the like.
 * - TOR of A field in PMP configuration register is not considered here.
 */
__STATIC_INLINE int __get_PMPENTRYx(unsigned int entry_idx, pmp_config *pmp_cfg)
{
    unsigned int cfg_shift, cfg_csr_idx, addr_csr_idx = 0;
    unsigned long cfgmask, pmpcfg, prot = 0;
    unsigned long t1, addr, pmpaddr, len = 0;
    uint8_t csr_cfg_num = 0;
    /* check parameters */
    if (entry_idx >= __PMP_ENTRY_NUM || !pmp_cfg) return -1;

    /* calculate PMP register and offset */
#if __RISCV_XLEN == 32
    csr_cfg_num = 4;
    cfg_csr_idx = entry_idx >> 2;
#elif __RISCV_XLEN == 64
    csr_cfg_num = 8;
    cfg_csr_idx = (entry_idx>> 2) & ~1;
#else
    // TODO Add RV128 Handling
    return -1;
#endif

    cfg_shift = (entry_idx & (csr_cfg_num - 1)) << 3;
    addr_csr_idx = entry_idx;

    /* decode PMP config */
    cfgmask = (0xFFUL << cfg_shift);
    pmpcfg = (__get_PMPCFGx(cfg_csr_idx) & cfgmask);
    prot = pmpcfg >> cfg_shift;

    /* decode PMP address */
    pmpaddr = __get_PMPADDRx(addr_csr_idx);
    if (PMP_A_NAPOT == (prot & PMP_A)) {
        t1 = __CTZ(~pmpaddr);
        addr = (pmpaddr & ~((1UL << t1) - 1)) << PMP_SHIFT;
        len = (t1 + PMP_SHIFT + 1);
    } else {
        addr = pmpaddr << PMP_SHIFT;
        len = PMP_SHIFT;
    }

    /* return details */
    pmp_cfg->protection = prot;
    pmp_cfg->base_addr = addr;
    pmp_cfg->order = len;

    return 0;
}

/** @} */ /* End of Doxygen Group NMSIS_Core_PMP_Functions */
#endif /* defined(__PMP_PRESENT) && (__PMP_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_PMP_H__ */
