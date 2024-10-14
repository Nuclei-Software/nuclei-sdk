/******************************************************************************
 * @file     matrix_functions.h
 * @brief    Public header file for NMSIS DSP Library
 * @version  V1.10.1
 * @date     10 August 2022
 * Target Processor: RISC-V cores
 ******************************************************************************/
/*
 * Copyright (c) 2010-2020 Arm Limited or its affiliates. All rights reserved.
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

 
#ifndef MATRIX_FUNCTIONS_H_
#define MATRIX_FUNCTIONS_H_

#include "riscv_math_types.h"
#include "riscv_math_memory.h"

#include "dsp/none.h"
#include "dsp/utils.h"

#ifdef   __cplusplus
extern "C"
{
#endif

/**
 * @defgroup groupMatrix Matrix Functions
 *
 * This set of functions provides basic matrix math operations.
 * The functions operate on matrix data structures.  For example,
 * the type
 * definition for the floating-point matrix structure is shown
 * below:
 * <pre>
 *     typedef struct
 *     {
 *       uint16_t numRows;     // number of rows of the matrix.
 *       uint16_t numCols;     // number of columns of the matrix.
 *       float32_t *pData;     // points to the data of the matrix.
 *     } riscv_matrix_instance_f32;
 * </pre>
 * There are similar definitions for Q15 and Q31 data types.
 *
 * The structure specifies the size of the matrix and then points to
 * an array of data.  The array is of size <code>numRows X numCols</code>
 * and the values are arranged in row order.  That is, the
 * matrix element (i, j) is stored at:
 * <pre>
 *     pData[i*numCols + j]
 * </pre>
 *
 * \par Init Functions
 * There is an associated initialization function for each type of matrix
 * data structure.
 * The initialization function sets the values of the internal structure fields.
 * Refer to \ref riscv_mat_init_f32(), \ref riscv_mat_init_q31() and \ref riscv_mat_init_q15()
 * for floating-point, Q31 and Q15 types,  respectively.
 *
 * \par
 * Use of the initialization function is optional. However, if initialization function is used
 * then the instance structure cannot be placed into a const data section.
 * To place the instance structure in a const data
 * section, manually initialize the data structure.  For example:
 * <pre>
 * <code>riscv_matrix_instance_f32 S = {nRows, nColumns, pData};</code>
 * <code>riscv_matrix_instance_q31 S = {nRows, nColumns, pData};</code>
 * <code>riscv_matrix_instance_q15 S = {nRows, nColumns, pData};</code>
 * </pre>
 * where <code>nRows</code> specifies the number of rows, <code>nColumns</code>
 * specifies the number of columns, and <code>pData</code> points to the
 * data array.
 *
 * \par Size Checking
 * By default all of the matrix functions perform size checking on the input and
 * output matrices. For example, the matrix addition function verifies that the
 * two input matrices and the output matrix all have the same number of rows and
 * columns. If the size check fails the functions return:
 * <pre>
 *     RISCV_MATH_SIZE_MISMATCH
 * </pre>
 * Otherwise the functions return
 * <pre>
 *     RISCV_MATH_SUCCESS
 * </pre>
 * There is some overhead associated with this matrix size checking.
 * The matrix size checking is enabled via the \#define
 * <pre>
 *     RISCV_MATH_MATRIX_CHECK
 * </pre>
 * within the library project settings.  By default this macro is defined
 * and size checking is enabled. By changing the project settings and
 * undefining this macro size checking is eliminated and the functions
 * run a bit faster. With size checking disabled the functions always
 * return <code>RISCV_MATH_SUCCESS</code>.
 */

  #define DEFAULT_HOUSEHOLDER_THRESHOLD_F64 (1.0e-16)
  #define DEFAULT_HOUSEHOLDER_THRESHOLD_F32 (1.0e-12f)

  /**
   * @brief Instance structure for the floating-point matrix structure.
   */
  typedef struct
  {
    uint16_t numRows;     /**< number of rows of the matrix.     */
    uint16_t numCols;     /**< number of columns of the matrix.  */
    float32_t *pData;     /**< points to the data of the matrix. */
  } riscv_matrix_instance_f32;
 
 /**
   * @brief Instance structure for the floating-point matrix structure.
   */
  typedef struct
  {
    uint16_t numRows;     /**< number of rows of the matrix.     */
    uint16_t numCols;     /**< number of columns of the matrix.  */
    float64_t *pData;     /**< points to the data of the matrix. */
  } riscv_matrix_instance_f64;

 /**
   * @brief Instance structure for the Q7 matrix structure.
   */
  typedef struct
  {
    uint16_t numRows;     /**< number of rows of the matrix.     */
    uint16_t numCols;     /**< number of columns of the matrix.  */
    q7_t *pData;         /**< points to the data of the matrix. */
  } riscv_matrix_instance_q7;

  /**
   * @brief Instance structure for the Q15 matrix structure.
   */
  typedef struct
  {
    uint16_t numRows;     /**< number of rows of the matrix.     */
    uint16_t numCols;     /**< number of columns of the matrix.  */
    q15_t *pData;         /**< points to the data of the matrix. */
  } riscv_matrix_instance_q15;

  /**
   * @brief Instance structure for the Q31 matrix structure.
   */
  typedef struct
  {
    uint16_t numRows;     /**< number of rows of the matrix.     */
    uint16_t numCols;     /**< number of columns of the matrix.  */
    q31_t *pData;         /**< points to the data of the matrix. */
  } riscv_matrix_instance_q31;

  /**
   * @brief Floating-point matrix addition.
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_add_f32(
  const riscv_matrix_instance_f32 * pSrcA,
  const riscv_matrix_instance_f32 * pSrcB,
        riscv_matrix_instance_f32 * pDst);

  /**
   * @brief Q15 matrix addition.
   * @param[in]   pSrcA  points to the first input matrix structure
   * @param[in]   pSrcB  points to the second input matrix structure
   * @param[out]  pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_add_q15(
  const riscv_matrix_instance_q15 * pSrcA,
  const riscv_matrix_instance_q15 * pSrcB,
        riscv_matrix_instance_q15 * pDst);

  /**
   * @brief Q31 matrix addition.
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_add_q31(
  const riscv_matrix_instance_q31 * pSrcA,
  const riscv_matrix_instance_q31 * pSrcB,
        riscv_matrix_instance_q31 * pDst);

  /**
   * @brief Floating-point, complex, matrix multiplication.
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_cmplx_mult_f32(
  const riscv_matrix_instance_f32 * pSrcA,
  const riscv_matrix_instance_f32 * pSrcB,
        riscv_matrix_instance_f32 * pDst);

  /**
   * @brief Q15, complex,  matrix multiplication.
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_cmplx_mult_q15(
  const riscv_matrix_instance_q15 * pSrcA,
  const riscv_matrix_instance_q15 * pSrcB,
        riscv_matrix_instance_q15 * pDst,
        q15_t * pScratch);

  /**
   * @brief Q31, complex, matrix multiplication.
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_cmplx_mult_q31(
  const riscv_matrix_instance_q31 * pSrcA,
  const riscv_matrix_instance_q31 * pSrcB,
        riscv_matrix_instance_q31 * pDst);

  /**
   * @brief Floating-point matrix transpose.
   * @param[in]  pSrc  points to the input matrix
   * @param[out] pDst  points to the output matrix
   * @return    The function returns either  <code>RISCV_MATH_SIZE_MISMATCH</code>
   * or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_trans_f32(
  const riscv_matrix_instance_f32 * pSrc,
        riscv_matrix_instance_f32 * pDst);

/**
   * @brief Floating-point matrix transpose.
   * @param[in]  pSrc  points to the input matrix
   * @param[out] pDst  points to the output matrix
   * @return    The function returns either  <code>RISCV_MATH_SIZE_MISMATCH</code>
   * or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_trans_f64(
  const riscv_matrix_instance_f64 * pSrc,
        riscv_matrix_instance_f64 * pDst);

  /**
   * @brief Floating-point complex matrix transpose.
   * @param[in]  pSrc  points to the input matrix
   * @param[out] pDst  points to the output matrix
   * @return    The function returns either  <code>RISCV_MATH_SIZE_MISMATCH</code>
   * or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_cmplx_trans_f32(
  const riscv_matrix_instance_f32 * pSrc,
  riscv_matrix_instance_f32 * pDst);


  /**
   * @brief Q15 matrix transpose.
   * @param[in]  pSrc  points to the input matrix
   * @param[out] pDst  points to the output matrix
   * @return    The function returns either  <code>RISCV_MATH_SIZE_MISMATCH</code>
   * or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_trans_q15(
  const riscv_matrix_instance_q15 * pSrc,
        riscv_matrix_instance_q15 * pDst);

  /**
   * @brief Q15 complex matrix transpose.
   * @param[in]  pSrc  points to the input matrix
   * @param[out] pDst  points to the output matrix
   * @return    The function returns either  <code>RISCV_MATH_SIZE_MISMATCH</code>
   * or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_cmplx_trans_q15(
  const riscv_matrix_instance_q15 * pSrc,
  riscv_matrix_instance_q15 * pDst);

  /**
   * @brief Q7 matrix transpose.
   * @param[in]  pSrc  points to the input matrix
   * @param[out] pDst  points to the output matrix
   * @return    The function returns either  <code>RISCV_MATH_SIZE_MISMATCH</code>
   * or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_trans_q7(
  const riscv_matrix_instance_q7 * pSrc,
        riscv_matrix_instance_q7 * pDst);

  /**
   * @brief Q31 matrix transpose.
   * @param[in]  pSrc  points to the input matrix
   * @param[out] pDst  points to the output matrix
   * @return    The function returns either  <code>RISCV_MATH_SIZE_MISMATCH</code>
   * or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_trans_q31(
  const riscv_matrix_instance_q31 * pSrc,
        riscv_matrix_instance_q31 * pDst);

  /**
   * @brief Q31 complex matrix transpose.
   * @param[in]  pSrc  points to the input matrix
   * @param[out] pDst  points to the output matrix
   * @return    The function returns either  <code>RISCV_MATH_SIZE_MISMATCH</code>
   * or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_cmplx_trans_q31(
  const riscv_matrix_instance_q31 * pSrc,
  riscv_matrix_instance_q31 * pDst);

  /**
   * @brief Floating-point matrix multiplication
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_mult_f32(
  const riscv_matrix_instance_f32 * pSrcA,
  const riscv_matrix_instance_f32 * pSrcB,
        riscv_matrix_instance_f32 * pDst);

  /**
   * @brief Floating-point matrix multiplication
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_mult_f64(
  const riscv_matrix_instance_f64 * pSrcA,
  const riscv_matrix_instance_f64 * pSrcB,
        riscv_matrix_instance_f64 * pDst);

  /**
   * @brief Floating-point matrix and vector multiplication
   * @param[in]  pSrcMat  points to the input matrix structure
   * @param[in]  pVec     points to vector
   * @param[out] pDst     points to output vector
   */
