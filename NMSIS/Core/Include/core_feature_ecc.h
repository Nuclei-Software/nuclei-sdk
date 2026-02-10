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
 */
#ifndef __CORE_FEATURE_ECC_H__
#define __CORE_FEATURE_ECC_H__
/*!
 * @file     core_feature_ecc.h
 * @brief    ECC feature API header file for Nuclei N/NX Core
 */
/*
 * ECC Feature Configuration Macro:
 *
 * 1. __ECC_PRESENT:  Define whether Error Correction Code (ECC) feature is present or not
 *   * 0: Not present
 *   * 1: Present
 * 2. __ICACHE_PRESENT:  Define whether I-Cache Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 3. __DCACHE_PRESENT:  Define whether D-Cache Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 4. __CCM_PRESENT:  Define whether Nuclei Cache Control and Maintainence(CCM) Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "core_feature_base.h"

#if defined(__ECC_PRESENT) && (__ECC_PRESENT == 1)
/**
 * \defgroup NMSIS_Core_ECC_Functions   ECC Functions
 * \ingroup  NMSIS_Core
 * \brief    Functions that related to the ECC feature
 * \details
 * These functions provide access to the Error Correction Code (ECC) feature
 * available in Nuclei N/NX processor cores. ECC is a memory protection mechanism
 * that can detect and correct single-bit errors and detect double-bit errors in
 * memory systems such as caches and local memories.
 *
 * The ECC feature includes:
 * - Detection of ECC support in various memory components (I-Cache, D-Cache, ILM, DLM, TLB)
 * - Control functions to enable/disable ECC for different memory components
 * - Error injection capabilities for testing purposes
 * - Error detection and clearing functions for both single-bit and double-bit errors
 * - Configuration of ECC exception handling
 *
 *   @{
 */

/**
 * \brief  Check if the core globally supports ECC
 * \details
 * This function reads the machine configuration info CSR
 * and checks if ECC is supported globally in the core.
 * \return 1 if ECC is globally supported, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsGlobalSupportECC(void)
{
    CSR_MCFGINFO_Type mcfginfo;
    mcfginfo.d = __RV_CSR_READ(CSR_MCFG_INFO);
    return mcfginfo.b.ecc;
}

/**
 * \brief  Check if I-Cache supports ECC
 * \details
 * This function checks if both I-Cache and ECC are supported in the core.
 * \return 1 if I-Cache supports ECC, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsICacheSupportECC(void)
{
    CSR_MCFGINFO_Type mcfginfo;
    CSR_MICFGINFO_Type micfginfo;
    mcfginfo.d = __RV_CSR_READ(CSR_MCFG_INFO);
    micfginfo.d = __RV_CSR_READ(CSR_MICFG_INFO);
    return mcfginfo.b.icache && micfginfo.b.ecc;
}

/**
 * \brief  Check if D-Cache supports ECC
 * \details
 * This function checks if both D-Cache and ECC are supported in the core.
 * \return 1 if D-Cache supports ECC, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsDCacheSupportECC(void)
{
    CSR_MCFGINFO_Type mcfginfo;
    CSR_MDCFGINFO_Type mdcfginfo;
    mcfginfo.d = __RV_CSR_READ(CSR_MCFG_INFO);
    mdcfginfo.d = __RV_CSR_READ(CSR_MDCFG_INFO);
    return mcfginfo.b.dcache && mdcfginfo.b.ecc;
}

/**
 * \brief  Check if TLB supports ECC
 * \details
 * This function checks if both PLIC and TLB ECC are supported in the core.
 * Note: TLB is only present with MMU, and when PLIC is present, MMU will be present.
 * \return 1 if TLB supports ECC, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsTLBSupportECC(void)
{
    CSR_MCFGINFO_Type mcfginfo;
    CSR_MTLBCFGINFO_Type mtlbcfginfo;
    mcfginfo.d = __RV_CSR_READ(CSR_MCFG_INFO);
    mtlbcfginfo.d = __RV_CSR_READ(CSR_MTLBCFG_INFO);
    /* TLB only present with MMU, when PLIC present MMU will present */
    return mcfginfo.b.plic && mtlbcfginfo.b.ecc;
}

