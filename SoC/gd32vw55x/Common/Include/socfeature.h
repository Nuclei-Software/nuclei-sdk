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
 * @brief    Nuclei SoC Feature Configuration Header File for GD32VW55x SoC
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
 * GD32VW55x is a single cluster SoC, its ``mhartid`` CSR is exactly the
 * hart id without any cluster id field, so the default value ``8`` of
 * ``CFG_SOC_CLUSTERID_OFS`` (the cluster id bit offset in mhartid, which
 * is also the number of bits used to encode the local hart id inside one
 * cluster) is large enough and harmless here.
 *
 * This file is not suitable for GD32VW55x actually, but required by the
 * shared RTOS porting assembly code (eg. ``portasm.S`` in FreeRTOS, and
 * ``context.S``/``interrupt.S`` in ThreadX) which includes ``socfeature.h``
 * to get ``CFG_SOC_LOCAL_HARTID_MASK`` for extracting the local hart id from
 * the ``mhartid`` CSR.
 *
 * For the SoC feature of evalsoc, please check the evalsoc one.
 */
#ifndef __SOCFEATURE_H__
#define __SOCFEATURE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   Cluster id bit offset in mhartid CSR
 * \details GD32VW55x is a single cluster SoC, the mhartid CSR is exactly the
 *          hart id without cluster id field, so keep the default value 8 here.
 */
#ifndef CFG_SOC_CLUSTERID_OFS
#define CFG_SOC_CLUSTERID_OFS       8
#endif

// Mask to get the local hart id inside current cluster
#ifndef CFG_SOC_LOCAL_HARTID_MASK
#define CFG_SOC_LOCAL_HARTID_MASK      ((1 << CFG_SOC_CLUSTERID_OFS) - 1)
#endif
#ifdef __cplusplus
}
#endif

#endif /* __SOCFEATURE_H__ */