void riscv_mat_vec_mult_f32(
  const riscv_matrix_instance_f32 *pSrcMat, 
  const float32_t *pVec, 
  float32_t *pDst);

  /**
   * @brief Q7 matrix multiplication
   * @param[in]  pSrcA   points to the first input matrix structure
   * @param[in]  pSrcB   points to the second input matrix structure
   * @param[out] pDst    points to output matrix structure
   * @param[in]  pState  points to the array for storing intermediate results
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_mult_q7(
  const riscv_matrix_instance_q7 * pSrcA,
  const riscv_matrix_instance_q7 * pSrcB,
        riscv_matrix_instance_q7 * pDst,
        q7_t * pState);

  /**
   * @brief Q7 matrix and vector multiplication
   * @param[in]  pSrcMat  points to the input matrix structure
   * @param[in]  pVec     points to vector
   * @param[out] pDst     points to output vector
   */
void riscv_mat_vec_mult_q7(
  const riscv_matrix_instance_q7 *pSrcMat, 
  const q7_t *pVec, 
  q7_t *pDst);

  /**
   * @brief Q15 matrix multiplication
   * @param[in]  pSrcA   points to the first input matrix structure
   * @param[in]  pSrcB   points to the second input matrix structure
   * @param[out] pDst    points to output matrix structure
   * @param[in]  pState  points to the array for storing intermediate results
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_mult_q15(
  const riscv_matrix_instance_q15 * pSrcA,
  const riscv_matrix_instance_q15 * pSrcB,
        riscv_matrix_instance_q15 * pDst,
        q15_t * pState);

  /**
   * @brief Q15 matrix and vector multiplication
   * @param[in]  pSrcMat  points to the input matrix structure
   * @param[in]  pVec     points to vector
   * @param[out] pDst     points to output vector
   */
