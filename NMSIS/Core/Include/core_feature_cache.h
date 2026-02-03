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
#ifndef __CORE_FEATURE_CACHE_H__
#define __CORE_FEATURE_CACHE_H__
/*!
 * @file     core_feature_cache.h
 * @brief    Cache feature API header file for Nuclei N/NX Core
 */
/*
 * Cache Feature Configuration Macro:
 * 1. __ICACHE_PRESENT:  Define whether I-Cache Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 2. __DCACHE_PRESENT:  Define whether D-Cache Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 3. __CCM_PRESENT:  Define whether Nuclei Cache Control and Maintainence(CCM) Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 4. __SMPCC_PRESENT:  Define whether SMP & Cluster Cache Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 5. __SMPCC_BASEADDR:  Base address of the SMP & Cluster Cache unit.
 */
#ifdef __cplusplus
 extern "C" {
#endif

#include "core_feature_base.h"


#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)

#if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1)
/**
 * \defgroup NMSIS_Core_CCache      Cluster-Cache Functions
 * \ingroup  NMSIS_Core_Cache
 * \brief    Functions that configure Cluster Cache by CCM
 * @{
 */

/**
 * \brief  Cluster Cache Control and Command Registers
 * \remarks This structure only holds a part of SMPCC registers, which are related to
 * Cluster Cache CCM operations. Other more registers of SMPCC are listed in core_feature_smpcc.h
 */
typedef struct {
    __IM uint8_t RESERVED0[16];     /*!< 0x00~0x0F (R) Not shown here for these registers are not related with cache operation */
    __IOM uint32_t CC_CTRL;         /*!< Offset: 0x10 (R/W) Cluster Cache Control Register */
    __IOM uint32_t CC_mCMD;         /*!< Offset: 0x14 (R/W) Cluster Cache M-mode Command Register */
    __IM uint8_t RESERVED1[168];    /*!< 0x18~0xBF (R) not shown here for these registers are not related with cache operation */
    __IOM uint32_t CC_sCMD;         /*!< Offset: 0xC0 (R/W) Cluster Cache S-mode Command Register */
    __IOM uint32_t CC_uCMD;         /*!< Offset: 0xC4 (R/W) Cluster Cache U-mode Command Register */
    __IM uint8_t RESERVED2[20];     /*!< 0xC8~0xDB (R) not shown here for these registers are not related with cache operation */
    __IOM uint32_t CC_INVALID_ALL;  /*!< Offset: 0xDC (R/W) Cluster Cache Invalid All Register */
} SMPCC_CMD_Type;

#define SMPCC_CMD_CTRL_SUP_EN_Pos                   9U                                      /*!< SMPCC_CMD CC_CTRL SUP_EN Position */
#define SMPCC_CMD_CTRL_SUP_EN_Msk                   (0x1UL << SMPCC_CMD_CTRL_SUP_EN_Pos)    /*!< SMPCC_CMD CC_CTRL SUP_EN Mask */
#define SMPCC_CMD_CTRL_SUP_EN_ENABLE                1U                                      /*!< SMPCC_CMD CC_CTRL SUP_EN Enable */
#define SMPCC_CMD_CTRL_SUP_EN_DISABLE               0U                                      /*!< SMPCC_CMD CC_CTRL SUP_EN Disable */

#define SMPCC_CMD_CTRL_USE_EN_Pos                   10U                                     /*!< SMPCC_CMD CC_CTRL USE_EN Position */
#define SMPCC_CMD_CTRL_USE_EN_Msk                   (0x1UL << SMPCC_CMD_CTRL_USE_EN_Pos)    /*!< SMPCC_CMD CC_CTRL USE_EN Mask */
#define SMPCC_CMD_CTRL_USE_EN_ENABLE                1U                                      /*!< SMPCC_CMD CC_CTRL USE_EN Enable */
#define SMPCC_CMD_CTRL_USE_EN_DISABLE               0U                                      /*!< SMPCC_CMD CC_CTRL USE_EN Disable */

#define SMPCC_CMD_xCMD_CMD_Pos                      0U                                      /*!< SMPCC_CMD register xCMD field CMD Position */
#define SMPCC_CMD_xCMD_CMD_Msk                      (0x1FUL << SMPCC_CMD_xCMD_CMD_Pos)      /*!< SMPCC_CMD register xCMD field CMD Mask */
#define SMPCC_CMD_xCMD_CMD_WB_ALL                   0x7U                                    /*!< SMPCC_CMD xCMD CMD WB_ALL */
#define SMPCC_CMD_xCMD_CMD_WBINVAL_ALL              0x6U                                    /*!< SMPCC_CMD xCMD CMD WBINVAL_ALL */

#define SMPCC_CMD_xCMD_RESULT_Pos                   26U                                     /*!< SMPCC_CMD xCMD RESULT Position */
#define SMPCC_CMD_xCMD_RESULT_Msk                   (0x1FUL << SMPCC_CMD_xCMD_RESULT_Pos)   /*!< SMPCC_CMD xCMD RESULT Mask */
#define SMPCC_CMD_xCMD_RESULT_SUCCESS               0x0U                                    /*!< SMPCC_CMD xCMD RESULT Success */
#define SMPCC_CMD_xCMD_RESULT_ENTRY_EXCEED_LIMIT    0x1U                                    /*!< SMPCC_CMD xCMD RESULT Exceed the upper entry num of lockable way */
#define SMPCC_CMD_xCMD_RESULT_REFILL_BUS_ERROR      0x3U                                    /*!< SMPCC_CMD xCMD RESULT Refill Bus Error */
#define SMPCC_CMD_xCMD_RESULT_ECC_ERROR             0x4U                                    /*!< SMPCC_CMD xCMD RESULT ECC Error */
#define SMPCC_CMD_xCMD_RESULT_CPBACK_BUS_ERROR      0x5U                                    /*!< SMPCC_CMD xCMD RESULT Copy Back Bus Error */

#define SMPCC_CMD_xCMD_COMPLETE_Pos                 31U                                     /*!< SMPCC_CMD xCMD COMPLETE Position */
#define SMPCC_CMD_xCMD_COMPLETE_Msk                 (0x1UL << SMPCC_CMD_xCMD_COMPLETE_Pos)  /*!< SMPCC_CMD xCMD COMPLETE Mask */

#define SMPCC_CMD_INVALID_ALL_Pos                   0U                                      /*!< SMPCC_CMD INVALID_ALL Position */
#define SMPCC_CMD_INVALID_ALL_Msk                   (0x1UL << SMPCC_CMD_INVALID_ALL_Pos)    /*!< SMPCC_CMD INVALID_ALL Mask */

#ifndef __SMPCC_BASEADDR
/* Base address of SMPCC(__SMPCC_BASEADDR) should be defined in <Device.h> */
#error "__SMPCC_BASEADDR is not defined, please check!"
#endif

/* SMPCC CMD registers Memory mapping of Device */
#define SMPCC_CMD_BASE          __SMPCC_BASEADDR                    /*!< SMPCC CMD Base Address */
#define SMPCC_CMD               ((SMPCC_CMD_Type *)SMPCC_CMD_BASE)  /*!< SMPCC CMD configuration struct */

/** @} */ /* End of Doxygen Group NMSIS_Core_CCache */
#endif /* #if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1) */

/* ##########################  Cache functions  #################################### */
/**
 * \defgroup NMSIS_Core_Cache       Cache Functions
 * \brief    Functions that configure Instruction and Data Cache.
 * @{
 *
 * Nuclei provide Cache Control and Maintainence(CCM) for software to control and maintain
 * the internal L1 I/D Cache and Cluster Cache of the RISC-V Core, software can manage the
 * cache flexibly to meet the actual application scenarios.
 *
 * The CCM operations have 3 types: by single address, by all and flush pipeline.
 * The CCM operations are done via CSR registers, M/S/U mode has its own CSR registers to
 * do CCM operations. By default, CCM operations are not allowed in S/U mode, you can execute
 * \ref EnableSUCCM in M-Mode to enable it.
 *
 * * API names started with M<operations>, such as \ref MInvalICacheLine must be called in M-Mode only.
 * * API names started with S<operations>, such as \ref SInvalICacheLine should be called in S-Mode.
 * * API names started with U<operations>, such as \ref UInvalICacheLine should be called in U-Mode.
 *
 */


/**
 * \brief Cache CCM Operation Fail Info
 */
typedef enum CCM_OP_FINFO {
    CCM_OP_SUCCESS = 0x0,               /*!< Lock Succeed */
    CCM_OP_EXCEED_ERR = 0x1,            /*!< Exceed the the number of lockable ways(N-Way I/D-Cache, lockable is N-1) */
    CCM_OP_PERM_CHECK_ERR = 0x2,        /*!< PMP/sPMP/Page-Table X(I-Cache)/R(D-Cache) permission check failed, or belong to Device/Non-Cacheable address range */
    CCM_OP_REFILL_BUS_ERR = 0x3,        /*!< Refill has Bus Error */
    CCM_OP_ECC_ERR = 0x4                /*!< Deprecated, ECC Error, this error code is removed in later Nuclei CCM RTL design, please don't use it */
} CCM_OP_FINFO_Type;

/**
 * \brief Cache CCM Command Types
 */
typedef enum CCM_CMD {
    CCM_DC_INVAL = 0x0,                 /*!< Unlock and invalidate D-Cache line and Cluster Cache line specified by CSR CCM_XBEGINADDR */
    CCM_DC_WB = 0x1,                    /*!< Flush the specific D-Cache line and Cluster Cache line specified by CSR CCM_XBEGINADDR */
    CCM_DC_WBINVAL = 0x2,               /*!< Unlock, flush and invalidate the specific D-Cache line and Cluster Cache line specified by CSR CCM_XBEGINADDR */
    CCM_DC_LOCK = 0x3,                  /*!< Lock the specific D-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_DC_UNLOCK = 0x4,                /*!< Unlock the specific D-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_DC_WBINVAL_ALL = 0x6,           /*!< Unlock and flush and invalidate all the valid and dirty D-Cache lines */
    CCM_DC_WB_ALL = 0x7,                /*!< Flush all the valid and dirty D-Cache lines */
    CCM_DC_INVAL_ALL = 0x17,            /*!< Unlock and invalidate all the D-Cache lines */
    CCM_IC_INVAL = 0x8,                 /*!< Unlock and invalidate I-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_IC_LOCK = 0xb,                  /*!< Lock the specific I-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_IC_UNLOCK = 0xc,                /*!< Unlock the specific I-Cache line specified by CSR CCM_XBEGINADDR */
    CCM_IC_INVAL_ALL = 0xd,             /*!< Unlock and invalidate all the I-Cache lines */
    CCM_CC_LOCK = 0x13,                 /*!< Lock the specific Cluster Cache line specified by CSR CCM_XBEGINADDR */
    CCM_CC_UNLOCK = 0x12,               /*!< Unlock the specific Cluster Cache line specified by CSR CCM_XBEGINADDR */
} CCM_CMD_Type;

/**
 * \brief Cache Information Type
 */
typedef struct CacheInfo {
    uint32_t linesize;                  /*!< Cache Line size in bytes */
    uint32_t ways;                      /*!< Cache ways */
    uint32_t setperway;                 /*!< Cache set per way */
    uint32_t size;                      /*!< Cache total size in bytes */
} CacheInfo_Type;

#if __riscv_xlen == 32
#define CCM_SUEN_SUEN_Msk               (0xFFFFFFFFUL)              /*!< CSR CCM_SUEN: SUEN Mask */
#else
#define CCM_SUEN_SUEN_Msk               (0xFFFFFFFFFFFFFFFFUL)      /*!< CSR CCM_SUEN: SUEN Mask */
#endif

/**
 * \brief  Enable CCM operation in Supervisor/User Mode
 * \details
 * This function enable CCM operation in Supervisor/User Mode.
 * If enabled, CCM operations in supervisor/user mode will
 * be allowed. Besides CCM registers, CC_sCMD and CC_uCMD registers
 * which belong to SMPCC module are also allowed.
 * \remarks
 * - This function can be called in M-Mode only.
 * \sa
 * - \ref DisableSUCCM
*/
__STATIC_FORCEINLINE void EnableSUCCM(void)
{
    __RV_CSR_SET(CSR_CCM_SUEN, CCM_SUEN_SUEN_Msk);
#if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1)
    SMPCC_CMD->CC_CTRL |= _VAL2FLD(SMPCC_CMD_CTRL_SUP_EN, SMPCC_CMD_CTRL_SUP_EN_ENABLE) |
                          _VAL2FLD(SMPCC_CMD_CTRL_USE_EN, SMPCC_CMD_CTRL_USE_EN_ENABLE);
#endif /* #if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1) */
}

/**
 * \brief  Disable CCM operation in Supervisor/User Mode
 * \details
 * This function disable CCM operation in Supervisor/User Mode.
 * If not enabled, CCM operations in supervisor/user mode will
 * trigger a *illegal intruction* exception, access to CC_sCMD
 * and CC_uCMD register is also forbidden.
 * \remarks
 * - This function can be called in M-Mode only.
 * \sa
 * - \ref EnableSUCCM
*/
__STATIC_FORCEINLINE void DisableSUCCM(void)
{
    __RV_CSR_CLEAR(CSR_CCM_SUEN, CCM_SUEN_SUEN_Msk);
#if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1)
    SMPCC_CMD->CC_CTRL &= ~_VAL2FLD(SMPCC_CMD_CTRL_SUP_EN, SMPCC_CMD_CTRL_SUP_EN_DISABLE) &
                          ~_VAL2FLD(SMPCC_CMD_CTRL_USE_EN, SMPCC_CMD_CTRL_USE_EN_DISABLE);
#endif /* #if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1) */
}

/**
 * \brief  Flush pipeline after CCM operation
 * \details
 * This function is used to flush pipeline after CCM operations
 * on Cache, it will ensure latest instructions or data can be
 * seen by pipeline.
 * \remarks
 * - This function can be called in M/S/U-Mode only.
*/
__STATIC_FORCEINLINE void FlushPipeCCM(void)
{
    __RV_CSR_WRITE(CSR_CCM_FPIPE, 0x1);
}
/** @} */ /* End of Doxygen Group NMSIS_Core_Cache */

#if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1)
/**
 * \ingroup  NMSIS_Core_CCache
 * @{
 */

/**
 * \brief  Lock one Cluster Cache line specified by address in M-Mode
 * \details
 * This function lock one Cluster Cache line specified by the address.
 * Command \ref CCM_CC_LOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long MLockCCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_CC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_MDATA);
}

/**
 * \brief  Lock several Cluster Cache lines specified by address in M-Mode
 * \details
 * This function lock several Cluster Cache lines specified by the address
 * and line count.
 * Command \ref CCM_CC_LOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long MLockCCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_CC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_MDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Lock one Cluster Cache line specified by address in S-Mode
 * \details
 * This function lock one Cluster Cache line specified by the address.
 * Command \ref CCM_CC_LOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long SLockCCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_CC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_SDATA);
}

/**
 * \brief  Lock several Cluster Cache lines specified by address in S-Mode
 * \details
 * This function lock several Cluster Cache lines specified by the address
 * and line count.
 * Command \ref CCM_CC_LOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long SLockCCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_CC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_SDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Lock one Cluster Cache line specified by address in U-Mode
 * \details
 * This function lock one Cluster Cache line specified by the address.
 * Command \ref CCM_CC_LOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long ULockCCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_CC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_UDATA);
}

/**
 * \brief  Lock several Cluster Cache lines specified by address in U-Mode
 * \details
 * This function lock several Cluster Cache lines specified by the address
 * and line count.
 * Command \ref CCM_CC_LOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long ULockCCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_CC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_UDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Unlock one Cluster Cache line specified by address in M-Mode
 * \details
 * This function unlock one Cluster Cache line specified by the address.
 * Command \ref CCM_CC_UNLOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void MUnlockCCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_CC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several Cluster Cache lines specified by address in M-Mode
 * \details
 * This function unlock several Cluster Cache lines specified
 * by the address and line count.
 * Command \ref CCM_CC_UNLOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void MUnlockCCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_CC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Unlock one Cluster Cache line specified by address in S-Mode
 * \details
 * This function unlock one Cluster Cache line specified by the address.
 * Command \ref CCM_CC_UNLOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void SUnlockCCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_CC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several Cluster Cache lines specified by address in S-Mode
 * \details
 * This function unlock several Cluster Cache lines specified
 * by the address and line count.
 * Command \ref CCM_CC_UNLOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void SUnlockCCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_CC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Unlock one Cluster Cache line specified by address in U-Mode
 * \details
 * This function unlock one Cluster Cache line specified by the address.
 * Command \ref CCM_CC_UNLOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void UUnlockCCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_CC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several Cluster Cache lines specified by address in U-Mode
 * \details
 * This function unlock several Cluster Cache lines specified
 * by the address and line count.
 * Command \ref CCM_CC_UNLOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void UUnlockCCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_CC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/** @} */ /* End of Doxygen Group NMSIS_Core_CCache */
#endif /* defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1) */

#endif /* defined(__CCM_PRESENT) && (__CCM_PRESENT == 1) */

#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1)

/**
 * \defgroup NMSIS_Core_ICache      I-Cache Functions
 * \ingroup  NMSIS_Core_Cache
 * \brief    Functions that configure Instruction Cache.
 * @{
 */

/**
 * \brief  Check ICache Unit Present or Not
 * \details
 * This function check icache unit present or not via mcfg_info csr
 * \remarks
 * - This function might not work for some old nuclei processors
 * - Please make sure the version of your nuclei processor contain ICACHE bit in mcfg_info
 * \return 1 if present otherwise 0
*/
__STATIC_INLINE int32_t ICachePresent(void)
{
    if (__RV_CSR_READ(CSR_MCFG_INFO) & MCFG_INFO_ICACHE) {
        return 1;
    }
    return 0;
}

/**
 * \brief  Enable ICache
 * \details
 * This function enable I-Cache
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control I Cache enable.
 * \sa
 * - \ref DisableICache
*/
__STATIC_FORCEINLINE void EnableICache(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_IC_EN);
}

