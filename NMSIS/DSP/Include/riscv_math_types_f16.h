/******************************************************************************
 * @file     riscv_math_types_f16.h
 * @brief    Public header file for f16 function of the NMSIS DSP Library
 * @version  V1.10.0
 * @date     08 July 2021
 * Target Processor: RISC-V Cores
 ******************************************************************************/
/*
 * Copyright (c) 2010-2021 Arm Limited or its affiliates. All rights reserved.
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

#ifndef _RISCV_MATH_TYPES_F16_H
#define _RISCV_MATH_TYPES_F16_H

//#include "riscv_math_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif



/*

Check if the type __fp16 is available.
If it is not available, f16 version of the kernels
won't be built.

*/

#if (defined (__riscv_zfh))
  #undef RISCV_FLOAT16_SUPPORTED
  #define RISCV_FLOAT16_SUPPORTED 1
#endif




#if defined(RISCV_FLOAT16_SUPPORTED)

typedef _Float16 float16_t;

#define F16INFINITY ((float16_t)__builtin_inf())


#define F16_MAX   ((float16_t)__FLT16_MAX__)
#define F16_MIN   (-(float16_t)__FLT16_MAX__)

#define F16_ABSMAX   ((float16_t)__FLT16_MAX__)
#define F16_ABSMIN   ((float16_t)0.0f16)

#endif /* RISCV_FLOAT16_SUPPORTED*/

#ifdef   __cplusplus
}
#endif

#endif /* _RISCV_MATH_F16_H */