void riscv_mat_vec_mult_q15(
  const riscv_matrix_instance_q15 *pSrcMat, 
  const q15_t *pVec, 
  q15_t *pDst);

  /**
   * @brief Q15 matrix multiplication (fast variant) for RISC-V Core with DSP enabled
   * @param[in]  pSrcA   points to the first input matrix structure
   * @param[in]  pSrcB   points to the second input matrix structure
   * @param[out] pDst    points to output matrix structure
   * @param[in]  pState  points to the array for storing intermediate results
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_mult_fast_q15(
  const riscv_matrix_instance_q15 * pSrcA,
  const riscv_matrix_instance_q15 * pSrcB,
        riscv_matrix_instance_q15 * pDst,
        q15_t * pState);

  /**
   * @brief Q31 matrix multiplication
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_mult_q31(
  const riscv_matrix_instance_q31 * pSrcA,
  const riscv_matrix_instance_q31 * pSrcB,
        riscv_matrix_instance_q31 * pDst);

  /**
   * @brief Q31 matrix multiplication
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @param[in]  pState  points to the array for storing intermediate results
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_mult_opt_q31(
  const riscv_matrix_instance_q31 * pSrcA,
  const riscv_matrix_instance_q31 * pSrcB,
        riscv_matrix_instance_q31 * pDst,
        q31_t *pState);

  /**
   * @brief Q31 matrix and vector multiplication
   * @param[in]  pSrcMat  points to the input matrix structure
   * @param[in]  pVec     points to vector
   * @param[out] pDst     points to output vector
   */
