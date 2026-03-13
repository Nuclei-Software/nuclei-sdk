/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_rvv_tables.h
 * Description:  common tables like fft twiddle factors, Bitreverse, reciprocal etc
 *               used for RVV implementation only
 *
 * @date     02 October 2024
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2024 ARM Limited or its affiliates. All rights reserved.
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

 #ifndef RISCV_RVV_TABLES_H
 #define RISCV_RVV_TABLES_H

#include "riscv_math_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif




#if defined(RISCV_MATH_VECTOR)


#define RISCV_RVV_TWIDDLES_16_F32_LEN 24
extern const uint32_t riscv_rvv_twdre_16_f32[RISCV_RVV_TWIDDLES_16_F32_LEN];
extern const uint32_t riscv_rvv_twdim_16_f32[RISCV_RVV_TWIDDLES_16_F32_LEN];
#define RISCV_RVV_TWIDDLES_32_F32_LEN 64
extern const uint32_t riscv_rvv_twdre_32_f32[RISCV_RVV_TWIDDLES_32_F32_LEN];
extern const uint32_t riscv_rvv_twdim_32_f32[RISCV_RVV_TWIDDLES_32_F32_LEN];
#define RISCV_RVV_TWIDDLES_64_F32_LEN 160
extern const uint32_t riscv_rvv_twdre_64_f32[RISCV_RVV_TWIDDLES_64_F32_LEN];
extern const uint32_t riscv_rvv_twdim_64_f32[RISCV_RVV_TWIDDLES_64_F32_LEN];
#define RISCV_RVV_TWIDDLES_128_F32_LEN 384
extern const uint32_t riscv_rvv_twdre_128_f32[RISCV_RVV_TWIDDLES_128_F32_LEN];
extern const uint32_t riscv_rvv_twdim_128_f32[RISCV_RVV_TWIDDLES_128_F32_LEN];
#define RISCV_RVV_TWIDDLES_256_F32_LEN 896
extern const uint32_t riscv_rvv_twdre_256_f32[RISCV_RVV_TWIDDLES_256_F32_LEN];
extern const uint32_t riscv_rvv_twdim_256_f32[RISCV_RVV_TWIDDLES_256_F32_LEN];
#define RISCV_RVV_TWIDDLES_512_F32_LEN 2048
extern const uint32_t riscv_rvv_twdre_512_f32[RISCV_RVV_TWIDDLES_512_F32_LEN];
extern const uint32_t riscv_rvv_twdim_512_f32[RISCV_RVV_TWIDDLES_512_F32_LEN];
#define RISCV_RVV_TWIDDLES_1024_F32_LEN 4608
extern const uint32_t riscv_rvv_twdre_1024_f32[RISCV_RVV_TWIDDLES_1024_F32_LEN];
extern const uint32_t riscv_rvv_twdim_1024_f32[RISCV_RVV_TWIDDLES_1024_F32_LEN];
#define RISCV_RVV_TWIDDLES_2048_F32_LEN 10240
extern const uint32_t riscv_rvv_twdre_2048_f32[RISCV_RVV_TWIDDLES_2048_F32_LEN];
extern const uint32_t riscv_rvv_twdim_2048_f32[RISCV_RVV_TWIDDLES_2048_F32_LEN];
#define RISCV_RVV_TWIDDLES_4096_F32_LEN 22528
extern const uint32_t riscv_rvv_twdre_4096_f32[RISCV_RVV_TWIDDLES_4096_F32_LEN];
extern const uint32_t riscv_rvv_twdim_4096_f32[RISCV_RVV_TWIDDLES_4096_F32_LEN];

