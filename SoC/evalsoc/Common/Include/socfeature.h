/*
 * Copyright (c) 2026 Nuclei Limited. All rights reserved.
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
/******************************************************************************
 * @file     socfeature.h
 * @brief    Nuclei SoC Feature Configuration Header File for <Device> SoC
 * @version  V1.00
 * @date     07. Sep 2026
 ******************************************************************************/
/*!
 * @file     socfeature.h
 * @brief    SoC feature configuration macros for Nuclei SoC
 *
 * This header file is used to tell us the feature configuration of the
 * Nuclei SoC, it is a companion of ``cpufeature.h`` which is used to
 * tell the feature configuration of the Nuclei CPU.
 *
 * In a multiple cluster SoC system, the ``mhartid`` CSR layout is defined
 * by the SoC architecture as below::
 *
 *    mhartid[x:CFG_SOC_CLUSTERID_OFS] : cluster id of the hart
 *    mhartid[CFG_SOC_CLUSTERID_OFS-1:0] : local hart id inside the cluster
 *
 * where ``CFG_SOC_CLUSTERID_OFS`` is the bit offset of the cluster id
 * field, it is also the number of bits used to encode the local hart id.
 *
 * Note the supervisor mode ``shartid`` CSR (if present) has the same layout
 * as the ``mhartid`` CSR, so this macro also applies to the ``__get_*_s()``
 * NMSIS APIs which read the shartid CSR.
 *
 * For a SoC which has a single cluster, the local hart id is exactly the
 * ``mhartid`` value, and the cluster id field is not used, so this macro
 * should be set to the number of bits needed to encode the maximum local
 * hart number + 1, eg. if the SoC has up to 4 harts in the single cluster,
 * then this macro should be 2 (2 bits can encode 4 harts).
 *
 * The default value is 8 to be compatible with the previous hardcoded
 * ``mhartid bit[7:0]`` layout used by the nuclei subsystem reference
 * design, where ``mhartid[7:0]`` is the local hart id, and the upper
 * bits are used for the cluster id.
 */
#ifndef __SOCFEATURE_H__
#define __SOCFEATURE_H__

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: This header file is used to tell us the SoC level feature,
// while cpufeature.h is used to tell us the CPU level feature.
// It is provided by the SoC vendor together with its SoC support (eg. the evalsoc
// one is created by Nuclei, and it is the reference implementation: when you integrate
// your own SoC, please refer to this evalsoc socfeature.h and create your own one
// according to your real SoC architecture, and just replace it).

/**
 * \brief   Cluster id bit offset in mhartid CSR
 * \details The bit offset of the cluster id field in mhartid CSR,
 *          it is also the number of bits used to encode the local
 *          hart id inside one cluster, so the local hart id in a
 *          cluster is within range [0, 2^CFG_SOC_CLUSTERID_OFS).
 *          eg. when it is 8, then mhartid[7:0] is local hart id,
 *          and mhartid[31:8] is cluster id.
 *
 *          You can also override it via compiler option
 *          -DCFG_SOC_CLUSTERID_OFS=<value> when you know the real
 *          mhartid layout of your SoC.
 */
#ifndef CFG_SOC_CLUSTERID_OFS
#define CFG_SOC_CLUSTERID_OFS       8
#endif

// NOTE: Derived macros from CFG_SOC_CLUSTERID_OFS
// Mask to get the local hart id inside current cluster.
// Keep it <= 0x7FF (2047): the actual local hart id of the CPU is never that large, and the mask
// is used with the RISC-V `andi` instruction in the SMP startup assembly, which only accepts a
// 12-bit signed immediate (max +2047); a larger value would fail to assemble. It can be overridden
// via -DCFG_SOC_LOCAL_HARTID_MASK=<value> when needed (the #ifndef below keeps that override).
// Keep it a plain integer constant (no UL suffix) so that it can be used directly both in C code
// and in assembly code (eg. `andi reg, reg, CFG_SOC_LOCAL_HARTID_MASK`).
#ifndef CFG_SOC_LOCAL_HARTID_MASK
#define CFG_SOC_LOCAL_HARTID_MASK      (((1 << CFG_SOC_CLUSTERID_OFS) - 1) & 0x7FF)
#endif

// NOTE: The mhartid/shartid value can be split into the cluster id and the local hart id with
// this CFG_SOC_CLUSTERID_OFS. The related hart concepts and their NMSIS APIs are:
//   - full hart id     : __get_hart_id()          = the full mhartid (cluster id + local hart id)
//   - cluster id       : __get_cluster_id()       = mhartid >> CFG_SOC_CLUSTERID_OFS
//   - local hart id    : __get_local_hart_id()    = mhartid & CFG_SOC_LOCAL_HARTID_MASK,
//                          the value BOOT_HARTID (see <Device>.h) compares against
//   - hart index       : __get_hart_index()       = local hart id - __HARTID_OFFSET, used only to
//                          address the per-hart resources (SysTimer MSIP/compare, PLIC context,
//                          CIDU/SMPCC core id, per-core arrays), in most SoCs it equals the local
//                          hart id since __HARTID_OFFSET is 0
// See core_feature_base.h and the NMSIS API docs for the full explanation.
#ifdef __cplusplus
}
#endif

#endif /* __SOCFEATURE_H__ */