/**
 * \brief  Check if ILM supports ECC
 * \details
 * This function checks if both ILM and ECC are supported in the core.
 * \return 1 if ILM supports ECC, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsILMSupportECC(void)
{
    CSR_MCFGINFO_Type mcfginfo;
    CSR_MICFGINFO_Type micfginfo;
    mcfginfo.d = __RV_CSR_READ(CSR_MCFG_INFO);
    micfginfo.d = __RV_CSR_READ(CSR_MICFG_INFO);
    return mcfginfo.b.ilm && micfginfo.b.lm_ecc;
}

/**
 * \brief  Check if DLM supports ECC
 * \details
 * This function checks if both DLM and ECC are supported in the core.
 * \return 1 if DLM supports ECC, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsDLMSupportECC(void)
{
    CSR_MCFGINFO_Type mcfginfo;
    CSR_MDCFGINFO_Type mdcfginfo;
    mcfginfo.d = __RV_CSR_READ(CSR_MCFG_INFO);
    mdcfginfo.d = __RV_CSR_READ(CSR_MDCFG_INFO);
    return mcfginfo.b.dlm && mdcfginfo.b.lm_ecc;
}

/**
 * \brief  Check if XOR error injection mode is supported
 * \details
 * This function reads the machine ECC code CSR and checks if error injection mode is suppported.
 * \return 1 if XOR error injection mode is enabled, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsXorErrorInjectMode(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.ecc_inj_mode;
}

/**
 * \brief  Enable ECC for I-Cache
 * \details
 * This function enables ECC for the instruction cache by setting the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableICacheECC(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_IC_ECC_EN);
}

/**
 * \brief  Disable ECC for I-Cache
 * \details
 * This function disables ECC for the instruction cache by clearing the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableICacheECC(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_IC_ECC_EN);
}

/**
 * \brief  Enable ECC exception for I-Cache
 * \details
 * This function enables ECC exception for the instruction cache by setting the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableICacheECCExcp(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_IC_ECC_EXCP_EN);
}

/**
 * \brief  Disable ECC exception for I-Cache
 * \details
 * This function disables ECC exception for the instruction cache by clearing the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableICacheECCExcp(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_IC_ECC_EXCP_EN);
}

/**
 * \brief  Enable ECC checking for I-Cache
 * \details
 * This function enables ECC checking for the instruction cache by setting the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableICacheECCCheck(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_IC_ECC_CHK_EN);
}

/**
 * \brief  Disable ECC checking for I-Cache
 * \details
 * This function disables ECC checking for the instruction cache by clearing the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableICacheECCCheck(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_IC_ECC_CHK_EN);
}

/**
 * \brief  Enable ECC for D-Cache
 * \details
 * This function enables ECC for the data cache by setting the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableDCacheECC(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_DC_ECC_EN);
}

/**
 * \brief  Disable ECC for D-Cache
 * \details
 * This function disables ECC for the data cache by clearing the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableDCacheECC(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_DC_ECC_EN);
}

/**
 * \brief  Enable ECC exception for D-Cache
 * \details
 * This function enables ECC exception for the data cache by setting the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableDCacheECCExcp(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_DC_ECC_EXCP_EN);
}

/**
 * \brief  Disable ECC exception for D-Cache
 * \details
 * This function disables ECC exception for the data cache by clearing the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableDCacheECCExcp(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_DC_ECC_EXCP_EN);
}

/**
 * \brief  Enable ECC checking for D-Cache
 * \details
 * This function enables ECC checking for the data cache by setting the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableDCacheECCCheck(void)
{
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_DC_ECC_CHK_EN);
}

/**
 * \brief  Disable ECC checking for D-Cache
 * \details
 * This function disables ECC checking for the data cache by clearing the appropriate bit
 * in the machine cache control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableDCacheECCCheck(void)
{
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_DC_ECC_CHK_EN);
}

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1)
/**
 * \brief  Inject error into I-Cache Tag RAM
 * \details
 * This function injects an error into the I-Cache Tag RAM at the specified address
 * with the given ECC code.
 * \param[in]  ecc_code  ECC code to inject
 * \param[in]  addr      Address where error should be injected
 */
__STATIC_FORCEINLINE void ECC_ICacheTRamErrInject(uint32_t ecc_code, void *addr)
{
    /* Write ecc_code into mecc_code csr also clear all error status */
    __RV_CSR_WRITE(CSR_MECC_CODE, ecc_code);
    MInvalICacheLine((unsigned long)addr);
    __RWMB();
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_IC_TRAM_ECC_INJ_EN);
    MLockICacheLine((unsigned long)addr);
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_IC_TRAM_ECC_INJ_EN);
    __RWMB();
}

/**
 * \brief  Inject error into I-Cache Data RAM
 * \details
 * This function injects an error into the I-Cache Data RAM at the specified address
 * with the given ECC code.
 * \param[in]  ecc_code  ECC code to inject
 * \param[in]  addr      Address where error should be injected
 */
__STATIC_FORCEINLINE void ECC_ICacheDRamErrInject(uint32_t ecc_code, void *addr)
{
    /* Write ecc_code into mecc_code csr also clear all error status */
    __RV_CSR_WRITE(CSR_MECC_CODE, ecc_code);
    MInvalICacheLine((unsigned long)addr);
    __RWMB();
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_IC_DRAM_ECC_INJ_EN);
    MLockICacheLine((unsigned long)addr);
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_IC_DRAM_ECC_INJ_EN);
    __RWMB();
}

/**
 * \brief  Restore I-Cache error at specified address
 * \details
 * This function restores the correct ECC code for the I-Cache line at the specified address.
 * \param[in]  addr  Address to restore
 */
__STATIC_FORCEINLINE void ECC_ICacheErrRestore(void *addr)
{
    /* Re-lock cache to restore the correct ecc code */
    MInvalICacheLine((unsigned long)addr);
    MLockICacheLine((unsigned long)addr);
}
#endif /* #if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1) */

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1)
/**
 * \brief  Inject error into D-Cache Tag RAM
 * \details
 * This function injects an error into the D-Cache Tag RAM at the specified address
 * with the given ECC code.
 * \param[in]  ecc_code  ECC code to inject
 * \param[in]  addr      Address where error should be injected
 */