#define RISCV_RVV_BITREV_16_F32_LEN 16
extern const uint16_t riscv_rvv_bitrev_16_f32[RISCV_RVV_BITREV_16_F32_LEN];
#define RISCV_RVV_BITREV_32_F32_LEN 32
extern const uint16_t riscv_rvv_bitrev_32_f32[RISCV_RVV_BITREV_32_F32_LEN];
#define RISCV_RVV_BITREV_64_F32_LEN 64
extern const uint16_t riscv_rvv_bitrev_64_f32[RISCV_RVV_BITREV_64_F32_LEN];
#define RISCV_RVV_BITREV_128_F32_LEN 128
extern const uint16_t riscv_rvv_bitrev_128_f32[RISCV_RVV_BITREV_128_F32_LEN];
#define RISCV_RVV_BITREV_256_F32_LEN 256
extern const uint16_t riscv_rvv_bitrev_256_f32[RISCV_RVV_BITREV_256_F32_LEN];
#define RISCV_RVV_BITREV_512_F32_LEN 512
extern const uint16_t riscv_rvv_bitrev_512_f32[RISCV_RVV_BITREV_512_F32_LEN];
#define RISCV_RVV_BITREV_1024_F32_LEN 1024
extern const uint16_t riscv_rvv_bitrev_1024_f32[RISCV_RVV_BITREV_1024_F32_LEN];
#define RISCV_RVV_BITREV_2048_F32_LEN 2048
extern const uint16_t riscv_rvv_bitrev_2048_f32[RISCV_RVV_BITREV_2048_F32_LEN];
#define RISCV_RVV_BITREV_4096_F32_LEN 4096
extern const uint16_t riscv_rvv_bitrev_4096_f32[RISCV_RVV_BITREV_4096_F32_LEN];