/**
 * \brief  Disable ICache
 * \details
 * This function Disable I-Cache
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control I Cache enable.
 * \sa
 * - \ref EnableICache
 */
__STATIC_FORCEINLINE void DisableICache(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_IC_EN);
}

/**
 * \brief  Enable ICache ECC
 * \details
 * This function enable I-Cache ECC
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control I Cache ECC enable.
 * \sa
 * - \ref DisableICacheECC
*/
__STATIC_FORCEINLINE void EnableICacheECC(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_IC_ECC_EN);
}

/**
 * \brief  Disable ICache ECC
 * \details
 * This function disable I-Cache ECC
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control I Cache ECC enable.
 * \sa
 * - \ref EnableICacheECC
*/
__STATIC_FORCEINLINE void DisableICacheECC(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_IC_ECC_EN);
}

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
/**
 * \brief  Get I-Cache Information
 * \details
 * This function get I-Cache Information
 * \remarks
 * - This function can be called in M-Mode only.
 * - You can use this function in combination with cache lines operations
 * \sa
 * - \ref GetDCacheInfo
 */
__STATIC_INLINE int32_t GetICacheInfo(CacheInfo_Type *info)
{
    if (info == NULL) {
        return -1;
    }
    CSR_MICFGINFO_Type csr_ccfg;
    csr_ccfg.d = __RV_CSR_READ(CSR_MICFG_INFO);
    info->setperway = (1UL << csr_ccfg.b.set) << 3;
    info->ways = (1 + csr_ccfg.b.way);
    if (csr_ccfg.b.lsize == 0) {
        info->linesize = 0;
    } else {
        info->linesize = (1UL << (csr_ccfg.b.lsize - 1)) << 3;
    }
    info->size = info->setperway * info->ways * info->linesize;
    return 0;
}

