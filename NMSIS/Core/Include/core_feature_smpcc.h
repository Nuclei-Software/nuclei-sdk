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
#ifndef __CORE_FEATURE_SMPCC_H__
#define __CORE_FEATURE_SMPCC_H__
/*!
 * @file     core_feature_smpcc.h
 * @brief    SMP & Cluster Cache feature API header file for Nuclei N/NX Core
 */
/*
 * SMP & Cluster Cache Feature Configuration Macro:
 *
 * 1. __SMPCC_PRESENT:  Define whether SMP & Cluster Cache feature is present or not
 *   * 0: Not present
 *   * 1: Present
 * 2. __CCM_PRESENT:  Define whether Nuclei Cache Control and Maintainence(CCM) Unit is present or not.
 *   * 0: Not present
 *   * 1: Present
 * 3. __SMPCC_BASEADDR:  Base address of the SMP & Cluster Cache unit.
 *
 */
#ifdef __cplusplus
 extern "C" {
#endif

#include "core_feature_base.h"

#if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1)
/**
 * \defgroup NMSIS_Core_SMPCC_Registers     Register Define and Type Definitions Of SMPCC
 * \ingroup NMSIS_Core_Registers
 * \brief   Type definitions and defines for smpcc registers.
 *
 * @{
 */

/**
 * \brief  Union type to access SMP_VER information register.
 */
typedef union
{
    struct {
        __IM uint32_t mic_ver:8;                /*!< bit:     0..7 micro version number */
        __IM uint32_t min_ver:8;                /*!< bit:     8..15 minor version number */
        __IM uint32_t maj_ver:8;                /*!< bit:     16..23 major version number */
        __IM uint32_t _reserved:8;              /*!< bit:     24..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    __IM uint32_t w;                            /*!< Type      used for word access */
} SMP_VER_Type;

/**
 * \brief  Union type to access SMP_CFG information register.
 */
typedef union
{
    struct {
        __IM uint32_t cc_present:1;             /*!< bit:     0 cluster cache present or not */
        __IM uint32_t smp_core_num:6;           /*!< bit:     1..6 core number in cluster */
        __IM uint32_t iocp_num:6;               /*!< bit:     7..12 IO coherency port number in the cluster */
        __IM uint32_t pmon_num:6;               /*!< bit:     13..18 performance monitor number in the cluster */
        __IM uint32_t _reserved:13;             /*!< bit:     19..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    __IM uint32_t w;                            /*!< Type      used for word access */
} SMP_CFG_Type;

/**
 * \brief  Union type to access CC_CFG information register.
 */
typedef union
{
    struct {
        __IM uint32_t cc_set:4;                 /*!< bit:     0..3 cluster cache set number = 2^(cc_set) */
        __IM uint32_t cc_way:4;                 /*!< bit:     4..7 cluster cache way number = cc_way + 1 */
        __IM uint32_t cc_lsize:3;               /*!< bit:     8..10 cluster cache line size = 2^(cc_lsize + 2) */
        __IM uint32_t cc_ecc:1;                 /*!< bit:     11 cluster cache ECC supports ECC or not */
        __IM uint32_t cc_tcycle:3;              /*!< bit:     12..14 L2 tag ram access cycle = cc_tcycle + 1 */
        __IM uint32_t cc_dcycle:3;              /*!< bit:     15..17 L2 Data sram access cycle = cc_dcycle + 1 */
        __IM uint32_t _reserved:14;             /*!< bit:     18..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CC_CFG_Type;

/**
 * \brief  Union type to access SMP_ENB configure register.
 */
typedef union
{
    struct {
        __IOM uint32_t smp_enable:16;           /*!< bit:     0..15 SMP enable bits for clients in cluster */
        __IM uint32_t _reserved:16;             /*!< bit:     16..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} SMP_ENB_Type;

/**
 * \brief  Union type to access CC_CTRL configure register.
 */
typedef union
{
    struct {
        __IOM uint32_t cc_en:1;                 /*!< bit:     0 cluster cache enable bit */
        __IOM uint32_t cc_ecc_en:1;             /*!< bit:     1 cluster cache ECC enable bit */
        __IOM uint32_t ecc_excp_en:1;           /*!< bit:     2 cluster cache ECC exception enable bit */
        __IOM uint32_t lock_ecc_cfg:1;          /*!< bit:     3 lock the cc ecc configuration bit */
        __IOM uint32_t lock_ecc_err_inj:1;      /*!< bit:     4 lock cc ecc error injection register */
        __IOM uint32_t recv_err_irq_en:1;       /*!< bit:     5 enable the interrupt when recoverable error count exceeds the threshold */
        __IOM uint32_t fatal_err_irq_en:1;      /*!< bit:     6 enable the interrupt when fatal error count exceeds the threshold */
        __IOM uint32_t bus_err_pend:1;          /*!< bit:     7 indicate if there is bus error pending */
        __IOM uint32_t bus_err_irq_en:1;        /*!< bit:     8 enable the buss error interrupt of cc maintain operation */
        __IOM uint32_t sup_cmd_en:1;            /*!< bit:     9 enable S mode can operate register CC_sCMD and SMP_PMON_SEL */
        __IOM uint32_t use_cmd_en:1;            /*!< bit:     10 enable U mode can operate register CC_uCMD and SMP_PMON_SEL */
        __IOM uint32_t ecc_chk_en:1;            /*!< bit:     11 cc ecc check enable bit */
        __IOM uint32_t clm_ecc_en:1;            /*!< bit:     12 clm ecc enable bit */
        __IOM uint32_t clm_excp_en:1;           /*!< bit:     13 clm ecc exception enable bit */
        __IOM uint32_t clm_ecc_chk_en:1;        /*!< bit:     14 clm ecc check enable bit */
        __IOM uint32_t pf_sh_cl_en:1;           /*!< bit:     15 enable L1 prefetch to snoop and share cacheline from other cores */
        __IOM uint32_t pf_l2_early_en:1;        /*!< bit:     16 enable L2 prefetch to initialize external bus read access while lookup the cluster cache */
        __IOM uint32_t pf_biu_outs_en:1;        /*!< bit:     17 enable the limit of outstanding L2 prefetch to the number of L2 prefetch line-buffer */
        __IOM uint32_t i_snoop_d_en:1;          /*!< bit:     18 snoop to dcache for icache refill reads enable */
        __IOM uint32_t iocc_err:1;              /*!< bit:     19 iocc has error */
        __IOM uint32_t early_wr_err:1;          /*!< bit:     20 early write response has error */
        __IOM uint32_t pf_no_wb:1;              /*!< bit:     21 enable L2 prefetch to abort and avoid dirty cacheline write back when filling the cluster cache */
        __IM uint32_t _reserved:10;             /*!< bit:     22..31 reserved */

    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CC_CTRL_Type;

#define SMPCC_CTRL_CC_EN_Pos                 0U                                          /*!< SMPCC CC_CTRL CC_EN Position */
#define SMPCC_CTRL_CC_EN_Msk                 (0x1UL << SMPCC_CTRL_CC_EN_Pos)             /*!< SMPCC CC_CTRL CC_EN Mask */
#define SMPCC_CTRL_CC_EN_ENABLE              1U                                          /*!< SMPCC CC_CTRL CC_EN Enable */
#define SMPCC_CTRL_CC_EN_DISABLE             0U                                          /*!< SMPCC CC_CTRL CC_EN Disable */

#define SMPCC_CTRL_CC_ECC_EN_Pos             1U                                          /*!< SMPCC CC_CTRL CC_ECC_EN Position */
#define SMPCC_CTRL_CC_ECC_EN_Msk             (0x1UL << SMPCC_CTRL_CC_ECC_EN_Pos)         /*!< SMPCC CC_CTRL CC_ECC_EN Mask */
#define SMPCC_CTRL_CC_ECC_EN_ENABLE          1U                                          /*!< SMPCC CC_CTRL CC_ECC_EN Enable */
#define SMPCC_CTRL_CC_ECC_EN_DISABLE         0U                                          /*!< SMPCC CC_CTRL CC_ECC_EN Disable */

#define SMPCC_CTRL_CC_ECC_EXCP_EN_Pos        2U                                          /*!< SMPCC CC_CTRL ECC_EXCP_EN Position */
#define SMPCC_CTRL_CC_ECC_EXCP_EN_Msk        (0x1UL << SMPCC_CTRL_CC_ECC_EXCP_EN_Pos)    /*!< SMPCC CC_CTRL ECC_EXCP_EN Mask */
#define SMPCC_CTRL_CC_ECC_EXCP_EN_ENABLE     1U                                          /*!< SMPCC CC_CTRL ECC_EXCP_EN Enable */
#define SMPCC_CTRL_CC_ECC_EXCP_EN_DISABLE    0U                                          /*!< SMPCC CC_CTRL ECC_EXCP_EN Disable */

#define SMPCC_CTRL_LOCK_ECC_CFG_Pos          3U                                          /*!< SMPCC CC_CTRL LOCK_ECC_CFG Position */
#define SMPCC_CTRL_LOCK_ECC_CFG_Msk          (0x1UL << SMPCC_CTRL_LOCK_ECC_CFG_Pos)      /*!< SMPCC CC_CTRL LOCK_ECC_CFG Mask */
#define SMPCC_CTRL_LOCK_ECC_CFG_LOCK         1U                                          /*!< SMPCC CC_CTRL LOCK_ECC_CFG Lock */

#define SMPCC_CTRL_LOCK_ECC_ERR_INJ_Pos      4U                                          /*!< SMPCC CC_CTRL LOCK_ECC_ERR_INJ Position */
#define SMPCC_CTRL_LOCK_ECC_ERR_INJ_Msk      (0x1UL << SMPCC_CTRL_LOCK_ECC_ERR_INJ_Pos)  /*!< SMPCC CC_CTRL LOCK_ECC_ERR_INJ Mask */
#define SMPCC_CTRL_LOCK_ECC_ERR_INJ_LOCK     1U                                          /*!< SMPCC CC_CTRL LOCK_ECC_ERR_INJ Lock */

#define SMPCC_CTRL_RECV_ERR_IRQ_EN_Pos       5U                                          /*!< SMPCC CC_CTRL RECV_ERR_IRQ_EN Position */
#define SMPCC_CTRL_RECV_ERR_IRQ_EN_Msk       (0x1UL << SMPCC_CTRL_RECV_ERR_IRQ_EN_Pos)   /*!< SMPCC CC_CTRL RECV_ERR_IRQ_EN Mask */
#define SMPCC_CTRL_RECV_ERR_IRQ_EN_ENABLE    1U                                          /*!< SMPCC CC_CTRL RECV_ERR_IRQ_EN Enable */
#define SMPCC_CTRL_RECV_ERR_IRQ_EN_DISABLE   0U                                          /*!< SMPCC CC_CTRL RECV_ERR_IRQ_EN Disable */

#define SMPCC_CTRL_FATAL_ERR_IRQ_EN_Pos      6U                                          /*!< SMPCC CC_CTRL FATAL_ERR_IRQ_EN Position */
#define SMPCC_CTRL_FATAL_ERR_IRQ_EN_Msk      (0x1UL << SMPCC_CTRL_FATAL_ERR_IRQ_EN_Pos)  /*!< SMPCC CC_CTRL FATAL_ERR_IRQ_EN Mask */
#define SMPCC_CTRL_FATAL_ERR_IRQ_EN_ENABLE   1U                                          /*!< SMPCC CC_CTRL FATAL_ERR_IRQ_EN Enable */
#define SMPCC_CTRL_FATAL_ERR_IRQ_EN_DISABLE  0U                                          /*!< SMPCC CC_CTRL FATAL_ERR_IRQ_EN Disable */

#define SMPCC_CTRL_BUS_ERR_PEND_Pos          7U                                          /*!< SMPCC CC_CTRL BUS_ERR_PEND Position */
#define SMPCC_CTRL_BUS_ERR_PEND_Msk          (0x1UL << SMPCC_CTRL_BUS_ERR_PEND_Pos)      /*!< SMPCC CC_CTRL BUS_ERR_PEND Mask */

#define SMPCC_CTRL_BUS_ERR_IRQ_EN_Pos        8U                                          /*!< SMPCC CC_CTRL BUS_ERR_IRQ_EN Position */
#define SMPCC_CTRL_BUS_ERR_IRQ_EN_Msk        (0x1UL << SMPCC_CTRL_BUS_ERR_IRQ_EN_Pos)    /*!< SMPCC CC_CTRL BUS_ERR_IRQ_EN Mask */
#define SMPCC_CTRL_BUS_ERR_IRQ_EN_ENABLE     1U                                          /*!< SMPCC CC_CTRL BUS_ERR_IRQ_EN Enable */
#define SMPCC_CTRL_BUS_ERR_IRQ_EN_DISABLE    0U                                          /*!< SMPCC CC_CTRL BUS_ERR_IRQ_EN Disable */

#define SMPCC_CTRL_SUP_CMD_EN_Pos            9U                                          /*!< SMPCC CC_CTRL SUP_CMD_EN Position */
#define SMPCC_CTRL_SUP_CMD_EN_Msk            (0x1UL << SMPCC_CTRL_SUP_CMD_EN_Pos)        /*!< SMPCC CC_CTRL SUP_CMD_EN Mask */
#define SMPCC_CTRL_SUP_CMD_EN_ENABLE         1U                                          /*!< SMPCC CC_CTRL SUP_CMD_EN Enable */
#define SMPCC_CTRL_SUP_CMD_EN_DISABLE        0U                                          /*!< SMPCC CC_CTRL SUP_CMD_EN Disable */

#define SMPCC_CTRL_USE_CMD_EN_Pos            10U                                         /*!< SMPCC CC_CTRL USE_CMD_EN Position */
#define SMPCC_CTRL_USE_CMD_EN_Msk            (0x1UL << SMPCC_CTRL_USE_CMD_EN_Pos)        /*!< SMPCC CC_CTRL USE_CMD_EN Mask */
#define SMPCC_CTRL_USE_CMD_EN_ENABLE         1U                                          /*!< SMPCC CC_CTRL USE_CMD_EN Enable */
#define SMPCC_CTRL_USE_CMD_EN_DISABLE        0U                                          /*!< SMPCC CC_CTRL USE_CMD_EN Disable */

#define SMPCC_CTRL_ECC_CHK_EN_Pos            11U                                         /*!< SMPCC CC_CTRL ECC_CHK_EN Position */
#define SMPCC_CTRL_ECC_CHK_EN_Msk            (0x1UL << SMPCC_CTRL_ECC_CHK_EN_Pos)        /*!< SMPCC CC_CTRL ECC_CHK_EN Mask */
#define SMPCC_CTRL_ECC_CHK_EN_ENABLE         1U                                          /*!< SMPCC CC_CTRL ECC_CHK_EN Enable */
#define SMPCC_CTRL_ECC_CHK_EN_DISABLE        0U                                          /*!< SMPCC CC_CTRL ECC_CHK_EN Disable */

#define SMPCC_CTRL_CLM_ECC_EN_Pos            12U                                         /*!< SMPCC CC_CTRL CLM_ECC_EN Position */
#define SMPCC_CTRL_CLM_ECC_EN_Msk            (0x1UL << SMPCC_CTRL_CLM_ECC_EN_Pos)        /*!< SMPCC CC_CTRL CLM_ECC_EN Mask */
#define SMPCC_CTRL_CLM_ECC_EN_ENABLE         1U                                          /*!< SMPCC CC_CTRL CLM_ECC_EN Enable */
#define SMPCC_CTRL_CLM_ECC_EN_DISABLE        0U                                          /*!< SMPCC CC_CTRL CLM_ECC_EN Disable */

#define SMPCC_CTRL_CLM_EXCP_EN_Pos          13U                                         /*!< SMPCC CC_CTRL CLM_EXCP_EN Position */
#define SMPCC_CTRL_CLM_EXCP_EN_Msk          (0x1UL << SMPCC_CTRL_CLM_EXCP_EN_Pos)      /*!< SMPCC CC_CTRL CLM_EXCP_EN Mask */
#define SMPCC_CTRL_CLM_EXCP_EN_ENABLE       1U                                          /*!< SMPCC CC_CTRL CLM_EXCP_EN Enable */
#define SMPCC_CTRL_CLM_EXCP_EN_DISABLE      0U                                          /*!< SMPCC CC_CTRL CLM_EXCP_EN Disable */

#define SMPCC_CTRL_CLM_ECC_CHK_EN_Pos        14U                                         /*!< SMPCC CC_CTRL CLM_ECC_CHK_EN Position */
#define SMPCC_CTRL_CLM_ECC_CHK_EN_Msk        (0x1UL << SMPCC_CTRL_CLM_ECC_CHK_EN_Pos)    /*!< SMPCC CC_CTRL CLM_ECC_CHK_EN Mask */
#define SMPCC_CTRL_CLM_ECC_CHK_EN_ENABLE     1U                                          /*!< SMPCC CC_CTRL CLM_ECC_CHK_EN Enable */
#define SMPCC_CTRL_CLM_ECC_CHK_EN_DISABLE    0U                                          /*!< SMPCC CC_CTRL CLM_ECC_CHK_EN Disable */

#define SMPCC_CTRL_PF_SH_CL_EN_Pos           15U                                         /*!< SMPCC CC_CTRL PF_SH_CL_EN Position */
#define SMPCC_CTRL_PF_SH_CL_EN_Msk           (0x1UL << SMPCC_CTRL_PF_SH_CL_EN_Pos)       /*!< SMPCC CC_CTRL PF_SH_CL_EN Mask */
#define SMPCC_CTRL_PF_SH_CL_EN_ENABLE        1U                                          /*!< SMPCC CC_CTRL PF_SH_CL_EN Enable */
#define SMPCC_CTRL_PF_SH_CL_EN_DISABLE       0U                                          /*!< SMPCC CC_CTRL PF_SH_CL_EN Disable */

#define SMPCC_CTRL_PF_L2_EARLY_EN_Pos        16U                                         /*!< SMPCC CC_CTRL PF_L2_EARLY_EN Position */
#define SMPCC_CTRL_PF_L2_EARLY_EN_Msk        (0x1UL << SMPCC_CTRL_PF_L2_EARLY_EN_Pos)    /*!< SMPCC CC_CTRL PF_L2_EARLY_EN Mask */
#define SMPCC_CTRL_PF_L2_EARLY_EN_ENABLE     1U                                          /*!< SMPCC CC_CTRL PF_L2_EARLY_EN Enable */
#define SMPCC_CTRL_PF_L2_EARLY_EN_DISABLE    0U                                          /*!< SMPCC CC_CTRL PF_L2_EARLY_EN Disable */

#define SMPCC_CTRL_PF_BIU_OUTS_EN_Pos        17U                                         /*!< SMPCC CC_CTRL PF_BIU_OUTS_EN Position */
#define SMPCC_CTRL_PF_BIU_OUTS_EN_Msk        (0x1UL << SMPCC_CTRL_PF_BIU_OUTS_EN_Pos)    /*!< SMPCC CC_CTRL PF_BIU_OUTS_EN Mask */
#define SMPCC_CTRL_PF_BIU_OUTS_EN_ENABLE     1U                                          /*!< SMPCC CC_CTRL PF_BIU_OUTS_EN Enable */
#define SMPCC_CTRL_PF_BIU_OUTS_EN_DISABLE    0U                                          /*!< SMPCC CC_CTRL PF_BIU_OUTS_EN Disable */

#define SMPCC_CTRL_I_SNOOP_D_EN_Pos          18U                                         /*!< SMPCC CC_CTRL I_SNOOP_D_EN Position */
#define SMPCC_CTRL_I_SNOOP_D_EN_Msk          (0x1UL << SMPCC_CTRL_I_SNOOP_D_EN_Pos)      /*!< SMPCC CC_CTRL I_SNOOP_D_EN Mask */
#define SMPCC_CTRL_I_SNOOP_D_EN_ENABLE       1U                                          /*!< SMPCC CC_CTRL I_SNOOP_D_EN Enable */
#define SMPCC_CTRL_I_SNOOP_D_EN_DISABLE      0U                                          /*!< SMPCC CC_CTRL I_SNOOP_D_EN Disable */

#define SMPCC_CTRL_IOCC_ERR_Pos              19U                                         /*!< SMPCC CC_CTRL IOCC_ERR Position */
#define SMPCC_CTRL_IOCC_ERR_Msk              (0x1UL << SMPCC_CTRL_IOCC_ERR_Pos)          /*!< SMPCC CC_CTRL IOCC_ERR Mask */

#define SMPCC_CTRL_EARLY_WR_ERR_Pos          20U                                         /*!< SMPCC CC_CTRL EARLY_WR_ERR Position */
#define SMPCC_CTRL_EARLY_WR_ERR_Msk          (0x1UL << SMPCC_CTRL_EARLY_WR_ERR_Pos)      /*!< SMPCC CC_CTRL EARLY_WR_ERR Mask */

#define SMPCC_CTRL_PF_NO_WB_Pos              21U                                         /*!< SMPCC CC_CTRL PF_NO_WB Position */
#define SMPCC_CTRL_PF_NO_WB_Msk              (0x1UL << SMPCC_CTRL_PF_NO_WB_Pos)          /*!< SMPCC CC_CTRL PF_NO_WB Mask */
#define SMPCC_CTRL_PF_NO_WB_ENABLE           1U                                          /*!< SMPCC CC_CTRL PF_NO_WB Enable */
#define SMPCC_CTRL_PF_NO_WB_DISABLE          0U                                          /*!< SMPCC CC_CTRL PF_NO_WB Disable */

/**
 * \brief  Union type to access CC_CMD register. This type is suitable for all m/s/u mode registers.
 */
typedef union
{
    struct {
        __IOM uint32_t cmd:5;                   /*!< bit:     0..4 cluster cache maintain command code */
        __IM uint32_t _reserved:18;             /*!< bit:     5..22 reserved */
        __IOM uint32_t reisc:1;                 /*!< bit:     23 recoverable error interrupt status, write 1 to clean */
        __IOM uint32_t feisc:1;                 /*!< bit:     24 fatal error interrupt status, write 1 to clean */
        __IOM uint32_t besc:1;                  /*!< bit:     25 bus error status, write 1 to clean */
        __IM uint32_t result_code:5;            /*!< bit:     26..30 result code */
        __IM uint32_t complete:1;               /*!< bit:     31 completion status */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CC_CMD_Type;

/**
 * \brief  Union type to access CC_ERR_INJ register.
 */
typedef union
{
    struct {
        __IOM uint32_t inj_data:1;              /*!< bit:     0 ECC error injection to data ram */
        __IOM uint32_t inj_tag:1;               /*!< bit:     1 ECC error injection to tag ram */
        __IOM uint32_t inj_clm:1;               /*!< bit:     2 ECC error injection to clm ram */
        __IOM uint32_t inj_mode:1;              /*!< bit:     3 ECC error injection mode: 0-direct write mode, 1-xor write mode */
        __IM uint32_t _reserved0:20;            /*!< bit:     4..23 reserved */
        /** \brief 24..32 ECC code for injection
         * \details Write to this bit field may use `sb` instruction (write only one byte),
         * which is not allowed for SMPCC registers. So this bit field is read-only here,
         * but actually it can be written. To write this bit field, you should write the
         * whole 32-bit register */
        __IM uint32_t inj_ecc_code:8;
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CC_ERR_INJ_Type;

#define SMPCC_ERR_INJ_INJDATA_Pos             0U                                          /*!< SMPCC CC_ERR_INJ INJDATA Position */
#define SMPCC_ERR_INJ_INJDATA_Msk             (0x1UL << SMPCC_ERR_INJ_INJDATA_Pos)        /*!< SMPCC CC_ERR_INJ INJDATA Mask */
#define SMPCC_ERR_INJ_INJDATA_ENABLE          1U                                          /*!< SMPCC CC_ERR_INJ INJDATA Enable */
#define SMPCC_ERR_INJ_INJDATA_DISABLE         0U                                          /*!< SMPCC CC_ERR_INJ INJDATA Disable */

#define SMPCC_ERR_INJ_INJTAG_Pos              1U                                          /*!< SMPCC CC_ERR_INJ INJTAG Position */
#define SMPCC_ERR_INJ_INJTAG_Msk              (0x1UL << SMPCC_ERR_INJ_INJTAG_Pos)         /*!< SMPCC CC_ERR_INJ INJTAG Mask */
#define SMPCC_ERR_INJ_INJTAG_ENABLE           1U                                          /*!< SMPCC CC_ERR_INJ INJTAG Enable */
#define SMPCC_ERR_INJ_INJTAG_DISABLE          0U                                          /*!< SMPCC CC_ERR_INJ INJTAG Disable */

#define SMPCC_ERR_INJ_INJCLM_Pos              2U                                          /*!< SMPCC CC_ERR_INJ INJCLM Position */
#define SMPCC_ERR_INJ_INJCLM_Msk              (0x1UL << SMPCC_ERR_INJ_INJCLM_Pos)         /*!< SMPCC CC_ERR_INJ INJCLM Mask */
#define SMPCC_ERR_INJ_INJCLM_ENABLE           1U                                          /*!< SMPCC CC_ERR_INJ INJCLM Enable */
#define SMPCC_ERR_INJ_INJCLM_DISABLE          0U                                          /*!< SMPCC CC_ERR_INJ INJCLM Disable */

#define SMPCC_ERR_INJ_INJMODE_Pos             3U                                          /*!< SMPCC CC_ERR_INJ INJMODE Position */
#define SMPCC_ERR_INJ_INJMODE_Msk             (0x1UL << SMPCC_ERR_INJ_INJMODE_Pos)        /*!< SMPCC CC_ERR_INJ INJMODE Mask */
#define SMPCC_ERR_INJ_INJMODE_DIRECT          0U                                          /*!< SMPCC CC_ERR_INJ INJMODE Direct write mode */
#define SMPCC_ERR_INJ_INJMODE_XOR             1U                                          /*!< SMPCC CC_ERR_INJ INJMODE XOR write mode */

#define SMPCC_ERR_INJ_INJECCCODE_Pos          24U                                         /*!< SMPCC CC_ERR_INJ INJECCCODE Position */
#define SMPCC_ERR_INJ_INJECCCODE_Msk          (0xFFUL << SMPCC_ERR_INJ_INJECCCODE_Pos)    /*!< SMPCC CC_ERR_INJ INJECCCODE Mask */

/**
 * \brief  Union type to access CC_RECV_CNT register.
 */
typedef union
{
    struct {
        __IOM uint32_t cnt:16;                  /*!< bit:     0..15 count of the recoverable error */
        __IM uint32_t _reserved:16;             /*!< bit:     16..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CC_RECV_CNT_Type;

/**
 * \brief  Union type to access CC_FATAL_CNT register.
 */
typedef union
{
    struct {
        __IOM uint32_t cnt:16;                  /*!< bit:     0..15 count of the fatal error */
        __IM uint32_t _reserved:16;             /*!< bit:     16..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CC_FATAL_CNT_Type;

/**
 * \brief  Union type to access CC_RECV_THV register.
 */
typedef union
{
    struct {
        __IOM uint32_t cnt:16;                  /*!< bit:     0..15 count of the recoverable error threshold value */
        __IM uint32_t _reserved:16;             /*!< bit:     16..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CC_RECV_THV_Type;

/**
 * \brief  Union type to access CC_FATAL_THV register.
 */
typedef union
{
    struct {
        __IOM uint32_t cnt:16;                  /*!< bit:     0..15 count of the fatal error threshold value */
        __IM uint32_t _reserved:16;             /*!< bit:     16..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CC_FATAL_THV_Type;

/**
 * \brief  Type to access CC_BUS_ERR_ADDR register.
 */
typedef __IO uint64_t CC_BUS_ERR_ADDR_Type;

/**
 * \brief  Union type to access CLIENT_ERR_STATUS register.
 */
typedef union
{
    struct {
        __IOM uint32_t read_bus_err:1;          /*!< bit:     0 read bus error */
        __IOM uint32_t write_bus_err:1;         /*!< bit:     1 write bus error */
        __IOM uint32_t cc_scu_ecc_err:1;        /*!< bit:     2 cc scu ecc error */
        __IOM uint32_t iocp_bus_err:1;          /*!< bit:     3 iocp bus error */
        __IM uint32_t _reserved:28;             /*!< bit:     4..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CLIENT_ERR_STATUS_Type;

#define SMPCC_CLIERRSTS_READ_BUS_ERR_Pos      0U                                            /*!< SMPCC CLIENT ERROR STATUS READ_BUS_ERR Position */
#define SMPCC_CLIERRSTS_READ_BUS_ERR_Msk      (0x1UL << SMPCC_CLIERRSTS_READ_BUS_ERR_Pos)   /*!< SMPCC CLIENT ERROR STATUS READ_BUS_ERR Mask */

#define SMPCC_CLIERRSTS_WRITE_BUS_ERR_Pos     1U                                            /*!< SMPCC CLIENT ERROR STATUS WRITE_BUS_ERR Position */
#define SMPCC_CLIERRSTS_WRITE_BUS_ERR_Msk     (0x1UL << SMPCC_CLIERRSTS_WRITE_BUS_ERR_Pos)  /*!< SMPCC CLIENT ERROR STATUS WRITE_BUS_ERR Mask */

#define SMPCC_CLIERRSTS_CC_SCU_ECC_ERR_Pos    2U                                            /*!< SMPCC CLIENT ERROR STATUS CC_SCU_ECC_ERR Position */
#define SMPCC_CLIERRSTS_CC_SCU_ECC_ERR_Msk    (0x1UL << SMPCC_CLIERRSTS_CC_SCU_ECC_ERR_Pos) /*!< SMPCC CLIENT ERROR STATUS CC_SCU_ECC_ERR Mask */

#define SMPCC_CLIERRSTS_IOCP_BUS_ERR_Pos      3U                                            /*!< SMPCC CLIENT ERROR STATUS IOCP_BUS_ERR Position */
#define SMPCC_CLIERRSTS_IOCP_BUS_ERR_Msk      (0x1UL << SMPCC_CLIERRSTS_IOCP_BUS_ERR_Pos)   /*!< SMPCC CLIENT ERROR STATUS IOCP_BUS_ERR Mask */

/**
 * \brief  Union type to access SNOOP_PENDING register.
 */
typedef union
{
    struct {
        __IM uint32_t snoop_pending:16;         /*!< bit:     0..15 snoop pending bit for each client */
        __IM uint32_t _reserved:16;             /*!< bit:     16..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    __IM uint32_t w;                            /*!< Type      used for word access */
} SNOOP_PENDING_Type;

/**
 * \brief  Union type to access TRANS_PENDING register.
 */
typedef union
{
    struct {
        __IM uint32_t trans_pending:16;         /*!< bit:     0..15 transaction pending bit for each client */
        __IM uint32_t _reserved:15;             /*!< bit:     16..30 reserved */
        __IM uint32_t ext_trans:1;              /*!< bit:     31 external memory bus transaction pending */
    } b;                                        /*!< Structure used for bit  access */
    __IM uint32_t w;                            /*!< Type      used for word access */
} TRANS_PENDING_Type;

/**
 * \brief  Union type to access CLM_ADDR_BASE register.
 */
typedef union
{
    struct {
        __IOM uint32_t clm_base32;
        __IM uint32_t _reserved;
    } clm32;             /*!< Structure used access only low 32-bits */
    uint64_t clm_base64; /*!< Type      used access whole 64-bits */
} CLM_ADDR_BASE_Type;

/**
 * \brief  Union type to access CLM_WAY_EN register.
 */
typedef union
{
    struct {
        __IOM uint32_t ena:16;                  /*!< bit:     0..15 This way is used as CLM or not */
        __IM uint32_t _reserved:16;             /*!< bit:     16..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CLM_WAY_EN_Type;

/**
 * \brief  Union type to access CC_INVALID_ALL register.
 */
typedef union
{
    struct {
        __IOM uint32_t cs:1;                    /*!< bit:     0 write 1 to invalid all cluster cache, and hardware auto clean when operation is done */
        __IM uint32_t _reserved:31;             /*!< bit:     1..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CC_INVALID_ALL_Type;

/**
 * \brief  Union type to access STM_CTRL register.
 */
typedef union
{
    struct {
        __IOM uint32_t rd_stm_en:1;             /*!< bit:     0 read stream enable */
        __IOM uint32_t wr_stm_en:1;             /*!< bit:     1 write stream enable */
        __IOM uint32_t trans_alloc:1;           /*!< bit:     2 translate allocate attribute to non-alloc attribute enable */
        __IOM uint32_t rd_merge_en:1;           /*!< bit:     3 non-cacheable attribute read merge enable */
        __IOM uint32_t cross_en:1;              /*!< bit:     4 read stream cross 4k enable */
        __IM uint32_t _reserved:27;             /*!< bit:     5..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} STM_CTRL_Type;

#define SMPCC_STMCTRL_RD_STM_EN_Pos        0U                                           /*!< SMPCC READ Stream Enable Position */
#define SMPCC_STMCTRL_RD_STM_EN_Msk        (0x1UL << SMPCC_STMCTRL_RD_STM_EN_Pos)       /*!< SMPCC READ Stream Enable Mask */
#define SMPCC_STMCTRL_RD_STM_EN_ENABLE     1U                                           /*!< SMPCC READ Stream Enable Enable */
#define SMPCC_STMCTRL_RD_STM_EN_DISABLE    0U                                           /*!< SMPCC READ Stream Enable Disable */

#define SMPCC_STMCTRL_WR_STM_EN_Pos        1U                                           /*!< SMPCC WRITE Stream Enable Position */
#define SMPCC_STMCTRL_WR_STM_EN_Msk        (0x1UL << SMPCC_STMCTRL_WR_STM_EN_Pos)       /*!< SMPCC WRITE Stream Enable Mask */
#define SMPCC_STMCTRL_WR_STM_EN_ENABLE     1U                                           /*!< SMPCC WRITE Stream Enable Enable */
#define SMPCC_STMCTRL_WR_STM_EN_DISABLE    0U                                           /*!< SMPCC WRITE Stream Enable Disable */

#define SMPCC_STMCTRL_TRANS_ALLOC_Pos      2U                                           /*!< SMPCC TRANSLATE ALLOC ATTRIBUTE Enable Position */
#define SMPCC_STMCTRL_TRANS_ALLOC_Msk      (0x1UL << SMPCC_STMCTRL_TRANS_ALLOC_Pos)     /*!< SMPCC TRANSLATE ALLOC ATTRIBUTE Enable Mask */
#define SMPCC_STMCTRL_TRANS_ALLOC_ENABLE   1U                                           /*!< SMPCC TRANSLATE ALLOC ATTRIBUTE Enable */
#define SMPCC_STMCTRL_TRANS_ALLOC_DISABLE  0U                                           /*!< SMPCC TRANSLATE ALLOC ATTRIBUTE Disable */

#define SMPCC_STMCTRL_RD_MERGE_EN_Pos      3U                                           /*!< SMPCC READ Merge Enable Position */
#define SMPCC_STMCTRL_RD_MERGE_EN_Msk      (0x1UL << SMPCC_STMCTRL_RD_MERGE_EN_Pos)     /*!< SMPCC READ Merge Enable Mask */
#define SMPCC_STMCTRL_RD_MERGE_EN_ENABLE   1U                                           /*!< SMPCC READ Merge Enable Enable */
#define SMPCC_STMCTRL_RD_MERGE_EN_DISABLE  0U                                           /*!< SMPCC READ Merge Enable Disable */

#define SMPCC_STMCTRL_CROSS_EN_Pos         4U                                           /*!< SMPCC READ STREAM CROSS 4K Enable Position */
#define SMPCC_STMCTRL_CROSS_EN_Msk         (0x1UL << SMPCC_STMCTRL_CROSS_EN_Pos)        /*!< SMPCC READ STREAM CROSS 4K Enable Mask */
#define SMPCC_STMCTRL_CROSS_EN_ENABLE      1U                                           /*!< SMPCC READ STREAM CROSS 4K Enable */
#define SMPCC_STMCTRL_CROSS_EN_DISABLE     0U                                           /*!< SMPCC READ STREAM CROSS 4K Disable */

/**
 * \brief  Union type to access STM_CFG register.
 */
typedef union
{
    struct {
        __IOM uint32_t rd_byte_threshold:10;    /*!< bit:     0..9 the prefetch number for read stream */
        __IM uint32_t _reserved0:2;             /*!< bit:     10..11 reserved */
        __IOM uint32_t rd_degree:3;             /*!< bit:     12..14 the delta between prefetch address and current bus address */
        __IM uint32_t _reserved1:1;             /*!< bit:     15 reserved */
        __IOM uint32_t rd_distance:3;           /*!< bit:     16..18 the threshold bytes matching write stream training successfully */
        __IM uint32_t _reserved2:1;             /*!< bit:     19 reserved */
        __IOM uint32_t wr_byte_threshold:10;    /*!< bit:     20..29 the line buffer timeout free time when no same cacheline transactions */
        __IM uint32_t _reserved3:2;             /*!< bit:     30..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} STM_CFG_Type;

#define SMPCC_STMCFG_RD_BYTE_THRE_Pos     0U                                            /*!< SMPCC READ BYTE THRESHOLD Position */
#define SMPCC_STMCFG_RD_BYTE_THRE_Msk     (0x3FFUL << SMPCC_STMCFG_RD_BYTE_THRE_Pos)    /*!< SMPCC READ BYTE THRESHOLD Mask */

#define SMPCC_STMCFG_RD_DEGREE_Pos        12U                                           /*!< SMPCC READ DEGREE Position */
#define SMPCC_STMCFG_RD_DEGREE_Msk        (0x7UL << SMPCC_STMCFG_RD_DEGREE_Pos)         /*!< SMPCC READ DEGREE Mask */

#define SMPCC_STMCFG_RD_DISTANCE_Pos      16U                                           /*!< SMPCC READ DISTANCE Position */
#define SMPCC_STMCFG_RD_DISTANCE_Msk      (0x7UL << SMPCC_STMCFG_RD_DISTANCE_Pos)       /*!< SMPCC READ DISTANCE Mask */

#define SMPCC_STMCFG_WR_BYTE_THRE_Pos     20U                                           /*!< SMPCC WRITE BYTE THRESHOLD Position */
#define SMPCC_STMCFG_WR_BYTE_THRE_Msk     (0x7FFUL << SMPCC_STMCFG_WR_BYTE_THRE_Pos)    /*!< SMPCC WRITE BYTE THRESHOLD Mask */

/**
 * \brief  Union type to access STM_TIMEOUT register.
 */
typedef union
{
    struct {
        __IOM uint32_t timeout:11;              /*!< bit:     0..10 write streaming wait clk num */
        __IM uint32_t _reserved:21;             /*!< bit:     11..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} STM_TIMEOUT_Type;

/**
 * \brief  Union type to access DFF_PROT register.
 */
typedef union
{
    struct {
        __IOM uint32_t chk_en:2;                /*!< bit:     0..1 register protect check enable. 2'b01: disable; 2'b10: enable */
        __IM uint32_t _reserved:30;             /*!< bit:     2..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} DFF_PROT_Type;

#define SMPCC_DFF_PROT_CHK_EN_Pos      0U                                      /*!< SMPCC DFF PROTECT CHECK ENABLE Position */
#define SMPCC_DFF_PROT_CHK_EN_Msk      (0x3UL << SMPCC_DFF_PROT_CHK_EN_Pos)    /*!< SMPCC DFF PROTECT CHECK ENABLE Mask */
#define SMPCC_DFF_PROT_CHK_EN_ENABLE   2U                                      /*!< SMPCC DFF PROTECT CHECK ENABLE ENABLE */
#define SMPCC_DFF_PROT_CHK_EN_DISABLE  1U                                      /*!< SMPCC DFF PROTECT CHECK ENABLE DISABLE */

/**
 * \brief  Union type to access ECC_ERR_MSK register.
 */
typedef union
{
    struct {
        __IOM uint32_t cc_l2_err_msk:1;         /*!< bit:     0 mask L2 double bit error output */
        __IOM uint32_t cc_core_err_mask:1;      /*!< bit:     1 mask core double bit error output */
        __IM uint32_t _reserved:29;             /*!< bit:     2..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} ECC_ERR_MSK_Type;

/**
 * \brief  Union type to access NS_RG register.
 */
typedef union
{
    struct {
        __IOM uint64_t cfg:2;                   /*!< bit:     0..1 0x00: disable region; 0x10:NACL; 0x11: NAPOT */
        __IOM uint64_t addr_hi:62;              /*!< bit:     2..63 address of the region */
    } b;                                        /*!< Structure used for bit  access */
    __IOM uint64_t dw;                          /*!< Type      used for double word access */
} NS_RG_Type;

#define SMPCC_NS_RG_CFG_Pos            0U                                       /*!< SMPCC Non-Shareable Region CFG Position */
#define SMPCC_NS_RG_CFG_Msk            (0x3UL << SMPCC_NS_RG_CFG_Pos)           /*!< SMPCC Non-Shareable Region CFG Mask */
#define SMPCC_NS_RG_CFG_DISABLE        0x00U                                    /*!< SMPCC Non-Shareable Region CFG DISABLE */
#define SMPCC_NS_RG_CFG_NACL           0x10U                                    /*!< SMPCC Non-Shareable Region CFG NACL */
#define SMPCC_NS_RG_CFG_NAPOT          0x11U                                    /*!< SMPCC Non-Shareable Region CFG NAPOT */

/**
 * \brief  Union type to access SMP_PMON_SEL register.
 */
typedef union
{
    struct {
        __IOM uint32_t event_sel:16;            /*!< bit:     0..15 select the event for this performance monitor counter */
        __IOM uint32_t client_sel:5;            /*!< bit:     16..20 specify the core in the cluster or external master number hooked to I/O coherency port */
        __IM uint32_t _reserved:11;             /*!< bit:     21..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} SMP_PMON_SEL_Type;

#define SMPCC_PMON_EVENT_SEL_Pos                    0U                                       /*!< SMPCC PMON EVENT SEL Position */
#define SMPCC_PMON_EVENT_SEL_Msk                    (0xFFFFUL << SMPCC_PMON_EVENT_SEL_Pos)   /*!< SMPCC PMON EVENT SEL Mask */
#define SMPCC_PMON_EVENT_DISABLE                    0U                                       /*!< SMPCC PMON EVENT DISABLE */
#define SMPCC_PMON_EVENT_DATA_READ_COUNT            1U                                       /*!< SMPCC PMON EVENT DATA READ COUNT */
#define SMPCC_PMON_EVENT_DATA_WRITE_COUNT           2U                                       /*!< SMPCC PMON EVENT DATA WRITE COUNT SABLE */
#define SMPCC_PMON_EVENT_INSTR_READ_COUNT           3U                                       /*!< SMPCC PMON EVENT INSTR READ COUNT */
#define SMPCC_PMON_EVENT_DATA_READ_HIT_COUNT        4U                                       /*!< SMPCC PMON EVENT DATA READ HIT COUNT */
#define SMPCC_PMON_EVENT_DATA_WRITE_REPLACE_COUNT   5U                                       /*!< SMPCC PMON EVENT DATA WRITE REPLACE COUNT */
#define SMPCC_PMON_EVENT_DATA_READ_REPLACE_COUNT    6U                                       /*!< SMPCC PMON EVENT DATA READ REPLACE COUNT */
#define SMPCC_PMON_EVENT_DATA_READ_MISS_COUNT       7U                                       /*!< SMPCC PMON EVENT DATA READ MISS COUNT */
#define SMPCC_PMON_EVENT_INSTR_READ_HIT_COUNT       8U                                       /*!< SMPCC PMON EVENT INSTR READ HIT COUNT */
#define SMPCC_PMON_EVENT_INSTR_READ_MISS_COUNT      9U                                       /*!< SMPCC PMON EVENT INSTR READ MISS COUNT */
#define SMPCC_PMON_EVENT_INSTR_READ_REPLACE_COUNT   10U                                      /*!< SMPCC PMON EVENT INSTR READ REPLACE COUNT */

#define SMPCC_PMON_CLIENT_SEL_Pos                   16U                                      /*!< SMPCC PMON CLIENT SEL Position */
#define SMPCC_PMON_CLIENT_SEL_Msk                   (0x1FUL << SMPCC_PMON_CLIENT_SEL_Pos)    /*!< SMPCC PMON CLIENT SEL Mask */

#define SMPCC_PMON_EVENT(event, client)                                        \
    (_VAL2FLD(SMPCC_PMON_EVENT_SEL, event) |                                   \
     _VAL2FLD(SMPCC_PMON_CLIENT_SEL, client))

/**
 * \brief  Type to access SMP_PMON_CNT register.
 */
typedef __IO uint64_t SMP_PMON_CNT_Type;

/**
 * \brief  Type to access CLIENT_ERR_ADDR register.
 */
typedef __IO uint64_t CLIENT_ERR_ADDR_Type;

/**
 * \brief  Union type to access CLIENT_WAY_MASK register.
 */
typedef union
{
    struct {
        __IOM uint32_t mask:16;                 /*!< bit:     0..15 mask this way for the client */
        __IM uint32_t _reserved:16;             /*!< bit:     16..31 reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CLIENT_WAY_MASK_Type;

/**
 * \brief Access to the structure of SMPCC Memory Map
 * \remarks Write to these memory-mapped registers should write with full register width.
 */
#pragma pack(4)
typedef struct {
    const SMP_VER_Type SMP_VER;                         /*!< Offset: 0x000 (R)  SMP version register */
    __IM SMP_CFG_Type SMP_CFG;                          /*!< Offset: 0x004 (R)  SMP Configuration register */
    __IM CC_CFG_Type CC_CFG;                            /*!< Offset: 0x008 (R)  CC config register */
    __IOM SMP_ENB_Type SMP_ENB;                         /*!< Offset: 0x00C (R/W)  SMP enable register */
    __IOM CC_CTRL_Type CC_CTRL;                         /*!< Offset: 0x010 (R/W)  CC control register */
    __IOM CC_CMD_Type CC_mCMD;                          /*!< Offset: 0x014 (R/W)  machine mode CC command and status register */
    __IOM CC_ERR_INJ_Type CC_ERR_INJ;                   /*!< Offset: 0x018 (R/W)  CC ECC error injection control register */
    __IOM CC_RECV_CNT_Type CC_RECV_CNT;                 /*!< Offset: 0x01C (R/W)  CC ECC recoverable error count register */
    __IOM CC_FATAL_CNT_Type CC_FATAL_CNT;               /*!< Offset: 0x020 (R/W)  CC ECC fatal error count register */
    __IOM CC_RECV_THV_Type CC_RECV_THV;                 /*!< Offset: 0x024 (R/W)  CC ECC recoverable error threshold register */
    __IOM CC_FATAL_THV_Type CC_FATAL_THV;               /*!< Offset: 0x028 (R/W)  CC ECC fatal error threshold register */
    __IOM CC_BUS_ERR_ADDR_Type CC_BUS_ERR_ADDR;         /*!< Offset: 0x02C (R/W)  CC bus error address register */
    __IM uint8_t RESERVED0[12];                         /*!< 0x034~0x03F reserved */
    __IOM CLIENT_ERR_STATUS_Type CLIENT_ERR_STATUS[32]; /*!< Offset: 0x040 (R/W) client error status register */
    __IOM CC_CMD_Type CC_sCMD;                          /*!< Offset: 0x0C0 (R/W) supervisor mode CC command and status register */
    __IOM CC_CMD_Type CC_uCMD;                          /*!< Offset: 0x0C4 (R/W) user mode CC command and status register */
    __IM SNOOP_PENDING_Type SNOOP_PENDING;              /*!< Offset: 0x0C8 (R)  indicate the core is being snooped or not in SCU */
    __IM TRANS_PENDING_Type TRANS_PENDING;              /*!< Offset: 0x0CC (R)  indicate the core's transaction is finished or not in the SCU */
    __IOM CLM_ADDR_BASE_Type CLM_ADDR_BASE;             /*!< Offset: 0x0D0 (R/W)  Cluster Local Memory base address */
    __IOM uint32_t CLM_WAY_EN;                          /*!< Offset: 0x0D8 (R/W)  CC way enable register */
    __IOM CC_INVALID_ALL_Type CC_INVALID_ALL;           /*!< Offset: 0x0DC (R/W)  CC invalidate all register */
    __IOM STM_CTRL_Type STM_CTRL;                       /*!< Offset: 0x0E0 (R/W)  Stream read/write control register */
    __IOM STM_CFG_Type STM_CFG;                         /*!< Offset: 0x0E4 (R/W)  Stream read/write configuration register */
    __IOM STM_TIMEOUT_Type STM_TIMEOUT;                 /*!< Offset: 0x0E8 (R/W)  Stream read/write timeout register */
    __IOM DFF_PROT_Type DFF_PROT;                       /*!< Offset: 0x0EC (R/W)  Hardware Register protect Enable register */
    __IOM ECC_ERR_MSK_Type ECC_ERR_MSK;                 /*!< Offset: 0x0F0 (R/W)  Mask L2M ECC Error register */
    __IM uint8_t RESERVED1[12];                         /*!< 0x0F4~0x0FF reserved */
    __IOM NS_RG_Type NS_RG[16];                         /*!< Offset: 0x100 (R/W)  Non-Sharable Memory Region register */
    __IOM SMP_PMON_SEL_Type SMP_PMON_SEL[16];           /*!< Offset: 0x180 (R/W)  Performance Monitor Event Selector */
    __IOM SMP_PMON_CNT_Type SMP_PMON_CNT[16];           /*!< Offset: 0x1C0 (R/W)  Performance Monitor Event Counter */
    __IM uint8_t RESERVED2[64];                         /*!< 0x240~0x27F reserved */
    __IOM CLIENT_ERR_ADDR_Type CLIENT_ERR_ADDR[32];     /*!< Offset: 0x280 (R/W)  The error address register */
    __IOM CLIENT_WAY_MASK_Type CLIENT_WAY_MASK[32];     /*!< Offset: 0x380 (R/W)  CC way mask control register */
} SMPCC_Type;
#pragma pack()

#ifndef __SMPCC_BASEADDR
/* Base address of SMPCC(__SMPCC_BASEADDR) should be defined in <Device.h> */
#error "__SMPCC_BASEADDR is not defined, please check!"
#endif

/* SMPCC Memory mapping of Device */
#define SMPCC_BASE          __SMPCC_BASEADDR             /*!< SMPCC Base Address */
#define SMPCC               ((SMPCC_Type *)SMPCC_BASE)   /*!< SMPCC configuration struct */

/** @} */ /* end of group NMSIS_Core_SMPCC_Registers */

/**
 * \defgroup NMSIS_Core_SMPCC_Functions SMPCC Functions
 * \ingroup  NMSIS_Core
 * \brief SMPCC related functions
 *
 *   @{
 */

/**
 * \brief  Get the SMP version number
 * \details
 * This function gets the hardware version information from SMP_VER register.
 * \return   hardware version number in SMP_VER register.
*/
__STATIC_FORCEINLINE SMP_VER_Type SMPCC_GetVersion(void)
{
    return SMPCC->SMP_VER;
}

/**
 * \brief  Check if cluster cache is present
 * \details
 * This function checks if the cluster cache is present in the system.
 * \return   1 if cluster cache is present, 0 otherwise
*/
__STATIC_FORCEINLINE uint8_t SMPCC_IsCCachePresent(void)
{
    return SMPCC->SMP_CFG.b.cc_present;
}

/**
 * \brief  Get the number of cores in the cluster
 * \details
 * This function returns the number of cores in the SMP cluster.
 * \return   Number of cores in the cluster
*/
__STATIC_FORCEINLINE uint8_t SMPCC_GetCoreNum(void)
{
    return SMPCC->SMP_CFG.b.smp_core_num + 1;
}

/**
 * \brief  Get the number of IO coherency ports
 * \details
 * This function returns the number of IO coherency ports in the cluster.
 * \return   Number of IO coherency ports
*/
__STATIC_FORCEINLINE uint8_t SMPCC_GetIOCPNum(void)
{
    return SMPCC->SMP_CFG.b.iocp_num;
}

/**
 * \brief  Get the number of performance monitors
 * \details
 * This function returns the number of performance monitors in the cluster.
 * \return   Number of performance monitors
*/
__STATIC_FORCEINLINE uint8_t SMPCC_GetPMONNum(void)
{
    return SMPCC->SMP_CFG.b.pmon_num;
}

/**
 * \brief  Get the number of cache sets
 * \details
 * This function returns the number of cache sets in the cluster cache (2^cc_set).
 * \return   Number of cache sets
*/
__STATIC_FORCEINLINE uint32_t SMPCC_GetCCacheSetNum(void)
{
    return 1U << SMPCC->CC_CFG.b.cc_set;
}

/**
 * \brief  Get the number of cache ways
 * \details
 * This function returns the number of cache ways in the cluster cache (cc_way + 1).
 * \return   Number of cache ways
*/
__STATIC_FORCEINLINE uint32_t SMPCC_GetCCacheWayNum(void)
{
    return SMPCC->CC_CFG.b.cc_way + 1;
}

/**
 * \brief  Get the cache line size
 * \details
 * This function returns the cache line size in the cluster cache (2^(cc_lsize + 2)).
 * \return   Cache line size in bytes
*/
__STATIC_FORCEINLINE uint8_t SMPCC_GetCCacheLineSize(void)
{
    return 1 << (SMPCC->CC_CFG.b.cc_lsize + 2);
}

/**
 * \brief  Check if cluster cache supports ECC
 * \details
 * This function checks if the cluster cache supports ECC functionality.
 * \return   1 if ECC is supported, 0 otherwise
*/
__STATIC_FORCEINLINE uint8_t SMPCC_IsCCacheSupportECC(void)
{
    return SMPCC->CC_CFG.b.cc_ecc;
}

/**
 * \brief  Enable snoop for specific clients
 * \details
 * This function enables snoop functionality for specified client mask.
 * \param [in]    client_msk    Client mask to enable snoop for
 * \sa
 * - \ref SMPCC_DisableSnoop
*/
__STATIC_FORCEINLINE void SMPCC_EnableSnoop(uint16_t client_msk)
{
    SMPCC->SMP_ENB.b.smp_enable |= client_msk;
}

/**
 * \brief  Disable snoop for specific clients
 * \details
 * This function disables snoop functionality for specified client mask.
 * \param [in]    client_msk    Client mask to disable snoop for
 * \sa
 * - \ref SMPCC_EnableSnoop
*/
__STATIC_FORCEINLINE void SMPCC_DisableSnoop(uint16_t client_msk)
{
    SMPCC->SMP_ENB.b.smp_enable &= ~client_msk;
}

/**
 * \brief  Enable cluster cache
 * \details
 * This function enables the cluster cache.
 * \sa
 * - \ref SMPCC_DisableCCache
*/
__STATIC_FORCEINLINE void SMPCC_EnableCCache(void)
{
    SMPCC->CC_CTRL.b.cc_en = SMPCC_CTRL_CC_EN_ENABLE;
}

/**
 * \brief  Disable cluster cache
 * \details
 * This function disables the cluster cache.
 * \sa
 * - \ref SMPCC_EnableCCache
*/
__STATIC_FORCEINLINE void SMPCC_DisableCCache(void)
{
    SMPCC->CC_CTRL.b.cc_en = SMPCC_CTRL_CC_EN_DISABLE;
}

/**
 * \brief  Get status of cluster cache
 * \details
 * This function returns the enable status of the cluster cache.
 * \return             Return the status of cluster cache (1 - enabled, 0 - disabled)
 * \sa
 * - \ref SMPCC_EnableCCache
*/
__STATIC_FORCEINLINE int32_t SMPCC_IsCCacheEnabled(void)
{
    return SMPCC->CC_CTRL.b.cc_en;
}

/**
 * \brief  Check if any client is being snooped
 * \details
 * This function checks if any client specified in the mask is currently being snooped.
 * \param [in]    client_msk    Client mask to check
 * \return        1 if any client is being snooped, 0 otherwise
*/
__STATIC_FORCEINLINE uint8_t SMPCC_IsAnySnoopPending(uint32_t client_msk)
{
    return (SMPCC->SNOOP_PENDING.b.snoop_pending & client_msk) != 0;
}

/**
 * \brief  Check if any transaction is pending for clients
 * \details
 * This function checks if any transaction is pending for clients specified in the mask.
 * \param [in]    client_msk    Client mask to check
 * \return        1 if any transaction is pending, 0 otherwise
*/
__STATIC_FORCEINLINE uint8_t SMPCC_IsAnyTransactionPending(uint32_t client_msk)
{
    return (SMPCC->TRANS_PENDING.b.trans_pending & client_msk) != 0;
}

/**
 * \brief  Set Cluster Local Memory to use all ways
 * \details
 * This function configures the Cluster Local Memory to use all cache ways at the specified address.
 * \param [in]    addr    Base address for Cluster Local Memory.
 * \remarks
 * - Access to the \c CLM_ADDR_BASE register depends on the physical address (PA) size.
 *   If the PA size is not larger than 32 bits, only the low 32 bits of the address can be accessed.
 * - Before changing the Cluster Cache to CLM mode, ensure that:
 *   - The Cluster Cache is disabled,
 *   - Its contents have been invalidated and flushed to memory.
 *
 * Example usage:
 * \code
 * SMPCC_DisableCCache();
 * MFlushInvalCCache();
 * SMPCC_SetCLMAllWays(addr);
 * \endcode
 *
 * \sa
 * - \ref SMPCC_SetCLMNWays
 * - \ref SMPCC_SetCLMNoWay
*/
__STATIC_FORCEINLINE void SMPCC_SetCLMAllWays(uint64_t addr)
{
#if (__CPU_PA_SIZE > 32)
    SMPCC->CLM_ADDR_BASE.clm_base64 = addr;
#else
    SMPCC->CLM_ADDR_BASE.clm32.clm_base32 = (uint32_t)addr;
#endif
    SMPCC->CLM_WAY_EN = 0xFFFFU;
}

/**
 * \brief  Set Cluster Local Memory to use specific ways
 * \details
 * This function configures the Cluster Local Memory to use specific cache ways at the specified address.
 * \param [in]    addr      Base address for Cluster Local Memory
 * \param [in]    way_msk   Way mask to configure
 * \remarks
 * - Access to the \c CLM_ADDR_BASE register depends on the physical address (PA) size.
 *   If the PA size is not larger than 32 bits, only the low 32 bits of the address can be accessed.
 * - Before changing the Cluster Cache to CLM mode, ensure that:
 *   - The Cluster Cache is disabled,
 *   - Its contents have been invalidated and flushed to memory.
 *
 * Example usage:
 * \code
 * SMPCC_DisableCCache();
 * MFlushInvalCCache();
 * SMPCC_SetCLMNWays(addr, way_msk);
 * SMPCC_EnableCCache(); // Enable the remaining CCache
 * \endcode
 *
 * \sa
 * - \ref SMPCC_SetCLMAllWays
 * - \ref SMPCC_SetCLMNoWay
*/
__STATIC_FORCEINLINE void SMPCC_SetCLMNWays(uint64_t addr, uint32_t way_msk)
{
#if (__CPU_PA_SIZE > 32)
    SMPCC->CLM_ADDR_BASE.clm_base64 = addr;
#else
    SMPCC->CLM_ADDR_BASE.clm32.clm_base32 = (uint32_t)addr;
#endif
    SMPCC->CLM_WAY_EN = way_msk;
}

/**
 * \brief  Configure Cluster Local Memory to use no ways
 * \details
 * This function configures the Cluster Local Memory to not use any cache ways.
 * \sa
 * - \ref SMPCC_SetCLMAllWays
 * - \ref SMPCC_SetCLMNWays
*/
__STATIC_FORCEINLINE void SMPCC_SetCLMNoWay(void)
{
    SMPCC->CLM_WAY_EN = 0x0000U;
}

/**
 * \brief  Set Cluster Cache Control register
 * \details
 * This function sets the value of the Cluster Cache Control register.
 * \param [in]    val    Value to set in the control register
 * \sa
 * - \ref SMPCC_GetCCacheControl
*/
__STATIC_FORCEINLINE void SMPCC_SetCCacheControl(uint32_t val)
{
     SMPCC->CC_CTRL.w = val;
}

/**
 * \brief  Get Cluster Cache Control register value
 * \details
 * This function returns the current value of the Cluster Cache Control register.
 * \return   Current value of the control register
 * \sa
 * - \ref SMPCC_SetCCacheControl
*/
__STATIC_FORCEINLINE uint32_t SMPCC_GetCCacheControl(void)
{
     return SMPCC->CC_CTRL.w;
}

/**
 * \brief  Enable Cluster Cache ECC
 * \details
 * This function enables ECC functionality for the cluster cache.
 * \sa
 * - \ref SMPCC_DisableCCacheECC
*/
__STATIC_FORCEINLINE void SMPCC_EnableCCacheECC(void)
{
     SMPCC->CC_CTRL.b.cc_ecc_en = SMPCC_CTRL_CC_ECC_EN_ENABLE;
}

/**
 * \brief  Disable Cluster Cache ECC
 * \details
 * This function disables ECC functionality for the cluster cache.
 * \sa
 * - \ref SMPCC_EnableCCacheECC
*/
__STATIC_FORCEINLINE void SMPCC_DisableCCacheECC(void)
{
     SMPCC->CC_CTRL.b.cc_ecc_en = SMPCC_CTRL_CC_ECC_EN_DISABLE;
}

/**
 * \brief  Enable Cluster Cache ECC Exception
 * \details
 * This function enables ECC exception handling for the cluster cache.
 * \sa
 * - \ref SMPCC_DisableCCacheECCExcp
*/
__STATIC_FORCEINLINE void SMPCC_EnableCCacheECCExcp(void)
{
     SMPCC->CC_CTRL.b.ecc_excp_en = SMPCC_CTRL_CC_ECC_EXCP_EN_ENABLE;
}

/**
 * \brief  Disable Cluster Cache ECC Exception
 * \details
 * This function disables ECC exception handling for the cluster cache.
 * \sa
 * - \ref SMPCC_EnableCCacheECCExcp
*/
__STATIC_FORCEINLINE void SMPCC_DisableCCacheECCExcp(void)
{
     SMPCC->CC_CTRL.b.ecc_excp_en = SMPCC_CTRL_CC_ECC_EXCP_EN_DISABLE;
}

/**
 * \brief  Lock ECC Configuration
 * \details
 * This function locks the ECC configuration to prevent further changes.
 * \sa
 * - \ref SMPCC_LockECCErrInjection
*/
__STATIC_FORCEINLINE void SMPCC_LockECCConfig(void)
{
     SMPCC->CC_CTRL.b.lock_ecc_cfg = SMPCC_CTRL_LOCK_ECC_CFG_LOCK;
}

/**
 * \brief  Lock ECC Error Injection Register
 * \details
 * This function locks the ECC error injection register to prevent further changes.
 * \sa
 * - \ref SMPCC_LockECCConfig
*/
__STATIC_FORCEINLINE void SMPCC_LockECCErrInjection(void)
{
     SMPCC->CC_CTRL.b.lock_ecc_err_inj = SMPCC_CTRL_LOCK_ECC_ERR_INJ_LOCK;
}

/**
 * \brief  Enable Recoverable Error Interrupt
 * \details
 * This function enables interrupt generation when recoverable error count exceeds the threshold.
 * \sa
 * - \ref SMPCC_DisableRecvErrIrq
*/
__STATIC_FORCEINLINE void SMPCC_EnableRecvErrIrq(void)
{
     SMPCC->CC_CTRL.b.recv_err_irq_en = SMPCC_CTRL_RECV_ERR_IRQ_EN_ENABLE;
}

/**
 * \brief  Disable Recoverable Error Interrupt
 * \details
 * This function disables interrupt generation when recoverable error count exceeds the threshold.
 * \sa
 * - \ref SMPCC_EnableRecvErrIrq
*/
__STATIC_FORCEINLINE void SMPCC_DisableRecvErrIrq(void)
{
     SMPCC->CC_CTRL.b.recv_err_irq_en = SMPCC_CTRL_RECV_ERR_IRQ_EN_DISABLE;
}

/**
 * \brief  Enable Fatal Error Interrupt
 * \details
 * This function enables interrupt generation when fatal error count exceeds the threshold.
 * \sa
 * - \ref SMPCC_DisableFatalErrIrq
*/
__STATIC_FORCEINLINE void SMPCC_EnableFatalErrIrq(void)
{
     SMPCC->CC_CTRL.b.fatal_err_irq_en = SMPCC_CTRL_FATAL_ERR_IRQ_EN_ENABLE;
}

/**
 * \brief  Disable Fatal Error Interrupt
 * \details
 * This function disables interrupt generation when fatal error count exceeds the threshold.
 * \sa
 * - \ref SMPCC_EnableFatalErrIrq
*/
__STATIC_FORCEINLINE void SMPCC_DisableFatalErrIrq(void)
{
     SMPCC->CC_CTRL.b.fatal_err_irq_en = SMPCC_CTRL_FATAL_ERR_IRQ_EN_DISABLE;
}

/**
 * \brief  Enable Bus Error Interrupt
 * \details
 * This function enables interrupt generation for bus errors in cluster cache maintenance operations.
 * \sa
 * - \ref SMPCC_DisableBusErrIrq
*/
__STATIC_FORCEINLINE void SMPCC_EnableBusErrIrq(void)
{
     SMPCC->CC_CTRL.b.bus_err_irq_en = SMPCC_CTRL_BUS_ERR_IRQ_EN_ENABLE;
}

/**
 * \brief  Disable Bus Error Interrupt
 * \details
 * This function disables interrupt generation for bus errors in cluster cache maintenance operations.
 * \sa
 * - \ref SMPCC_EnableBusErrIrq
*/
__STATIC_FORCEINLINE void SMPCC_DisableBusErrIrq(void)
{
     SMPCC->CC_CTRL.b.bus_err_irq_en = SMPCC_CTRL_BUS_ERR_IRQ_EN_DISABLE;
}

/**
 * \brief  Enable Supervisor Mode Commands
 * \details
 * This function enables supervisor mode to operate CC_sCMD and SMP_PMON_SEL registers.
 * \sa
 * - \ref SMPCC_DisableSModeCmd
*/
__STATIC_FORCEINLINE void SMPCC_EnableSModeCmd(void)
{
     SMPCC->CC_CTRL.b.sup_cmd_en = SMPCC_CTRL_SUP_CMD_EN_ENABLE;
}

/**
 * \brief  Disable Supervisor Mode Commands
 * \details
 * This function disables supervisor mode from operating CC_sCMD and SMP_PMON_SEL registers.
 * \sa
 * - \ref SMPCC_EnableSModeCmd
*/
__STATIC_FORCEINLINE void SMPCC_DisableSModeCmd(void)
{
     SMPCC->CC_CTRL.b.sup_cmd_en = SMPCC_CTRL_SUP_CMD_EN_DISABLE;
}

/**
 * \brief  Enable User Mode Commands
 * \details
 * This function enables user mode to operate CC_uCMD and SMP_PMON_SEL registers.
 * \sa
 * - \ref SMPCC_DisableUModeCmd
*/
__STATIC_FORCEINLINE void SMPCC_EnableUModeCmd(void)
{
     SMPCC->CC_CTRL.b.use_cmd_en = SMPCC_CTRL_USE_CMD_EN_ENABLE;
}

/**
 * \brief  Disable User Mode Commands
 * \details
 * This function disables user mode from operating CC_uCMD and SMP_PMON_SEL registers.
 * \sa
 * - \ref SMPCC_EnableUModeCmd
*/
__STATIC_FORCEINLINE void SMPCC_DisableUModeCmd(void)
{
     SMPCC->CC_CTRL.b.use_cmd_en = SMPCC_CTRL_USE_CMD_EN_DISABLE;
}

/**
 * \brief  Enable Cluster Cache ECC Check
 * \details
 * This function enables ECC check functionality for the cluster cache.
 * \sa
 * - \ref SMPCC_DisableCCacheECCCheck
*/
__STATIC_FORCEINLINE void SMPCC_EnableCCacheECCCheck(void)
{
     SMPCC->CC_CTRL.b.ecc_chk_en = SMPCC_CTRL_ECC_CHK_EN_ENABLE;
}

/**
 * \brief  Disable Cluster Cache ECC Check
 * \details
 * This function disables ECC check functionality for the cluster cache.
 * \sa
 * - \ref SMPCC_EnableCCacheECCCheck
*/
__STATIC_FORCEINLINE void SMPCC_DisableCCacheECCCheck(void)
{
     SMPCC->CC_CTRL.b.ecc_chk_en = SMPCC_CTRL_ECC_CHK_EN_DISABLE;
}

/**
 * \brief  Enable Cluster Local Memory ECC
 * \details
 * This function enables ECC functionality for the Cluster Local Memory.
 * \sa
 * - \ref SMPCC_DisableCLMECC
*/
__STATIC_FORCEINLINE void SMPCC_EnableCLMECC(void)
{
     SMPCC->CC_CTRL.b.clm_ecc_en = SMPCC_CTRL_CLM_ECC_EN_ENABLE;
}

/**
 * \brief  Disable Cluster Local Memory ECC
 * \details
 * This function disables ECC functionality for the Cluster Local Memory.
 * \sa
 * - \ref SMPCC_EnableCLMECC
*/
__STATIC_FORCEINLINE void SMPCC_DisableCLMECC(void)
{
     SMPCC->CC_CTRL.b.clm_ecc_en = SMPCC_CTRL_CLM_ECC_EN_DISABLE;
}

/**
 * \brief  Enable Cluster Local Memory ECC Check
 * \details
 * This function enables ECC check functionality for the Cluster Local Memory.
 * \sa
 * - \ref SMPCC_DisableCLMCCCheck
*/
__STATIC_FORCEINLINE void SMPCC_EnableCLMECCCheck(void)
{
     SMPCC->CC_CTRL.b.clm_ecc_chk_en = SMPCC_CTRL_CLM_ECC_CHK_EN_ENABLE;
}

/**
 * \brief  Disable Cluster Local Memory ECC Check
 * \details
 * This function disables ECC check functionality for the Cluster Local Memory.
 * \sa
 * - \ref SMPCC_EnableCLMECCCheck
*/
__STATIC_FORCEINLINE void SMPCC_DisableCLMECCCheck(void)
{
     SMPCC->CC_CTRL.b.clm_ecc_chk_en = SMPCC_CTRL_CLM_ECC_CHK_EN_DISABLE;
}

/**
 * \brief  Enable Cluster Local Memory ECC Exception
 * \details
 * This function enables ECC exception handling for the Cluster Local Memory.
 * \sa
 * - \ref SMPCC_DisableCLMECCExcp
*/
__STATIC_FORCEINLINE void SMPCC_EnableCLMECCExcp(void)
{
     SMPCC->CC_CTRL.b.clm_excp_en = SMPCC_CTRL_CLM_EXCP_EN_ENABLE;
}

/**
 * \brief  Disable Cluster Local Memory ECC Exception
 * \details
 * This function disables ECC exception handling for the Cluster Local Memory.
 * \sa
 * - \ref SMPCC_EnableCLMECCExcp
*/
__STATIC_FORCEINLINE void SMPCC_DisableCLMECCExcp(void)
{
     SMPCC->CC_CTRL.b.clm_excp_en = SMPCC_CTRL_CLM_EXCP_EN_DISABLE;
}

/**
 * \brief  Enable L1 Prefetch to Snoop and Share Cacheline
 * \details
 * This function enables L1 prefetch to snoop and share cacheline from other cores.
 * \sa
 * - \ref SMPCC_DisableL1PrefetchShareCacheline
*/
__STATIC_FORCEINLINE void SMPCC_EnableL1PrefetchShareCacheline(void)
{
    SMPCC->CC_CTRL.b.pf_sh_cl_en = SMPCC_CTRL_PF_SH_CL_EN_ENABLE;
}

/**
 * \brief  Disable L1 Prefetch to Snoop and Share Cacheline
 * \details
 * This function disables L1 prefetch to snoop and share cacheline from other cores.
 * \sa
 * - \ref SMPCC_EnableL1PrefetchShareCacheline
*/
__STATIC_FORCEINLINE void SMPCC_DisableL1PrefetchShareCacheline(void)
{
    SMPCC->CC_CTRL.b.pf_sh_cl_en = SMPCC_CTRL_PF_SH_CL_EN_DISABLE;
}

/**
 * \brief  Enable Cluster Cache Early Prefetch
 * \details
 * This function enables L2 prefetch to initialize external bus read access while looking up the cluster cache.
 * \sa
 * - \ref SMPCC_DisableCCacheEarlyPrefetch
*/
__STATIC_FORCEINLINE void SMPCC_EnableCCacheEarlyPrefetch(void)
{
    SMPCC->CC_CTRL.b.pf_l2_early_en = SMPCC_CTRL_PF_L2_EARLY_EN_ENABLE;
}

/**
 * \brief  Disable Cluster Cache Early Prefetch
 * \details
 * This function disables L2 prefetch to initialize external bus read access while looking up the cluster cache.
 * \sa
 * - \ref SMPCC_EnableCCacheEarlyPrefetch
*/
__STATIC_FORCEINLINE void SMPCC_DisableCCacheEarlyPrefetch(void)
{
    SMPCC->CC_CTRL.b.pf_l2_early_en = SMPCC_CTRL_PF_L2_EARLY_EN_DISABLE;
}

/**
 * \brief  Limit Cluster Cache Prefetch Outstanding Number
 * \details
 * This function enables the limit of outstanding L2 prefetch to the number of L2 prefetch line-buffer.
 * \sa
 * - \ref SMPCC_UnlimitCCachePrefetchOutsNum
*/
__STATIC_FORCEINLINE void SMPCC_LimitCCachePrefetchOutsNum(void)
{
    SMPCC->CC_CTRL.b.pf_biu_outs_en = SMPCC_CTRL_PF_BIU_OUTS_EN_ENABLE;
}

/**
 * \brief  Unlimit Cluster Cache Prefetch Outstanding Number
 * \details
 * This function disables the limit of outstanding L2 prefetch to the number of L2 prefetch line-buffer.
 * \sa
 * - \ref SMPCC_LimitCCachePrefetchOutsNum
*/
__STATIC_FORCEINLINE void SMPCC_UnlimitCCachePrefetchOutsNum(void)
{
    SMPCC->CC_CTRL.b.pf_biu_outs_en = SMPCC_CTRL_PF_BIU_OUTS_EN_DISABLE;
}

/**
 * \brief  Enable Cluster Cache Prefetch to Avoid Write Back
 * \details
 * This function enables L2 prefetch to abort and avoid dirty cacheline write back when filling the cluster cache.
 * \sa
 * - \ref SMPCC_DisableCCachePrefetchNoWb
*/
__STATIC_FORCEINLINE void SMPCC_EnableCCachePrefetchNoWb(void)
{
    SMPCC->CC_CTRL.b.pf_no_wb = SMPCC_CTRL_PF_NO_WB_ENABLE;
}

/**
 * \brief  Disable Cluster Cache Prefetch to Avoid Write Back
 * \details
 * This function disables L2 prefetch from aborting and avoiding dirty cacheline write back when filling the cluster cache.
 * \sa
 * - \ref SMPCC_EnableCCachePrefetchNoWb
*/
__STATIC_FORCEINLINE void SMPCC_DisableCCachePrefetchNoWb(void)
{
    SMPCC->CC_CTRL.b.pf_no_wb = SMPCC_CTRL_PF_NO_WB_DISABLE;
}

/**
 * \brief  Enable ICache to Snoop DCache
 * \details
 * This function enables snoop to dcache for icache refill reads.
 * \sa
 * - \ref SMPCC_DisableICacheSnoopDCache
*/
__STATIC_FORCEINLINE void SMPCC_EnableICacheSnoopDCache(void)
{
    SMPCC->CC_CTRL.b.i_snoop_d_en = SMPCC_CTRL_I_SNOOP_D_EN_ENABLE;
}

/**
 * \brief  Disable ICache to Snoop DCache
 * \details
 * This function disables snoop to dcache for icache refill reads.
 * \sa
 * - \ref SMPCC_EnableICacheSnoopDCache
*/
__STATIC_FORCEINLINE void SMPCC_DisableICacheSnoopDCache(void)
{
    SMPCC->CC_CTRL.b.i_snoop_d_en = SMPCC_CTRL_I_SNOOP_D_EN_DISABLE;
}

/**
 * \brief  Get Recoverable Error Count
 * \details
 * This function returns the current count of recoverable errors.
 * \return   Current count of recoverable errors
 * \sa
 * - \ref SMPCC_ClearRecvErrCount
*/
__STATIC_FORCEINLINE uint32_t SMPCC_GetRecvErrCount(void)
{
    return SMPCC->CC_RECV_CNT.b.cnt;
}

/**
 * \brief  Clear Recoverable Error Count
 * \details
 * This function clears the recoverable error count register.
 * \sa
 * - \ref SMPCC_GetRecvErrCount
*/
__STATIC_FORCEINLINE void SMPCC_ClearRecvErrCount(void)
{
    SMPCC->CC_RECV_CNT.w = 0;
}

/**
 * \brief  Get Fatal Error Count
 * \details
 * This function returns the current count of fatal errors.
 * \return   Current count of fatal errors
 * \sa
 * - \ref SMPCC_ClearFatalErrCount
*/
__STATIC_FORCEINLINE uint32_t SMPCC_GetFatalErrCount(void)
{
    return SMPCC->CC_FATAL_CNT.b.cnt;
}

/**
 * \brief  Clear Fatal Error Count
 * \details
 * This function clears the fatal error count register.
 * \sa
 * - \ref SMPCC_GetFatalErrCount
*/
__STATIC_FORCEINLINE void SMPCC_ClearFatalErrCount(void)
{
    SMPCC->CC_FATAL_CNT.w = 0;
}

/**
 * \brief  Set Recoverable Error Count Threshold
 * \details
 * This function sets the threshold value for recoverable error count.
 * \param [in]    threshold    Threshold value to set
 * \sa
 * - \ref SMPCC_GetRecvErrCntThreshold
*/
__STATIC_FORCEINLINE void SMPCC_SetRecvErrCntThreshold(uint16_t threshold)
{
    SMPCC->CC_RECV_THV.b.cnt = threshold;
}

/**
 * \brief  Get Recoverable Error Count Threshold
 * \details
 * This function returns the current threshold value for recoverable error count.
 * \return   Current threshold value for recoverable error count
 * \sa
 * - \ref SMPCC_SetRecvErrCntThreshold
*/
__STATIC_FORCEINLINE uint16_t SMPCC_GetRecvErrCntThreshold(void)
{
    return SMPCC->CC_RECV_THV.b.cnt;
}

/**
 * \brief  Set Fatal Error Count Threshold
 * \details
 * This function sets the threshold value for fatal error count.
 * \param [in]    threshold    Threshold value to set
 * \sa
 * - \ref SMPCC_GetFatalErrCntThreshold
*/
__STATIC_FORCEINLINE void SMPCC_SetFatalErrCntThreshold(uint16_t threshold)
{
    SMPCC->CC_FATAL_THV.b.cnt = threshold;
}

/**
 * \brief  Get Fatal Error Count Threshold
 * \details
 * This function returns the current threshold value for fatal error count.
 * \return   Current threshold value for fatal error count
 * \sa
 * - \ref SMPCC_SetFatalErrCntThreshold
*/
__STATIC_FORCEINLINE uint16_t SMPCC_GetFatalErrCntThreshold(void)
{
    return SMPCC->CC_FATAL_THV.b.cnt;
}

/**
 * \brief  Get Client Error Status
 * \details
 * This function returns the error status for a specific client.
 * \param [in]    client_id    ID of the client to get error status for
 * \return        Error status of the specified client
*/
__STATIC_FORCEINLINE uint32_t SMPCC_GetClientErrStatus(uint8_t client_id)
{
    return SMPCC->CLIENT_ERR_STATUS[client_id].w;
}

/**
 * \brief  Set Stream Control Register
 * \details
 * This function sets the value of the Stream Control register.
 * \param [in]    val    Value to set in the stream control register
 * \sa
 * - \ref SMPCC_GetSTMControl
*/
__STATIC_FORCEINLINE void SMPCC_SetSTMControl(uint32_t val)
{
    SMPCC->STM_CTRL.w = val;
}

/**
 * \brief  Get Stream Control Register Value
 * \details
 * This function returns the current value of the Stream Control register.
 * \return   Current value of the stream control register
 * \sa
 * - \ref SMPCC_SetSTMControl
*/
__STATIC_FORCEINLINE uint32_t SMPCC_GetSTMControl(void)
{
    return SMPCC->STM_CTRL.w;
}

/**
 * \brief  Enable Stream Read
 * \details
 * This function enables stream read functionality.
 * \sa
 * - \ref SMPCC_DisableStreamRead
*/
__STATIC_FORCEINLINE void SMPCC_EnableStreamRead(void)
{
    SMPCC->STM_CTRL.b.rd_stm_en = SMPCC_STMCTRL_RD_STM_EN_ENABLE;
}

/**
 * \brief  Disable Stream Read
 * \details
 * This function disables stream read functionality.
 * \sa
 * - \ref SMPCC_EnableStreamRead
*/
__STATIC_FORCEINLINE void SMPCC_DisableStreamRead(void)
{
    SMPCC->STM_CTRL.b.rd_stm_en = SMPCC_STMCTRL_RD_STM_EN_DISABLE;
}

/**
 * \brief  Enable Stream Write
 * \details
 * This function enables stream write functionality.
 * \sa
 * - \ref SMPCC_DisableStreamWrite
*/
__STATIC_FORCEINLINE void SMPCC_EnableStreamWrite(void)
{
    SMPCC->STM_CTRL.b.wr_stm_en = SMPCC_STMCTRL_WR_STM_EN_ENABLE;
}

/**
 * \brief  Disable Stream Write
 * \details
 * This function disables stream write functionality.
 * \sa
 * - \ref SMPCC_EnableStreamWrite
*/
__STATIC_FORCEINLINE void SMPCC_DisableStreamWrite(void)
{
    SMPCC->STM_CTRL.b.wr_stm_en = SMPCC_STMCTRL_WR_STM_EN_DISABLE;
}

/**
 * \brief  Enable Stream Translate Allocate
 * \details
 * This function enables translation of allocate attribute to non-alloc attribute.
 * \sa
 * - \ref SMPCC_DisableStreamTransAlloc
*/
__STATIC_FORCEINLINE void SMPCC_EnableStreamTransAlloc(void)
{
    SMPCC->STM_CTRL.b.trans_alloc = SMPCC_STMCTRL_TRANS_ALLOC_ENABLE;
}

/**
 * \brief  Disable Stream Translate Allocate
 * \details
 * This function disables translation of allocate attribute to non-alloc attribute.
 * \sa
 * - \ref SMPCC_EnableStreamTransAlloc
*/
__STATIC_FORCEINLINE void SMPCC_DisableStreamTransAlloc(void)
{
    SMPCC->STM_CTRL.b.trans_alloc = SMPCC_STMCTRL_TRANS_ALLOC_DISABLE;
}

/**
 * \brief  Enable Stream Merge Non-Cacheable Read
 * \details
 * This function enables non-cacheable attribute read merge functionality.
 * \sa
 * - \ref SMPCC_DisableStreamMergeNCRead
*/
__STATIC_FORCEINLINE void SMPCC_EnableStreamMergeNCRead(void)
{
    SMPCC->STM_CTRL.b.rd_merge_en = SMPCC_STMCTRL_RD_MERGE_EN_ENABLE;
}

/**
 * \brief  Disable Stream Merge Non-Cacheable Read
 * \details
 * This function disables non-cacheable attribute read merge functionality.
 * \sa
 * - \ref SMPCC_EnableStreamMergeNCRead
*/
__STATIC_FORCEINLINE void SMPCC_DisableStreamMergeNCRead(void)
{
    SMPCC->STM_CTRL.b.rd_merge_en = SMPCC_STMCTRL_RD_MERGE_EN_DISABLE;
}

/**
 * \brief  Enable Stream Read Cross 4K Boundary
 * \details
 * This function enables read stream to cross 4K boundary.
 * \sa
 * - \ref SMPCC_DisableStreamReadCross4K
*/
__STATIC_FORCEINLINE void SMPCC_EnableStreamReadCross4K(void)
{
    SMPCC->STM_CTRL.b.cross_en = SMPCC_STMCTRL_CROSS_EN_ENABLE;
}

/**
 * \brief  Disable Stream Read Cross 4K Boundary
 * \details
 * This function disables read stream from crossing 4K boundary.
 * \sa
 * - \ref SMPCC_EnableStreamReadCross4K
*/
__STATIC_FORCEINLINE void SMPCC_DisableStreamReadCross4K(void)
{
    SMPCC->STM_CTRL.b.cross_en = SMPCC_STMCTRL_CROSS_EN_DISABLE;
}

/**
 * \brief  Set Non-Shareable Region to NACL
 * \details
 * This function configures a non-shareable region as NACL (NAPOT/CA with length) at the specified address.
 * \param [in]    region_id    ID of the region to configure
 * \param [in]    addr         Address for the region
 * \sa
 * - \ref SMPCC_SetNSRegionNAPOT
 * - \ref SMPCC_DisableNSRegion
*/
__STATIC_FORCEINLINE void SMPCC_SetNSRegionNACL(uint8_t region_id, uint64_t addr)
{
    SMPCC->NS_RG[region_id].dw = SMPCC_NS_RG_CFG_NACL | addr;
}

/**
 * \brief  Set Non-Shareable Region to NAPOT
 * \details
 * This function configures a non-shareable region as NAPOT (Naturally Aligned Power of Two) at the specified address.
 * \param [in]    region_id    ID of the region to configure
 * \param [in]    addr         Address for the region
 * \sa
 * - \ref SMPCC_SetNSRegionNACL
 * - \ref SMPCC_DisableNSRegion
*/
__STATIC_FORCEINLINE void SMPCC_SetNSRegionNAPOT(uint8_t region_id, uint64_t addr)
{
    SMPCC->NS_RG[region_id].dw = SMPCC_NS_RG_CFG_NAPOT | addr;
}

/**
 * \brief  Disable Non-Shareable Region
 * \details
 * This function disables a non-shareable region.
 * \param [in]    region_id    ID of the region to disable
 * \sa
 * - \ref SMPCC_SetNSRegionNAPOT
 * - \ref SMPCC_SetNSRegionNACL
*/
__STATIC_FORCEINLINE void SMPCC_DisableNSRegion(uint8_t region_id)
{
    SMPCC->NS_RG[region_id].dw = SMPCC_NS_RG_CFG_DISABLE;
}

/**
 * \brief  Set Performance Monitor Event Selection
 * \details
 * This function configures a performance monitor to select a specific event and client.
 * \param [in]    idx         Index of the performance monitor
 * \param [in]    client_id   ID of the client to monitor
 * \param [in]    event       Event to monitor
 * \sa
 * - \ref SMPCC_GetPMONEventSelect
 * - \ref SMPCC_GetPMONCount
 * - \ref SMPCC_ClearPMONCount
 *
*/
__STATIC_FORCEINLINE void SMPCC_SetPMONEventSelect(uint8_t idx, uint8_t client_id, uint8_t event)
{
    SMPCC->SMP_PMON_SEL[idx].w = SMPCC_PMON_EVENT(event, client_id);
}

/**
 * \brief  Get Performance Monitor Event Selection
 * \details
 * This function gets a performance monitor configuration value.
 * \param [in]    idx    Index of the performance monitor
 * \return              The performance monitor configuration value.
 * \sa
 * - \ref SMPCC_SetPMONEventSelect
 * - \ref SMPCC_GetPMONCount
 * - \ref SMPCC_ClearPMONCount
 *
*/
__STATIC_FORCEINLINE uint32_t SMPCC_GetPMONEventSelect(uint8_t idx)
{
    return SMPCC->SMP_PMON_SEL[idx].w;
}

/**
 * \brief  Get Performance Monitor Count
 * \details
 * This function returns the current count value of a performance monitor.
 * \param [in]    idx    Index of the performance monitor
 * \return             Current count value of the performance monitor
 * \sa
 * - \ref SMPCC_SetPMONEventSelect
 * - \ref SMPCC_GetPMONEventSelect
 * - \ref SMPCC_ClearPMONCount
*/
__STATIC_FORCEINLINE uint64_t SMPCC_GetPMONCount(uint8_t idx)
{
    return SMPCC->SMP_PMON_CNT[idx];
}

/**
 * \brief  Clear Performance Monitor Count
 * \details
 * This function clears the count value of a performance monitor.
 * \param [in]    idx    Index of the performance monitor to clear
 * \sa
 * - \ref SMPCC_SetPMONEventSelect
 * - \ref SMPCC_GetPMONEventSelect
 * - \ref SMPCC_GetPMONCount
*/
__STATIC_FORCEINLINE void SMPCC_ClearPMONCount(uint8_t idx)
{
    SMPCC->SMP_PMON_CNT[idx] = 0;
}

/**
 * \brief  Get Client Error Address
 * \details
 * This function returns the error address for a specific client.
 * \param [in]    client_id    ID of the client to get error address for
 * \return                   Error address of the specified client
*/
__STATIC_FORCEINLINE uint64_t SMPCC_GetClientErrAddr(uint8_t client_id)
{
    return SMPCC->CLIENT_ERR_ADDR[client_id];
}

/**
 * \brief  Mask Client Cluster Cache Ways
 * \details
 * This function masks specific ways in the cluster cache for a specific client.
 * \param [in]    client_id    ID of the client
 * \param [in]    way_msk      Way mask to apply
*/
__STATIC_FORCEINLINE void SMPCC_MaskClientCCacheWays(uint8_t client_id, uint32_t way_msk)
{
    SMPCC->CLIENT_WAY_MASK[client_id].w = way_msk;
}

/**
 * \brief   Check if ECC error injection mode is XOR mode
 * \details This function checks which ECC error injection mode is supported.
 *          Returns 1 if XOR mode is supported, 0 if direct write mode is supported.
 * \return  1 if XOR mode is supported, 0 if direct write mode is supported
 */
__STATIC_FORCEINLINE int32_t SMPCC_IsXorErrorInjectMode(void)
{
    return SMPCC->CC_ERR_INJ.b.inj_mode;
}

/**
 * \brief   Set ECC code for error injection
 * \details This function sets the ECC code to be used for error injection.
 * \param   ecc_code  ECC code to be set for error injection
 * \return  None
 */
__STATIC_FORCEINLINE void SMPCC_SetECCCode(uint32_t ecc_code)
{
    SMPCC->CC_ERR_INJ.w = (SMPCC->CC_ERR_INJ.w & ~SMPCC_ERR_INJ_INJECCCODE_Msk) |
                          _VAL2FLD(SMPCC_ERR_INJ_INJECCCODE, ecc_code);
}

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
/**
 * \brief   Inject ECC error to cluster cache tag RAM
 * \details This function injects an ECC error into the cluster cache tag RAM at the specified address.
 * \param   ecc_code  ECC code to be injected
 * \param   addr      Address where the error should be injected
 * \return  None
 */
__STATIC_FORCEINLINE void SMPCC_CCacheTramErrInject(uint32_t ecc_code, void *addr)
{
    SMPCC_SetECCCode(ecc_code);
    SMPCC_DisableCCacheECCCheck();
    MInvalICacheLine((unsigned long)addr);
    MFlushInvalDCacheCCacheLine((unsigned long)addr);
    __RWMB();
    SMPCC->CC_ERR_INJ.b.inj_tag = SMPCC_ERR_INJ_INJTAG_ENABLE;
    MLockCCacheLine((unsigned long)addr);
    SMPCC->CC_ERR_INJ.b.inj_tag = SMPCC_ERR_INJ_INJTAG_DISABLE;
    __RWMB();
    SMPCC_EnableCCacheECCCheck();
}

/**
 * \brief   Inject ECC error to cluster cache data RAM
 * \details This function injects an ECC error into the cluster cache data RAM at the specified address.
 * \param   ecc_code  ECC code to be injected
 * \param   addr      Address where the error should be injected
 * \return  None
 */
__STATIC_FORCEINLINE void SMPCC_CCacheDramErrInject(uint32_t ecc_code, void *addr)
{
    SMPCC_SetECCCode(ecc_code);
    SMPCC_DisableCCacheECCCheck();
    MInvalICacheLine((unsigned long)addr);
    MFlushInvalDCacheCCacheLine((unsigned long)addr);
    __RWMB();
    SMPCC->CC_ERR_INJ.b.inj_data = SMPCC_ERR_INJ_INJDATA_ENABLE;
    MLockCCacheLine((unsigned long)addr);
    SMPCC->CC_ERR_INJ.b.inj_data = SMPCC_ERR_INJ_INJDATA_DISABLE;
    __RWMB();
    SMPCC_EnableCCacheECCCheck();
}

/**
 * \brief   Inject ECC error to CLM (Cluster Local Memory)
 * \details This function injects an ECC error into the CLM at the specified address.
 * Only the ecc code can be injected, the data will keep as it is.
 * \param   ecc_code  ECC code to be injected
 * \param   addr      Address where the error should be injected
 * \return  None
 */
__STATIC_FORCEINLINE void SMPCC_CLMErrInject(uint32_t ecc_code, void *addr)
{
    SMPCC_SetECCCode(ecc_code);
    SMPCC_DisableCLMECCCheck();
    uint32_t val = __LW(addr);
    __RWMB();
    SMPCC->CC_ERR_INJ.b.inj_clm = SMPCC_ERR_INJ_INJCLM_ENABLE;
    __SW(addr, val);
    SMPCC->CC_ERR_INJ.b.inj_clm = SMPCC_ERR_INJ_INJCLM_DISABLE;
    __RWMB();
    SMPCC_EnableCLMECCCheck();
}

/**
 * \brief   Restore cluster cache after error injection
 * \details This function restores the cluster cache after an error injection operation.
 * \param   addr      Address to be restored
 * \return  None
 */
__STATIC_FORCEINLINE void SMPCC_CCacheErrRestore(void *addr)
{
    SMPCC_DisableCCacheECCCheck();
    MInvalICacheLine((unsigned long)addr);
    MFlushInvalDCacheCCacheLine((unsigned long)addr);
    MLockCCacheLine((unsigned long)addr);
    SMPCC_EnableCCacheECCCheck();
}
#endif /* #if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1) */

/** @} */ /* End of Doxygen Group NMSIS_Core_SMPCC_Functions */
#endif /* #if defined(__SMPCC_PRESENT) && (__SMPCC_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_SMPCC_H__ */