void riscv_mat_vec_mult_q31(
  const riscv_matrix_instance_q31 *pSrcMat, 
  const q31_t *pVec, 
  q31_t *pDst);

  /**
   * @brief Q31 matrix multiplication (fast variant) for RISC-V Core with DSP enabled
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_mult_fast_q31(
  const riscv_matrix_instance_q31 * pSrcA,
  const riscv_matrix_instance_q31 * pSrcB,
        riscv_matrix_instance_q31 * pDst);

  /**
   * @brief Floating-point matrix subtraction
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_sub_f32(
  const riscv_matrix_instance_f32 * pSrcA,
  const riscv_matrix_instance_f32 * pSrcB,
        riscv_matrix_instance_f32 * pDst);

  /**
   * @brief Floating-point matrix subtraction
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_sub_f64(
  const riscv_matrix_instance_f64 * pSrcA,
  const riscv_matrix_instance_f64 * pSrcB,
        riscv_matrix_instance_f64 * pDst);

  /**
   * @brief Q15 matrix subtraction
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_sub_q15(
  const riscv_matrix_instance_q15 * pSrcA,
  const riscv_matrix_instance_q15 * pSrcB,
        riscv_matrix_instance_q15 * pDst);

  /**
   * @brief Q31 matrix subtraction
   * @param[in]  pSrcA  points to the first input matrix structure
   * @param[in]  pSrcB  points to the second input matrix structure
   * @param[out] pDst   points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_sub_q31(
  const riscv_matrix_instance_q31 * pSrcA,
  const riscv_matrix_instance_q31 * pSrcB,
        riscv_matrix_instance_q31 * pDst);

  /**
   * @brief Floating-point matrix scaling.
   * @param[in]  pSrc   points to the input matrix
   * @param[in]  scale  scale factor
   * @param[out] pDst   points to the output matrix
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_scale_f32(
  const riscv_matrix_instance_f32 * pSrc,
        float32_t scale,
        riscv_matrix_instance_f32 * pDst);

  /**
   * @brief Q15 matrix scaling.
   * @param[in]  pSrc        points to input matrix
   * @param[in]  scaleFract  fractional portion of the scale factor
   * @param[in]  shift       number of bits to shift the result by
   * @param[out] pDst        points to output matrix
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_scale_q15(
  const riscv_matrix_instance_q15 * pSrc,
        q15_t scaleFract,
        int32_t shift,
        riscv_matrix_instance_q15 * pDst);

  /**
   * @brief Q31 matrix scaling.
   * @param[in]  pSrc        points to input matrix
   * @param[in]  scaleFract  fractional portion of the scale factor
   * @param[in]  shift       number of bits to shift the result by
   * @param[out] pDst        points to output matrix structure
   * @return     The function returns either
   * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
   */