/**
 * \brief  Invalidate one I-Cache line specified by address in M-Mode
 * \details
 * This function unlock and invalidate one I-Cache line specified
 * by the address.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void MInvalICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several I-Cache lines specified by address in M-Mode
 * \details
 * This function unlock and invalidate several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void MInvalICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate one I-Cache line specified by address in S-Mode
 * \details
 * This function unlock and invalidate one I-Cache line specified
 * by the address.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void SInvalICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several I-Cache lines specified by address in S-Mode
 * \details
 * This function unlock and invalidate several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void SInvalICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate one I-Cache line specified by address in U-Mode
 * \details
 * This function unlock and invalidate one I-Cache line specified
 * by the address.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void UInvalICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several I-Cache lines specified by address in U-Mode
 * \details
 * This function unlock and invalidate several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_INVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void UInvalICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

#if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1)
/**
 * \brief  Invalidate one I-Cache and Cluster Cache line specified by address in M-Mode
 * \details
 * This function unlock and invalidate one I-Cache line and corresponding Cluster Cache line
 * specified by the address.
 * Commands \ref CCM_IC_INVAL and \ref CCM_DC_INVAL are written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void MInvalICacheCCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_INVAL);
    /* Trigger Cluster Cache invalidation by DC_INVAL */
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several I-Cache and Cluster Cache lines specified by address in M-Mode
 * \details
 * This function unlock and invalidate several I-Cache lines and corresponding Cluster Cache lines
 * specified by the address and line count.
 * Commands \ref CCM_IC_INVAL and \ref CCM_DC_INVAL are written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void MInvalICacheCCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_INVAL);
            /* Trigger Cluster Cache invalidation by DC_INVAL */
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate one I-Cache and Cluster Cache line specified by address in S-Mode
 * \details
 * This function unlock and invalidate one I-Cache line and corresponding Cluster Cache line
 * specified by the address.
 * Commands \ref CCM_IC_INVAL and \ref CCM_DC_INVAL are written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void SInvalICacheCCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_INVAL);
    /* Trigger Cluster Cache invalidation by DC_INVAL */
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several I-Cache and Cluster Cache lines specified by address in S-Mode
 * \details
 * This function unlock and invalidate several I-Cache lines and corresponding Cluster Cache lines
 * specified by the address and line count.
 * Commands \ref CCM_IC_INVAL and \ref CCM_DC_INVAL are written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void SInvalICacheCCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_INVAL);
            /* Trigger Cluster Cache invalidation by DC_INVAL */
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate one I-Cache and Cluster Cache line specified by address in U-Mode
 * \details
 * This function unlock and invalidate one I-Cache line and corresponding Cluster Cache line
 * specified by the address.
 * Commands \ref CCM_IC_INVAL and \ref CCM_DC_INVAL are written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void UInvalICacheCCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_INVAL);
    /* Trigger Cluster Cache invalidation by DC_INVAL */
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several I-Cache and Cluster Cache lines specified by address in U-Mode
 * \details
 * This function unlock and invalidate several I-Cache lines and corresponding Cluster Cache lines
 * specified by the address and line count.
 * Commands \ref CCM_IC_INVAL and \ref CCM_DC_INVAL are written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void UInvalICacheCCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_INVAL);
            /* Trigger Cluster Cache invalidation by DC_INVAL */
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}
#endif /* #if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1) */

