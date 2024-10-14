/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_common_tables_f16.h
 * Description:  Extern declaration for common tables
 *
 * @version  V1.10.0
 * @date     08 July 2021
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2010-2021 ARM Limited or its affiliates. All rights reserved.
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

#ifndef RISCV_COMMON_TABLES_F16_H
#define RISCV_COMMON_TABLES_F16_H

#include "riscv_math_types_f16.h"

#ifdef   __cplusplus
extern "C"
{
#endif


  /* F16 */
  #if defined(RISCV_FLOAT16_SUPPORTED)
    extern const float16_t twiddleCoefF16_16[32];

    extern const float16_t twiddleCoefF16_32[64];

    extern const float16_t twiddleCoefF16_64[128];

    extern const float16_t twiddleCoefF16_128[256];

    extern const float16_t twiddleCoefF16_256[512];

    extern const float16_t twiddleCoefF16_512[1024];

    extern const float16_t twiddleCoefF16_1024[2048];

    extern const float16_t twiddleCoefF16_2048[4096];

    extern const float16_t twiddleCoefF16_4096[8192];
    #define twiddleCoefF16 twiddleCoefF16_4096

    extern const float16_t twiddleCoefF16_rfft_32[32];

    extern const float16_t twiddleCoefF16_rfft_64[64];

    extern const float16_t twiddleCoefF16_rfft_128[128];

    extern const float16_t twiddleCoefF16_rfft_256[256];

    extern const float16_t twiddleCoefF16_rfft_512[512];

    extern const float16_t twiddleCoefF16_rfft_1024[1024];

    extern const float16_t twiddleCoefF16_rfft_2048[2048];

    extern const float16_t twiddleCoefF16_rfft_4096[4096];

  #endif /* #if defined(RISCV_FLOAT16_SUPPORTED) */

#ifdef   __cplusplus
}
#endif

#endif /*  _RISCV_COMMON_TABLES_F16_H */