__STATIC_FORCEINLINE void ECC_DCacheTRamErrInject(uint32_t ecc_code, void *addr)
{
    /* Write ecc_code into mecc_code csr also clear all error status */
    __RV_CSR_WRITE(CSR_MECC_CODE, ecc_code);
    ECC_DisableDCacheECCCheck();
    MFlushInvalDCacheLine((unsigned long)addr);
    __RWMB();
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_DC_TRAM_ECC_INJ_EN);
    MLockDCacheLine((unsigned long)addr);
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_DC_TRAM_ECC_INJ_EN);
    __RWMB();
    ECC_EnableDCacheECCCheck();
}

/**
 * \brief  Inject error into D-Cache Data RAM
 * \details
 * This function injects an error into the D-Cache Data RAM at the specified address
 * with the given ECC code.
 * \param[in]  ecc_code  ECC code to inject
 * \param[in]  addr      Address where error should be injected
 */
__STATIC_FORCEINLINE void ECC_DCacheDRamErrInject(uint32_t ecc_code, void *addr)
{
    /* Write ecc_code into mecc_code csr also clear all error status */
    __RV_CSR_WRITE(CSR_MECC_CODE, ecc_code);
    ECC_DisableDCacheECCCheck();
    MFlushInvalDCacheLine((unsigned long)addr);
    __RWMB();
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_DC_DRAM_ECC_INJ_EN);
    MLockDCacheLine((unsigned long)addr);
    __RV_CSR_CLEAR(CSR_MCACHE_CTL, MCACHE_CTL_DC_DRAM_ECC_INJ_EN);
    __RWMB();
    ECC_EnableDCacheECCCheck();
}

/**
 * \brief  Restore D-Cache error at specified address
 * \details
 * This function restores the correct ECC code for the D-Cache line at the specified address.
 * \param[in]  addr  Address to restore
 */
__STATIC_FORCEINLINE void ECC_DCacheErrRestore(void *addr)
{
    ECC_DisableDCacheECCCheck();
    MFlushInvalDCacheLine((unsigned long)addr);
    MLockDCacheLine((unsigned long)addr);
    ECC_EnableDCacheECCCheck();
}
#endif /* #if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1) */
#endif /* #if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1) */

/**
 * \brief  Enable ILM
 * \details
 * This function enables ILM by setting the appropriate bit in the machine ILM control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableILM(void)
{
    __RV_CSR_SET(CSR_MILM_CTL, MILM_CTL_ILM_EN);
}

/**
 * \brief  Disable ILM
 * \details
 * This function disables ILM by clearing the appropriate bit in the machine ILM control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableILM(void)
{
    __RV_CSR_CLEAR(CSR_MILM_CTL, MILM_CTL_ILM_EN);
}

/**
 * \brief  Enable ECC for ILM
 * \details
 * This function enables ECC for ILM by setting the appropriate bit in the machine ILM control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableILMECC(void)
{
    __RV_CSR_SET(CSR_MILM_CTL, MILM_CTL_ILM_ECC_EN);
}

/**
 * \brief  Disable ECC for ILM
 * \details
 * This function disables ECC for ILM by clearing the appropriate bit in the machine ILM control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableILMECC(void)
{
    __RV_CSR_CLEAR(CSR_MILM_CTL, MILM_CTL_ILM_ECC_EN);
}

/**
 * \brief  Enable ECC exception for ILM
 * \details
 * This function enables ECC exception for ILM by setting the appropriate bit in the machine ILM control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableILMECCExcp(void)
{
    __RV_CSR_SET(CSR_MILM_CTL, MILM_CTL_ILM_ECC_EXCP_EN);
}

/**
 * \brief  Disable ECC exception for ILM
 * \details
 * This function disables ECC exception for ILM by clearing the appropriate bit in the machine ILM control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableILMECCExcp(void)
{
    __RV_CSR_CLEAR(CSR_MILM_CTL, MILM_CTL_ILM_ECC_EXCP_EN);
}

/**
 * \brief  Enable ECC checking for ILM
 * \details
 * This function enables ECC checking for ILM by setting the appropriate bit in the machine ILM control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableILMECCCheck(void)
{
    __RV_CSR_SET(CSR_MILM_CTL, MILM_CTL_ILM_ECC_CHK_EN);
}

/**
 * \brief  Disable ECC checking for ILM
 * \details
 * This function disables ECC checking for ILM by clearing the appropriate bit in the machine ILM control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableILMECCCheck(void)
{
    __RV_CSR_CLEAR(CSR_MILM_CTL, MILM_CTL_ILM_ECC_CHK_EN);
}

/**
 * \brief  Inject error into ILM
 * \details
 * This function injects an error into the ILM at the specified address with the given ECC code.
 * \param[in]  ecc_code  ECC code to inject
 * \param[in]  addr      Address where error should be injected
 */