/**
 * \brief  Lock one I-Cache line specified by address in M-Mode
 * \details
 * This function lock one I-Cache line specified by the address.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long MLockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_MDATA);
}

/**
 * \brief  Lock several I-Cache lines specified by address in M-Mode
 * \details
 * This function lock several I-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long MLockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_MDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Lock one I-Cache line specified by address in S-Mode
 * \details
 * This function lock one I-Cache line specified by the address.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long SLockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_SDATA);
}

/**
 * \brief  Lock several I-Cache lines specified by address in S-Mode
 * \details
 * This function lock several I-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long SLockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_SDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Lock one I-Cache line specified by address in U-Mode
 * \details
 * This function lock one I-Cache line specified by the address.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long ULockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_UDATA);
}

/**
 * \brief  Lock several I-Cache lines specified by address in U-Mode
 * \details
 * This function lock several I-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_IC_LOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long ULockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_UDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Unlock one I-Cache line specified by address in M-Mode
 * \details
 * This function unlock one I-Cache line specified by the address.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void MUnlockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several I-Cache lines specified by address in M-Mode
 * \details
 * This function unlock several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void MUnlockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Unlock one I-Cache line specified by address in S-Mode
 * \details
 * This function unlock one I-Cache line specified by the address.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void SUnlockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several I-Cache lines specified by address in S-Mode
 * \details
 * This function unlock several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void SUnlockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Unlock one I-Cache line specified by address in U-Mode
 * \details
 * This function unlock one I-Cache line specified by the address.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void UUnlockICacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several I-Cache lines specified by address in U-Mode
 * \details
 * This function unlock several I-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_IC_UNLOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void UUnlockICacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate all I-Cache lines in M-Mode
 * \details
 * This function invalidate all I-Cache lines.
 * Command \ref CCM_IC_INVAL_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void MInvalICache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate all I-Cache lines in S-Mode
 * \details
 * This function invalidate all I-Cache lines.
 * Command \ref CCM_IC_INVAL_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void SInvalICache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate all I-Cache lines in U-Mode
 * \details
 * This function invalidate all I-Cache lines.
 * Command \ref CCM_IC_INVAL_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void UInvalICache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

#if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1)
/**
 * \brief  Invalidate all Cluster Cache in M-Mode
 * \details
 * This function invalidate all Cluster Cache.
 * \remarks
 * This function must be executed in M-Mode only.
 */
__STATIC_INLINE void MInvalCCache(void)
{
    SMPCC_CMD->CC_INVALID_ALL = _VAL2FLD(SMPCC_CMD_INVALID_ALL, 1);
    while(_FLD2VAL(SMPCC_CMD_INVALID_ALL, SMPCC_CMD->CC_INVALID_ALL));
    __RWMB();
}

/**
 * \brief  Invalidate all Cluster Cache in S-Mode
 * \details
 * This function invalidate all Cluster Cache.
 * \remarks
 * This function must be executed in M/S-Mode only.
 */
__STATIC_INLINE void SInvalCCache(void)
{
    SMPCC_CMD->CC_INVALID_ALL = _VAL2FLD(SMPCC_CMD_INVALID_ALL, 1);
    while(_FLD2VAL(SMPCC_CMD_INVALID_ALL, SMPCC_CMD->CC_INVALID_ALL));
    __RWMB();
}

/**
 * \brief  Invalidate all Cluster Cache in U-Mode
 * \details
 * This function invalidate all Cluster Cache.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 */
