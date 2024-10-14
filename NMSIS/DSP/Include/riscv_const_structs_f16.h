/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_const_structs_f16.h
 * Description:  Constant structs that are initialized for user convenience.
 *               For example, some can be given as arguments to the riscv_cfft_f16() function.
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

#ifndef RISCV_CONST_STRUCTS_F16_H
#define RISCV_CONST_STRUCTS_F16_H

#include "riscv_math_types_f16.h"
#include "riscv_common_tables.h"
#include "riscv_common_tables_f16.h"
#include "dsp/transform_functions_f16.h"

#ifdef   __cplusplus
extern "C"
{
#endif

#if defined(RISCV_FLOAT16_SUPPORTED)
   extern const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len16;
   extern const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len32;
   extern const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len64;
   extern const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len128;
   extern const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len256;
   extern const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len512;
   extern const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len1024;
   extern const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len2048;
   extern const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len4096;
#endif

#ifdef   __cplusplus
}
#endif

#endif
