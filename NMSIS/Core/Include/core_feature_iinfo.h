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
#ifndef __CORE_FEATURE_IINFO_H__
#define __CORE_FEATURE_IINFO_H__
/*!
 * @file     core_feature_iinfo.h
 * @brief    IREGION INFO feature API header file for Nuclei N/NX Core
 * @remarks Most registers in IREGION INFO are used to convey CPU information.
 * It is recommended to inspect or view this information using the `cpuinfo`
 * application. The macros and functions defined in this file provide only the
 * most commonly used operations and do not offer comprehensive support for
 * inspecting all available information.
 */
/*
 * IREGION INFO Configuration Macro:
 *
 * 1. __IINFO_BASE:  Base address of the IREGION INFO
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "core_feature_base.h"

/**
 * IREGION may not exist in legacy Nuclei CPU, so IREGION INFO related
 * APIs are only available when __IINFO_BASEADDR is defined
 */
#ifdef __IINFO_BASEADDR

/**
 * \defgroup NMSIS_Core_IINFO_Registers     Register Define and Type Definitions Of IINFO
 * \ingroup NMSIS_Core_Registers
 * \brief   Type definitions and defines for iinfo registers.
 *
 * @{
 */

/**
 * \brief  Union type to access MCMO_INFO register.
 */