riscv_status riscv_mat_scale_q31(
  const riscv_matrix_instance_q31 * pSrc,
        q31_t scaleFract,
        int32_t shift,
        riscv_matrix_instance_q31 * pDst);

  /**
   * @brief  Q31 matrix initialization.
   * @param[in,out] S         points to an instance of the Q31-type matrix structure.
   * @param[in]     nRows     number of rows in the matrix.
   * @param[in]     nColumns  number of columns in the matrix.
   * @param[in]     pData     points to the matrix data array.
   */
void riscv_mat_init_q31(
        riscv_matrix_instance_q31 * S,
        uint16_t nRows,
        uint16_t nColumns,
        q31_t * pData);

  /**
   * @brief  Q15 matrix initialization.
   * @param[in,out] S         points to an instance of the Q15-type matrix structure.
   * @param[in]     nRows     number of rows in the matrix.
   * @param[in]     nColumns  number of columns in the matrix.
   * @param[in]     pData     points to the matrix data array.
   */
void riscv_mat_init_q15(
        riscv_matrix_instance_q15 * S,
        uint16_t nRows,
        uint16_t nColumns,
        q15_t * pData);

  /**
   * @brief  Q7 matrix initialization.
   * @param[in,out] S         points to an instance of the Q7-type matrix structure.
   * @param[in]     nRows     number of rows in the matrix.
   * @param[in]     nColumns  number of columns in the matrix.
   * @param[in]     pData     points to the matrix data array.
   */
void riscv_mat_init_q7(
        riscv_matrix_instance_q7 * S,
        uint16_t nRows,
        uint16_t nColumns,
        q7_t * pData);

  /**
   * @brief  Floating-point matrix initialization.
   * @param[in,out] S         points to an instance of the floating-point matrix structure.
   * @param[in]     nRows     number of rows in the matrix.
   * @param[in]     nColumns  number of columns in the matrix.
   * @param[in]     pData     points to the matrix data array.
   */
void riscv_mat_init_f32(
        riscv_matrix_instance_f32 * S,
        uint16_t nRows,
        uint16_t nColumns,
        float32_t * pData);

/**
 * @brief  Floating-point matrix initialization.
 * @param[in,out] S         points to an instance of the floating-point matrix structure.
 * @param[in]     nRows     number of rows in the matrix.
 * @param[in]     nColumns  number of columns in the matrix.
 * @param[in]     pData     points to the matrix data array.
 */