__STATIC_FORCEINLINE void ECC_ILMErrInject(uint32_t ecc_code, void *addr)
{
    /* Write ecc_code into mecc_code csr also clear all error status */
    __RV_CSR_WRITE(CSR_MECC_CODE, ecc_code);
    ECC_DisableILMECCCheck();
    uint32_t val = __LW(addr);
    __RWMB(); // make sure setting the ECC_INJ_EN bit before any other memory access
    __RV_CSR_SET(CSR_MILM_CTL, MILM_CTL_ILM_ECC_INJ_EN);
    __SW(addr, val);
    __RWMB(); // make sure the error injection is finished
    __RV_CSR_CLEAR(CSR_MILM_CTL, MILM_CTL_ILM_ECC_INJ_EN);
    __RWMB(); // make sure clearing the ECC_INJ_EN bit before any other memory access
    ECC_EnableILMECCCheck();
}

/**
 * \brief  Restore ILM error at specified address
 * \details
 * This function restores the correct ECC code for the ILM at the specified address.
 * \param[in]  addr  Address to restore
 */
__STATIC_FORCEINLINE void ECC_ILMErrRestore(void *addr)
{
    ECC_DisableILMECCCheck();
    __RWMB();
    __SW(addr, __LW(addr));
    __RWMB();
    ECC_EnableILMECCCheck();
}

/**
 * \brief  Enable DLM
 * \details
 * This function enables DLM by setting the appropriate bit in the machine DLM control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableDLM(void)
{
    __RV_CSR_SET(CSR_MDLM_CTL, MDLM_CTL_DLM_EN);
}

/**
 * \brief  Disable DLM
 * \details
 * This function disables DLM by clearing the appropriate bit in the machine DLM control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableDLM(void)
{
    __RV_CSR_CLEAR(CSR_MDLM_CTL, MDLM_CTL_DLM_EN);
}

/**
 * \brief  Enable ECC for DLM
 * \details
 * This function enables ECC for DLM by setting the appropriate bit in the machine DLM control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableDLMECC(void)
{
    __RV_CSR_SET(CSR_MDLM_CTL, MDLM_CTL_DLM_ECC_EN);
}

/**
 * \brief  Disable ECC for DLM
 * \details
 * This function disables ECC for DLM by clearing the appropriate bit in the machine DLM control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableDLMECC(void)
{
    __RV_CSR_CLEAR(CSR_MDLM_CTL, MDLM_CTL_DLM_ECC_EN);
}

/**
 * \brief  Enable ECC exception for DLM
 * \details
 * This function enables ECC exception for DLM by setting the appropriate bit in the machine DLM control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableDLMECCExcp(void)
{
    __RV_CSR_SET(CSR_MDLM_CTL, MDLM_CTL_DLM_ECC_EXCP_EN);
}

/**
 * \brief  Disable ECC exception for DLM
 * \details
 * This function disables ECC exception for DLM by clearing the appropriate bit in the machine DLM control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableDLMECCExcp(void)
{
    __RV_CSR_CLEAR(CSR_MDLM_CTL, MDLM_CTL_DLM_ECC_EXCP_EN);
}

/**
 * \brief  Enable ECC checking for DLM
 * \details
 * This function enables ECC checking for DLM by setting the appropriate bit in the machine DLM control CSR.
 */
__STATIC_FORCEINLINE void ECC_EnableDLMECCCheck(void)
{
    __RV_CSR_SET(CSR_MDLM_CTL, MDLM_CTL_DLM_ECC_CHK_EN);
}

/**
 * \brief  Disable ECC checking for DLM
 * \details
 * This function disables ECC checking for DLM by clearing the appropriate bit in the machine DLM control CSR.
 */
__STATIC_FORCEINLINE void ECC_DisableDLMECCCheck(void)
{
    __RV_CSR_CLEAR(CSR_MDLM_CTL, MDLM_CTL_DLM_ECC_CHK_EN);
}

/**
 * \brief  Inject error into DLM
 * \details
 * This function injects an error into the DLM at the specified address with the given ECC code.
 * \param[in]  ecc_code  ECC code to inject
 * \param[in]  addr      Address where error should be injected
 */
__STATIC_FORCEINLINE void ECC_DLMErrInject(uint32_t ecc_code, void *addr)
{
    /* Write ecc_code into mecc_code csr also clear all error status */
    __RV_CSR_WRITE(CSR_MECC_CODE, ecc_code);
    ECC_DisableDLMECCCheck();
    uint32_t val = __LW(addr);
    __RWMB(); // make sure setting the ECC_INJ_EN bit before any other memory access
    __RV_CSR_SET(CSR_MDLM_CTL, MDLM_CTL_DLM_ECC_INJ_EN);
    __SW(addr, val);
    __RWMB(); // make sure the error injection is finished
    __RV_CSR_CLEAR(CSR_MDLM_CTL, MDLM_CTL_DLM_ECC_INJ_EN);
    __RWMB(); // make sure clearing the ECC_INJ_EN bit before any other memory access
    ECC_EnableDLMECCCheck();
}

/**
 * \brief  Restore DLM error at specified address
 * \details
 * This function restores the correct ECC code for the DLM at the specified address.
 * \param[in]  addr  Address to restore
 */