__STATIC_INLINE void UInvalCCache(void)
{
    SMPCC_CMD->CC_INVALID_ALL = _VAL2FLD(SMPCC_CMD_INVALID_ALL, 1);
    while(_FLD2VAL(SMPCC_CMD_INVALID_ALL, SMPCC_CMD->CC_INVALID_ALL));
    __RWMB();
}

/**
 * \brief  Invalidate all I-Cache and Cluster Cache in M-Mode
 * \details
 * This function unlock and invalidate all I-Cache and Cluster Cache.
 * Command \ref CCM_IC_INVAL_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 */
__STATIC_INLINE void MInvalICacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_IC_INVAL_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_INVALID_ALL = _VAL2FLD(SMPCC_CMD_INVALID_ALL, 1);
    while(_FLD2VAL(SMPCC_CMD_INVALID_ALL, SMPCC_CMD->CC_INVALID_ALL));
    __RWMB();
}

/**
 * \brief  Invalidate all I-Cache and Cluster Cache in S-Mode
 * \details
 * This function unlock and invalidate all I-Cache and Cluster Cache.
 * Command \ref CCM_IC_INVAL_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 */
__STATIC_INLINE void SInvalICacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_IC_INVAL_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_INVALID_ALL = _VAL2FLD(SMPCC_CMD_INVALID_ALL, 1);
    while(_FLD2VAL(SMPCC_CMD_INVALID_ALL, SMPCC_CMD->CC_INVALID_ALL));
    __RWMB();
}

/**
 * \brief  Invalidate all I-Cache and Cluster Cache in U-Mode
 * \details
 * This function unlock and invalidate all I-Cache and Cluster Cache.
 * Command \ref CCM_IC_INVAL_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 */
__STATIC_INLINE void UInvalICacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_IC_INVAL_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_INVALID_ALL = _VAL2FLD(SMPCC_CMD_INVALID_ALL, 1);
    while(_FLD2VAL(SMPCC_CMD_INVALID_ALL, SMPCC_CMD->CC_INVALID_ALL));
    __RWMB();
}
#endif /* #if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1) */

#endif /* defined(__CCM_PRESENT) && (__CCM_PRESENT == 1) */
/** @} */ /* End of Doxygen Group NMSIS_Core_ICache */
#endif /* defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1) */

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1)
/**
 * \defgroup NMSIS_Core_DCache      D-Cache Functions
 * \ingroup  NMSIS_Core_Cache
 * \brief    Functions that configure Data Cache.
 * @{
 */

/**
 * \brief  Check DCache Unit Present or Not
 * \details
 * This function check dcache unit present or not via mcfg_info csr
 * \remarks
 * - This function might not work for some old nuclei processors
 * - Please make sure the version of your nuclei processor contain DCACHE bit in mcfg_info
 * \return 1 if present otherwise 0
*/
__STATIC_INLINE int32_t DCachePresent(void)
{
    if (__RV_CSR_READ(CSR_MCFG_INFO) & MCFG_INFO_DCACHE) {
        return 1;
    }
    return 0;
}

/**
 * \brief  Enable DCache
 * \details
 * This function enable D-Cache
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control D Cache enable.
 * \sa
 * - \ref DisableDCache
*/
__STATIC_FORCEINLINE void EnableDCache(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_DC_EN);
}

/**
 * \brief  Disable DCache
 * \details
 * This function Disable D-Cache
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control D Cache enable.
 * \sa
 * - \ref EnableDCache
 */
__STATIC_FORCEINLINE void DisableDCache(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_DC_EN);
}

/**
 * \brief  Enable DCache ECC
 * \details
 * This function enable D-Cache ECC
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control D Cache ECC enable.
 * \sa
 * - \ref DisableDCacheECC
*/
__STATIC_FORCEINLINE void EnableDCacheECC(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_DC_ECC_EN);
}

/**
 * \brief  Disable DCache ECC
 * \details
 * This function disable D-Cache ECC
 * \remarks
 * - This function can be called in M-Mode only.
 * - This \ref CSR_MCACHE_CTL register control D Cache ECC enable.
 * \sa
 * - \ref EnableDCacheECC
*/
__STATIC_FORCEINLINE void DisableDCacheECC(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_DC_ECC_EN);
}

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
/**
 * \brief  Get D-Cache Information
 * \details
 * This function get D-Cache Information
 * \remarks
 * - This function can be called in M-Mode only.
 * - You can use this function in combination with cache lines operations
 * \sa
 * - \ref GetICacheInfo
 */
__STATIC_INLINE int32_t GetDCacheInfo(CacheInfo_Type *info)
{
    if (info == NULL) {
        return -1;
    }
    CSR_MDCFGINFO_Type csr_ccfg;
    csr_ccfg.d = __RV_CSR_READ(CSR_MDCFG_INFO);
    info->setperway = (1UL << csr_ccfg.b.set) << 3;
    info->ways = (1 + csr_ccfg.b.way);
    if (csr_ccfg.b.lsize == 0) {
        info->linesize = 0;
    } else {
        info->linesize = (1UL << (csr_ccfg.b.lsize - 1)) << 3;
    }
    info->size = info->setperway * info->ways * info->linesize;
    return 0;
}

/**
 * \brief  Invalidate one D-Cache line specified by address in M-Mode
 * \details
 * This function unlock and invalidate one D-Cache line specified
 * by the address.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void MInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several D-Cache lines specified by address in M-Mode
 * \details
 * This function unlock and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void MInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate one D-Cache line specified by address in S-Mode
 * \details
 * This function unlock and invalidate one D-Cache line specified
 * by the address.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void SInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several D-Cache lines specified by address in S-Mode
 * \details
 * This function unlock and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void SInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate one D-Cache line specified by address in U-Mode
 * \details
 * This function unlock and invalidate one D-Cache line specified
 * by the address.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void UInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_INVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate several D-Cache lines specified by address in U-Mode
 * \details
 * This function unlock and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_INVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
__STATIC_INLINE void UInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_INVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush one D-Cache line specified by address in M-Mode
 * \details
 * This function flush one D-Cache line specified by the address.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void MFlushDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WB);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush several D-Cache lines specified by address in M-Mode
 * \details
 * This function flush several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed
 * \param [in]    cnt     count of cache lines to be flushed
 */
__STATIC_INLINE void MFlushDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WB);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush one D-Cache line specified by address in S-Mode
 * \details
 * This function flush one D-Cache line specified by the address.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void SFlushDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WB);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush several D-Cache lines specified by address in S-Mode
 * \details
 * This function flush several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed
 * \param [in]    cnt     count of cache lines to be flushed
 */
