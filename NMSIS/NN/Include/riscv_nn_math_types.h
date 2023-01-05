/******************************************************************************
 * @file     riscv_nn_math_types.h
 * @brief    Compiler include and basic types
 * @version  V1.1.0
 * @date     09 March 2022
 * Target Processor: RISC-V Cores
 ******************************************************************************/
/*
 * Copyright (c) 2010-2022 Arm Limited or its affiliates.
 * Copyright (c) 2022 Nuclei Limited. All rights reserved.
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

#ifndef _RISCV_NN_MATH_TYPES_H_

#define _RISCV_NN_MATH_TYPES_H_

/* DSP inlcude for enum riscv_status. */
#include "riscv_math_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Compiler specific diagnostic adjustment */
#if   defined(__GNUC__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wsign-conversion"
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wunused-parameter"

#elif defined(__TI_RISCV__)

#elif defined(__CSMC__)

#elif defined(__TASKING__)

#elif defined(_MSC_VER)

#else
#error Unknown compiler
#endif


/* Included for instrinsics definitions */
#if defined (_MSC_VER )
#include <stdint.h>
#define __STATIC_FORCEINLINE static __forceinline
#define __STATIC_INLINE static __inline
#define __ALIGNED(x) __declspec(align(x))

#elif defined (__GNUC_PYTHON__)
#include <stdint.h>
#define  __ALIGNED(x) __attribute__((aligned(x)))
#define __STATIC_FORCEINLINE static inline __attribute__((always_inline))
#define __STATIC_INLINE static inline

#else

#define __NMSIS_GENERIC
#if (defined (__riscv_dsp))
    #undef __DSP_PRESENT
    #define __DSP_PRESENT       1
    #undef __RISCV_FEATURE_DSP
    #define __RISCV_FEATURE_DSP 1
#endif
#if (defined (__riscv_vector))
    #undef __VECTOR_PRESENT
    #define __VECTOR_PRESENT       1
    #undef __RISCV_FEATURE_VECTOR
    #define __RISCV_FEATURE_VECTOR 1
#endif

#if (defined (__riscv_bitmanip))
    #define __BITMANIP_PRESENT      1
    #undef __RISCV_FEATURE_BITMANIP
    #define __RISCV_FEATURE_BITMANIP 1
#endif

/* Include intrinisc header files for rvb/rvp/rvv */
#undef __INC_INTRINSIC_API
#define __INC_INTRINSIC_API     1

#include "nmsis_core.h"
#undef __NMSIS_GENERIC

#endif

#include <float.h>
#include <limits.h>
#include <math.h>
#include <string.h>

/* evaluate RISCV DSP feature */
#if (defined(__RISCV_FEATURE_DSP) && (__RISCV_FEATURE_DSP == 1))
  #define RISCV_MATH_DSP                    1
#endif

/* evaluate RISCV Vector feature */
#if (defined(__riscv_vector))
  /* previous RISCV_VECTOR is replaced by RISCV_MATH_VECTOR  */
  #define RISCV_MATH_VECTOR                 1
#endif

/* evaluate RISCV Bitmanip feature */
#if (defined(__riscv_bitmanip))
  #define RISCV_MATH_BITMANIP               1
#endif

/* Compiler specific diagnostic adjustment */
#if   defined(__GNUC__)
#pragma GCC diagnostic pop

#elif defined(__TI_RISCV__)

#elif defined(__CSMC__)

#elif defined(__TASKING__)

#elif defined(_MSC_VER)

#else
#error Unknown compiler
#endif

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif

 /**
   * @brief 8-bit fractional data type in 1.7 format.
   */
  typedef int8_t q7_t;

  /**
   * @brief 16-bit fractional data type in 1.15 format.
   */
  typedef int16_t q15_t;

  /**
   * @brief 32-bit fractional data type in 1.31 format.
   */
  typedef int32_t q31_t;

  /**
   * @brief 64-bit fractional data type in 1.63 format.
   */
  typedef int64_t q63_t;

  /**
   * @brief 32-bit floating-point type definition.
   */
  typedef float float32_t;

  /**
   * @brief 64-bit floating-point type definition.
   */
  typedef double float64_t;

/**
 * @brief Add necessary typedefs
 */

#define NN_Q31_MAX ((q31_t)(0x7FFFFFFFL))
#define NN_Q15_MAX ((q15_t)(0x7FFF))
#define NN_Q7_MAX ((q7_t)(0x7F))
#define NN_Q31_MIN ((q31_t)(0x80000000L))
#define NN_Q15_MIN ((q15_t)(0x8000))
#define NN_Q7_MIN ((q7_t)(0x80))

/**
 * @brief Error status returned by some functions in the library.
 */

typedef enum
{
    RISCV_NMSIS_NN_SUCCESS = 0,        /**< No error */
    RISCV_NMSIS_NN_ARG_ERROR = -1,     /**< One or more arguments are incorrect */
    RISCV_NMSIS_NN_NO_IMPL_ERROR = -2, /**<  No implementation available */
} riscv_nmsis_nn_status;

#ifdef __cplusplus
}
#endif

#endif /*ifndef _RISCV_NN_MATH_TYPES_H_ */