#define RISCV_RVV_RFFT_TWIDDLES_32_F32_LEN 16
extern const uint32_t riscv_rvv_rfft_twdre_32_f32[RISCV_RVV_RFFT_TWIDDLES_32_F32_LEN];
extern const uint32_t riscv_rvv_rfft_twdim_32_f32[RISCV_RVV_RFFT_TWIDDLES_32_F32_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_64_F32_LEN 32
extern const uint32_t riscv_rvv_rfft_twdre_64_f32[RISCV_RVV_RFFT_TWIDDLES_64_F32_LEN];
extern const uint32_t riscv_rvv_rfft_twdim_64_f32[RISCV_RVV_RFFT_TWIDDLES_64_F32_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_128_F32_LEN 64
extern const uint32_t riscv_rvv_rfft_twdre_128_f32[RISCV_RVV_RFFT_TWIDDLES_128_F32_LEN];
extern const uint32_t riscv_rvv_rfft_twdim_128_f32[RISCV_RVV_RFFT_TWIDDLES_128_F32_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_256_F32_LEN 128
extern const uint32_t riscv_rvv_rfft_twdre_256_f32[RISCV_RVV_RFFT_TWIDDLES_256_F32_LEN];
extern const uint32_t riscv_rvv_rfft_twdim_256_f32[RISCV_RVV_RFFT_TWIDDLES_256_F32_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_512_F32_LEN 256
extern const uint32_t riscv_rvv_rfft_twdre_512_f32[RISCV_RVV_RFFT_TWIDDLES_512_F32_LEN];
extern const uint32_t riscv_rvv_rfft_twdim_512_f32[RISCV_RVV_RFFT_TWIDDLES_512_F32_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_1024_F32_LEN 512
extern const uint32_t riscv_rvv_rfft_twdre_1024_f32[RISCV_RVV_RFFT_TWIDDLES_1024_F32_LEN];
extern const uint32_t riscv_rvv_rfft_twdim_1024_f32[RISCV_RVV_RFFT_TWIDDLES_1024_F32_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_2048_F32_LEN 1024
extern const uint32_t riscv_rvv_rfft_twdre_2048_f32[RISCV_RVV_RFFT_TWIDDLES_2048_F32_LEN];
extern const uint32_t riscv_rvv_rfft_twdim_2048_f32[RISCV_RVV_RFFT_TWIDDLES_2048_F32_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_4096_F32_LEN 2048
extern const uint32_t riscv_rvv_rfft_twdre_4096_f32[RISCV_RVV_RFFT_TWIDDLES_4096_F32_LEN];
extern const uint32_t riscv_rvv_rfft_twdim_4096_f32[RISCV_RVV_RFFT_TWIDDLES_4096_F32_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_8192_F32_LEN 4096
extern const uint32_t riscv_rvv_rfft_twdre_8192_f32[RISCV_RVV_RFFT_TWIDDLES_8192_F32_LEN];
extern const uint32_t riscv_rvv_rfft_twdim_8192_f32[RISCV_RVV_RFFT_TWIDDLES_8192_F32_LEN];

#endif /* #if defined(RISCV_MATH_VECTOR) */

#if defined(RISCV_MATH_VECTOR)

#define RISCV_RVV_TWIDDLES_16_Q15_LEN 24
extern const q15_t riscv_rvv_twdre_16_q15[RISCV_RVV_TWIDDLES_16_Q15_LEN];
extern const q15_t riscv_rvv_twdim_16_q15[RISCV_RVV_TWIDDLES_16_Q15_LEN];
#define RISCV_RVV_TWIDDLES_32_Q15_LEN 64
extern const q15_t riscv_rvv_twdre_32_q15[RISCV_RVV_TWIDDLES_32_Q15_LEN];
extern const q15_t riscv_rvv_twdim_32_q15[RISCV_RVV_TWIDDLES_32_Q15_LEN];
#define RISCV_RVV_TWIDDLES_64_Q15_LEN 160
extern const q15_t riscv_rvv_twdre_64_q15[RISCV_RVV_TWIDDLES_64_Q15_LEN];
extern const q15_t riscv_rvv_twdim_64_q15[RISCV_RVV_TWIDDLES_64_Q15_LEN];
#define RISCV_RVV_TWIDDLES_128_Q15_LEN 384
extern const q15_t riscv_rvv_twdre_128_q15[RISCV_RVV_TWIDDLES_128_Q15_LEN];
extern const q15_t riscv_rvv_twdim_128_q15[RISCV_RVV_TWIDDLES_128_Q15_LEN];
#define RISCV_RVV_TWIDDLES_256_Q15_LEN 896
extern const q15_t riscv_rvv_twdre_256_q15[RISCV_RVV_TWIDDLES_256_Q15_LEN];
extern const q15_t riscv_rvv_twdim_256_q15[RISCV_RVV_TWIDDLES_256_Q15_LEN];
#define RISCV_RVV_TWIDDLES_512_Q15_LEN 2048
extern const q15_t riscv_rvv_twdre_512_q15[RISCV_RVV_TWIDDLES_512_Q15_LEN];
extern const q15_t riscv_rvv_twdim_512_q15[RISCV_RVV_TWIDDLES_512_Q15_LEN];
#define RISCV_RVV_TWIDDLES_1024_Q15_LEN 4608
extern const q15_t riscv_rvv_twdre_1024_q15[RISCV_RVV_TWIDDLES_1024_Q15_LEN];
extern const q15_t riscv_rvv_twdim_1024_q15[RISCV_RVV_TWIDDLES_1024_Q15_LEN];
#define RISCV_RVV_TWIDDLES_2048_Q15_LEN 10240
extern const q15_t riscv_rvv_twdre_2048_q15[RISCV_RVV_TWIDDLES_2048_Q15_LEN];
extern const q15_t riscv_rvv_twdim_2048_q15[RISCV_RVV_TWIDDLES_2048_Q15_LEN];
#define RISCV_RVV_TWIDDLES_4096_Q15_LEN 22528
extern const q15_t riscv_rvv_twdre_4096_q15[RISCV_RVV_TWIDDLES_4096_Q15_LEN];
extern const q15_t riscv_rvv_twdim_4096_q15[RISCV_RVV_TWIDDLES_4096_Q15_LEN];

#define RISCV_RVV_BITREV_16_Q15_LEN 16
extern const uint16_t riscv_rvv_bitrev_16_q15[RISCV_RVV_BITREV_16_Q15_LEN];
#define RISCV_RVV_BITREV_32_Q15_LEN 32
extern const uint16_t riscv_rvv_bitrev_32_q15[RISCV_RVV_BITREV_32_Q15_LEN];
#define RISCV_RVV_BITREV_64_Q15_LEN 64
extern const uint16_t riscv_rvv_bitrev_64_q15[RISCV_RVV_BITREV_64_Q15_LEN];
#define RISCV_RVV_BITREV_128_Q15_LEN 128
extern const uint16_t riscv_rvv_bitrev_128_q15[RISCV_RVV_BITREV_128_Q15_LEN];
#define RISCV_RVV_BITREV_256_Q15_LEN 256
extern const uint16_t riscv_rvv_bitrev_256_q15[RISCV_RVV_BITREV_256_Q15_LEN];
#define RISCV_RVV_BITREV_512_Q15_LEN 512
extern const uint16_t riscv_rvv_bitrev_512_q15[RISCV_RVV_BITREV_512_Q15_LEN];
#define RISCV_RVV_BITREV_1024_Q15_LEN 1024
extern const uint16_t riscv_rvv_bitrev_1024_q15[RISCV_RVV_BITREV_1024_Q15_LEN];
#define RISCV_RVV_BITREV_2048_Q15_LEN 2048
extern const uint16_t riscv_rvv_bitrev_2048_q15[RISCV_RVV_BITREV_2048_Q15_LEN];
#define RISCV_RVV_BITREV_4096_Q15_LEN 4096
extern const uint16_t riscv_rvv_bitrev_4096_q15[RISCV_RVV_BITREV_4096_Q15_LEN];

#define RISCV_RVV_RFFT_TWIDDLES_32_Q15_LEN 16
extern const q15_t riscv_rvv_rfft_twdre_32_q15[RISCV_RVV_RFFT_TWIDDLES_32_Q15_LEN];
extern const q15_t riscv_rvv_rfft_twdim_32_q15[RISCV_RVV_RFFT_TWIDDLES_32_Q15_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_64_Q15_LEN 32
extern const q15_t riscv_rvv_rfft_twdre_64_q15[RISCV_RVV_RFFT_TWIDDLES_64_Q15_LEN];
extern const q15_t riscv_rvv_rfft_twdim_64_q15[RISCV_RVV_RFFT_TWIDDLES_64_Q15_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_128_Q15_LEN 64
extern const q15_t riscv_rvv_rfft_twdre_128_q15[RISCV_RVV_RFFT_TWIDDLES_128_Q15_LEN];
extern const q15_t riscv_rvv_rfft_twdim_128_q15[RISCV_RVV_RFFT_TWIDDLES_128_Q15_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_256_Q15_LEN 128
extern const q15_t riscv_rvv_rfft_twdre_256_q15[RISCV_RVV_RFFT_TWIDDLES_256_Q15_LEN];
extern const q15_t riscv_rvv_rfft_twdim_256_q15[RISCV_RVV_RFFT_TWIDDLES_256_Q15_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_512_Q15_LEN 256
extern const q15_t riscv_rvv_rfft_twdre_512_q15[RISCV_RVV_RFFT_TWIDDLES_512_Q15_LEN];
extern const q15_t riscv_rvv_rfft_twdim_512_q15[RISCV_RVV_RFFT_TWIDDLES_512_Q15_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_1024_Q15_LEN 512
extern const q15_t riscv_rvv_rfft_twdre_1024_q15[RISCV_RVV_RFFT_TWIDDLES_1024_Q15_LEN];
extern const q15_t riscv_rvv_rfft_twdim_1024_q15[RISCV_RVV_RFFT_TWIDDLES_1024_Q15_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_2048_Q15_LEN 1024
extern const q15_t riscv_rvv_rfft_twdre_2048_q15[RISCV_RVV_RFFT_TWIDDLES_2048_Q15_LEN];
extern const q15_t riscv_rvv_rfft_twdim_2048_q15[RISCV_RVV_RFFT_TWIDDLES_2048_Q15_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_4096_Q15_LEN 2048
extern const q15_t riscv_rvv_rfft_twdre_4096_q15[RISCV_RVV_RFFT_TWIDDLES_4096_Q15_LEN];
extern const q15_t riscv_rvv_rfft_twdim_4096_q15[RISCV_RVV_RFFT_TWIDDLES_4096_Q15_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_8192_Q15_LEN 4096
extern const q15_t riscv_rvv_rfft_twdre_8192_q15[RISCV_RVV_RFFT_TWIDDLES_8192_Q15_LEN];
extern const q15_t riscv_rvv_rfft_twdim_8192_q15[RISCV_RVV_RFFT_TWIDDLES_8192_Q15_LEN];

#define RISCV_RVV_TWIDDLES_16_Q31_LEN 24
extern const q31_t riscv_rvv_twdre_16_q31[RISCV_RVV_TWIDDLES_16_Q31_LEN];
extern const q31_t riscv_rvv_twdim_16_q31[RISCV_RVV_TWIDDLES_16_Q31_LEN];
#define RISCV_RVV_TWIDDLES_32_Q31_LEN 64
extern const q31_t riscv_rvv_twdre_32_q31[RISCV_RVV_TWIDDLES_32_Q31_LEN];
extern const q31_t riscv_rvv_twdim_32_q31[RISCV_RVV_TWIDDLES_32_Q31_LEN];
#define RISCV_RVV_TWIDDLES_64_Q31_LEN 160
extern const q31_t riscv_rvv_twdre_64_q31[RISCV_RVV_TWIDDLES_64_Q31_LEN];
extern const q31_t riscv_rvv_twdim_64_q31[RISCV_RVV_TWIDDLES_64_Q31_LEN];
#define RISCV_RVV_TWIDDLES_128_Q31_LEN 384
extern const q31_t riscv_rvv_twdre_128_q31[RISCV_RVV_TWIDDLES_128_Q31_LEN];
extern const q31_t riscv_rvv_twdim_128_q31[RISCV_RVV_TWIDDLES_128_Q31_LEN];
#define RISCV_RVV_TWIDDLES_256_Q31_LEN 896
extern const q31_t riscv_rvv_twdre_256_q31[RISCV_RVV_TWIDDLES_256_Q31_LEN];
extern const q31_t riscv_rvv_twdim_256_q31[RISCV_RVV_TWIDDLES_256_Q31_LEN];
#define RISCV_RVV_TWIDDLES_512_Q31_LEN 2048
extern const q31_t riscv_rvv_twdre_512_q31[RISCV_RVV_TWIDDLES_512_Q31_LEN];
extern const q31_t riscv_rvv_twdim_512_q31[RISCV_RVV_TWIDDLES_512_Q31_LEN];
#define RISCV_RVV_TWIDDLES_1024_Q31_LEN 4608
extern const q31_t riscv_rvv_twdre_1024_q31[RISCV_RVV_TWIDDLES_1024_Q31_LEN];
extern const q31_t riscv_rvv_twdim_1024_q31[RISCV_RVV_TWIDDLES_1024_Q31_LEN];
#define RISCV_RVV_TWIDDLES_2048_Q31_LEN 10240
extern const q31_t riscv_rvv_twdre_2048_q31[RISCV_RVV_TWIDDLES_2048_Q31_LEN];
extern const q31_t riscv_rvv_twdim_2048_q31[RISCV_RVV_TWIDDLES_2048_Q31_LEN];
#define RISCV_RVV_TWIDDLES_4096_Q31_LEN 22528
extern const q31_t riscv_rvv_twdre_4096_q31[RISCV_RVV_TWIDDLES_4096_Q31_LEN];
extern const q31_t riscv_rvv_twdim_4096_q31[RISCV_RVV_TWIDDLES_4096_Q31_LEN];

#define RISCV_RVV_BITREV_16_Q31_LEN 16
extern const uint16_t riscv_rvv_bitrev_16_q31[RISCV_RVV_BITREV_16_Q31_LEN];
#define RISCV_RVV_BITREV_32_Q31_LEN 32
extern const uint16_t riscv_rvv_bitrev_32_q31[RISCV_RVV_BITREV_32_Q31_LEN];
#define RISCV_RVV_BITREV_64_Q31_LEN 64
extern const uint16_t riscv_rvv_bitrev_64_q31[RISCV_RVV_BITREV_64_Q31_LEN];
#define RISCV_RVV_BITREV_128_Q31_LEN 128
extern const uint16_t riscv_rvv_bitrev_128_q31[RISCV_RVV_BITREV_128_Q31_LEN];
#define RISCV_RVV_BITREV_256_Q31_LEN 256
extern const uint16_t riscv_rvv_bitrev_256_q31[RISCV_RVV_BITREV_256_Q31_LEN];
#define RISCV_RVV_BITREV_512_Q31_LEN 512
extern const uint16_t riscv_rvv_bitrev_512_q31[RISCV_RVV_BITREV_512_Q31_LEN];
#define RISCV_RVV_BITREV_1024_Q31_LEN 1024
extern const uint16_t riscv_rvv_bitrev_1024_q31[RISCV_RVV_BITREV_1024_Q31_LEN];
#define RISCV_RVV_BITREV_2048_Q31_LEN 2048
extern const uint16_t riscv_rvv_bitrev_2048_q31[RISCV_RVV_BITREV_2048_Q31_LEN];
#define RISCV_RVV_BITREV_4096_Q31_LEN 4096
extern const uint16_t riscv_rvv_bitrev_4096_q31[RISCV_RVV_BITREV_4096_Q31_LEN];

#define RISCV_RVV_RFFT_TWIDDLES_32_Q31_LEN 16
extern const q31_t riscv_rvv_rfft_twdre_32_q31[RISCV_RVV_RFFT_TWIDDLES_32_Q31_LEN];
extern const q31_t riscv_rvv_rfft_twdim_32_q31[RISCV_RVV_RFFT_TWIDDLES_32_Q31_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_64_Q31_LEN 32
extern const q31_t riscv_rvv_rfft_twdre_64_q31[RISCV_RVV_RFFT_TWIDDLES_64_Q31_LEN];
extern const q31_t riscv_rvv_rfft_twdim_64_q31[RISCV_RVV_RFFT_TWIDDLES_64_Q31_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_128_Q31_LEN 64
extern const q31_t riscv_rvv_rfft_twdre_128_q31[RISCV_RVV_RFFT_TWIDDLES_128_Q31_LEN];
extern const q31_t riscv_rvv_rfft_twdim_128_q31[RISCV_RVV_RFFT_TWIDDLES_128_Q31_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_256_Q31_LEN 128
extern const q31_t riscv_rvv_rfft_twdre_256_q31[RISCV_RVV_RFFT_TWIDDLES_256_Q31_LEN];
extern const q31_t riscv_rvv_rfft_twdim_256_q31[RISCV_RVV_RFFT_TWIDDLES_256_Q31_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_512_Q31_LEN 256
extern const q31_t riscv_rvv_rfft_twdre_512_q31[RISCV_RVV_RFFT_TWIDDLES_512_Q31_LEN];
extern const q31_t riscv_rvv_rfft_twdim_512_q31[RISCV_RVV_RFFT_TWIDDLES_512_Q31_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_1024_Q31_LEN 512
extern const q31_t riscv_rvv_rfft_twdre_1024_q31[RISCV_RVV_RFFT_TWIDDLES_1024_Q31_LEN];
extern const q31_t riscv_rvv_rfft_twdim_1024_q31[RISCV_RVV_RFFT_TWIDDLES_1024_Q31_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_2048_Q31_LEN 1024
extern const q31_t riscv_rvv_rfft_twdre_2048_q31[RISCV_RVV_RFFT_TWIDDLES_2048_Q31_LEN];
extern const q31_t riscv_rvv_rfft_twdim_2048_q31[RISCV_RVV_RFFT_TWIDDLES_2048_Q31_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_4096_Q31_LEN 2048
extern const q31_t riscv_rvv_rfft_twdre_4096_q31[RISCV_RVV_RFFT_TWIDDLES_4096_Q31_LEN];
extern const q31_t riscv_rvv_rfft_twdim_4096_q31[RISCV_RVV_RFFT_TWIDDLES_4096_Q31_LEN];
#define RISCV_RVV_RFFT_TWIDDLES_8192_Q31_LEN 4096
extern const q31_t riscv_rvv_rfft_twdre_8192_q31[RISCV_RVV_RFFT_TWIDDLES_8192_Q31_LEN];
extern const q31_t riscv_rvv_rfft_twdim_8192_q31[RISCV_RVV_RFFT_TWIDDLES_8192_Q31_LEN];

#endif /* defined(RISCV_MATH_VECTOR) */



#ifdef   __cplusplus
}
#endif

#endif /*RISCV_RVV_TABLES_H*/