__STATIC_INLINE void SFlushDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WB);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush one D-Cache line specified by address in U-Mode
 * \details
 * This function flush one D-Cache line specified by the address.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void UFlushDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WB);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush several D-Cache lines specified by address in U-Mode
 * \details
 * This function flush several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WB is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed
 * \param [in]    cnt     count of cache lines to be flushed
 */
__STATIC_INLINE void UFlushDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WB);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush and invalidate one D-Cache line specified by address in M-Mode
 * \details
 * This function flush and invalidate one D-Cache line specified by the address.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 */
__STATIC_INLINE void MFlushInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WBINVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate several D-Cache lines specified by address in M-Mode
 * \details
 * This function flush and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 * \param [in]    cnt     count of cache lines to be flushed and invalidated
 */
__STATIC_INLINE void MFlushInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WBINVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush and invalidate one D-Cache line specified by address in S-Mode
 * \details
 * This function flush and invalidate one D-Cache line specified by the address.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 */
__STATIC_INLINE void SFlushInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WBINVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate several D-Cache lines specified by address in S-Mode
 * \details
 * This function flush and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 * \param [in]    cnt     count of cache lines to be flushed and invalidated
 */
__STATIC_INLINE void SFlushInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WBINVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Flush and invalidate one D-Cache line specified by address in U-Mode
 * \details
 * This function flush and invalidate one D-Cache line specified by the address.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 */
__STATIC_INLINE void UFlushInvalDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WBINVAL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate several D-Cache lines specified by address in U-Mode
 * \details
 * This function flush and invalidate several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_WBINVAL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 * \param [in]    cnt     count of cache lines to be flushed and invalidated
 */
__STATIC_INLINE void UFlushInvalDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WBINVAL);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

#if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1)
/**
 * \brief  Invalidate one D-Cache and Cluster Cache line specified by address in M-Mode
 * \details
 * This macro is an alias for MInvalDCacheLine. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
#define MInvalDCacheCCacheLine(addr) MInvalDCacheLine(addr)

/**
 * \brief  Invalidate several D-Cache and Cluster Cache lines specified by address in M-Mode
 * \details
 * This macro is an alias for MInvalDCacheLines. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
#define MInvalDCacheCCacheLines(addr, cnt) MInvalDCacheLines(addr, cnt)

/**
 * \brief  Invalidate one D-Cache and Cluster Cache line specified by address in S-Mode
 * \details
 * This macro is an alias for SInvalDCacheLine. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
#define SInvalDCacheCCacheLine(addr) SInvalDCacheLine(addr)

/**
 * \brief  Invalidate several D-Cache and Cluster Cache lines specified by address in S-Mode
 * \details
 * This macro is an alias for SInvalDCacheLines. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
#define SInvalDCacheCCacheLines(addr, cnt) SInvalDCacheLines(addr, cnt)

/**
 * \brief  Invalidate one D-Cache and Cluster Cache line specified by address in U-Mode
 * \details
 * This macro is an alias for UInvalDCacheLine. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
#define UInvalDCacheCCacheLine(addr) UInvalDCacheLine(addr)

/**
 * \brief  Invalidate several D-Cache and Cluster Cache lines specified by address in U-Mode
 * \details
 * This macro is an alias for UInvalDCacheLines. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 * \param [in]    cnt     count of cache lines to be invalidated
 */
#define UInvalDCacheCCacheLines(addr, cnt) UInvalDCacheLines(addr, cnt)
/**
 * \brief  Flush one D-Cache and Cluster Cache line specified by address in M-Mode
 * \details
 * This macro is an alias for MFlushDCacheLine. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed
 */
#define MFlushDCacheCCacheLine(addr) MFlushDCacheLine(addr)

/**
 * \brief  Flush several D-Cache and Cluster Cache lines specified by address in M-Mode
 * \details
 * This macro is an alias for MFlushDCacheLines. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed
 * \param [in]    cnt     count of cache lines to be flushed
 */
#define MFlushDCacheCCacheLines(addr, cnt) MFlushDCacheLines(addr, cnt)

/**
 * \brief  Flush one D-Cache and Cluster Cache line specified by address in S-Mode
 * \details
 * This macro is an alias for SFlushDCacheLine. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed
 */
#define SFlushDCacheCCacheLine(addr) SFlushDCacheLine(addr)

/**
 * \brief  Flush several D-Cache and Cluster Cache lines specified by address in S-Mode
 * \details
 * This macro is an alias for SFlushDCacheLines. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed
 * \param [in]    cnt     count of cache lines to be flushed
 */
#define SFlushDCacheCCacheLines(addr, cnt) SFlushDCacheLines(addr, cnt)

/**
 * \brief  Flush one D-Cache and Cluster Cache line specified by address in U-Mode
 * \details
 * This macro is an alias for UFlushDCacheLine. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed
 */
#define UFlushDCacheCCacheLine(addr) UFlushDCacheLine(addr)

/**
 * \brief  Flush several D-Cache and Cluster Cache lines specified by address in U-Mode
 * \details
 * This macro is an alias for UFlushDCacheLines. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed
 * \param [in]    cnt     count of cache lines to be flushed
 */
#define UFlushDCacheCCacheLines(addr, cnt) UFlushDCacheLines(addr, cnt)
/**
 * \brief  Flush and invalidate one D-Cache and Cluster Cache line specified by address in M-Mode
 * \details
 * This macro is an alias for MFlushInvalDCacheLine. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 */
#define MFlushInvalDCacheCCacheLine(addr) MFlushInvalDCacheLine(addr)

/**
 * \brief  Flush and invalidate several D-Cache and Cluster Cache lines specified by address in M-Mode
 * \details
 * This macro is an alias for MFlushInvalDCacheLines. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 * \param [in]    cnt     count of cache lines to be flushed and invalidated
 */
#define MFlushInvalDCacheCCacheLines(addr, cnt) MFlushInvalDCacheLines(addr, cnt)

/**
 * \brief  Flush and invalidate one D-Cache and Cluster Cache line specified by address in S-Mode
 * \details
 * This macro is an alias for SFlushInvalDCacheLine. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 */
#define SFlushInvalDCacheCCacheLine(addr) SFlushInvalDCacheLine(addr)

/**
 * \brief  Flush and invalidate several D-Cache and Cluster Cache lines specified by address in S-Mode
 * \details
 * This macro is an alias for SFlushInvalDCacheLines. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 * \param [in]    cnt     count of cache lines to be flushed and invalidated
 */
#define SFlushInvalDCacheCCacheLines(addr, cnt) SFlushInvalDCacheLines(addr, cnt)

/**
 * \brief  Flush and invalidate one D-Cache and Cluster Cache line specified by address in U-Mode
 * \details
 * This macro is an alias for UFlushInvalDCacheLine. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 */