__STATIC_FORCEINLINE void ECC_DLMErrRestore(void *addr)
{
    ECC_DisableDLMECCCheck();
    __RWMB();
    __SW(addr, __LW(addr));
    __RWMB();
    ECC_EnableDLMECCCheck();
}

#define ECC_ERROR_RAMID_MASK_ICACHE 1U
#define ECC_ERROR_RAMID_MASK_DCACHE 2U
#define ECC_ERROR_RAMID_MASK_TLB 4U
#define ECC_ERROR_RAMID_MASK_ILM 8U
#define ECC_ERROR_RAMID_MASK_DLM 16U

/**
 * \brief  Check if any single-bit error has occurred
 * \details
 * This function checks if any single-bit error has occurred by reading the machine ECC code CSR.
 * \return 1 if any single-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsAnySingleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.sramid;
}

/**
 * \brief  Check if I-Cache single-bit error has occurred
 * \details
 * This function checks if a single-bit error has occurred in the I-Cache.
 * \return 1 if I-Cache single-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsICacheSingleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.sramid & ECC_ERROR_RAMID_MASK_ICACHE;
}

/**
 * \brief  Check if D-Cache single-bit error has occurred
 * \details
 * This function checks if a single-bit error has occurred in the D-Cache.
 * \return 1 if D-Cache single-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsDCacheSingleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.sramid & ECC_ERROR_RAMID_MASK_DCACHE;
}

/**
 * \brief  Check if TLB single-bit error has occurred
 * \details
 * This function checks if a single-bit error has occurred in the TLB.
 * \return 1 if TLB single-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsTLBSingleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.sramid & ECC_ERROR_RAMID_MASK_DLM;
}

/**
 * \brief  Check if ILM single-bit error has occurred
 * \details
 * This function checks if a single-bit error has occurred in the ILM.
 * \return 1 if ILM single-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsILMSingleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.sramid & ECC_ERROR_RAMID_MASK_ILM;
}

/**
 * \brief  Check if DLM single-bit error has occurred
 * \details
 * This function checks if a single-bit error has occurred in the DLM.
 * \return 1 if DLM single-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsDLMSingleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.sramid & ECC_ERROR_RAMID_MASK_DLM;
}

/**
 * \brief  Clear all single-bit errors
 * \details
 * This function clears all single-bit errors by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearAllSingleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_SRAMID);
}

/**
 * \brief  Clear I-Cache single-bit error
 * \details
 * This function clears the single-bit error in the I-Cache by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearICacheSingleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_SRAMID_IC);
}

/**
 * \brief  Clear D-Cache single-bit error
 * \details
 * This function clears the single-bit error in the D-Cache by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearDCacheSingleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_SRAMID_DC);
}

/**
 * \brief  Clear TLB single-bit error
 * \details
 * This function clears the single-bit error in the TLB by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearTLBSingleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_SRAMID_TLB);
}

/**
 * \brief  Clear ILM single-bit error
 * \details
 * This function clears the single-bit error in the ILM by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearILMSingleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_SRAMID_ILM);
}

/**
 * \brief  Clear DLM single-bit error
 * \details
 * This function clears the single-bit error in the DLM by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearDLMSingleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_SRAMID_DLM);
}

/**
 * \brief  Check if any double-bit error has occurred
 * \details
 * This function checks if any double-bit error has occurred by reading the machine ECC code CSR.
 * \return 1 if any double-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsAnyDoubleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.ramid;
}

/**
 * \brief  Check if I-Cache double-bit error has occurred
 * \details
 * This function checks if a double-bit error has occurred in the I-Cache.
 * \return 1 if I-Cache double-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsICacheDoubleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.ramid & ECC_ERROR_RAMID_MASK_ICACHE;
}

/**
 * \brief  Check if D-Cache double-bit error has occurred
 * \details
 * This function checks if a double-bit error has occurred in the D-Cache.
 * \return 1 if D-Cache double-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsDCacheDoubleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.ramid & ECC_ERROR_RAMID_MASK_DCACHE;
}

/**
 * \brief  Check if TLB double-bit error has occurred
 * \details
 * This function checks if a double-bit error has occurred in the TLB.
 * \return 1 if TLB double-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsTLBDoubleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.ramid & ECC_ERROR_RAMID_MASK_DLM;
}

/**
 * \brief  Check if ILM double-bit error has occurred
 * \details
 * This function checks if a double-bit error has occurred in the ILM.
 * \return 1 if ILM double-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsILMDoubleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.ramid & ECC_ERROR_RAMID_MASK_ILM;
}

/**
 * \brief  Check if DLM double-bit error has occurred
 * \details
 * This function checks if a double-bit error has occurred in the DLM.
 * \return 1 if DLM double-bit error has occurred, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t ECC_IsDLMDoubleBitErrorOccured(void)
{
    CSR_MECC_CODE_Type mecc_code;
    mecc_code.d = __RV_CSR_READ(CSR_MECC_CODE);
    return mecc_code.b.ramid & ECC_ERROR_RAMID_MASK_DLM;
}

/**
 * \brief  Clear all double-bit errors
 * \details
 * This function clears all double-bit errors by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearAllDoubleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_RAMID);
}

/**
 * \brief  Clear I-Cache double-bit error
 * \details
 * This function clears the double-bit error in the I-Cache by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearICacheDoubleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_RAMID_IC);
}

/**
 * \brief  Clear D-Cache double-bit error
 * \details
 * This function clears the double-bit error in the D-Cache by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearDCacheDoubleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_RAMID_DC);
}

/**
 * \brief  Clear TLB double-bit error
 * \details
 * This function clears the double-bit error in the TLB by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearTLBDoubleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_RAMID_TLB);
}

/**
 * \brief  Clear ILM double-bit error
 * \details
 * This function clears the double-bit error in the ILM by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearILMDoubleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_RAMID_ILM);
}

/**
 * \brief  Clear DLM double-bit error
 * \details
 * This function clears the double-bit error in the DLM by clearing the appropriate bits
 * in the machine ECC code CSR.
 */