void riscv_mat_init_f64(
      riscv_matrix_instance_f64 * S,
      uint16_t nRows,
      uint16_t nColumns,
      float64_t * pData);


  /**
   * @brief Floating-point matrix inverse.
   * @param[in]  src   points to the instance of the input floating-point matrix structure.
   * @param[out] dst   points to the instance of the output floating-point matrix structure.
   * @return The function returns RISCV_MATH_SIZE_MISMATCH, if the dimensions do not match.
   * If the input matrix is singular (does not have an inverse), then the algorithm terminates and returns error status RISCV_MATH_SINGULAR.
   */
  riscv_status riscv_mat_inverse_f32(
  const riscv_matrix_instance_f32 * src,
  riscv_matrix_instance_f32 * dst);


  /**
   * @brief Floating-point matrix inverse.
   * @param[in]  src   points to the instance of the input floating-point matrix structure.
   * @param[out] dst   points to the instance of the output floating-point matrix structure.
   * @return The function returns RISCV_MATH_SIZE_MISMATCH, if the dimensions do not match.
   * If the input matrix is singular (does not have an inverse), then the algorithm terminates and returns error status RISCV_MATH_SINGULAR.
   */
  riscv_status riscv_mat_inverse_f64(
  const riscv_matrix_instance_f64 * src,
  riscv_matrix_instance_f64 * dst);

 /**
   * @brief Floating-point Cholesky decomposition of Symmetric Positive Definite Matrix.
   * @param[in]  src   points to the instance of the input floating-point matrix structure.
   * @param[out] dst   points to the instance of the output floating-point matrix structure.
   * @return The function returns RISCV_MATH_SIZE_MISMATCH, if the dimensions do not match.
   * If the input matrix does not have a decomposition, then the algorithm terminates and returns error status RISCV_MATH_DECOMPOSITION_FAILURE.
   * If the matrix is ill conditioned or only semi-definite, then it is better using the LDL^t decomposition.
   * The decomposition is returning a lower triangular matrix.
   */
  riscv_status riscv_mat_cholesky_f64(
  const riscv_matrix_instance_f64 * src,
  riscv_matrix_instance_f64 * dst);

 /**
   * @brief Floating-point Cholesky decomposition of Symmetric Positive Definite Matrix.
   * @param[in]  src   points to the instance of the input floating-point matrix structure.
   * @param[out] dst   points to the instance of the output floating-point matrix structure.
   * @return The function returns RISCV_MATH_SIZE_MISMATCH, if the dimensions do not match.
   * If the input matrix does not have a decomposition, then the algorithm terminates and returns error status RISCV_MATH_DECOMPOSITION_FAILURE.
   * If the matrix is ill conditioned or only semi-definite, then it is better using the LDL^t decomposition.
   * The decomposition is returning a lower triangular matrix.
   */
  riscv_status riscv_mat_cholesky_f32(
  const riscv_matrix_instance_f32 * src,
  riscv_matrix_instance_f32 * dst);

  /**
   * @brief Solve UT . X = A where UT is an upper triangular matrix
   * @param[in]  ut  The upper triangular matrix
   * @param[in]  a  The matrix a
   * @param[out] dst The solution X of UT . X = A
   * @return The function returns RISCV_MATH_SINGULAR, if the system can't be solved.
  */
  riscv_status riscv_mat_solve_upper_triangular_f32(
  const riscv_matrix_instance_f32 * ut,
  const riscv_matrix_instance_f32 * a,
  riscv_matrix_instance_f32 * dst);

 /**
   * @brief Solve LT . X = A where LT is a lower triangular matrix
   * @param[in]  lt  The lower triangular matrix
   * @param[in]  a  The matrix a
   * @param[out] dst The solution X of LT . X = A
   * @return The function returns RISCV_MATH_SINGULAR, if the system can't be solved.
   */
  riscv_status riscv_mat_solve_lower_triangular_f32(
  const riscv_matrix_instance_f32 * lt,
  const riscv_matrix_instance_f32 * a,
  riscv_matrix_instance_f32 * dst);


  /**
   * @brief Solve UT . X = A where UT is an upper triangular matrix
   * @param[in]  ut  The upper triangular matrix
   * @param[in]  a  The matrix a
   * @param[out] dst The solution X of UT . X = A
   * @return The function returns RISCV_MATH_SINGULAR, if the system can't be solved.
  */
  riscv_status riscv_mat_solve_upper_triangular_f64(
  const riscv_matrix_instance_f64 * ut,
  const riscv_matrix_instance_f64 * a,
  riscv_matrix_instance_f64 * dst);

 /**
   * @brief Solve LT . X = A where LT is a lower triangular matrix
   * @param[in]  lt  The lower triangular matrix
   * @param[in]  a  The matrix a
   * @param[out] dst The solution X of LT . X = A
   * @return The function returns RISCV_MATH_SINGULAR, if the system can't be solved.
   */
  riscv_status riscv_mat_solve_lower_triangular_f64(
  const riscv_matrix_instance_f64 * lt,
  const riscv_matrix_instance_f64 * a,
  riscv_matrix_instance_f64 * dst);


  /**
   * @brief Floating-point LDL decomposition of Symmetric Positive Semi-Definite Matrix.
   * @param[in]  src   points to the instance of the input floating-point matrix structure.
   * @param[out] l   points to the instance of the output floating-point triangular matrix structure.
   * @param[out] d   points to the instance of the output floating-point diagonal matrix structure.
   * @param[out] p   points to the instance of the output floating-point permutation vector.
   * @return The function returns RISCV_MATH_SIZE_MISMATCH, if the dimensions do not match.
   * If the input matrix does not have a decomposition, then the algorithm terminates and returns error status RISCV_MATH_DECOMPOSITION_FAILURE.
   * The decomposition is returning a lower triangular matrix.
   */
  riscv_status riscv_mat_ldlt_f32(
  const riscv_matrix_instance_f32 * src,
  riscv_matrix_instance_f32 * l,
  riscv_matrix_instance_f32 * d,
  uint16_t * pp);

 /**
   * @brief Floating-point LDL decomposition of Symmetric Positive Semi-Definite Matrix.
   * @param[in]  src   points to the instance of the input floating-point matrix structure.
   * @param[out] l   points to the instance of the output floating-point triangular matrix structure.
   * @param[out] d   points to the instance of the output floating-point diagonal matrix structure.
   * @param[out] p   points to the instance of the output floating-point permutation vector.
   * @return The function returns RISCV_MATH_SIZE_MISMATCH, if the dimensions do not match.
   * If the input matrix does not have a decomposition, then the algorithm terminates and returns error status RISCV_MATH_DECOMPOSITION_FAILURE.
   * The decomposition is returning a lower triangular matrix.
   */
  riscv_status riscv_mat_ldlt_f64(
  const riscv_matrix_instance_f64 * src,
  riscv_matrix_instance_f64 * l,
  riscv_matrix_instance_f64 * d,
  uint16_t * pp);