#define UFlushInvalDCacheCCacheLine(addr) UFlushInvalDCacheLine(addr)

/**
 * \brief  Flush and invalidate several D-Cache and Cluster Cache lines specified by address in U-Mode
 * \details
 * This macro is an alias for UFlushInvalDCacheLines. D-Cache CCM operations are also effective for Cluster Cache.
 * \remarks
 * This macro must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed and invalidated
 * \param [in]    cnt     count of cache lines to be flushed and invalidated
 */
#define UFlushInvalDCacheCCacheLines(addr, cnt) UFlushInvalDCacheLines(addr, cnt)
#endif /* #if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1) */

/**
 * \brief  Lock one D-Cache line specified by address in M-Mode
 * \details
 * This function lock one D-Cache line specified by the address.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long MLockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_MDATA);
}

/**
 * \brief  Lock several D-Cache lines specified by address in M-Mode
 * \details
 * This function lock several D-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long MLockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_MDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Lock one D-Cache line specified by address in S-Mode
 * \details
 * This function lock one D-Cache line specified by the address.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long SLockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_SDATA);
}

/**
 * \brief  Lock several D-Cache lines specified by address in S-Mode
 * \details
 * This function lock several D-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long SLockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_SDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Lock one D-Cache line specified by address in U-Mode
 * \details
 * This function lock one D-Cache line specified by the address.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long ULockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_LOCK);
    FlushPipeCCM();
    __RWMB();
    return __RV_CSR_READ(CSR_CCM_UDATA);
}

/**
 * \brief  Lock several D-Cache lines specified by address in U-Mode
 * \details
 * This function lock several D-Cache lines specified by the address
 * and line count.
 * Command \ref CCM_DC_LOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be locked
 * \param [in]    cnt     count of cache lines to be locked
 * \return result of CCM lock operation, see enum \ref CCM_OP_FINFO
 */
__STATIC_INLINE unsigned long ULockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        unsigned long fail_info = CCM_OP_SUCCESS;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_LOCK);
            FlushPipeCCM();
            __RWMB();
            fail_info = __RV_CSR_READ(CSR_CCM_UDATA);
            if (CCM_OP_SUCCESS != fail_info) {
                return fail_info;
            }
        }
    }
    return CCM_OP_SUCCESS;
}

/**
 * \brief  Unlock one D-Cache line specified by address in M-Mode
 * \details
 * This function unlock one D-Cache line specified by the address.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void MUnlockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several D-Cache lines specified by address in M-Mode
 * \details
 * This function unlock several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void MUnlockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_MBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Unlock one D-Cache line specified by address in S-Mode
 * \details
 * This function unlock one D-Cache line specified by the address.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void SUnlockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several D-Cache lines specified by address in S-Mode
 * \details
 * This function unlock several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void SUnlockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_SBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Unlock one D-Cache line specified by address in U-Mode
 * \details
 * This function unlock one D-Cache line specified by the address.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be unlocked
 */
__STATIC_INLINE void UUnlockDCacheLine(unsigned long addr)
{
    __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_UNLOCK);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Unlock several D-Cache lines specified by address in U-Mode
 * \details
 * This function unlock several D-Cache lines specified
 * by the address and line count.
 * Command \ref CCM_DC_UNLOCK is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be unlocked
 * \param [in]    cnt     count of cache lines to be unlocked
 */
__STATIC_INLINE void UUnlockDCacheLines(unsigned long addr, unsigned long cnt)
{
    if (cnt > 0) {
        unsigned long i;
        __RV_CSR_WRITE(CSR_CCM_UBEGINADDR, addr);
        for (i = 0; i < cnt; i++) {
            __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_UNLOCK);
        }
        FlushPipeCCM();
        __RWMB();
    }
}

/**
 * \brief  Invalidate all D-Cache lines in M-Mode
 * \details
 * This function invalidate all D-Cache lines.
 * Command \ref CCM_DC_INVAL_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void MInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate all D-Cache lines in S-Mode
 * \details
 * This function invalidate all D-Cache lines.
 * Command \ref CCM_DC_INVAL_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void SInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Invalidate all D-Cache lines in U-Mode
 * \details
 * This function invalidate all D-Cache lines.
 * In U-Mode, this operation will be automatically
 * translated to flush and invalidate operations by hardware.
 * Command \ref CCM_DC_INVAL_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be invalidated
 */
__STATIC_INLINE void UInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_INVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush all D-Cache lines in M-Mode
 * \details
 * This function flush all D-Cache lines.
 * Command \ref CCM_DC_WB_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void MFlushDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WB_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush all D-Cache lines in S-Mode
 * \details
 * This function flush all D-Cache lines.
 * Command \ref CCM_DC_WB_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void SFlushDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WB_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush all D-Cache lines in U-Mode
 * \details
 * This function flush all D-Cache lines.
 * Command \ref CCM_DC_WB_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed
 */
__STATIC_INLINE void UFlushDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WB_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate all D-Cache lines in M-Mode
 * \details
 * This function flush and invalidate all D-Cache lines.
 * Command \ref CCM_DC_WBINVAL_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \param [in]    addr    start address to be flushed and locked
 */
__STATIC_INLINE void MFlushInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WBINVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate all D-Cache lines in S-Mode
 * \details
 * This function flush and invalidate all D-Cache lines.
 * Command \ref CCM_DC_WBINVAL_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \param [in]    addr    start address to be flushed and locked
 */
__STATIC_INLINE void SFlushInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WBINVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

/**
 * \brief  Flush and invalidate all D-Cache lines in U-Mode
 * \details
 * This function flush and invalidate all D-Cache lines.
 * Command \ref CCM_DC_WBINVAL_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \param [in]    addr    start address to be flushed and locked
 */
__STATIC_INLINE void UFlushInvalDCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WBINVAL_ALL);
    FlushPipeCCM();
    __RWMB();
}