typedef union {
    struct {
        __IM uint32_t cmo_cfg:1;                    /*!< bit: 0 CMO exist */
        __IM uint32_t cmo_pft:1;                    /*!< bit: 1 CMO has prefetch */
        __IM uint32_t cmo_size:4;                   /*!< bit: 2..5 cache block size */
        __IM uint32_t cbozero_size:4;               /*!< bit: 6..9 cache block size of cbo.zero */
        uint32_t :22;                               /*!< bit: 10..31 Reserved 0 */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_MCMO_INFO_Type;

/**
 * \brief  Union type to access ISA_SUPPORT0 register.
 */
typedef union {
    struct {
        __IM uint32_t exist:1;                      /*!< bit: 0 whether this register is valid */
        __IM uint32_t vector:1;                     /*!< bit: 1 support Vector extension */
        __IM uint32_t vector_b:1;                   /*!< bit: 2 support Vector-B extension */
        __IM uint32_t vector_k:1;                   /*!< bit: 3 support Vector-K extension */
        __IM uint32_t smepmp:1;                     /*!< bit: 4 support Smepmp extension */
        __IM uint32_t sscofpmf:1;                   /*!< bit: 5 support Sscofpmf extension */
        __IM uint32_t zfh:1;                        /*!< bit: 6 support Zfh extension */
        __IM uint32_t zfhmin:1;                     /*!< bit: 7 support Zfhmin extension */
        __IM uint32_t zfa:1;                        /*!< bit: 8 support Zfa extension */
        __IM uint32_t svnapot:1;                    /*!< bit: 9 support Svnapot extension */
        __IM uint32_t svpbmt:1;                     /*!< bit: 10 support Svpbmt extension */
        __IM uint32_t svinval:1;                    /*!< bit: 11 support Svinval extension */
        __IM uint32_t bf16:1;                       /*!< bit: 12 support Bf16 extension */
        __IM uint32_t zve32x:1;                     /*!< bit: 13 support Zve32x extension */
        __IM uint32_t zve32f:1;                     /*!< bit: 14 support Zve32f extension */
        __IM uint32_t zve64x:1;                     /*!< bit: 15 support Zve64x extension */
        __IM uint32_t zve64f:1;                     /*!< bit: 16 support Zve64f extension */
        __IM uint32_t zve64d:1;                     /*!< bit: 17 support Zve64d extension */
        __IM uint32_t zimop:1;                      /*!< bit: 18 support Zimop extension */
        __IM uint32_t zcmop:1;                      /*!< bit: 19 support Zomop extension */
        __IM uint32_t zicond:1;                     /*!< bit: 20 support Zicond extension */
        __IM uint32_t zihintntl:1;                  /*!< bit: 21 support Zihintntl extension */
        __IM uint32_t zihintpause:1;                /*!< bit: 22 support Zihintpause extension */
        __IM uint32_t zvfh:1;                       /*!< bit: 23 support Zvfh extension */
        __IM uint32_t zvfhmin:1;                    /*!< bit: 24 support Zvfhmin extension */
        __IM uint32_t smrnmi:1;                     /*!< bit: 25 support Smrnmi extension */
        __IM uint32_t zihpm:1;                      /*!< bit: 26 support Zihpm extension */
        __IM uint32_t smcntrpmf:1;                  /*!< bit: 27 support Smcntrpmf extension */
        __IM uint32_t zicntr:1;                     /*!< bit: 28 support Zicntr extension */
        __IM uint32_t zawrs:1;                      /*!< bit: 29 support Zawrs extension */
        uint32_t :2;                                /*!< bit: 30..31 reserved */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_ISA_SUPPORT0_Type;

/**
 * \brief  Union type to access ISA_SUPPORT1 register.
 */
typedef union {
    struct {
        __IM uint32_t exist:1;                      /*!< bit: 0 whether this register is valid */
        __IM uint32_t ssqosid:1;                    /*!< bit: 1 support Ssqosid extension */
        __IM uint32_t zicflip:1;                    /*!< bit: 2 support zicflip extension */
        __IM uint32_t zicfiss:1;                    /*!< bit: 3 support zicfiss extension */
        __IM uint32_t smctr:1;                      /*!< bit: 4 support Smctr extension */
        __IM uint32_t zacas:1;                      /*!< bit: 5 support Zacas extension */
        __IM uint32_t zabha:1;                      /*!< bit: 6 support Zabha extension */
        __IM uint32_t smdbltrp:1;                   /*!< bit: 7 support Smdbltrp extension */
        __IM uint32_t ssdbltrp:1;                   /*!< bit: 8 support Ssdbltrp extension */
        __IM uint32_t smcdeleg:1;                   /*!< bit: 9 support Smcdeleg extension */
        __IM uint32_t smmpm:1;                      /*!< bit: 10 support Smmpm extension */
        __IM uint32_t smnpm:1;                      /*!< bit: 11 support Smnpm extension */
        __IM uint32_t ssnpm:1;                      /*!< bit: 12 support Smnpm extension */
        __IM uint32_t smstateen:1;                  /*!< bit: 13 support Smstateen extension */
        __IM uint32_t sstateen:1;                   /*!< bit: 14 support Sstateen extension */
        __IM uint32_t smcsrind:1;                   /*!< bit: 15 support Smcsrind extension */
        __IM uint32_t sscsrind:1;                   /*!< bit: 16 support Sscsrind extension */
        __IM uint32_t svadu:1;                      /*!< bit: 17 support Svadu extension */
        uint32_t :14;                               /*!< bit: 18..31 reserved */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_ISA_SUPPORT1_Type;

/**
 * \brief  Union type to access PERFORMANCE_CFG0 register.
 */
typedef union {
    struct {
        __IM uint32_t exist:1;                      /*!< bit: 0 whether this register is valid */
        __IM uint32_t fpu_cycle:5;                  /*!< bit: 1..5 fpu cycle count */
        __IM uint32_t high_div:1;                   /*!< bit: 6 high performance divider */
        __IM uint32_t dcache_2stage:1;              /*!< bit: 7 dcache 2 stage */
        __IM uint32_t delay_branch_flush:1;         /*!< bit: 8 delay branch flash */
        __IM uint32_t bus_type:3;                   /*!< bit: 9..11 memory bus protocol type */
        __IM uint32_t dual_issue:1;                 /*!< bit: 12 dual issue */
        __IM uint32_t cross_4k:1;                   /*!< bit: 13 cross 4k */
        __IM uint32_t dlm_2stage:1;                 /*!< bit: 14 DLM 2 stage */
        __IM uint32_t lsu_cut_fwd:1;                /*!< bit: 15 LSU cut forwarding */
        __IM uint32_t dsp_cycle:4;                  /*!< bit: 16..19 DSP cycle */
        __IM uint32_t ifu_cut_timing:1;             /*!< bit: 20 IFU cut timing */
        __IM uint32_t mem_cut_timing:1;             /*!< bit: 21 MEM cut timing */
        __IM uint32_t dcache_prefetch:1;            /*!< bit: 22 dcache prefetch */
        __IM uint32_t dcache_lbuf_num:5;            /*!< bit: 23..27 dcache line buffer number */
        __IM uint32_t mul_cyc:3;                    /*!< bit: 28..30 multiplier cycle */
        uint32_t :1;                                /*!< bit: 31 reserved */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_PERFORMANCE_CFG0_Type;

/**
 * \brief  Union type to access PERFORMANCE_CFG1 register.
 */
typedef union {
    struct {
        __IM uint32_t exist:1;                      /*!< bit: 0 whether this register is valid */
        __IM uint32_t vfpu_cyc:5;                   /*!< bit: 1..5 Vector FPU cycle */
        __IM uint32_t bht_entry_width:5;            /*!< bit: 6..10 BHT entry width */
        __IM uint32_t high_performance:1;           /*!< bit: 11 high performance */
        __IM uint32_t agu_quick_forward:1;          /*!< bit: 12 agu quick forward */
        __IM uint32_t cau_fwd:1;                    /*!< bit: 13 crypto instruction forward */
        __IM uint32_t hpm_ver:2;                    /*!< bit: 14..15 HPM version */
        uint32_t :16;                               /*!< bit: 16..31 reserved */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_PERFORMANCE_CFG1_Type;

/**
 * \brief  Union type to access PFL1DCTRL1 register.
 */
typedef union {
    struct {
        __IOM uint32_t l1d_ena:1;                   /*!< bit: 0 L1 dcache prefetch enable */
        __IOM uint32_t cc_ena:1;                    /*!< bit: 1 cluster cache prefetch enable */
        __IOM uint32_t scalar_ena:1;                /*!< bit: 2 scalar pipeline prefetch enable */
        __IOM uint32_t vector_ena:1;                /*!< bit: 3 vector pipeline prefetch enable */
        __IOM uint32_t write_pref_ena:1;            /*!< bit: 4 store prefetch enable */
        __IOM uint32_t cross_page_pref_ena:1;       /*!< bit: 5 prefetch cross page enable */
        __IOM uint32_t mmu_ena:1;                   /*!< bit: 6 mmu prefetch enable */
        __IOM uint32_t pl2_ena:1;                   /*!< bit: 7 private L2 prefetch enable */
        __IOM uint32_t pref_conflict_stop_th:4;     /*!< bit: 8..11 prefetch inc counter stop threshold */
        __IOM uint32_t pref_conflict_decr_sel:3;    /*!< bit: 12..14 prefetch num to dec counter */
        uint32_t :17;                               /*!< bit: 15..31 reserved */
    } b;                                            /*!< Structure used for bit access */
    struct {
        /**
         * \brief 0..7 prefetch enable level
         * \details Writing to this bit field may use the `sb` instruction (write only one byte),
         * which is not allowed for IINFO registers. So this bit field is read-only here,
         * but actually it can be written. To write this bit field, you should write the
         * whole 32-bit register
         * \sa
         *   - \ref IINFO_SetPrefetchLevel
         *
         * The recommended values are:
         *   - 0x3F: Enable all prefetch except MMU and private L2 cache prefetch.
         *   - 0xBF: Enable all prefetch except MMU prefetch.
         * Enabling **cross page prefetch** in bare-metal environment is always recommended
         * for better performance.
         */
        __IM uint32_t level:8;
        __IM uint32_t pref_conflict_stop_th:4;      /*!< bit: 8..11 prefetch inc counter stop threshold */
        __IM uint32_t pref_conflict_decr_sel:3;     /*!< bit: 12..14 prefetch num to dec counter */
        uint32_t :17;                               /*!< bit: 15..31 reserved */
    } lv;                                           /*!< Structure used for enable level field setup */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_PFL1DCTRL1_Type;

#define IINFO_PFL1DCTRL1_LEVEL_Pos              (1UL << 0)                              /* IINFO PFL1DCTRL1 level position */
#define IINFO_PFL1DCTRL1_LEVEL_Msk              (0xFFUL << IINFO_PFL1DCTRL1_LEVEL_Pos)  /* IINFO PFL1DCTRL1 level mask */

#define IINFO_PFL1DCTRL1_DISABLE                (0UL)        /*!< Disable all prefetch */
#define IINFO_PFL1DCTRL1_L1D_ENA                (1UL << 0)   /*!< L1 dcache prefetch enable mask */
#define IINFO_PFL1DCTRL1_CC_ENA                 (1UL << 1)   /*!< Cluster cache prefetch enable mask */
#define IINFO_PFL1DCTRL1_SCALAR_ENA             (1UL << 2)   /*!< Scalar pipeline prefetch enable mask */
#define IINFO_PFL1DCTRL1_VECTOR_ENA             (1UL << 3)   /*!< Vector pipeline prefetch enable mask */
#define IINFO_PFL1DCTRL1_WRITE_PREF_ENA         (1UL << 4)   /*!< Store prefetch enable mask */
#define IINFO_PFL1DCTRL1_CROSS_PAGE_PREF_ENA    (1UL << 5)   /*!< Cross page prefetch enable mask */
#define IINFO_PFL1DCTRL1_MMU_PREF_ENA           (1UL << 6)   /*!< MMU prefetch enable mask */
#define IINFO_PFL1DCTRL1_PL2_ENA                (1UL << 7)   /*!< Private L2 prefetch enable mask */

/**
 * \brief  Union type to access PFL1DCTRL2 register.
 */
typedef union {
    struct {
        __IOM uint32_t degree_incr_th:6;            /*!< bit: 0..5 prefetch degree threshold of incr */
        __IOM uint32_t degree_decr_th:6;            /*!< bit: 6..11 prefetch degree threshold of decr */
        __IOM uint32_t next_line_ena_th:4;          /*!< bit: 12..15 next line enable threshold */
        __IOM uint32_t write_noalloc_l1_th:2;       /*!< bit: 16..17 see ISA doc */
        __IOM uint32_t write_noalloc_l2_th:2;       /*!< bit: 18..19 see ISA doc */
        uint32_t :12;                               /*!< bit: 20..31 reserved */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_PFL1DCTRL2_Type;

/**
 * \brief  Union type to access MERGEL1DCTRL register.
 */
typedef union {
    struct {
        __IOM uint32_t ws_tmout_max:12;             /*!< bit: 0..11 write streaming tmout cnt max value */
        uint32_t :4;                                /*!< bit: 12..15 reserved */
        __IOM uint32_t nc_tmout_max:8;              /*!< bit: 16..23 non-cacheable tmout cnt max value */
        __IOM uint32_t dev_store_early_ret: 1;      /*!< bit: 24 whether device region store is blocking */
        uint32_t :7;                                /*!< bit: 25..31 reserved */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_MERGEL1DCTRL_Type;

/**
 * \brief  Union type to access SAFETY_CTRL register.
 */
typedef union {
    struct {
        __IOM uint32_t reg_prot_chck_en:2;          /*!< bit: 0..1 register protect check enable */
        uint32_t :30;                               /*!< bit: 2..31 reserved */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_SAFETY_CTRL_Type;

/**
 * \brief  Union type to access ACCESS_CTRL register.
 */
typedef union {
    struct {
        uint32_t :1;                                /*!< bit: 0 reversed */
        __IOM uint32_t pf_access: 1;                /*!< bit: 1 s-mode register access, 0 is enable */
        __IOM uint32_t cache_csr_access: 1;         /*!< bit: 2 s-mode cache csr access */
        __IOM uint32_t pma_csr_access: 1;           /*!< bit: 3 s-mode pma csr access */
        uint32_t :28;                               /*!< bit: 4..31 reserved */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_ACCESS_CTRL_Type;

/**
 * \brief  Union type to access PFL1DCTRL3 register.
 */
typedef union {
    struct {
        __IOM uint32_t max_stream_l1_degree:4;      /*!< bit: 0..3 mat stream prefetch L1 degree */
        uint32_t :1;                                /*!< bit: 4 reserved */
        __IOM uint32_t max_stream_l2_degree:7;      /*!< bit: 5..11 next line enable threshold */
        uint32_t :4;                                /*!< bit: 12..15 reserved */
        __IOM uint32_t max_stride_cplx_l1_degree:4; /*!< bit: 16..19 the max stride/cplx pref L1 degree */
        uint32_t :1;                                /*!< bit: 20 reserved */
        __IOM uint32_t max_stride_cplx_l2_degree:7; /*!< bit: 21..27 the max stride/cplx pref L2 degree */
        uint32_t :4;                                /*!< bit: 28..31 reserved */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_PFL1DCTRL3_Type;

/**
 * \brief  Union type to access PFL1DCTRL4 register.
 */
typedef union {
    struct {
        __IOM uint32_t pf_enable:1;                 /*!< bit: 0 prefetch overall enable */
        __IOM uint32_t cc_short_enable:1;           /*!< bit: 1 cluster cache short enable */
        uint32_t :30;                               /*!< bit: 2..31 reserved */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_PFL1DCTRL4_Type;

/**
 * \brief  Union type to access PFL1INFO register.
 */
typedef union {
    struct {
        __IM uint32_t pf_cfg:8;                     /*!< bit: 0..7 prefetch configuration */
        __IM uint32_t l2_pf_lbuf_num:8;             /*!< bit: 8..15 L2 prefetch req address buf number */
        __IM uint32_t l2_pf_dbuf_num:8;             /*!< bit: 16..23 L2 prefetch data buf number */
        __IM uint32_t pf_ver:8;                     /*!< bit: 24..31 prefetch version */
    } b;                                            /*!< Structure used for bit access */
    uint32_t d;                                     /*!< Type      used for register data access */
} IINFO_PFL1INFO_Type;

/* IREGION INFO Memory-Mapped Register Type*/
typedef struct {
    __IM uint32_t mpasize;                              /*!< offset 0x0000 */
    __IM IINFO_MCMO_INFO_Type cmo_info;                 /*!< offset 0x0004 */
    __IM uint32_t sec_base_addr_lo;                     /*!< offset 0x0008 */
    __IM uint32_t sec_base_addr_hi;                     /*!< offset 0x000C */
    __IM uint32_t sec_cfg_info;                         /*!< offset 0x0010 */
    __IM uint32_t reserved0[4];                         /*!< offset 0x0014 */
    __IM uint32_t mvlm_cfg_lo;                          /*!< offset 0x0024 */
    __IM uint32_t mvlm_cfg_hi;                          /*!< offset 0x0028 */
    __IM uint32_t flash_base_addr_lo;                   /*!< offset 0x002C */
    __IM uint32_t flash_base_addr_hi;                   /*!< offset 0x0030 */
    __IM uint32_t reserved1[7];                         /*!< offset 0x0034 */
    __IM uint32_t vpu_cfg_info;                         /*!< offset 0x0050 */
    __IOM uint32_t mem_region0_cfg_lo;                  /*!< offset 0x0054 */
    __IM uint32_t mem_region0_cfg_hi;                   /*!< offset 0x0058 */
    __IOM uint32_t mem_region1_cfg_lo;                  /*!< offset 0x005c */
    __IM uint32_t mem_region1_cfg_hi;                   /*!< offset 0x0060 */
    uint32_t reserved2[3];                              /*!< offset 0x0064 */
    __IM IINFO_ISA_SUPPORT0_Type isa_support0;          /*!< offset 0x0070 */
    __IM IINFO_ISA_SUPPORT1_Type isa_support1;          /*!< offset 0x0074 */
    uint32_t reserved3[2];                              /*!< offset 0x0078 */
    __IOM uint32_t mcppi_cfg_lo;                        /*!< offset 0x0080 */
    __IM uint32_t mcppi_cfg_hi;                         /*!< offset 0x0084 */
    uint32_t reserved4[2];                              /*!< offset 0x0088 */
    __IM IINFO_PERFORMANCE_CFG0_Type performance_cfg0;  /*!< offset 0x0090 */
    __IM IINFO_PERFORMANCE_CFG1_Type performance_cfg1;  /*!< offset 0x0094 */
    uint32_t reserved5[26];                             /*!< offset 0x0098 */
    __IOM IINFO_PFL1DCTRL1_Type pfl1dctrl1;             /*!< offset 0x0100 */
    __IOM IINFO_PFL1DCTRL2_Type pfl1dctrl2;             /*!< offset 0x0104 */
    __IOM IINFO_MERGEL1DCTRL_Type mergel1dctrl;         /*!< offset 0x0108 */
    uint32_t reserved6;                                 /*!< offset 0x010C */
    __IOM IINFO_SAFETY_CTRL_Type safety_ctrl;           /*!< offset 0x0110 */
    __IOM IINFO_ACCESS_CTRL_Type access_ctrl;           /*!< offset 0x0114 */
    uint32_t reserved7[2];                              /*!< offset 0x0118 */
    __IOM IINFO_PFL1DCTRL3_Type pfl1dctrl3;             /*!< offset 0x0120 */
    __IOM IINFO_PFL1DCTRL4_Type pfl1dctrl4;             /*!< offset 0x0124 */
    __IM IINFO_PFL1INFO_Type pfl1info;                  /*!< offset 0x0128 */
    uint32_t reserved8[27];                             /*!< offset 0x012C */
    __IM uint32_t crc_rf0;                              /*!< offset 0x0198 */
    __IM uint32_t crc_rf1;                              /*!< offset 0x019C */
    __IM uint32_t crc_fp0;                              /*!< offset 0x01A0 */
} IINFO_Type;

/* IREGION INFO Memory mapping of Device */
#define IINFO_BASE          __IINFO_BASEADDR             /*!< IINFO Base Address */
#define IINFO               ((IINFO_Type *)IINFO_BASE)   /*!< IINFO configuration struct */

/**
 * \brief  Enum type to distinguish HPM version.
 */
typedef enum {
    IINFO_HPM_VER_UNKNOWN = 0,                  /*!< HPM version unknown */
    IINFO_HPM_VER_1,                            /*!< HPM version 1 */
    IINFO_HPM_VER_2,                            /*!< HPM version 2 */
    IINFO_HPM_VER_MAX                           /*!< Max number of HPM version */
} IINFO_HPM_VER_Type;

/** @} */ /* end of group NMSIS_Core_IINFO_Registers */

/**
 * \defgroup NMSIS_Core_IINFO_Functions IINFO Functions
 * \ingroup  NMSIS_Core
 * \brief IINFO related functions
 *
 *   @{
 */

/**
 * \brief   Check if CMO is supported
 * \details Checks whether the CMO feature is supported by reading the cmo_cfg bit
 * from the CMO_INFO register.
 * \return  1 if CMO is supported, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t IINFO_IsCMOSupported(void)
{
    return IINFO->cmo_info.b.cmo_cfg;
}

/**
 * \brief   Check if CMO prefetch is supported
 * \details Checks whether the CMO prefetch feature is supported
 *          by reading the cmo_pft bit from the CMO_INFO register.
 * \return  1 if CMO prefetch is supported, 0 otherwise
 */
__STATIC_FORCEINLINE int32_t IINFO_IsCMOPrefetchSupported(void)
{
    return IINFO->cmo_info.b.cmo_cfg && IINFO->cmo_info.b.cmo_pft;
}

/**
 * \brief   Get Hardware Performance Monitor (HPM) version
 * \details Gets the version of the Hardware Performance Monitor (HPM) feature
 *          by reading the hpm_ver bits from the PERFORMANCE_CFG1 register.
 *          If the PERFORMANCE_CFG1 register does not exist, returns IINFO_HPM_VER_UNKNOWN.
 * \return  HPM version as IINFO_HPM_VER_Type enum value
 */
__STATIC_FORCEINLINE IINFO_HPM_VER_Type IINFO_GetHPMVersion(void)
{
    if (!IINFO->performance_cfg1.b.exist) {
        return IINFO_HPM_VER_UNKNOWN;
    }
    return (IINFO_HPM_VER_Type)IINFO->performance_cfg1.b.hpm_ver;
}

/**
 * \brief   Set prefetch enable level bits
 * \details This function writes the specified bitmask to the prefetch enable field
 *          (bits 0–7) of the PFL1DCTRL1 register. Each bit corresponds to a specific
 *          prefetch, and enabling a bit activates it.
 *
 *          To construct the \p val argument, use the predefined bitmask macros
 *          (e.g., \c IINFO_PFL1DCTRL1_LEVEL_L1D_ENA). Multiple prefetch can be
 *          enabled simultaneously by bitwise-ORing their respective masks.
 *
 * \param[in] val  Bitmask value for prefetch enable bits (bits 0–7 of PFL1DCTRL1).
 *                 Only the lower 8 bits are used; higher bits are ignored.
 * \sa
 *   - \ref IINFO_GetPrefetchLevel
 *   - \ref IINFO_PFL1DCTRL1_DISABLE
 *   - \ref IINFO_PFL1DCTRL1_L1D_ENA
 *   - \ref IINFO_PFL1DCTRL1_CC_ENA
 *   - \ref IINFO_PFL1DCTRL1_SCALAR_ENA
 *   - \ref IINFO_PFL1DCTRL1_VECTOR_ENA
 *   - \ref IINFO_PFL1DCTRL1_WRITE_PREF_ENA
 *   - \ref IINFO_PFL1DCTRL1_CROSS_PAGE_PREF_ENA
 *   - \ref IINFO_PFL1DCTRL1_MMU_PREF_ENA
 *   - \ref IINFO_PFL1DCTRL1_PL2_ENA
 *
 * \example
 * \code
 * // Enable all prefetch except MMU and private L2 (PL2)
 * IINFO_SetPrefetchLevel(IINFO_PFL1DCTRL1_L1D_ENA |
 *                        IINFO_PFL1DCTRL1_CC_ENA |
 *                        IINFO_PFL1DCTRL1_SCALAR_ENA |
 *                        IINFO_PFL1DCTRL1_VECTOR_ENA |
 *                        IINFO_PFL1DCTRL1_WRITE_PREF_ENA |
 *                        IINFO_PFL1DCTRL1_CROSS_PAGE_PREF_ENA);
 * \endcode
 */
__STATIC_FORCEINLINE void IINFO_SetPrefetchLevel(uint32_t val)
{
    IINFO->pfl1dctrl1.d = (IINFO->pfl1dctrl1.d & ~IINFO_PFL1DCTRL1_LEVEL_Msk) |
                          _VAL2FLD(IINFO_PFL1DCTRL1_LEVEL, val);
}

/**
 * \brief   Get prefetch enable level bits
 * \details Gets the prefetch enable level bits from the PFL1DCTRL1 register using the enable field.
 * \return  Current value of prefetch enable bits (bits 0-7)
 * \sa
 *   - \ref IINFO_SetPrefetchLevel
 */
__STATIC_FORCEINLINE uint32_t IINFO_GetPrefetchLevel(void)
{
    return IINFO->pfl1dctrl1.lv.level;
}

/**
 * \brief   Set PFL1DCTRL2 register value
 * \details Sets the value of the PFL1DCTRL2 register.
 * \param   val  Value to write to the PFL1DCTRL2 register
 */
__STATIC_FORCEINLINE void IINFO_SetPFL1DCTRL2(uint32_t val)
{
    IINFO->pfl1dctrl2.d = val;
}

/**
 * \brief   Get PFL1DCTRL2 register value
 * \details Gets the current value of the PFL1DCTRL2 register.
 * \return  Current value of the PFL1DCTRL2 register
 */
__STATIC_FORCEINLINE uint32_t IINFO_GetPFL1DCTRL2(void)
{
    return IINFO->pfl1dctrl2.d;
}

/**
 * \brief   Set PFL1DCTRL3 register value
 * \details Sets the value of the PFL1DCTRL3 register.
 * \param   val  Value to write to the PFL1DCTRL3 register
 */
__STATIC_FORCEINLINE void IINFO_SetPFL1DCTRL3(uint32_t val)
{
    IINFO->pfl1dctrl3.d = val;
}

/**
 * \brief   Get PFL1DCTRL3 register value
 * \details Gets the current value of the PFL1DCTRL3 register.
 * \return  Current value of the PFL1DCTRL3 register
 */
__STATIC_FORCEINLINE uint32_t IINFO_GetPFL1DCTRL3(void)
{
    return IINFO->pfl1dctrl3.d;
}

/**
 * \brief   Enable prefetch overall
 * \details Enables the prefetch overall by setting the pf_enable bit in the PFL1DCTRL4 register.
 */
__STATIC_FORCEINLINE void IINFO_EnablePrefetchOverall(void)
{
    IINFO->pfl1dctrl4.b.pf_enable = 1;
}

/**
 * \brief   Disable prefetch overall
 * \details Disables the prefetch overall by clearing the pf_enable bit in the PFL1DCTRL4 register.
 */
__STATIC_FORCEINLINE void IINFO_DisablePrefetchOverall(void)
{
    IINFO->pfl1dctrl4.b.pf_enable = 0;
}

/** @} */ /* End of Doxygen Group NMSIS_Core_IINFO_Functions */

#endif /* #ifdef __IINFO_BASEADDR */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_IINFO_H__ */