/**
  @brief         QR decomposition of a m x n floating point matrix with m >= n.
  @param[in]     pSrc      points to input matrix structure. The source matrix is modified by the function.
  @param[in]     threshold norm2 threshold.
  @param[out]    pOutR     points to output R matrix structure of dimension m x n
  @param[out]    pOutQ     points to output Q matrix structure of dimension m x m
  @param[out]    pOutTau   points to Householder scaling factors of dimension n
  @param[inout]  pTmpA     points to a temporary vector of dimension m.
  @param[inout]  pTmpB     points to a temporary vector of dimension n.
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed
                   - \ref RISCV_MATH_SINGULAR      : Input matrix is found to be singular (non-invertible)
 */

riscv_status riscv_mat_qr_f32(
    const riscv_matrix_instance_f32 * pSrc,
    const float32_t threshold,
    riscv_matrix_instance_f32 * pOutR,
    riscv_matrix_instance_f32 * pOutQ,
    float32_t * pOutTau,
    float32_t *pTmpA,
    float32_t *pTmpB
    );

/**
  @brief         QR decomposition of a m x n floating point matrix with m >= n.
  @param[in]     pSrc      points to input matrix structure. The source matrix is modified by the function.
  @param[in]     threshold norm2 threshold.
  @param[out]    pOutR     points to output R matrix structure of dimension m x n
  @param[out]    pOutQ     points to output Q matrix structure of dimension m x m
  @param[out]    pOutTau   points to Householder scaling factors of dimension n
  @param[inout]  pTmpA     points to a temporary vector of dimension m.
  @param[inout]  pTmpB     points to a temporary vector of dimension n.
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed
                   - \ref RISCV_MATH_SINGULAR      : Input matrix is found to be singular (non-invertible)
 */

riscv_status riscv_mat_qr_f64(
    const riscv_matrix_instance_f64 * pSrc,
    const float64_t threshold,
    riscv_matrix_instance_f64 * pOutR,
    riscv_matrix_instance_f64 * pOutQ,
    float64_t * pOutTau,
    float64_t *pTmpA,
    float64_t *pTmpB
    );

/**
  @brief         Householder transform of a floating point vector.
  @param[in]     pSrc        points to the input vector.
  @param[in]     threshold   norm2 threshold.
  @param[in]     blockSize   dimension of the vector space.
  @param[outQ]   pOut        points to the output vector.
  @return        beta        return the scaling factor beta
 */

float32_t riscv_householder_f32(
    const float32_t * pSrc,
    const float32_t threshold,
    uint32_t    blockSize,
    float32_t * pOut
    );

/**
  @brief         Householder transform of a double floating point vector.
  @param[in]     pSrc        points to the input vector.
  @param[in]     threshold   norm2 threshold.
  @param[in]     blockSize   dimension of the vector space.
  @param[outQ]   pOut        points to the output vector.
  @return        beta        return the scaling factor beta
 */

float64_t riscv_householder_f64(
    const float64_t * pSrc,
    const float64_t threshold,
    uint32_t    blockSize,
    float64_t * pOut
    );

#ifdef   __cplusplus
}
#endif

#endif /* ifndef _MATRIX_FUNCTIONS_H_ */