#if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1)
/**
 * \brief  Flush all Cluster Cache in M-Mode
 * \details
 * This function flush all Cluster Cache.
 * Command \ref SMPCC_CMD_WB_ALL is written to SMPCC CMD register.
 * \remarks
 * This function must be executed in M-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t MFlushCCache(void)
{
    SMPCC_CMD->CC_mCMD = (SMPCC_CMD->CC_mCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WB_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_mCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_mCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Flush all Cluster Cache in S-Mode
 * \details
 * This function flush all Cluster Cache.
 * Command \ref SMPCC_CMD_WB_ALL is written to SMPCC CMD register.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t SFlushCCache(void)
{
    SMPCC_CMD->CC_sCMD = (SMPCC_CMD->CC_sCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WB_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_sCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_sCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Flush all Cluster Cache in U-Mode
 * \details
 * This function flush all Cluster Cache.
 * Command \ref SMPCC_CMD_WB_ALL is written to SMPCC CMD register.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t UFlushCCache(void)
{
    SMPCC_CMD->CC_uCMD = (SMPCC_CMD->CC_uCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WB_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_uCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_uCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Flush and invalidate all Cluster Cache in M-Mode
 * \details
 * This function flush and invalidate all Cluster Cache.
 * Command \ref SMPCC_CMD_WBINVAL_ALL is written to SMPCC CMD register.
 * \remarks
 * This function must be executed in M-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t MFlushInvalCCache(void)
{
    SMPCC_CMD->CC_mCMD = (SMPCC_CMD->CC_mCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WBINVAL_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_mCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_mCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Flush and invalidate all Cluster Cache in S-Mode
 * \details
 * This function flush and invalidate all Cluster Cache.
 * Command \ref SMPCC_CMD_WBINVAL_ALL is written to SMPCC CMD register.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t SFlushInvalCCache(void)
{
    SMPCC_CMD->CC_sCMD = (SMPCC_CMD->CC_sCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WBINVAL_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_sCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_sCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Flush and invalidate all Cluster Cache in U-Mode
 * \details
 * This function flush and invalidate all Cluster Cache.
 * Command \ref SMPCC_CMD_WBINVAL_ALL is written to SMPCC CMD register.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t UFlushInvalCCache(void)
{
    SMPCC_CMD->CC_uCMD = (SMPCC_CMD->CC_uCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WBINVAL_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_uCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_uCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Invalidate all D-Cache and Cluster Cache in M-Mode
 * \details
 * This function unlock and invalidate all D-Cache and Cluster Cache.
 * Command \ref CCM_DC_INVAL_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 */
__STATIC_INLINE void MInvalDCacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_INVAL_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_INVALID_ALL = _VAL2FLD(SMPCC_CMD_INVALID_ALL, 1);
    while(_FLD2VAL(SMPCC_CMD_INVALID_ALL, SMPCC_CMD->CC_INVALID_ALL));
    __RWMB();
}

/**
 * \brief  Invalidate all D-Cache and Cluster Cache in S-Mode
 * \details
 * This function unlock and invalidate all D-Cache and Cluster Cache.
 * Command \ref CCM_DC_INVAL_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 */
__STATIC_INLINE void SInvalDCacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_INVAL_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_INVALID_ALL = _VAL2FLD(SMPCC_CMD_INVALID_ALL, 1);
    while(_FLD2VAL(SMPCC_CMD_INVALID_ALL, SMPCC_CMD->CC_INVALID_ALL));
    __RWMB();
}

/**
 * \brief  Invalidate all D-Cache and Cluster Cache in U-Mode
 * \details
 * This function unlock and invalidate all D-Cache and Cluster Cache.
 * Command \ref CCM_DC_INVAL_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 */
__STATIC_INLINE void UInvalDCacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_INVAL_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_INVALID_ALL = _VAL2FLD(SMPCC_CMD_INVALID_ALL, 1);
    while(_FLD2VAL(SMPCC_CMD_INVALID_ALL, SMPCC_CMD->CC_INVALID_ALL));
    __RWMB();
}

/**
 * \brief  Flush all D-Cache and Cluster Cache in M-Mode
 * \details
 * This function flush all D-Cache and Cluster Cache.
 * Command \ref CCM_DC_WB_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t MFlushDCacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WB_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_mCMD = (SMPCC_CMD->CC_mCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WB_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_mCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_mCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Flush all D-Cache and Cluster Cache in S-Mode
 * \details
 * This function flush all D-Cache and Cluster Cache.
 * Command \ref CCM_DC_WB_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t SFlushDCacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WB_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_sCMD = (SMPCC_CMD->CC_sCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WB_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_sCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_sCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Flush all D-Cache and Cluster Cache in U-Mode
 * \details
 * This function flush all D-Cache and Cluster Cache.
 * Command \ref CCM_DC_WB_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t UFlushDCacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WB_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_uCMD = (SMPCC_CMD->CC_uCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WB_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_uCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_uCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Flush and invalidate all D-Cache and Cluster Cache in M-Mode
 * \details
 * This function flush and invalidate all D-Cache and Cluster Cache.
 * Command \ref CCM_DC_WBINVAL_ALL is written to CSR \ref CSR_CCM_MCOMMAND.
 * \remarks
 * This function must be executed in M-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t MFlushInvalDCacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_MCOMMAND, CCM_DC_WBINVAL_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_mCMD = (SMPCC_CMD->CC_mCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WBINVAL_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_mCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_mCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Flush and invalidate all D-Cache and Cluster Cache in S-Mode
 * \details
 * This function flush and invalidate all D-Cache and Cluster Cache.
 * Command \ref CCM_DC_WBINVAL_ALL is written to CSR \ref CSR_CCM_SCOMMAND.
 * \remarks
 * This function must be executed in M/S-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t SFlushInvalDCacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_SCOMMAND, CCM_DC_WBINVAL_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_sCMD = (SMPCC_CMD->CC_sCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WBINVAL_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_sCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_sCMD);
    __RWMB();
    return res;
}

/**
 * \brief  Flush and invalidate all D-Cache and Cluster Cache in U-Mode
 * \details
 * This function flush and invalidate all D-Cache and Cluster Cache.
 * Command \ref CCM_DC_WBINVAL_ALL is written to CSR \ref CSR_CCM_UCOMMAND.
 * \remarks
 * This function must be executed in M/S/U-Mode only.
 * \return Operation result, see enum \ref SMPCC_CMD_RESULT
 */
__STATIC_INLINE int32_t UFlushInvalDCacheCCache(void)
{
    __RV_CSR_WRITE(CSR_CCM_UCOMMAND, CCM_DC_WBINVAL_ALL);
    FlushPipeCCM();
    SMPCC_CMD->CC_uCMD = (SMPCC_CMD->CC_uCMD & ~SMPCC_CMD_xCMD_CMD_Msk) |
                         _VAL2FLD(SMPCC_CMD_xCMD_CMD, SMPCC_CMD_xCMD_CMD_WBINVAL_ALL);
    while(_FLD2VAL(SMPCC_CMD_xCMD_COMPLETE, SMPCC_CMD->CC_uCMD) == 0);
    int32_t res = _FLD2VAL(SMPCC_CMD_xCMD_RESULT, SMPCC_CMD->CC_uCMD);
    __RWMB();
    return res;
}
#endif /* #if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1) */

#endif /* defined(__CCM_PRESENT) && (__CCM_PRESENT == 1) */

/** @} */ /* End of Doxygen Group NMSIS_Core_DCache */
#endif /* defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_CACHE_H__ */