__STATIC_FORCEINLINE void ECC_ClearDLMDoubleBitError(void)
{
    __RV_CSR_CLEAR(CSR_MECC_CODE, MECC_CODE_RAMID_DLM);
}

/**
 * \brief  Generate ECC code for a 32-bit value
 * \details
 * This function calculates the ECC code for a 32-bit input value
 * using Hamming code algorithm. It generates a 7-bit ECC code that can be used to
 * detect and correct single-bit errors and detect double-bit errors.
 * \param[in]  a  32-bit value for which ECC code is to be generated
 * \return  8-bit ECC code (7 bits used, 1 bit unused)
 */
static uint8_t ECC_GenerateECCCodeU32(uint32_t a)
{
    uint8_t ecc_bits = 0;
    uint8_t ecc_bit_0 = ((a >> 0) & 1) ^ ((a >> 1) & 1) ^ ((a >> 3) & 1) ^ ((a >> 4) & 1) ^
                        ((a >> 6) & 1) ^ ((a >> 8) & 1) ^ ((a >> 10) & 1) ^ ((a >> 12) & 1) ^
                        ((a >> 14) & 1) ^ ((a >> 17) & 1) ^ ((a >> 19) & 1) ^ ((a >> 20) & 1) ^
                        ((a >> 24) & 1) ^ ((a >> 28) & 1);
    uint8_t ecc_bit_1 = ((a >> 0) & 1) ^ ((a >> 2) & 1) ^ ((a >> 3) & 1) ^ ((a >> 5) & 1) ^
                        ((a >> 6) & 1) ^ ((a >> 9) & 1) ^ ((a >> 11) & 1) ^ ((a >> 12) & 1) ^
                        ((a >> 15) & 1) ^ ((a >> 20) & 1) ^ ((a >> 22) & 1) ^ ((a >> 25) & 1) ^
                        ((a >> 29) & 1);
    uint8_t ecc_bit_2 = ((a >> 1) & 1) ^ ((a >> 2) & 1) ^ ((a >> 3) & 1) ^ ((a >> 7) & 1) ^
                        ((a >> 8) & 1) ^ ((a >> 9) & 1) ^ ((a >> 13) & 1) ^ ((a >> 14) & 1) ^
                        ((a >> 15) & 1) ^ ((a >> 18) & 1) ^ ((a >> 21) & 1) ^ ((a >> 22) & 1) ^
                        ((a >> 26) & 1) ^ ((a >> 30) & 1);
    uint8_t ecc_bit_3 = ((a >> 4) & 1) ^ ((a >> 5) & 1) ^ ((a >> 6) & 1) ^ ((a >> 7) & 1) ^
                        ((a >> 8) & 1) ^ ((a >> 9) & 1) ^ ((a >> 16) & 1) ^ ((a >> 17) & 1) ^
                        ((a >> 18) & 1) ^ ((a >> 23) & 1) ^ ((a >> 24) & 1) ^ ((a >> 25) & 1) ^
                        ((a >> 26) & 1) ^ ((a >> 31) & 1);
    uint8_t ecc_bit_4 = ((a >> 10) & 1) ^ ((a >> 11) & 1) ^ ((a >> 12) & 1) ^ ((a >> 13) & 1) ^
                        ((a >> 14) & 1) ^ ((a >> 15) & 1) ^ ((a >> 16) & 1) ^ ((a >> 17) & 1) ^
                        ((a >> 18) & 1) ^ ((a >> 27) & 1) ^ ((a >> 28) & 1) ^ ((a >> 29) & 1) ^
                        ((a >> 30) & 1) ^ ((a >> 31) & 1);
    uint8_t ecc_bit_5 = ((a >> 19) & 1) ^ ((a >> 20) & 1) ^ ((a >> 21) & 1) ^ ((a >> 22) & 1) ^
                        ((a >> 23) & 1) ^ ((a >> 24) & 1) ^ ((a >> 25) & 1) ^ ((a >> 26) & 1) ^
                        ((a >> 27) & 1) ^ ((a >> 28) & 1) ^ ((a >> 29) & 1) ^ ((a >> 30) & 1) ^
                        ((a >> 31) & 1);
    uint8_t ecc_bit_6 = ((a >> 0) & 1) ^ ((a >> 1) & 1) ^ ((a >> 2) & 1) ^ ((a >> 4) & 1) ^
                        ((a >> 5) & 1) ^ ((a >> 7) & 1) ^ ((a >> 10) & 1) ^ ((a >> 11) & 1) ^
                        ((a >> 13) & 1) ^ ((a >> 16) & 1) ^ ((a >> 19) & 1) ^ ((a >> 21) & 1) ^
                        ((a >> 23) & 1) ^ ((a >> 27) & 1);
    ecc_bits = (ecc_bit_6 << 6) | (ecc_bit_5 << 5) | (ecc_bit_4 << 4) | (ecc_bit_3 << 3) |
               (ecc_bit_2 << 2) | (ecc_bit_1 << 1) | ecc_bit_0;
    return ecc_bits;
}

/**
 * \brief  Generate ECC code for a 64-bit value
 * \details
 * This function calculates the ECC for a 64-bit input value
 * using Hamming code algorithm. It generates an 8-bit ECC code that can be used to
 * detect and correct single-bit errors and detect double-bit errors.
 * \param[in]  a  64-bit value for which ECC code is to be generated
 * \return  8-bit ECC code
 */
static uint8_t ECC_GenerateECCCodeU64(uint64_t a)
{
    uint8_t ecc_bits = 0;
    uint8_t ecc_bit_0 =
        (((a >> 0) & 1) ^ ((a >> 1) & 1) ^ ((a >> 3) & 1) ^ ((a >> 4) & 1) ^ ((a >> 6) & 1) ^
         ((a >> 8) & 1) ^ ((a >> 10) & 1) ^ ((a >> 12) & 1) ^ ((a >> 14) & 1) ^ ((a >> 17) & 1) ^
         ((a >> 20) & 1) ^ ((a >> 22) & 1) ^ ((a >> 24) & 1) ^ ((a >> 27) & 1) ^ ((a >> 30) & 1) ^
         ((a >> 32) & 1) ^ ((a >> 36) & 1) ^ ((a >> 37) & 1) ^ ((a >> 38) & 1) ^ ((a >> 40) & 1) ^
         ((a >> 42) & 1) ^ ((a >> 45) & 1) ^ ((a >> 47) & 1) ^ ((a >> 51) & 1) ^ ((a >> 54) & 1) ^
         ((a >> 57) & 1));
    uint8_t ecc_bit_1 =
        (((a >> 0) & 1) ^ ((a >> 2) & 1) ^ ((a >> 3) & 1) ^ ((a >> 5) & 1) ^ ((a >> 6) & 1) ^
         ((a >> 9) & 1) ^ ((a >> 11) & 1) ^ ((a >> 12) & 1) ^ ((a >> 15) & 1) ^ ((a >> 18) & 1) ^
         ((a >> 21) & 1) ^ ((a >> 22) & 1) ^ ((a >> 25) & 1) ^ ((a >> 28) & 1) ^ ((a >> 30) & 1) ^
         ((a >> 33) & 1) ^ ((a >> 37) & 1) ^ ((a >> 39) & 1) ^ ((a >> 40) & 1) ^ ((a >> 43) & 1) ^
         ((a >> 46) & 1) ^ ((a >> 47) & 1) ^ ((a >> 49) & 1) ^ ((a >> 52) & 1) ^ ((a >> 58) & 1) ^
         ((a >> 63) & 1));
    uint8_t ecc_bit_2 =
        (((a >> 1) & 1) ^ ((a >> 2) & 1) ^ ((a >> 3) & 1) ^ ((a >> 7) & 1) ^ ((a >> 8) & 1) ^
         ((a >> 9) & 1) ^ ((a >> 13) & 1) ^ ((a >> 14) & 1) ^ ((a >> 15) & 1) ^ ((a >> 19) & 1) ^
         ((a >> 23) & 1) ^ ((a >> 24) & 1) ^ ((a >> 25) & 1) ^ ((a >> 29) & 1) ^ ((a >> 30) & 1) ^
         ((a >> 34) & 1) ^ ((a >> 41) & 1) ^ ((a >> 42) & 1) ^ ((a >> 43) & 1) ^ ((a >> 48) & 1) ^
         ((a >> 49) & 1) ^ ((a >> 53) & 1) ^ ((a >> 54) & 1) ^ ((a >> 59) & 1) ^ ((a >> 62) & 1) ^
         ((a >> 63) & 1));
    uint8_t ecc_bit_3 =
        (((a >> 4) & 1) ^ ((a >> 5) & 1) ^ ((a >> 6) & 1) ^ ((a >> 7) & 1) ^ ((a >> 8) & 1) ^
         ((a >> 9) & 1) ^ ((a >> 16) & 1) ^ ((a >> 17) & 1) ^ ((a >> 18) & 1) ^ ((a >> 19) & 1) ^
         ((a >> 26) & 1) ^ ((a >> 27) & 1) ^ ((a >> 28) & 1) ^ ((a >> 29) & 1) ^ ((a >> 30) & 1) ^
         ((a >> 35) & 1) ^ ((a >> 36) & 1) ^ ((a >> 37) & 1) ^ ((a >> 44) & 1) ^ ((a >> 45) & 1) ^
         ((a >> 46) & 1) ^ ((a >> 47) & 1) ^ ((a >> 48) & 1) ^ ((a >> 49) & 1) ^ ((a >> 55) & 1) ^
         ((a >> 60) & 1));
    uint8_t ecc_bit_4 =
        (((a >> 10) & 1) ^ ((a >> 11) & 1) ^ ((a >> 12) & 1) ^ ((a >> 13) & 1) ^ ((a >> 14) & 1) ^
         ((a >> 15) & 1) ^ ((a >> 16) & 1) ^ ((a >> 17) & 1) ^ ((a >> 18) & 1) ^ ((a >> 19) & 1) ^
         ((a >> 31) & 1) ^ ((a >> 32) & 1) ^ ((a >> 33) & 1) ^ ((a >> 34) & 1) ^ ((a >> 35) & 1) ^
         ((a >> 36) & 1) ^ ((a >> 37) & 1) ^ ((a >> 50) & 1) ^ ((a >> 51) & 1) ^ ((a >> 52) & 1) ^
         ((a >> 53) & 1) ^ ((a >> 54) & 1) ^ ((a >> 55) & 1) ^ ((a >> 61) & 1) ^ ((a >> 62) & 1) ^
         ((a >> 63) & 1));
    uint8_t ecc_bit_5 =
        (((a >> 20) & 1) ^ ((a >> 21) & 1) ^ ((a >> 22) & 1) ^ ((a >> 23) & 1) ^ ((a >> 24) & 1) ^
         ((a >> 25) & 1) ^ ((a >> 26) & 1) ^ ((a >> 27) & 1) ^ ((a >> 28) & 1) ^ ((a >> 29) & 1) ^
         ((a >> 30) & 1) ^ ((a >> 31) & 1) ^ ((a >> 32) & 1) ^ ((a >> 33) & 1) ^ ((a >> 34) & 1) ^
         ((a >> 35) & 1) ^ ((a >> 36) & 1) ^ ((a >> 37) & 1) ^ ((a >> 56) & 1) ^ ((a >> 57) & 1) ^
         ((a >> 58) & 1) ^ ((a >> 59) & 1) ^ ((a >> 60) & 1) ^ ((a >> 61) & 1) ^ ((a >> 62) & 1) ^
         ((a >> 63) & 1));
    uint8_t ecc_bit_6 =
        (((a >> 38) & 1) ^ ((a >> 39) & 1) ^ ((a >> 40) & 1) ^ ((a >> 41) & 1) ^ ((a >> 42) & 1) ^
         ((a >> 43) & 1) ^ ((a >> 44) & 1) ^ ((a >> 45) & 1) ^ ((a >> 46) & 1) ^ ((a >> 47) & 1) ^
         ((a >> 48) & 1) ^ ((a >> 49) & 1) ^ ((a >> 50) & 1) ^ ((a >> 51) & 1) ^ ((a >> 52) & 1) ^
         ((a >> 53) & 1) ^ ((a >> 54) & 1) ^ ((a >> 55) & 1) ^ ((a >> 56) & 1) ^ ((a >> 57) & 1) ^
         ((a >> 58) & 1) ^ ((a >> 59) & 1) ^ ((a >> 60) & 1) ^ ((a >> 61) & 1) ^ ((a >> 62) & 1) ^
         ((a >> 63) & 1));
    uint8_t ecc_bit_7 =
        (((a >> 0) & 1) ^ ((a >> 1) & 1) ^ ((a >> 2) & 1) ^ ((a >> 4) & 1) ^ ((a >> 5) & 1) ^
         ((a >> 7) & 1) ^ ((a >> 10) & 1) ^ ((a >> 11) & 1) ^ ((a >> 13) & 1) ^ ((a >> 16) & 1) ^
         ((a >> 20) & 1) ^ ((a >> 21) & 1) ^ ((a >> 23) & 1) ^ ((a >> 26) & 1) ^ ((a >> 31) & 1) ^
         ((a >> 36) & 1) ^ ((a >> 38) & 1) ^ ((a >> 39) & 1) ^ ((a >> 41) & 1) ^ ((a >> 44) & 1) ^
         ((a >> 47) & 1) ^ ((a >> 49) & 1) ^ ((a >> 50) & 1) ^ ((a >> 54) & 1) ^ ((a >> 56) & 1) ^
         ((a >> 62) & 1));
    ecc_bits = (ecc_bit_7 << 7) | (ecc_bit_6 << 6) | (ecc_bit_5 << 5) | (ecc_bit_4 << 4) |
               (ecc_bit_3 << 3) | (ecc_bit_2 << 2) | (ecc_bit_1 << 1) | ecc_bit_0;
    return ecc_bits;
}

#endif /* defined(__ECC_PRESENT) && (__ECC_PRESENT == 1) */

/** @} */ /* End of Doxygen Group NMSIS_Core_ECC_Functions */
#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_ECC_H__ */
