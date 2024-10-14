/*
 * SPDX-FileCopyrightText: Copyright 2010-2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

/* ----------------------------------------------------------------------
 * Project:      NMSIS NN Library
 * Title:        riscv_nnsupportfunctions.h
 * Description:  Public header file of support functions for NMSIS NN Library
 *
 * $Date:        30 April 2024
 * $Revision:    V.22.0.0
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */

#ifndef RISCV_NNSUPPORTFUNCTIONS_H
#define RISCV_NNSUPPORTFUNCTIONS_H

#include "riscv_nn_math_types.h"
#include "riscv_nn_types.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USE_FAST_DW_CONV_S16_FUNCTION(dw_conv_params, filter_dims, input_dims)                                         \
    (dw_conv_params->ch_mult == 1 && dw_conv_params->dilation.w == 1 && dw_conv_params->dilation.h == 1 &&             \
     filter_dims->w * filter_dims->h < 512)

#define LEFT_SHIFT(_shift) (_shift > 0 ? _shift : 0)
#define RIGHT_SHIFT(_shift) (_shift > 0 ? 0 : -_shift)
#define MASK_IF_ZERO(x) (x) == 0 ? ~0 : 0
#define MASK_IF_NON_ZERO(x) (x) != 0 ? ~0 : 0
#define SELECT_USING_MASK(mask, a, b) ((mask) & (a)) ^ (~(mask) & (b))

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define CLAMP(x, h, l) MAX(MIN((x), (h)), (l))
#define REDUCE_MULTIPLIER(_mult) ((_mult < 0x7FFF0000) ? ((_mult + (1 << 15)) >> 16) : 0x7FFF)

/*
 * Use RVV may help if data length > RVV_OPT_THRESHOLD, otherwise use pure C version
 * RVV_OPT_THRESHOLD could be {0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F ...}
 */
#define RVV_OPT_THRESHOLD 0xF
// For input of int16 when number of columns are above this limit int64 accumulation is needed
// to not loose precision.
#define MAX_COL_COUNT (512)

/**
 * @brief definition to pack four 8 bit values.
 */
#define PACK_S8x4_32x1(v0, v1, v2, v3)                                                                                 \
    ((((int32_t)(v0) << 0) & (int32_t)0x000000FF) | (((int32_t)(v1) << 8) & (int32_t)0x0000FF00) |                     \
     (((int32_t)(v2) << 16) & (int32_t)0x00FF0000) | (((int32_t)(v3) << 24) & (int32_t)0xFF000000))

/**
 * @brief definition to pack two 16 bit values.
 */
#define PACK_Q15x2_32x1(v0, v1) (((int32_t)v0 & (int32_t)0xFFFF) | ((int32_t)v1 << 16))

/**
 * @defgroup groupSupport Private
 *
 * Internal Support functions. Not intended to be called direclty by a NMSIS-NN user.
 *
 */

/**
 * @defgroup genPrivTypes Structure Types
 * @ingroup groupSupport
 * @brief Data structure types used by private functions.
 * @{
 */

/**
 * @brief Union for SIMD access of q31/s16/s8 types
 */
union riscv_nnword
{
    int32_t word;
    /**< q31 type */
    int16_t half_words[2];
    /**< s16 type */
    int8_t bytes[4];
    /**< s8 type */
};

/**
 * @brief Union for data type long long
 */
struct riscv_nn_double
{
    uint32_t low;
    int32_t high;
};

union riscv_nn_long_long
{
    int64_t long_long;
    struct riscv_nn_double word;
};

#ifndef RISCV_MATH_DSP
  /**
   * @brief definition to pack two 16 bit values.
   */
  #define __NN_PKHBT(ARG1, ARG2, ARG3) ( (((int32_t)(ARG1) <<    0) & (int32_t)0x0000FFFF) | \
                                      (((int32_t)(ARG2) << ARG3) & (int32_t)0xFFFF0000)  )
  #define __NN_PKHTB(ARG1, ARG2, ARG3) ( (((int32_t)(ARG1) <<    0) & (int32_t)0xFFFF0000) | \
                                      (((int32_t)(ARG2) >> ARG3) & (int32_t)0x0000FFFF)  )

  /**
   * @brief Clips Q63 to Q31 values.
   */
  __STATIC_FORCEINLINE q31_t nn_clip_q63_to_q31(
  q63_t x)
  {
    return ((q31_t) (x >> 32) != ((q31_t) x >> 31)) ?
      ((0x7FFFFFFF ^ ((q31_t) (x >> 63)))) : (q31_t) x;
  }

  /*
   * @brief C custom defined QADD
   */
  __STATIC_FORCEINLINE int32_t __NN_QADD(
  int32_t x,
  int32_t y)
  {
    return ((int32_t)(nn_clip_q63_to_q31((q63_t)x + (q31_t)y)));
  }

  /*
   * @brief C custom defined QADD16
   */
  __STATIC_FORCEINLINE uint32_t __NN_QADD16(
  uint32_t x,
  uint32_t y)
  {
/*  q31_t r,     s;  without initialisation 'riscv_offset_q15 test' fails  but 'intrinsic' tests pass! */
    q31_t r = 0, s = 0;

    r = __SSAT(((((q31_t)x << 16) >> 16) + (((q31_t)y << 16) >> 16)), 16) & (int32_t)0x0000FFFF;
    s = __SSAT(((((q31_t)x      ) >> 16) + (((q31_t)y      ) >> 16)), 16) & (int32_t)0x0000FFFF;

    return ((uint32_t)((s << 16) | (r      )));
  }

  /*
   * @brief C custom defined SXTB16
   */
  __STATIC_FORCEINLINE uint32_t __NN_SXTB16(
  uint32_t x)
  {
    return ((uint32_t)(((((q31_t)x << 24) >> 24) & (q31_t)0x0000FFFF) |
                       ((((q31_t)x <<  8) >>  8) & (q31_t)0xFFFF0000)  ));
  }

#else

#define __NN_PKHBT __PKHBT
#define __NN_PKHTB __PKHTB
#define __NN_QADD __QADD
#define __NN_QADD16 __QADD16
#define __NN_SXTB16 __SXTB16

#endif

   /**
   * @brief definition to pack four 8 bit values.
   */
  #define __NN_PACKq7(v0,v1,v2,v3) ( (((int32_t)(v0) <<  0) & (int32_t)0x000000FF) | \
                                  (((int32_t)(v1) <<  8) & (int32_t)0x0000FF00) | \
                                  (((int32_t)(v2) << 16) & (int32_t)0x00FF0000) | \
                                  (((int32_t)(v3) << 24) & (int32_t)0xFF000000)  )

/**
 * @} // end group groupPrivTypes
 */

/**
 * @defgroup supportConversion Data Conversion
 *
 * Perform data type conversion in-between neural network operations
 *
 */

/**
 * @brief Converts the elements of the q7 vector to q15 vector without left-shift
 * @param[in]       *pSrc points to the q7 input vector
 * @param[out]      *pDst points to the q15 output vector
 * @param[in]       blockSize length of the input vector
 *
 */
void riscv_q7_to_q15_no_shift(const q7_t *pSrc, q15_t *pDst, uint32_t blockSize);

void riscv_q7_to_q7_no_shift(const q7_t * pSrc, q7_t * pDst, uint32_t blockSize);

/**
 * @brief Non-saturating addition of elements of a q7 vector
 * @param[in]       *input Pointer to the q7 input vector
 * @param[out]      *output Pointer to the q31 output variable.
 * @param[in]       block_size length of the input vector
 * \par Description:
 *
 * 2^24 samples can be added without saturating the result.
 *
 * The equation used for the conversion process is:
 *
 * <pre>
 *  sum = input[0] + input[1] + .. + input[block_size -1]
 * </pre>
 *
 * */
void riscv_nn_add_q7(const q7_t *input, q31_t *output, uint32_t block_size);

/**
 * @brief  Converts the elements of the s8 vector to reordered q15 vector without left-shift
 * @param[in]       *pSrc points to the s8 input vector
 * @param[out]      *pDst points to the s16 output vector
 * @param[in]       blockSize length of the input vector
 * @return none.
 *
 */
void riscv_q7_to_q15_reordered_no_shift(const q7_t *pSrc, q15_t *pDst, uint32_t blockSize);

void riscv_q7_to_q7_reordered_no_shift(const q7_t * pSrc, q7_t * pDst, uint32_t blockSize);

/**
 * @brief Converts the elements from a s8 vector to a s16 vector with an added offset
 * @param[in]    src        pointer to the s8 input vector
 * @param[out]   dst        pointer to the s16 output vector
 * @param[in]    block_size length of the input vector
 * @param[in]    offset     s16 offset to be added to each input vector element.
 *
 * \par Description:
 *
 * Output elements are ordered.
 * The equation used for the conversion process is:
 *
 * <pre>
 *  dst[n] = (int16_t) src[n] + offset;   0 <= n < block_size.
 * </pre>
 *
 */
void riscv_q7_to_q15_with_offset(const int8_t *src, int16_t *dst, int32_t block_size, int16_t offset);

#if defined(RISCV_MATH_DSP)
/**
 * @brief Converts the elements from a s8 vector to a s16 vector with an added offset
 * @param[in]    src        pointer to the s8 input vector
 * @param[out]   dst        pointer to the s16 output vector
 * @param[in]    block_size length of the input vector
 * @param[in]    offset     s16 offset to be added to each input vector element.
 *
 * \par Description:
 *
 * No additonal ordering is done with the result that output elements are not in order.
 * Instead of ABCD order will be ACBD.
 * Note this is for processors with DSP extension only.
 * The equation used for the conversion process is:
 *
 * <pre>
 *  dst[n - 0] = (int16_t) src[n - 0] + offset;   0 <= n < block_size.
 *  dst[n - 1] = (int16_t) src[n - 2] + offset;   0 <= n < block_size.
 *  dst[n - 2] = (int16_t) src[n - 1] + offset;   0 <= n < block_size.
 *  dst[n - 3] = (int16_t) src[n - 3] + offset;   0 <= n < block_size.
 * </pre>
 *
 */
void riscv_s8_to_s16_unordered_with_offset(const int8_t *src, int16_t *dst, int32_t block_size, int16_t offset);

#endif

/**
 * @brief Get the required buffer size for optimized s8 depthwise convolution
 *        function with constraint that in_channel equals out_channel.
 *        This is for processors with DSP extension.
 *        Refer to riscv_depthwise_conv_s8_opt_get_buffer_size() for function argument details.
 *
 * @note  Intended for compilation on Host. If compiling for an Riscv target, use
 *        riscv_depthwise_conv_s8_opt_get_buffer_size(). Note also this is a support function,
 *        so not recommended to call directly even on Host.
 *
 */
int32_t riscv_depthwise_conv_s8_opt_get_buffer_size_dsp(const nmsis_nn_dims *input_dims,
                                                      const nmsis_nn_dims *filter_dims);
/**
 * @brief Converts the elements from a s8 vector to a s16 vector with an added offset
 * @param[in]    src        pointer to the s8 input vector
 * @param[out]   dst        pointer to the s16 output vector
 * @param[in]       block_size length of the input vector
 * @param[in]       offset     offset to be added to each input vector element.
 * @return none.
 *
 * @details  This function does the q7 to q15 expansion with re-ordering of bytes. Re-ordering is a consequence of
 *           the sign extension intrinsic(DSP extension). The tail (i.e., last (N % 4) elements) retains its
 * original order.
 *
 */
void riscv_q7_to_q15_reordered_with_offset(const q7_t *src, q15_t *dst, uint32_t block_size, q15_t offset);

/**
 * @brief Converts the elements from a q7 vector and accumulate to a q15 vector
 * @param[in]    *src       points to the q7 input vector
 * @param[out]   *dst       points to the q15 output vector
 * @param[in]    block_size length of the input vector
 *
 * \par Description:
 *
 * The equation used for the conversion process is:
 *
 * <pre>
 *  dst[n] += (q15_t) src[n] ;   0 <= n < block_size.
 * </pre>
 *
 */
void riscv_nn_accumulate_q7_to_q15(q15_t *dst, const q7_t *src, uint32_t block_size);

/**
 * @brief Depthwise conv on an im2col buffer where the input channel equals output channel.
 * @param[in]    row     pointer to row
 * @param[in]    col     pointer to im2col buffer, always consists of 2 columns.
 * @param[in]    num_ch   number of channels
 * @param[in]    out_shift  pointer to per output channel requantization shift parameter.
 * @param[in]    out_mult   pointer to per output channel requantization multiplier parameter.
 * @param[in]    out_offset      output tensor offset.
 * @param[in]    activation_min   minimum value to clamp the output to. Range : int8
 * @param[in]    activation_max   maximum value to clamp the output to. Range : int8
 * @param[in]    kernel_size   number of elements in one column.
 * @param[in]    output_bias per output channel bias. Range : int32
 * @param[out]   out         pointer to output
 * @return     The function returns one of the two
 *              1. The incremented output pointer for a successful operation or
 *              2. NULL if implementation is not available.
 *
 * @details     Supported framework: TensorFlow Lite micro.
 */
int8_t *riscv_nn_depthwise_conv_s8_core(const int8_t *row,
                                      const int16_t *col,
                                      const uint16_t num_ch,
                                      const int32_t *out_shift,
                                      const int32_t *out_mult,
                                      const int32_t out_offset,
                                      const int32_t activation_min,
                                      const int32_t activation_max,
                                      const uint16_t kernel_size,
                                      const int32_t *const output_bias,
                                      int8_t *out);

/**
 * @brief General Matrix-multiplication function with per-channel requantization.
 * @param[in]       input_row    pointer to row operand
 * @param[in]       input_col    pointer to col operand
 * @param[in]       output_ch    number of rows of input_row
 * @param[in]       col_batches  number of column batches. Range: 1 to 4
 * @param[in]       output_shift  pointer to per output channel requantization shift parameter.
 * @param[in]       output_mult   pointer to per output channel requantization multiplier parameter.
 * @param[in]       out_offset    output tensor offset.
 * @param[in]       col_offset    input tensor(col) offset.
 * @param[in]       row_offset    kernel offset(row). Not used.
 * @param[in]       out_activation_min   minimum value to clamp the output to. Range : int8
 * @param[in]       out_activation_max   maximum value to clamp the output to. Range : int8
 * @param[in]       row_len       number of elements in each row
 * @param[in]       bias          per output channel bias. Range : int32
 * @param[in,out]   out           pointer to output
 * @return     The function returns one of the two
 *              1. The incremented output pointer for a successful operation or
 *              2. NULL if implementation is not available.
 *
 * @details   Supported framework: TensorFlow Lite
 */
int8_t *riscv_nn_mat_mult_s8(const int8_t *input_row,
                           const int8_t *input_col,
                           const uint16_t output_ch,
                           const uint16_t col_batches,
                           const int32_t *output_shift,
                           const int32_t *output_mult,
                           const int32_t out_offset,
                           const int32_t col_offset,
                           const int32_t row_offset,
                           const int16_t out_activation_min,
                           const int16_t out_activation_max,
                           const uint16_t row_len,
                           const int32_t *const bias,
                           int8_t *out);
/**
 * @brief Matrix-multiplication function for convolution with per-channel requantization for 16 bits convolution.
 * @param[in]       input_a     pointer to operand A
 * @param[in]       input_b     pointer to operand B, always consists of 2 vectors.
 * @param[in]       output_ch   number of rows of A
 * @param[in]       out_shift   pointer to per output channel requantization shift parameter.
 * @param[in]       out_mult    pointer to per output channel requantization multiplier parameter.
 * @param[in]       activation_min   minimum value to clamp the output to. Range : int16
 * @param[in]       activation_max   maximum value to clamp the output to. Range : int16
 * @param[in]       num_col_a   number of columns of A
 * @param[in]       bias_data   pointer to struct with bias vector. The length of this vector is equal to the number
 *                              of output columns (or RHS input rows). The vector can be int32 or int64 indicated by a
 *                              flag in the struct.
 * @param[in,out]   out_0       pointer to output
 * @return     The function returns one of the two
 *              1. The incremented output pointer for a successful operation or
 *              2. NULL if implementation is not available.
 *
 * @details   This function does the matrix multiplication of weight matrix for all output channels
 *            with 2 columns from im2col and produces two elements/output_channel. The outputs are
 *            clamped in the range provided by activation min and max.
 *            Supported framework: TensorFlow Lite micro.
 */
int16_t *riscv_nn_mat_mult_kernel_s16(const int8_t *input_a,
                                    const int16_t *input_b,
                                    const int32_t output_ch,
                                    const int32_t *out_shift,
                                    const int32_t *out_mult,
                                    const int32_t activation_min,
                                    const int32_t activation_max,
                                    const int32_t num_col_a,
                                    const nmsis_nn_bias_data *const bias_data,
                                    int16_t *out_0);

/**
 * @brief General Vector by Matrix multiplication with requantization and storage of result.
 * @param[in]       row_elements          number of row elements
 * @param[in]       skipped_row_elements  number of row elements skipped due to padding.
 *                                        row_elements + skipped_row_elements = (kernel_x * kernel_y) * input_ch
 * @param[in]       row_base_ref          pointer to row operand
 * @param[in]       col_base_ref          pointer to col operand
 * @param[out]      out_ch                Number of output channels
 * @param[in]       conv_params           Pointer to convolution parameters like offsets and activation values
 * @param[in]       quant_params          Pointer to per-channel quantization parameters
 * @param[in]       bias                  Pointer to optional per-channel bias
 * @param[out]      output                Pointer to output where int8 results are stored.
 * @return     The function performs matrix(row_base_ref) multiplication with vector(col_base_ref) and
 *             scaled result is stored in memory.
 *
 * @details Pseudo-code
 *      *output = 0
 *      sum_col = 0
 *      for (j = 0; j < out_ch; j++)
 *      for (i = 0; i < row_elements; i++)
 *          *output += row_base_ref[i] * col_base_ref[i]
 *          sum_col += col_base_ref[i]
 *      scale sum_col using quant_params and bias
 *      store result in 'output'
 *
 *
 */
riscv_nmsis_nn_status riscv_nn_mat_mul_core_1x_s8(int32_t row_elements,
                                              const int32_t skipped_row_elements,
                                              const int8_t *row_base_ref,
                                              const int8_t *col_base_ref,
                                              const int32_t out_ch,
                                              const nmsis_nn_conv_params *conv_params,
                                              const nmsis_nn_per_channel_quant_params *quant_params,
                                              const int32_t *bias,
                                              int8_t *output);

/**
 * @brief General Vector by Matrix multiplication with requantization, storage of result and int4 weights packed into an
 * int8 buffer.
 * @param[in]       row_elements          number of row elements
 * @param[in]       skipped_row_elements  number of row elements skipped due to padding.
 *                                        row_elements + skipped_row_elements = (kernel_x * kernel_y) * input_ch
 * @param[in]       row_base_ref          pointer to row operand
 * @param[in]       col_base_ref          pointer to col operand as packed int4
 * @param[out]      out_ch                Number of output channels
 * @param[in]       conv_params           Pointer to convolution parameters like offsets and activation values
 * @param[in]       quant_params          Pointer to per-channel quantization parameters
 * @param[in]       bias                  Pointer to optional per-channel bias
 * @param[out]      output                Pointer to output where int8 results are stored.
 * @return     The function performs matrix(row_base_ref) multiplication with vector(col_base_ref) and
 *             scaled result is stored in memory.
 *
 * @details Pseudo-code as int8 example. Int4 filter data will be unpacked.
 *      *output = 0
 *      sum_col = 0
 *      for (j = 0; j < out_ch; j++)
 *      for (i = 0; i < row_elements; i++)
 *          *output += row_base_ref[i] * col_base_ref[i]
 *          sum_col += col_base_ref[i]
 *      scale sum_col using quant_params and bias
 *      store result in 'output'
 *
 *
 */
riscv_nmsis_nn_status riscv_nn_mat_mul_core_1x_s4(int32_t row_elements,
                                              const int32_t skipped_row_elements,
                                              const int8_t *row_base_ref,
                                              const int8_t *col_base_ref,
                                              const int32_t out_ch,
                                              const nmsis_nn_conv_params *conv_params,
                                              const nmsis_nn_per_channel_quant_params *quant_params,
                                              const int32_t *bias,
                                              int8_t *output);

/**
 * @brief Matrix-multiplication with requantization & activation function for four rows and one column
 * @param[in]       row_elements  number of row elements
 * @param[in]       offset        offset between rows. Can be the same as row_elements.
 *                                For e.g, in a 1x1 conv scenario with stride as 1.
 * @param[in]       row_base      pointer to row operand
 * @param[in]       col_base      pointer to col operand
 * @param[in]       out_ch        Number of output channels
 * @param[in]       conv_params   Pointer to convolution parameters like offsets and activation values
 * @param[in]       quant_params  Pointer to per-channel quantization parameters
 * @param[in]       bias          Pointer to per-channel bias
 * @param[out]      output        Pointer to output where int8 results are stored.
 *
 * @return     The function returns the updated output pointer or NULL if implementation is not available.
 *
 * @details Compliant to TFLM int8 specification. MVE implementation only
 */
int8_t *riscv_nn_mat_mul_core_4x_s8(const int32_t row_elements,
                                  const int32_t offset,
                                  const int8_t *row_base,
                                  const int8_t *col_base,
                                  const int32_t out_ch,
                                  const nmsis_nn_conv_params *conv_params,
                                  const nmsis_nn_per_channel_quant_params *quant_params,
                                  const int32_t *bias,
                                  int8_t *output);

/**
 * @brief General Matrix-multiplication function with per-channel requantization.
 *        This function assumes:
 *        - LHS input matrix NOT transposed (nt)
 *        - RHS input matrix transposed (t)
 *        - RHS is int8 packed with 2x int4
 *        - LHS is int8
 *
 *  @note This operation also performs the broadcast bias addition before the requantization
 *
 * @param[in]  lhs                Pointer to the LHS input matrix
 * @param[in]  rhs                Pointer to the RHS input matrix
 * @param[in]  bias               Pointer to the bias vector. The length of this vector is equal to the number of
 *                                output columns (or RHS input rows)
 * @param[out] dst                Pointer to the output matrix with "m" rows and "n" columns
 * @param[in]  dst_multipliers    Pointer to the multipliers vector needed for the per-channel requantization.
 *                                The length of this vector is equal to the number of output columns (or RHS input
 *                                rows)
 * @param[in]  dst_shifts         Pointer to the shifts vector needed for the per-channel requantization. The length
 *                                of this vector is equal to the number of output columns (or RHS input rows)
 * @param[in]  lhs_rows           Number of LHS input rows
 * @param[in]  rhs_rows           Number of RHS input rows
 * @param[in]  rhs_cols           Number of LHS/RHS input columns
 * @param[in]  lhs_offset         Offset to be applied to the LHS input value
 * @param[in]  dst_offset         Offset to be applied the output result
 * @param[in]  activation_min     Minimum value to clamp down the output. Range : int8
 * @param[in]  activation_max     Maximum value to clamp up the output. Range : int8
 * @param[in]  lhs_cols_offset    Column offset between subsequent lhs_rows
 *
 * @return     The function returns <code>RISCV_NMSIS_NN_SUCCESS</code>
 *
 */
riscv_nmsis_nn_status riscv_nn_mat_mult_nt_t_s4(const int8_t *lhs,
                                            const int8_t *rhs,
                                            const int32_t *bias,
                                            int8_t *dst,
                                            const int32_t *dst_multipliers,
                                            const int32_t *dst_shifts,
                                            const int32_t lhs_rows,
                                            const int32_t rhs_rows,
                                            const int32_t rhs_cols,
                                            const int32_t lhs_offset,
                                            const int32_t dst_offset,
                                            const int32_t activation_min,
                                            const int32_t activation_max,
                                            const int32_t lhs_cols_offset);

/**
 * @brief General Matrix-multiplication function with per-channel requantization.
 *        This function assumes:
 *        - LHS input matrix NOT transposed (nt)
 *        - RHS input matrix transposed (t)
 *
 *  @note This operation also performs the broadcast bias addition before the requantization
 *
 * @param[in]  lhs                Pointer to the LHS input matrix
 * @param[in]  rhs                Pointer to the RHS input matrix
 * @param[in]  bias               Pointer to the bias vector. The length of this vector is equal to the number of
 *                                output columns (or RHS input rows)
 * @param[out] dst                Pointer to the output matrix with "m" rows and "n" columns
 * @param[in]  dst_multipliers    Pointer to the multipliers vector needed for the per-channel requantization.
 *                                The length of this vector is equal to the number of output columns (or RHS input
 *                                rows)
 * @param[in]  dst_shifts         Pointer to the shifts vector needed for the per-channel requantization. The length
 *                                of this vector is equal to the number of output columns (or RHS input rows)
 * @param[in]  lhs_rows           Number of LHS input rows
 * @param[in]  rhs_rows           Number of RHS input rows
 * @param[in]  rhs_cols           Number of LHS/RHS input columns
 * @param[in]  lhs_offset         Offset to be applied to the LHS input value
 * @param[in]  dst_offset         Offset to be applied the output result
 * @param[in]  activation_min     Minimum value to clamp down the output. Range : int8
 * @param[in]  activation_max     Maximum value to clamp up the output. Range : int8
 * @param[in]  row_address_offset Address offset between rows in output.
 * @param[in]  lhs_cols_offset    Column offset between subsequent lhs_rows
 *
 * @return     The function returns <code>RISCV_NMSIS_NN_SUCCESS</code>
 *
 */
riscv_nmsis_nn_status riscv_nn_mat_mult_nt_t_s8(const int8_t *lhs,
                                            const int8_t *rhs,
                                            const int32_t *bias,
                                            int8_t *dst,
                                            const int32_t *dst_multipliers,
                                            const int32_t *dst_shifts,
                                            const int32_t lhs_rows,
                                            const int32_t rhs_rows,
                                            const int32_t rhs_cols,
                                            const int32_t lhs_offset,
                                            const int32_t dst_offset,
                                            const int32_t activation_min,
                                            const int32_t activation_max,
                                            const int32_t row_address_offset,
                                            const int32_t lhs_cols_offset);

/**
 * @brief General Matrix-multiplication function with per-channel requantization and int16 input (LHS) and output.
 *        This function assumes:
 *        - LHS input matrix NOT transposed (nt)
 *        - RHS input matrix transposed (t)
 *
 *  @note This operation also performs the broadcast bias addition before the requantization
 *
 * @param[in]  lhs                Pointer to the LHS input matrix
 * @param[in]  rhs                Pointer to the RHS input matrix
 * @param[in]  bias_data          Pointer to struct with bias vector. The length of this vector is equal to the number
 *                                of output columns (or RHS input rows). The vector can be int32 or int64 indicated by a
 *                                flag in the struct.
 * @param[out] dst                Pointer to the output matrix with "m" rows and "n" columns
 * @param[in]  dst_multipliers    Pointer to the multipliers vector needed for the per-channel requantization.
 *                                The length of this vector is equal to the number of output columns (or RHS input
 *                                rows)
 * @param[in]  dst_shifts         Pointer to the shifts vector needed for the per-channel requantization. The length
 *                                of this vector is equal to the number of output columns (or RHS input rows)
 * @param[in]  lhs_rows           Number of LHS input rows
 * @param[in]  rhs_rows           Number of RHS input rows
 * @param[in]  rhs_cols           Number of LHS/RHS input columns
 * @param[in]  activation_min     Minimum value to clamp down the output. Range : int16
 * @param[in]  activation_max     Maximum value to clamp up the output. Range : int16
 *
 * @details MVE implementation only.
 *
 * @return     The function returns <code>RISCV_NMSIS_NN_SUCCESS</code> or
 *                                  <code>RISCV_NMSIS_NN_NO_IMPL_ERROR</code> if not for MVE
 *
 */
riscv_nmsis_nn_status riscv_nn_mat_mult_nt_t_s16(const int16_t *lhs,
                                             const int8_t *rhs,
                                             const nmsis_nn_bias_data *bias_data,
                                             int16_t *dst,
                                             const int32_t *dst_multipliers,
                                             const int32_t *dst_shifts,
                                             const int32_t lhs_rows,
                                             const int32_t rhs_rows,
                                             const int32_t rhs_cols,
                                             const int32_t activation_min,
                                             const int32_t activation_max);

/**
 * @brief General Matrix-multiplication function with int8 input and int32 output.
 *        This function assumes:
 *        - LHS input matrix NOT transposed (nt)
 *        - RHS input matrix transposed (t)
 *
 * @note  Dst/output buffer must be zeroed out before calling this function.
 *
 * @param[in]  lhs                Pointer to the LHS input matrix
 * @param[in]  rhs                Pointer to the RHS input matrix
 * @param[out] dst                Pointer to the output matrix with "m" rows and "n" columns
 * @param[in]  lhs_rows           Number of LHS input rows
 * @param[in]  rhs_rows           Number of LHS input columns/RHS input rows
 * @param[in]  rhs_cols           Number of RHS input columns
 * @param[in]  lhs_offset         Offset to be applied to the LHS input value
 * @param[in]  dst_idx_offset     Offset between subsequent output results
 *
 * @return     The function returns <code>RISCV_NMSIS_NN_SUCCESS</code>
 *
 */
riscv_nmsis_nn_status riscv_nn_mat_mult_nt_t_s8_s32(const int8_t *lhs,
                                                const int8_t *rhs,
                                                int32_t *dst,
                                                const int32_t lhs_rows,
                                                const int32_t rhs_rows,
                                                const int32_t rhs_cols,
                                                const int32_t lhs_offset,
                                                const int32_t dst_idx_offset);

/**
 * @brief s4 Vector by Matrix (transposed) multiplication
 *
 * @param[in]      lhs             Input left-hand side vector
 * @param[in]      packed_rhs      Input right-hand side matrix (transposed)
 * @param[in]      bias            Input bias
 * @param[out]     dst             Output vector
 * @param[in]      lhs_offset      Offset to be added to the input values of the left-hand side vector.
 *                                 Range: -127 to 128
 * @param[in]      dst_offset      Offset to be added to the output values. Range: -127 to 128
 * @param[in]      dst_multiplier  Output multiplier
 * @param[in]      dst_shift       Output shift
 * @param[in]      rhs_cols        Number of columns in the right-hand side input matrix
 * @param[in]      rhs_rows        Number of rows in the right-hand side input matrix
 * @param[in]      activation_min  Minimum value to clamp the output to. Range: int8
 * @param[in]      activation_max  Maximum value to clamp the output to. Range: int8
 *
 * @return         The function returns <code>RISCV_NMSIS_NN_SUCCESS</code>
 *
 */
riscv_nmsis_nn_status riscv_nn_vec_mat_mult_t_s4(const int8_t *lhs,
                                             const int8_t *packed_rhs,
                                             const int32_t *bias,
                                             int8_t *dst,
                                             const int32_t lhs_offset,
                                             const int32_t dst_offset,
                                             const int32_t dst_multiplier,
                                             const int32_t dst_shift,
                                             const int32_t rhs_cols,
                                             const int32_t rhs_rows,
                                             const int32_t activation_min,
                                             const int32_t activation_max);

/**
 * @brief s8 Vector by Matrix (transposed) multiplication
 *
 * @param[in]      lhs             Input left-hand side vector
 * @param[in]      rhs             Input right-hand side matrix (transposed)
 * @param[in]      kernel_sum      Kernel sums of the kernels (rhs). See riscv_vector_sum_s8 for more info.
 * @param[in]      bias            Input bias
 * @param[out]     dst             Output vector
 * @param[in]      lhs_offset      Offset to be added to the input values of the left-hand side vector.
 *                                 Range: -127 to 128
 * @param[in]      dst_offset      Offset to be added to the output values. Range: -127 to 128
 * @param[in]      dst_multiplier  Output multiplier
 * @param[in]      dst_shift       Output shift
 * @param[in]      rhs_cols        Number of columns in the right-hand side input matrix
 * @param[in]      rhs_rows        Number of rows in the right-hand side input matrix
 * @param[in]      activation_min  Minimum value to clamp the output to. Range: int8
 * @param[in]      activation_max  Maximum value to clamp the output to. Range: int8
 * @param[in]      address_offset  Memory position offset for dst. First output is stored at 'dst', the
 *                                 second at 'dst + address_offset' and so on. Default value is typically 1.
 * @param[in]      rhs_offset      Offset to be added to the input values of the right-hand side vector.
 *                                 Range: -127 to 128
 *
 * @return         The function returns <code>RISCV_NMSIS_NN_SUCCESS</code>
 *
 */
riscv_nmsis_nn_status riscv_nn_vec_mat_mult_t_s8(const int8_t *lhs,
                                             const int8_t *rhs,
                                             const int32_t *kernel_sum,
                                             const int32_t *bias,
                                             int8_t *dst,
                                             const int32_t lhs_offset,
                                             const int32_t dst_offset,
                                             const int32_t dst_multiplier,
                                             const int32_t dst_shift,
                                             const int32_t rhs_cols,
                                             const int32_t rhs_rows,
                                             const int32_t activation_min,
                                             const int32_t activation_max,
                                             const int32_t address_offset,
                                             const int32_t rhs_offset);

/**
 * @brief s16 Vector by Matrix (transposed) multiplication
 *
 * @param[in]      lhs             Input left-hand side vector
 * @param[in]      rhs             Input right-hand side matrix (transposed)
 * @param[in]      bias            Input bias
 * @param[out]     dst             Output vector
 * @param[in]      dst_multiplier  Output multiplier
 * @param[in]      dst_shift       Output shift
 * @param[in]      rhs_cols        Number of columns in the right-hand side input matrix
 * @param[in]      rhs_rows        Number of rows in the right-hand side input matrix
 * @param[in]      activation_min  Minimum value to clamp the output to. Range: int16
 * @param[in]      activation_max  Maximum value to clamp the output to. Range: int16
 *
 * @return         The function returns <code>RISCV_NMSIS_NN_SUCCESS</code>
 *
 */
riscv_nmsis_nn_status riscv_nn_vec_mat_mult_t_s16(const int16_t *lhs,
                                              const int8_t *rhs,
                                              const int64_t *bias,
                                              int16_t *dst,
                                              const int32_t dst_multiplier,
                                              const int32_t dst_shift,
                                              const int32_t rhs_cols,
                                              const int32_t rhs_rows,
                                              const int32_t activation_min,
                                              const int32_t activation_max);

/**
 * @brief s8 Vector by Matrix (transposed) multiplication with s16 output
 *
 * @param[in]      lhs             Input left-hand side vector
 * @param[in]      rhs             Input right-hand side matrix (transposed)
 * @param[out]     dst             Output vector
 * @param[in]      lhs_offset      Offset to be added to the input values of the left-hand side
 *                                 vector. Range: -127 to 128
 * @param[in]      scatter_offset  Address offset for dst. First output is stored at 'dst', the
 *                                 second at 'dst + scatter_offset' and so on.
 * @param[in]      dst_multiplier  Output multiplier
 * @param[in]      dst_shift       Output shift
 * @param[in]      rhs_cols        Number of columns in the right-hand side input matrix
 * @param[in]      rhs_rows        Number of rows in the right-hand side input matrix
 * @param[in]      activation_min  Minimum value to clamp the output to. Range: int16
 * @param[in]      activation_max  Maximum value to clamp the output to. Range: int16
 *
 * @return         The function returns <code>RISCV_NMSIS_NN_SUCCESS</code>
 *
 */
riscv_nmsis_nn_status riscv_nn_vec_mat_mult_t_svdf_s8(const int8_t *lhs,
                                                  const int8_t *rhs,
                                                  int16_t *dst,
                                                  const int32_t lhs_offset,
                                                  const int32_t scatter_offset,
                                                  const int32_t dst_multiplier,
                                                  const int32_t dst_shift,
                                                  const int32_t rhs_cols,
                                                  const int32_t rhs_rows,
                                                  const int32_t activation_min,
                                                  const int32_t activation_max);

/**
 * @brief Depthwise convolution of transposed rhs matrix with 4 lhs matrices. To be used in padded cases where
 *        the padding is -lhs_offset(Range: int8). Dimensions are the same for lhs and rhs.
 *
 * @param[in]      lhs             Input left-hand side matrix
 * @param[in]      rhs             Input right-hand side matrix (transposed)
 * @param[in]      lhs_offset      LHS matrix offset(input offset). Range: -127 to 128
 * @param[in]      active_ch       Subset of total_ch processed
 * @param[in]      total_ch        Number of channels in LHS/RHS
 * @param[in]      out_shift       Per channel output shift. Length of vector is equal to number of channels
 * @param[in]      out_mult        Per channel output multiplier. Length of vector is equal to number of channels
 * @param[in]      out_offset      Offset to be added to the output values. Range: -127 to 128
 * @param[in]      activation_min  Minimum value to clamp the output to. Range: int8
 * @param[in]      activation_max  Maximum value to clamp the output to. Range: int8
 * @param[in]       row_x_col       (row_dimension * col_dimension) of LHS/RHS matrix
 * @param[in]      output_bias     Per channel output bias. Length of vector is equal to number of channels
 * @param[in]      out             Output pointer
 *
 * @return         The function returns one of the two
 *                  - Updated output pointer if an implementation is available
 *                  - NULL if no implementation is available.
 *
 * @note           If number of channels is not a multiple of 4, upto 3 elements outside the boundary will be read
 * out for the following.
 *                  - Output shift
 *                  - Output multiplier
 *                  - Output bias
 *                  - rhs
 */
riscv_nmsis_nn_status riscv_nn_depthwise_conv_nt_t_padded_s8(const int8_t *lhs,
                                                         const int8_t *rhs,
                                                         const int32_t lhs_offset,
                                                         const int32_t active_ch,
                                                         const int32_t total_ch,
                                                         const int32_t *out_shift,
                                                         const int32_t *out_mult,
                                                         const int32_t out_offset,
                                                         const int32_t activation_min,
                                                         const int32_t activation_max,
                                                         const uint16_t row_x_col,
                                                         const int32_t *const output_bias,
                                                         int8_t *out);

/**
 * @brief Depthwise convolution of transposed rhs matrix with 4 lhs matrices. To be used in non-padded cases.
 *        Dimensions are the same for lhs and rhs.
 *
 * @param[in]      lhs             Input left-hand side matrix
 * @param[in]      rhs             Input right-hand side matrix (transposed)
 * @param[in]      lhs_offset      LHS matrix offset(input offset). Range: -127 to 128
 * @param[in]      active_ch       Subset of total_ch processed
 * @param[in]      total_ch        Number of channels in LHS/RHS
 * @param[in]      out_shift       Per channel output shift. Length of vector is equal to number of channels.
 * @param[in]      out_mult        Per channel output multiplier. Length of vector is equal to number of channels.
 * @param[in]      out_offset      Offset to be added to the output values. Range: -127 to 128
 * @param[in]      activation_min  Minimum value to clamp the output to. Range: int8
 * @param[in]      activation_max  Maximum value to clamp the output to. Range: int8
 * @param[in]       row_x_col       (row_dimension * col_dimension) of LHS/RHS matrix
 * @param[in]      output_bias     Per channel output bias. Length of vector is equal to number of channels.
 * @param[in]      out             Output pointer
 *
 * @return         The function returns one of the two
 *                  - Updated output pointer if an implementation is available
 *                  - NULL if no implementation is available.
 *
 * @note           If number of channels is not a multiple of 4, upto 3 elements outside the boundary will be read
 * out for the following.
 *                  - Output shift
 *                  - Output multiplier
 *                  - Output bias
 *                  - rhs
 */
riscv_nmsis_nn_status riscv_nn_depthwise_conv_nt_t_s8(const int8_t *lhs,
                                                  const int8_t *rhs,
                                                  const int32_t lhs_offset,
                                                  const int32_t active_ch,
                                                  const int32_t total_ch,
                                                  const int32_t *out_shift,
                                                  const int32_t *out_mult,
                                                  const int32_t out_offset,
                                                  const int32_t activation_min,
                                                  const int32_t activation_max,
                                                  const uint16_t row_x_col,
                                                  const int32_t *const output_bias,
                                                  int8_t *out);

/**
 * @brief Depthwise convolution of transposed rhs matrix with 4 lhs matrices. To be used in non-padded cases. rhs
 * consists of packed int4 data. Dimensions are the same for lhs and rhs.
 *
 * @param[in]      lhs             Input left-hand side matrix
 * @param[in]      rhs             Input right-hand side matrix (transposed). Consists of int4 data packed in an int8
 * buffer.
 * @param[in]      lhs_offset      LHS matrix offset(input offset). Range: -127 to 128
 * @param[in]      active_ch       Subset of total_ch processed
 * @param[in]      total_ch        Number of channels in LHS/RHS
 * @param[in]      out_shift       Per channel output shift. Length of vector is equal to number of channels.
 * @param[in]      out_mult        Per channel output multiplier. Length of vector is equal to number of channels.
 * @param[in]      out_offset      Offset to be added to the output values. Range: -127 to 128
 * @param[in]      activation_min  Minimum value to clamp the output to. Range: int8
 * @param[in]      activation_max  Maximum value to clamp the output to. Range: int8
 * @param[in]       row_x_col       (row_dimension * col_dimension) of LHS/RHS matrix
 * @param[in]      output_bias     Per channel output bias. Length of vector is equal to number of channels.
 * @param[in]      out             Output pointer
 *
 * @return         The function returns one of the two
 *                  - Updated output pointer if an implementation is available
 *                  - NULL if no implementation is available.
 *
 * @note           If number of channels is not a multiple of 4, upto 3 elements outside the boundary will be read
 * out for the following.
 *                  - Output shift
 *                  - Output multiplier
 *                  - Output bias
 *                  - rhs
 */
riscv_nmsis_nn_status riscv_nn_depthwise_conv_nt_t_s4(const int8_t *lhs,
                                                  const int8_t *rhs,
                                                  const int32_t lhs_offset,
                                                  const int32_t active_ch,
                                                  const int32_t total_ch,
                                                  const int32_t *out_shift,
                                                  const int32_t *out_mult,
                                                  const int32_t out_offset,
                                                  const int32_t activation_min,
                                                  const int32_t activation_max,
                                                  const uint16_t row_x_col,
                                                  const int32_t *const output_bias,
                                                  int8_t *out);

/**
 * @brief Depthwise convolution of transposed rhs matrix with 4 lhs matrices. To be used in non-padded cases.
 *        Dimensions are the same for lhs and rhs.
 *
 * @param[in]      lhs             Input left-hand side matrix
 * @param[in]      rhs             Input right-hand side matrix (transposed)
 * @param[in]      num_ch          Number of channels in LHS/RHS
 * @param[in]      out_shift       Per channel output shift. Length of vector is equal to number of channels.
 * @param[in]      out_mult        Per channel output multiplier. Length of vector is equal to number of channels.
 * @param[in]      activation_min  Minimum value to clamp the output to. Range: int8
 * @param[in]      activation_max  Maximum value to clamp the output to. Range: int8
 * @param[in]       row_x_col       (row_dimension * col_dimension) of LHS/RHS matrix
 * @param[in]      output_bias     Per channel output bias. Length of vector is equal to number of channels.
 * @param[in]      out             Output pointer
 *
 * @return         The function returns one of the two
 *                  - Updated output pointer if an implementation is available
 *                  - NULL if no implementation is available.
 *
 * @note           If number of channels is not a multiple of 4, upto 3 elements outside the boundary will be read
 * out for the following.
 *                  - Output shift
 *                  - Output multiplier
 *                  - Output bias
 *                  - rhs
 */
int16_t *riscv_nn_depthwise_conv_nt_t_s16(const int16_t *lhs,
                                        const int8_t *rhs,
                                        const uint16_t num_ch,
                                        const int32_t *out_shift,
                                        const int32_t *out_mult,
                                        const int32_t activation_min,
                                        const int32_t activation_max,
                                        const uint16_t row_x_col,
                                        const int64_t *const output_bias,
                                        int16_t *out);

/**
 *@brief Matrix-multiplication function for convolution with reordered columns
 *@param[in]       pA          pointer to operand A
 *@param[in]       pInBuffer   pointer to operand B, always conssists of 2 vectors
 *@param[in]       ch_im_out   numRow of A
 *@param[in]       numCol_A    numCol of A
 *@param[in]       bias_shift  amount of left-shift for bias
 *@param[in]       out_shift   amount of right-shift for output
 *@param[in]       bias        the bias
 *@param[in,out]   pOut        pointer to output
 *@return     The function returns the incremented output pointer
 *
 *@details  This function assumes that data in pInBuffer are reordered
 */
q7_t *riscv_nn_mat_mult_kernel_q7_q15_reordered(const q7_t *pA,
                                              const q15_t *pInBuffer,
                                              const uint16_t ch_im_out,
                                              const uint16_t numCol_A,
                                              const uint16_t bias_shift,
                                              const uint16_t out_shift,
                                              const q7_t *bias,
                                              q7_t *pOut);

q7_t *riscv_nn_mat_mult_kernel_q7_reordered(const q7_t * pA,
                                              const q7_t * pInBuffer,
                                              const uint16_t ch_im_out,
                                              const uint16_t numCol_A,
                                              const uint16_t bias_shift,
                                              const uint16_t out_shift,
                                              const q7_t * bias,
                                              q7_t * pOut);


#define __SIMD32_TYPE int32_t

#define __SIMD32(addr)        (*(__SIMD32_TYPE **) & (addr))
#define __SIMD32_CONST(addr)  ( (__SIMD32_TYPE * )   (addr))
#define _SIMD32_OFFSET(addr)  (*(__SIMD32_TYPE * )   (addr))
#define __SIMD64(addr)        (*(      int64_t **) & (addr))

/**
  @brief         Read 2 s16 elements and post increment pointer.
  @param[in]     in_q15   Pointer to pointer that holds address of input.
  @return        q31 value
 */
__STATIC_FORCEINLINE int32_t riscv_nn_read_q15x2_ia(const int16_t **in_q15)
{
    int32_t val;

#ifdef __RISCV_FEATURE_UNALIGNED
    memcpy(&val, *in_q15, 4);
#else
    __ASM volatile ("lw %0, 0(%1)" : "=r" (val) : "r" (*in_q15));
#endif
    *in_q15 += 2;

    return (val);
}

/**
  @brief         Read 4 s8 from s8 pointer and post increment pointer.
  @param[in]     in_s8       Pointer to pointer that holds address of input.
  @return        q31 value
 */
__STATIC_FORCEINLINE int32_t riscv_nn_read_s8x4_ia(const int8_t **in_s8)
{
    int32_t val;
#ifdef __RISCV_FEATURE_UNALIGNED
  memcpy (&val, *in_s8, 4);
#else
  val = __LW((int8_t *)(* in_s8));
#endif
  *in_s8 += 4;

  return (val);
}

/**
  @brief         Read 2 s8 from s8 pointer and post increment pointer.
  @param[in]     in_s8    Pointer to pointer that holds address of input.
  @return        q31      value
 */
__STATIC_FORCEINLINE int32_t riscv_nn_read_s8x2_ia(const int8_t **in_s8)
{
    int32_t val;
    memcpy(&val, *in_s8, 2);
    *in_s8 += 2;

    return (val);
}

__STATIC_FORCEINLINE int64_t riscv_nn_read_s8x8_ia(const int8_t **in_s8)
{
    int64_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD((int8_t *)(*in_s8));
#else
  val = *((int64_t *)(*in_s8));
#endif /* __RISCV_XLEN == 64 */
#else
  memcpy(&val, *in_s8, 8);
#endif
    *in_s8 += 8;

    return (val);
}
/**
  @brief         Read 2 int16 values from int16 pointer.
  @param[in]     in     pointer to address of input.
  @return        s32    value
 */
__STATIC_FORCEINLINE int32_t riscv_nn_read_s16x2(const int16_t *in)
{
  int32_t val;
#ifdef __RISCV_FEATURE_UNALIGNED
  memcpy (&val, in, 4);
#else
  val = __LW((int16_t *)in);
#endif

  return (val);
}

/**
  @brief         Read 2 int16 values from int16 pointer and increment pointer afterwards.
  @param[in]     in      points to input value
  @return        int64  value
 */
__STATIC_FORCEINLINE int32_t riscv_nn_read_s16x2_ia(const int16_t ** in)
{
  int64_t val;

  val = riscv_nn_read_s16x2(*in);
  *in += 2;

  return (val);
}

/**
  @brief         Write 2 int16 values to int16 pointer.
  @param[in]     in      points to input value
  @param[in]     value   int32 value
  @return        none
 */
__STATIC_FORCEINLINE void riscv_nn_write_s16x2(int16_t * in, int32_t value)
{
#ifdef __RISCV_FEATURE_UNALIGNED
  memcpy (in, &value, 4);
#else
  __SW(in, value);
#endif
}

/**
  @brief         Write 2 int16 values to int16 pointer and increment pointer afterwards.
  @param[in]     in      points to input value
  @param[in]     value   int32 value
  @return        none
 */
__STATIC_FORCEINLINE void riscv_nn_write_s16x2_ia(int16_t ** in, int32_t value)
{
  riscv_nn_write_s16x2(*in, value);
  *in += 2;
}

/**
  @brief         Read 4 int16 values from int16 pointer.
  @param[in]     in     pointer to address of input.
  @return        s32    value
 */
__STATIC_FORCEINLINE int64_t riscv_nn_read_s16x4(const int16_t *in)
{
  int64_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD((int16_t *)in);
#else
  val = *((int64_t *)in);
#endif /* __RISCV_XLEN == 64 */
#else
  memcpy((void *)(&val), (void *)(in), 8);
#endif
  return (val);
}

/**
  @brief         Read 4 int16 values from int16 pointer and increment pointer afterwards.
  @param[in]     in      points to input value
  @return        S64 value
 */
__STATIC_FORCEINLINE int64_t riscv_nn_read_s16x4_ia(const int16_t ** in)
{
  int64_t val;

  val = riscv_nn_read_s16x4(*in);
  *in += 4;

  return (val);
}

/**
  @brief         Write 4 int16 values to int16 pointer.
  @param[in]     in      points to input value
  @param[in]     value     S64 value
  @return        none
 */
__STATIC_FORCEINLINE void riscv_nn_write_s16x4(int16_t * in, int64_t value)
{
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  __SD(in, value);
#else
  *((int64_t *)in) = value;
#endif
#else
  memcpy((void *)(in), (void *)(&value), 8);
#endif
}

/**
  @brief         Write 4 int16 values to int16 pointer and increment pointer afterwards.
  @param[in]     in      points to input value
  @param[in]     value     int64_t value
  @return        none
 */
__STATIC_FORCEINLINE void riscv_nn_write_s16x4_ia(int16_t ** in, int64_t value)
{
  riscv_nn_write_s16x4(*in, value);
  *in += 4;
}

/**
  @brief         Read 4 s8 values.
  @param[in]     in_s8       pointer to address of input.
  @return        s32 value
 */
__STATIC_FORCEINLINE int32_t riscv_nn_read_s8x4(const int8_t *in_s8)
{
    int32_t val;
#ifdef __RISCV_FEATURE_UNALIGNED
    memcpy(&val, in_s8, 4);
#else
    val = __LW((int8_t *)(in_s8));
#endif

    return (val);
}

/**
  @brief         Read 2 s8 values.
  @param[in]     in_s8    pointer to address of input.
  @return        s32      value
 */
__STATIC_FORCEINLINE int32_t riscv_nn_read_s8x2(const int8_t *in_s8)
{
    int32_t val;
    memcpy(&val, in_s8, 2);

    return (val);
}

/**
  @brief         Write four s8 to s8 pointer and increment pointer afterwards.
  @param[in]     in       Double pointer to input value
  @param[in]     value    Four bytes to copy
 */
__STATIC_FORCEINLINE void riscv_nn_write_s8x4_ia(int8_t **in, int32_t value)
{
#ifdef __RISCV_FEATURE_UNALIGNED
    memcpy(*in, &value, 4);
#else
  __SW(*in, value);
#endif
  *in += 4;
}

/**
  @brief         Read 4 Q15 from Q15 pointer.
  @param[in]     pQ15      points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t riscv_nn_read_q15x4 (
		q15_t const * pQ15)
{
  q63_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD((q15_t *)pQ15);
#else
  val = *((q63_t *)pQ15);
#endif /* __RISCV_XLEN == 64 */
#else
  memcpy((void *)(&val), (void *)(pQ15), 8);
#endif
  return (val);
}

/**
  @brief         Read 4 Q15 from Q15 pointer and increment pointer afterwards.
  @param[in]     pQ15      points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t riscv_nn_read_q15x4_ia (
  q15_t ** pQ15)
{
  q63_t val;

  val = riscv_nn_read_q15x4(*pQ15);
  *pQ15 += 4;

  return (val);
}

/**
  @brief         Write 4 Q15 to Q15 pointer.
  @param[in]     pQ15      points to input value
  @param[in]     value     Q31 value
  @return        none
 */
__STATIC_FORCEINLINE void riscv_nn_write_q15x4 (
		q15_t * pQ15,
		q63_t   value)
{
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  __SD(pQ15, value);
#else
  *((q63_t *)pQ15) = value;
#endif
#else
  memcpy((void *)(pQ15), (void *)(&value), 8);
#endif
}

/**
  @brief         Write 4 Q15 to Q15 pointer and increment pointer afterwards.
  @param[in]     pQ15      points to input value
  @param[in]     value     Q31 value
  @return        none
 */
__STATIC_FORCEINLINE void riscv_nn_write_q15x4_ia (
  q15_t ** pQ15,
  q63_t    value)
{
  riscv_nn_write_q15x4(*pQ15, value);
  *pQ15 += 4;
}

/**
  @brief         Read 4 q7 from q7 pointer and post increment pointer.
  @param[in]     in_q7       Pointer to pointer that holds address of input.
  @return        q31 value
 */
__STATIC_FORCEINLINE q31_t riscv_nn_read_q7x4_ia(const q7_t **in_q7)
{
    q31_t val;
#ifdef __RISCV_FEATURE_UNALIGNED
    memcpy (&val, *in_q7, 4);
#else
    __ASM volatile ("lw %0, 0(%1)" : "=r" (val) : "r" (*in_q7));
#endif
  *in_q7 += 4;

    return (val);
}

/**
  @brief         Read 2 q15 from q15 pointer.
  @param[in]     in_q15   pointer to address of input.
  @return        q31 value
 */
__STATIC_FORCEINLINE q31_t riscv_nn_read_q15x2(const q15_t *in_q15)
{
    q31_t val;
#ifdef __RISCV_FEATURE_UNALIGNED
    memcpy (&val, in_q15, 4);
#else
    __ASM volatile ("lw %0, 0(%1)" : "=r" (val) : "r" (in_q15));
#endif


    return (val);
}

/**
  @brief         Read 4 q7 values.
  @param[in]     in_q7       pointer to address of input.
  @return        q31 value
 */
__STATIC_FORCEINLINE q31_t riscv_nn_read_q7x4(const q7_t *in_q7)
{
    q31_t val;
#ifdef __RISCV_FEATURE_UNALIGNED
    memcpy (&val, in_q7, 4);
#else
    __ASM volatile ("lw %0, 0(%1)" : "=r" (val) : "r" (in_q7));
#endif

    return (val);
}

/**
  @brief         Read 8 Q7 from Q7 pointer.
  @param[in]     pQ7       points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t riscv_nn_read_q7x8 (
		q7_t const * pQ7)
{
	q63_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD((q7_t *)pQ7);
#else
  val = *((q63_t *)pQ7);
#endif
#else
  memcpy((void *)(&val), (void *)pQ7, 8);
#endif

  return val;
}

/**
  @brief         Read 8 Q7 from Q7 pointer and increment pointer afterwards.
  @param[in]     pQ7       points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t riscv_nn_read_q7x8_ia (
  q7_t ** pQ7)
{
  q63_t val;

  val = riscv_nn_read_q7x8(*pQ7);
  *pQ7 += 8;

  return val;
}

/**
  @brief         Write four q7 to q7 pointer and increment pointer afterwards.
  @param[in]     in       Double pointer to input value
  @param[in]     value    Four bytes to copy
 */
__STATIC_FORCEINLINE void riscv_nn_write_q7x4_ia(q7_t **in, q31_t value)
{
    memcpy(*in, &value, 4);
    *in += 4;
}


/**
  @brief         Write 8 Q7 to Q7 pointer and increment pointer afterwards.
  @param[in]     pQ7       points to input value
  @param[in]     value     Q63 value
  @return        none
 */
__STATIC_FORCEINLINE void riscv_nn_write_q7x8_ia (
		q7_t ** pQ7,
		q63_t   value)
{
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  __SD(*pQ7,value);
#else
  *((q63_t *)*pQ7) = value;
#endif
#else
  memcpy((void *)(*pQ7), (void *)(&value), 8);
#endif
  *pQ7 += 8;
}

/**
 * @brief           memset
 * @param[in, out]  dst         Destination pointer
 * @param[in]       val         Value to set
 * @param[in]       block_size  Number of bytes to copy.
 *
 */
__STATIC_FORCEINLINE void riscv_memset_s8(int8_t *dst, const int8_t val, uint32_t block_size)
{
    memset(dst, val, block_size);
}

#if defined(RISCV_MATH_DSP)

/**
 * @brief read and expand one s4 word into two s8 words.
 */
__STATIC_FORCEINLINE void read_and_pad_s4(const int8_t *source, int32_t *out1, int32_t *out2)
{
    int16_t in = riscv_nn_read_s8x2(source);
    int32_t inA = (in & 0x00FF) | ((in & 0xFF00) << 8);

    *out1 = __SXTB16_RORn(__SXTB16(inA << 4), 4);
    *out2 = __SXTB16_RORn(__SXTB16(inA), 4);
}

/**
 * @brief read and expand one s4 word into two s8 words.
 * @details   The s4 elements are not evenly aligned on the byte boundary, so 3 bytes need to be read instead of 2.
 *            In other words first nibble to read start at the middle of a byte.
 *            byte index, s4 element
 *            0,          s4_x
 *            0,          s4_0
 *            1,          s4_1
 *            1,          s4_2
 *            2,          s4_3
 *            2,          s4_x
 */
__STATIC_FORCEINLINE void read_and_pad_s4_uneven(const int8_t *source, int32_t *out1, int32_t *out2)
{
    int32_t inA1 = (source[0] & 0xFF) | ((source[1] & 0xFF) << 16);
    int32_t inA2 = (source[1] & 0xFF) | ((source[2] & 0xFF) << 16);

    *out1 = __SXTB16_RORn(__SXTB16(inA2 << 4), 4);
    *out2 = __SXTB16_RORn(__SXTB16(inA1), 4);
}

/**
 * @brief read and expand one s4 word into two s16 words with ordering.
 */
__STATIC_FORCEINLINE void read_and_pad_s4_ordered(const int8_t *source, int32_t *out1, int32_t *out2)
{
    int16_t in = riscv_nn_read_s8x2(source);
    int32_t inA = (in & 0x00FF) | ((in & 0xFF00) << 8);
    int32_t inAbuf1 = __SXTB16_RORn(__SXTB16(inA), 4);
    int32_t inAbuf2 = __SXTB16_RORn(__SXTB16(inA << 4), 4);
    *out2 = (int32_t)(__PKHTB(inAbuf1, inAbuf2, 16));
    *out1 = (int32_t)(__PKHBT(inAbuf2, inAbuf1, 16));
}

/**
 * @brief read and expand two s8 word into four s16 words with ordering.
 */
#if __RISCV_XLEN == 64
__STATIC_FORCEINLINE const int8_t *read_and_pad64(const int8_t *source, int64_t *out1, int64_t *out2)
{
    int64_t inA = riscv_nn_read_s8x8_ia(&source);
    int64_t tmp1 = __SXTB16(__ROR64((uint64_t)inA, 8)); // __RV_SUNPKD820
    int64_t tmp2 = __SXTB16(inA);

    int64_t inAbuf1 = (int64_t)(__PKHBT64(tmp2, tmp1, 16));
    int64_t inAbuf2 = (int64_t)(__PKHTB64(tmp1, tmp2, 16));
    *out2 = __RV_PKTT32(inAbuf2, inAbuf1);
    *out1 = __RV_PKBB32(inAbuf2, inAbuf1);

    return source;
}
#else
#if defined (NUCLEI_DSP_N2)
__STATIC_FORCEINLINE const int8_t *read_and_pad64(const int8_t *source, int64_t *out1, int64_t *out2)
{
    int64_t inA = riscv_nn_read_s8x8_ia(&source);
    int64_t tmp1 = __RV_DSUNPKD820(__ROR64((uint64_t)inA, 8));
    int64_t tmp2 = __RV_DSUNPKD820(inA);

    int64_t inAbuf1 = (int64_t)(__PKHBT64(tmp2, tmp1, 16));
    int64_t inAbuf2 = (int64_t)(__PKHTB64(tmp1, tmp2, 16));
    *out1 = __RV_DPKBB32(inAbuf2, inAbuf1);
    *out2 = __RV_DPKTT32(inAbuf2, inAbuf1);

    return source;
}
#endif /* defined (NUCLEI_DSP_N2) */
#endif /* __RISCV_XLEN == 64 */

/**
 * @brief read and expand one s8 word into two s16 words with ordering.
 */
__STATIC_FORCEINLINE const int8_t *read_and_pad(const int8_t *source, int32_t *out1, int32_t *out2)
{
    int32_t inA = riscv_nn_read_s8x4_ia(&source);
    int32_t inAbuf1 = __SXTB16_RORn((uint32_t)inA, 8);
    int32_t inAbuf2 = __SXTB16(inA);

    *out2 = (int32_t)(__NN_PKHTB(inAbuf1, inAbuf2, 16));
    *out1 = (int32_t)(__NN_PKHBT(inAbuf2, inAbuf1, 16));

    return source;
}

/**
 * @brief read and expand one s8 word into two s16 words with ordering and addition.
 */
__STATIC_FORCEINLINE void read_pad_and_add_s8(const int8_t *source, int32_t *out1, int32_t *out2, const uint32_t add)
{
    int32_t inA = riscv_nn_read_s8x4(source);
    int32_t inAbuf1 = __SXTAB16_RORn(add, (uint32_t)inA, 8);
    int32_t inAbuf2 = __SXTAB16(add, inA);

    #ifndef RISCV_MATH_BIG_ENDIAN
    *out2 = (int32_t)(__PKHTB(inAbuf1, inAbuf2, 16));
    *out1 = (int32_t)(__PKHBT(inAbuf2, inAbuf1, 16));
    #else
    *out1 = (int32_t)(__PKHTB(inAbuf1, inAbuf2, 16));
    *out2 = (int32_t)(__PKHBT(inAbuf2, inAbuf1, 16));
    #endif
}

/**
 * @brief read and expand two bytes into one word with ordering.
 */
__STATIC_FORCEINLINE void read_and_pad_s8x2(const int8_t *source, int32_t *out)
{
    int16_t in = riscv_nn_read_s8x2(source);
    int32_t inA = (in & 0x00FF) | ((in & 0xFF00) << 8);
    *out = __SXTB16(inA);
}

/**
 * @brief read and expand two bytes into one word with ordering and addition.
 */
__STATIC_FORCEINLINE void read_pad_and_add_s8x2(const int8_t *source, int32_t *out, const uint32_t add)
{
    int16_t in = riscv_nn_read_s8x2(source);
    int32_t inA = (in & 0x00FF) | ((in & 0xFF00) << 8);
    *out = __SXTAB16(add, inA);
}

/**
 * @brief read and expand two s8 word into four s16 words with no additional ordering.
 */
#if __RISCV_XLEN == 64
__STATIC_FORCEINLINE const int8_t *read_and_pad_reordered64(const int8_t *source, int64_t *out1, int64_t *out2)
{
    int64_t inA = riscv_nn_read_s8x8_ia(&source);
    int64_t tmp2 = __RV_SUNPKD820(__ROR64((uint64_t)inA, 8));
    int64_t tmp1 = __RV_SUNPKD820(inA);
    *out1 = __RV_PKBB32(tmp2, tmp1);
    *out2 = __RV_PKTT32(tmp2, tmp1);

    return source;
}
#else
#if defined (NUCLEI_DSP_N2)
__STATIC_FORCEINLINE const int8_t *read_and_pad_reordered64(const int8_t *source, int64_t *out1, int64_t *out2)
{
    int64_t inA = riscv_nn_read_s8x8_ia(&source);
    int64_t tmp2 = __RV_DSUNPKD820(__ROR64((uint64_t)inA, 8));
    int64_t tmp1 = __RV_DSUNPKD820(inA);
    *out1 = __RV_DPKBB32(tmp2, tmp1);
    *out2 = __RV_DPKTT32(tmp2, tmp1);

    return source;
}
#endif /* defined (NUCLEI_DSP_N2) */
#endif /* __RISCV_XLEN == 64 */

/**
 * @brief read and expand one s8 word into two s16 words with no additional ordering.
 */

__STATIC_FORCEINLINE const int8_t *read_and_pad_reordered(const int8_t *source, int32_t *out1, int32_t *out2)
{
    int32_t inA = riscv_nn_read_s8x4_ia(&source);
    *out2 = __SXTB16(__ROR((uint32_t)inA, 8));
    *out1 = __SXTB16(inA);

    return source;
}

/**
 * @brief read and expand one q7 word into two q15 words with reordering and add an offset
 */
__STATIC_FORCEINLINE const q7_t *
read_and_pad_reordered_with_offset(const q7_t *source, q31_t *out1, q31_t *out2, q31_t offset)
{
    q31_t inA = riscv_nn_read_q7x4_ia(&source);

    *out2 = __SXTB16(__ROR((uint32_t)inA, 8));
    *out1 = __SXTB16(inA);
    *out1 = __NN_QADD16(*out1, offset);
    *out2 = __NN_QADD16(*out2, offset);

    return source;
}

#endif

/**
 * @brief Matrix-multiplication function for convolution with per-channel requantization and 4 bit weights.
 * @param[in]       input_a            pointer to operand A, int8 packed with 2x int4.
 * @param[in]       input_b            pointer to operand B, always consists of 2 vectors.
 * @param[in]       output_ch          number of rows of A
 * @param[in]       out_shift          pointer to per output channel requantization shift parameter.
 * @param[in]       out_mult           pointer to per output channel requantization multiplier parameter.
 * @param[in]       out_offset         output tensor offset.
 * @param[in]       activation_min     minimum value to clamp the output to. Range : int8
 * @param[in]       activation_max     maximum value to clamp the output to. Range : int8
 * @param[in]       num_col_a          number of columns of A
 * @param[in]       output_bias        per output channel bias. Range : int32
 * @param[in,out]   out_0              pointer to output
 * @return     The function returns one of the two
 *              1. The incremented output pointer for a successful operation or
 *              2. NULL if implementation is not available.
 *
 * @details   This function does the matrix multiplication of weight matrix for all output channels
 *            with 2 columns from im2col and produces two elements/output_channel. The outputs are
 *            clamped in the range provided by activation min and max.
 *            Supported framework: TensorFlow Lite micro.
 */
int8_t *riscv_nn_mat_mult_kernel_s4_s16(const int8_t *input_a,
                                      const int16_t *input_b,
                                      const uint16_t output_ch,
                                      const int32_t *out_shift,
                                      const int32_t *out_mult,
                                      const int32_t out_offset,
                                      const int32_t activation_min,
                                      const int32_t activation_max,
                                      const int32_t num_col_a,
                                      const int32_t *const output_bias,
                                      int8_t *out_0);

/**
 * @defgroup NNBasicMath Basic Math Functions for Neural Network Computation
 *
 * Basic Math Functions for Neural Network Computation
 *
 */

/**
 * @brief           q7 vector multiplication with variable output shifts
 * @param[in]       *pSrcA        pointer to the first input vector
 * @param[in]       *pSrcB        pointer to the second input vector
 * @param[out]      *pDst         pointer to the output vector
 * @param[in]       out_shift     amount of right-shift for output
 * @param[in]       blockSize     number of samples in each vector
 * @return none.
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function uses saturating arithmetic.
 * Results outside of the allowable q15 range [0x8000 0x7FFF] will be saturated.
 */

void riscv_nn_mult_q15(q15_t *pSrcA, q15_t *pSrcB, q15_t *pDst, const uint16_t out_shift, uint32_t blockSize);

/**
 * @brief           q7 vector multiplication with variable output shifts
 * @param[in]       *pSrcA        pointer to the first input vector
 * @param[in]       *pSrcB        pointer to the second input vector
 * @param[out]      *pDst         pointer to the output vector
 * @param[in]       out_shift     amount of right-shift for output
 * @param[in]       blockSize     number of samples in each vector
 * @return none.
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function uses saturating arithmetic.
 * Results outside of the allowable q7 range [0x80 0x7F] will be saturated.
 */

void riscv_nn_mult_q7(q7_t *pSrcA, q7_t *pSrcB, q7_t *pDst, const uint16_t out_shift, uint32_t blockSize);

/**
 * @brief Matrix-multiplication function for convolution with per-channel requantization.
 * @param[in]       input_a            pointer to operand A
 * @param[in]       input_b            pointer to operand B, always consists of 2 vectors.
 * @param[in]       output_ch          number of rows of A
 * @param[in]       out_shift          pointer to per output channel requantization shift parameter.
 * @param[in]       out_mult           pointer to per output channel requantization multiplier parameter.
 * @param[in]       out_offset         output tensor offset.
 * @param[in]       activation_min     minimum value to clamp the output to. Range : int8
 * @param[in]       activation_max     maximum value to clamp the output to. Range : int8
 * @param[in]       num_col_a          number of columns of A
 * @param[in]       aligned_num_col_a  number of columns of A aligned by 4
 * @param[in]       output_bias        per output channel bias. Range : int32
 * @param[in,out]   out_0              pointer to output
 * @return     The function returns one of the two
 *              1. The incremented output pointer for a successful operation or
 *              2. NULL if implementation is not available.
 *
 * @details   This function does the matrix multiplication of weight matrix for all output channels
 *            with 2 columns from im2col and produces two elements/output_channel. The outputs are
 *            clamped in the range provided by activation min and max.
 *            Supported framework: TensorFlow Lite micro.
 */
int8_t *riscv_nn_mat_mult_kernel_s8_s16(const int8_t *input_a,
                                      const int16_t *input_b,
                                      const uint16_t output_ch,
                                      const int32_t *out_shift,
                                      const int32_t *out_mult,
                                      const int32_t out_offset,
                                      const int16_t activation_min,
                                      const int16_t activation_max,
                                      const int32_t num_col_a,
                                      const int32_t aligned_num_col_a,
                                      const int32_t *const output_bias,
                                      int8_t *out_0);

/**
 * @brief Matrix-multiplication function for convolution with per-channel requantization, supporting an address offset
 * between rows.
 * @param[in]       input_a            pointer to operand A
 * @param[in]       input_b            pointer to operand B, always consists of 2 vectors.
 * @param[in]       output_ch          number of rows of A
 * @param[in]       out_shift          pointer to per output channel requantization shift parameter.
 * @param[in]       out_mult           pointer to per output channel requantization multiplier parameter.
 * @param[in]       out_offset         output tensor offset.
 * @param[in]       activation_min     minimum value to clamp the output to. Range : int8
 * @param[in]       activation_max     maximum value to clamp the output to. Range : int8
 * @param[in]       num_col_a          number of columns of A
 * @param[in]       aligned_num_col_a  number of columns of A aligned by 4
 * @param[in]       output_bias        per output channel bias. Range : int32
 * @param[in]       row_address_offset address offset between rows in the output
 * @param[in,out]   out_0              pointer to output
 * @return     The function returns one of the two
 *              1. The incremented output pointer for a successful operation or
 *              2. NULL if implementation is not available.
 *
 * @details   This function does the matrix multiplication of weight matrix for all output channels
 *            with 2 columns from im2col and produces two elements/output_channel. The outputs are
 *            clamped in the range provided by activation min and max.
 *
 *            This function is slighly less performant than riscv_nn_mat_mult_kernel_s8_s16, but allows support for
 * grouped convolution. Supported framework: TensorFlow Lite micro.
 */
int8_t *riscv_nn_mat_mult_kernel_row_offset_s8_s16(const int8_t *input_a,
                                                 const int16_t *input_b,
                                                 const uint16_t output_ch,
                                                 const int32_t *out_shift,
                                                 const int32_t *out_mult,
                                                 const int32_t out_offset,
                                                 const int16_t activation_min,
                                                 const int16_t activation_max,
                                                 const int32_t num_col_a,
                                                 const int32_t aligned_num_col_a,
                                                 const int32_t *const output_bias,
                                                 const int32_t row_address_offset,
                                                 int8_t *out_0);

/**
 * @brief Common softmax function for s8 input and s8 or s16 output
 * @param[in]  input          Pointer to the input tensor
 * @param[in]  num_rows       Number of rows in the input tensor
 * @param[in]  row_size       Number of elements in each input row
 * @param[in]  mult           Input quantization multiplier
 * @param[in]  shift          Input quantization shift within the range [0, 31]
 * @param[in]  diff_min       Minimum difference with max in row. Used to check if
 *                            the quantized exponential operation can be performed
 * @param[in]  int16_output   Indicating s8 output if 0 else s16 output
 * @param[out] output         Pointer to the output tensor
 *
 * @note Supported framework: TensorFlow Lite micro (bit-accurate)
 *
 */
void riscv_nn_softmax_common_s8(const int8_t *input,
                              const int32_t num_rows,
                              const int32_t row_size,
                              const int32_t mult,
                              const int32_t shift,
                              const int32_t diff_min,
                              const bool int16_output,
                              void *output);

/**
 * @brief macro for adding rounding offset
 */
#ifndef RISCV_NN_TRUNCATE
    #define NN_ROUND(out_shift) ((0x1 << out_shift) >> 1)
#else
    #define NN_ROUND(out_shift) 0
#endif

// Macros for shortening quantization functions' names and avoid long lines
#define MUL_SAT(a, b) riscv_nn_doubling_high_mult((a), (b))
#define MUL_POW2(a, b) riscv_nn_mult_by_power_of_two((a), (b))

#define DIV_POW2(a, b) riscv_nn_divide_by_power_of_two((a), (b))

#define EXP_ON_NEG(x) riscv_nn_exp_on_negative_values((x))
#define ONE_OVER1(x) riscv_nn_one_over_one_plus_x_for_x_in_0_1((x))

/**
 * @brief           Saturating doubling high multiply. Result matches
 *                  NEON instruction VQRDMULH.
 * @param[in]       m1        Multiplicand. Range: {NN_Q31_MIN, NN_Q31_MAX}
 * @param[in]       m2        Multiplier. Range: {NN_Q31_MIN, NN_Q31_MAX}
 * @return          Result of multiplication.
 *
 */
__STATIC_FORCEINLINE int32_t riscv_nn_doubling_high_mult(const int32_t m1, const int32_t m2)
{
    int32_t result = 0;
    // Rounding offset to add for a right shift of 31
    int64_t mult = 1 << 30;

    if ((m1 < 0) ^ (m2 < 0))
    {
        mult = 1 - mult;
    }
    // Gets resolved as a SMLAL instruction
    mult = mult + (int64_t)m1 * m2;

    // Utilize all of the upper 32 bits. This is the doubling step
    // as well.
    result = (int32_t)(mult / (1ll << 31));

    if ((m1 == m2) && (m1 == (int32_t)NN_Q31_MIN))
    {
        result = NN_Q31_MAX;
    }
    return result;
}

/**
 * @brief           Doubling high multiply without saturation. This is intended
 *                  for requantization where the scale is a positive integer
 *
 * @param[in]       m1        Multiplicand. Range: {NN_Q31_MIN, NN_Q31_MAX}
 * @param[in]       m2        Multiplier Range: {NN_Q31_MIN, NN_Q31_MAX}
 * @return          Result of multiplication.
 * @note            The result of this matches that of neon instruction
 *                  VQRDMULH for m1 in range {NN_Q31_MIN, NN_Q31_MAX} and m2 in
 *                  range {NN_Q31_MIN + 1, NN_Q31_MAX}. Saturation occurs when
 *                  m1 equals m2 equals NN_Q31_MIN and that is not handled by
 *                  this function.
 *
 */
__STATIC_FORCEINLINE int32_t riscv_nn_doubling_high_mult_no_sat(const int32_t m1, const int32_t m2)
{
    int32_t result = 0;
    union riscv_nn_long_long mult;

    // Rounding offset to add for a right shift of 31
    mult.word.low = 1 << 30;
    mult.word.high = 0;

    // Gets resolved as a SMLAL instruction
    mult.long_long = mult.long_long + (int64_t)m1 * m2;

    // Utilize all of the upper 32 bits. This is the doubling step
    // as well.
    result = (int32_t)(mult.long_long >> 31);

    return result;
}

/**
 * @brief           Rounding divide by power of two.
 * @param[in]       dividend - Dividend
 * @param[in]       exponent - Divisor = power(2, exponent)
 *                             Range: [0, 31]
 * @return          Rounded result of division. Midpoint is rounded away from zero.
 *
 */
__STATIC_FORCEINLINE int32_t riscv_nn_divide_by_power_of_two(const int32_t dividend, const int32_t exponent)
{
    int32_t result = 0;
    const int32_t remainder_mask = (1 << exponent) - 1;
    int32_t remainder = remainder_mask & dividend;

    // Basic division
    result = dividend >> exponent;

    // Adjust 'result' for rounding (mid point away from zero)
    int32_t threshold = remainder_mask >> 1;
    if (result < 0)
    {
        threshold++;
    }
    if (remainder > threshold)
    {
        result++;
    }

    return result;
}

/**
 * @brief           Requantize a given value.
 * @details         Essentially returns (val * multiplier)/(2 ^ shift) with different rounding depending if
 *                  NMSIS_NN_USE_SINGLE_ROUNDING is defined or not.
 * @param[in]       val         Value to be requantized
 * @param[in]       multiplier  Multiplier. Range {NN_Q31_MIN + 1, Q32_MAX}
 * @param[in]       shift       Shift. Range: {-31, 30}
 *                              Default branch:
 *                                  If shift is positive left shift 'val * multiplier' with shift
 *                                  If shift is negative right shift 'val * multiplier' with abs(shift)
 *                              Single round branch:
 *                                  Input for total_shift in divide by '2 ^ total_shift'
 *
 * @return          Default branch:
 *                      Returns (val * multiplier) with rounding divided by (2 ^ shift) with rounding
 *                  Single round branch:
 *                      Returns (val * multiplier)/(2 ^ (31 - shift)) with rounding
 *
 */
__STATIC_FORCEINLINE int32_t riscv_nn_requantize(const int32_t val, const int32_t multiplier, const int32_t shift)
{
    return riscv_nn_divide_by_power_of_two(riscv_nn_doubling_high_mult_no_sat(val * (1 << LEFT_SHIFT(shift)), multiplier),
                                         RIGHT_SHIFT(shift));
}

#if defined(RISCV_MATH_VECTOR)
__STATIC_FORCEINLINE vint32m4_t riscv_nn_requantize_m4_rvv(vint32m4_t valm4, size_t l, const q31_t multiplier, const q31_t shift)
{
    if (shift >= 0) {
        valm4 = __riscv_vsmul_vx_i32m4(__riscv_vsll_vx_i32m4(valm4, shift, l), multiplier, __RISCV_VXRM_RNU, l);
    } else {
        q31_t exponent = -shift;
        q31_t remainder_mask = (1 << exponent) - 1;
        q31_t threshold = remainder_mask >> 1;
        vint32m4_t b32m4, c32m4;
        valm4 = __riscv_vsmul_vx_i32m4(valm4, multiplier, __RISCV_VXRM_RNU, l);
        b32m4 = __riscv_vsra_vx_i32m4(valm4, exponent, l);
        valm4 = __riscv_vand_vx_i32m4(valm4, remainder_mask, l);

        c32m4 = __riscv_vmv_v_x_i32m4(threshold, l);
        vbool8_t mask = __riscv_vmslt_vx_i32m4_b8(b32m4, 0, l);
        c32m4 = __riscv_vadd_vx_i32m4_tumu(mask, c32m4, c32m4, 1, l);

        mask = __riscv_vmsgt_vv_i32m4_b8(valm4, c32m4, l);
        valm4 = __riscv_vadd_vx_i32m4_tumu(mask, b32m4, b32m4, 1, l);
    }
    return valm4;
}

__STATIC_FORCEINLINE vint32m8_t riscv_nn_requantize_m8_rvv(vint32m8_t valm8, size_t l, const q31_t multiplier, const q31_t shift)
{
    if (shift >= 0) {
        valm8 = __riscv_vsmul_vx_i32m8(__riscv_vsll_vx_i32m8(valm8, shift, l), multiplier, __RISCV_VXRM_RNU, l);
    } else {
        q31_t exponent = -shift;
        q31_t remainder_mask = (1 << exponent) - 1;
        q31_t threshold = remainder_mask >> 1;
        vint32m8_t b32m8, c32m8;
        valm8 = __riscv_vsmul_vx_i32m8(valm8, multiplier, __RISCV_VXRM_RNU, l);
        b32m8 = __riscv_vsra_vx_i32m8(valm8, exponent, l);
        valm8 = __riscv_vand_vx_i32m8(valm8, remainder_mask, l);

        c32m8 = __riscv_vmv_v_x_i32m8(threshold, l);
        vbool4_t mask = __riscv_vmslt_vx_i32m8_b4(b32m8, 0, l);
        c32m8 = __riscv_vadd_vx_i32m8_tumu(mask, c32m8, c32m8, 1, l);

        mask = __riscv_vmsgt_vv_i32m8_b4(valm8, c32m8, l);
        valm8 = __riscv_vadd_vx_i32m8_tumu(mask, b32m8, b32m8, 1, l);
    }
    return valm8;
}

#endif

/**
 * @brief           Requantize a given 64 bit value.
 * @param[in]       val                 Value to be requantized in the range {-(1<<47)} to {(1<<47) - 1}
 * @param[in]       reduced_multiplier  Reduced multiplier in the range {NN_Q31_MIN + 1, Q32_MAX} to {Q16_MIN + 1,
 * Q16_MAX}
 * @param[in]       shift               Left or right shift for 'val * multiplier' in the range {-31} to {7}
 *
 * @return          Returns (val * multiplier)/(2 ^ shift)
 *
 */
__STATIC_FORCEINLINE int32_t riscv_nn_requantize_s64(const int64_t val,
                                                   const int32_t reduced_multiplier,
                                                   const int32_t shift)
{
    const int64_t new_val = val * reduced_multiplier;

    int32_t result = new_val >> (14 - shift); // 64->32 bit reduction
    result = (result + 1) >> 1;               // Last shift position and insert round

    return result;
}

/**
 * @brief           memcpy
 * @param[in, out]  dst         Destination pointer
 * @param[in]       src         Source pointer.
 * @param[in]       block_size  Number of bytes to copy.
 *
 */
__STATIC_FORCEINLINE void riscv_memcpy_s8(int8_t *__RESTRICT dst, const int8_t *__RESTRICT src, uint32_t block_size)
{
    memcpy(dst, src, block_size);
}

/**
 * @brief           memcpy
 * @param[in, out]  dst         Destination pointer
 * @param[in]       src         Source pointer.
 * @param[in]       block_size  Number of bytes to copy.
 *
 */
__STATIC_FORCEINLINE void riscv_memcpy_q7(q7_t *__RESTRICT dst, const q7_t *__RESTRICT src, uint32_t block_size)
{
    memcpy(dst, src, block_size);
}

/**
 * @brief           memcpy wrapper for int16
 * @param[in, out]  dst         Destination pointer
 * @param[in]       src         Source pointer.
 * @param[in]       block_size  Number of bytes to copy.
 *
 */
__STATIC_FORCEINLINE void riscv_memcpy_q15(int16_t *__RESTRICT dst, const int16_t *__RESTRICT src, uint32_t block_size)
{
    memcpy(dst, src, block_size);
}


// @note The following functions are used only for softmax layer, scaled bits = 5 assumed

__STATIC_FORCEINLINE int32_t riscv_nn_exp_on_negative_values(int32_t val)
{
    int32_t mask = 0;
    int32_t shift = 24;

    const int32_t val_mod_minus_quarter = (val & ((1 << shift) - 1)) - (1 << shift);
    const int32_t remainder = val_mod_minus_quarter - val;
    const int32_t x = (val_mod_minus_quarter << 5) + (1 << 28);
    const int32_t x2 = MUL_SAT(x, x);

    int32_t result = 1895147668 +
        MUL_SAT(1895147668, x + DIV_POW2(MUL_SAT(DIV_POW2(MUL_SAT(x2, x2), 2) + MUL_SAT(x2, x), 715827883) + x2, 1));

#define SELECT_IF_NON_ZERO(x)                                                                                          \
    {                                                                                                                  \
        mask = MASK_IF_NON_ZERO(remainder & (1 << shift++));                                                           \
        result = SELECT_USING_MASK(mask, MUL_SAT(result, x), result);                                                  \
    }

    SELECT_IF_NON_ZERO(1672461947)
    SELECT_IF_NON_ZERO(1302514674)
    SELECT_IF_NON_ZERO(790015084)
    SELECT_IF_NON_ZERO(290630308)
    SELECT_IF_NON_ZERO(39332535)
    SELECT_IF_NON_ZERO(720401)
    SELECT_IF_NON_ZERO(242)

#undef SELECT_IF_NON_ZERO

    mask = MASK_IF_ZERO(val);
    return SELECT_USING_MASK(mask, NN_Q31_MAX, result);
}

__STATIC_FORCEINLINE int32_t riscv_nn_mult_by_power_of_two(const int32_t val, const int32_t exp)
{
    const int32_t thresh = ((1 << (31 - exp)) - 1);
    int32_t result = val << exp;
    result = SELECT_USING_MASK(MASK_IF_NON_ZERO(val > thresh), NN_Q31_MAX, result);
    result = SELECT_USING_MASK(MASK_IF_NON_ZERO(val < -thresh), NN_Q31_MIN, result);
    return result;
}

__STATIC_FORCEINLINE int32_t riscv_nn_one_over_one_plus_x_for_x_in_0_1(int32_t val)
{
    const int64_t sum = (int64_t)val + (int64_t)NN_Q31_MAX;
    const int32_t half_denominator = (int32_t)((sum + (sum >= 0 ? 1 : -1)) / 2L);
    int32_t x = 1515870810 + MUL_SAT(half_denominator, -1010580540);

    const int32_t shift = (1 << 29);
    x += MUL_POW2(MUL_SAT(x, shift - MUL_SAT(half_denominator, x)), 2);
    x += MUL_POW2(MUL_SAT(x, shift - MUL_SAT(half_denominator, x)), 2);
    x += MUL_POW2(MUL_SAT(x, shift - MUL_SAT(half_denominator, x)), 2);

    return MUL_POW2(x, 1);
}

/**
  @brief         Write 2 s16 elements and post increment pointer.
  @param[in]     dest_q15  Pointer to pointer that holds address of destination.
  @param[in]     src_q31   Input value to be written.
 */
__STATIC_FORCEINLINE void riscv_nn_write_q15x2_ia(int16_t **dest_q15, int32_t src_q31)
{
    int32_t val = src_q31;

    memcpy(*dest_q15, &val, 4);
    *dest_q15 += 2;
}

/**
  @brief         Write 2 s8 elements and post increment pointer.
  @param[in]     dst  Pointer to pointer that holds address of destination.
  @param[in]     src  Input value to be written.
 */
__STATIC_FORCEINLINE void riscv_nn_write_s8x2_ia(int8_t **dst, int16_t src)
{
    memcpy(*dst, &src, 2);
    *dst += 2;
}

  /**
   * @brief  Copies the elements of a Q7 vector.
   * @param[in]  pSrc       input pointer
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
void riscv_nn_copy_q7(
  const q7_t * pSrc,
        q7_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Copies the elements of a Q15 vector.
   * @param[in]  pSrc       input pointer
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
void riscv_nn_copy_q15(
  const q15_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize);

  /**
   * @brief  Fills a constant value into a Q7 vector.
   * @param[in]  value      input value to be filled
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
void riscv_nn_fill_q7(
        q7_t value,
        q7_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Fills a constant value into a Q15 vector.
   * @param[in]  value      input value to be filled
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
void riscv_nn_fill_q15(
        q15_t value,
        q15_t * pDst,
        uint32_t blockSize);

// Support functions for LSTM
/**
 * @brief Update LSTM function for an iteration step using s8 input and output, and s16 internally.
 *
 * @param[in]   data_in                         Data input pointer
 * @param[in]   hidden_in                       Hidden state/ recurrent input pointer
 * @param[out]  hidden_out                      Hidden state/ recurrent output pointer
 * @param[in]   params                          Struct containg all information about the lstm operator, see
 * riscv_nn_types.
 * @param[in]   buffers                         Struct containg pointers to all temporary scratch buffers needed for the
 * lstm operator, see riscv_nn_types.
 * @param[in]   batch_offset                    Number of timesteps between consecutive batches.
 * E.g for params->timing_major = true, all batches for t=0 are stored sequentially, so batch offset = 1.
 * For params->time major = false, all time steps are stored continously before the next batch, so
 * batch offset = params->time_steps.
 * @return                                      The function returns RISCV_NMSIS_NN_SUCCESS

 */
riscv_nmsis_nn_status riscv_nn_lstm_step_s8(const int8_t *data_in,
                                        const int8_t *hidden_in,
                                        int8_t *hidden_out,
                                        const nmsis_nn_lstm_params *params,
                                        nmsis_nn_lstm_context *buffers,
                                        const int32_t batch_offset);

/**
 * @brief Update LSTM function for an iteration step using s16 input and output, and s16 internally.
 *
 * @param[in]   data_in                         Data input pointer
 * @param[in]   hidden_in                       Hidden state/ recurrent input pointer
 * @param[out]  hidden_out                      Hidden state/ recurrent output pointer
 * @param[in]   params                          Struct containg all information about the lstm operator, see
 * riscv_nn_types.
 * @param[in]   buffers                         Struct containg pointers to all temporary scratch buffers needed for the
 * lstm operator, see riscv_nn_types.
 * @param[in]   batch_offset                    Number of timesteps between consecutive batches.
 * E.g for params->timing_major = true, all batches for t=0 are stored sequentially, so batch offset = 1.
 * For params->time major = false, all time steps are stored continously before the next batch, so
 * batch offset = params->time_steps.
 * @return                                      The function returns RISCV_NMSIS_NN_SUCCESS

 */
riscv_nmsis_nn_status riscv_nn_lstm_step_s16(const int16_t *data_in,
                                         const int16_t *hidden_in,
                                         int16_t *hidden_out,
                                         const nmsis_nn_lstm_params *params,
                                         nmsis_nn_lstm_context *buffers,
                                         const int32_t batch_offset);

/**
 * @brief Updates a LSTM gate for an iteration step of LSTM function, int8x8_16 version.
 *
 * @param[in]   data_in                         Data input pointer
 * @param[in]   hidden_in                       Hidden state/ recurrent input pointer
 * @param[in]   gate_data                       Struct containing all information about the gate caluclation, see
 * riscv_nn_types.
 * @param[in]   params                          Struct containing all information about the lstm_operation, see
 * riscv_nn_types
 * @param[out]  output                          Hidden state/ recurrent output pointer
 * @param[in]   batch_offset                    Number of timesteps between consecutive batches, see
 * riscv_nn_lstm_step_s8.
 * @return                                      The function returns RISCV_NMSIS_NN_SUCCESS
 */
riscv_nmsis_nn_status riscv_nn_lstm_calculate_gate_s8_s16(const int8_t *data_in,
                                                      const int8_t *hidden_in,
                                                      const nmsis_nn_lstm_gate *gate_data,
                                                      const nmsis_nn_lstm_params *params,
                                                      int16_t *output,
                                                      const int32_t batch_offset);

/**
 * @brief Updates a LSTM gate for an iteration step of LSTM function, int16x8_16 version.
 *
 * @param[in]   data_in                         Data input pointer
 * @param[in]   hidden_in                       Hidden state/ recurrent input pointer
 * @param[in]   gate_data                       Struct containing all information about the gate caluclation, see
 * riscv_nn_types.
 * @param[in]   params                          Struct containing all information about the lstm_operation, see
 * riscv_nn_types
 * @param[out]  output                          Hidden state/ recurrent output pointer
 * @param[in]   batch_offset                    Number of timesteps between consecutive batches, see
 * riscv_nn_lstm_step_s16.
 * @return                                      The function returns RISCV_NMSIS_NN_SUCCESS
 */
riscv_nmsis_nn_status riscv_nn_lstm_calculate_gate_s16(const int16_t *data_in,
                                                   const int16_t *hidden_in,
                                                   const nmsis_nn_lstm_gate *gate_data,
                                                   const nmsis_nn_lstm_params *params,
                                                   int16_t *output,
                                                   const int32_t batch_offset);

/**
 * @brief The result of the multiplication is accumulated to the passed result buffer.
 * Multiplies a matrix by a "batched" vector (i.e. a matrix with a batch dimension composed by input vectors independent
 * from each other).
 *
 * @param[in]   lhs              Batched vector
 * @param[in]   rhs              Weights - input matrix (H(Rows)xW(Columns))
 * @param[in]   effective_bias   Bias + lhs_offset * kernel_sum term precalculated into a constant vector.
 * @param[out]  dst              Output
 * @param[in]   dst_multiplier   Multiplier for quantization
 * @param[in]   dst_shift        Shift for quantization
 * @param[in]   rhs_cols         Vector/matarix column length
 * @param[in]   rhs_rows         Row count of matrix
 * @param[in]   batches          Batch size
 * @param[in]   batch_offset     Number of timesteps between consecutive batches in input, see riscv_nn_lstm_step_s8. Note
 that the output is always stored with sequential batches.
 * @return                       The function returns <code>RISCV_NMSIS_NN_SUCCESS</code>

 */
riscv_nmsis_nn_status riscv_nn_vec_mat_mul_result_acc_s8_s16(const int8_t *lhs,
                                                         const int8_t *rhs,
                                                         const int32_t *effective_bias,
                                                         int16_t *dst,
                                                         const int32_t dst_multiplier,
                                                         const int32_t dst_shift,
                                                         const int32_t rhs_cols,
                                                         const int32_t rhs_rows,
                                                         const int32_t batches,
                                                         const int32_t batch_offset);

/**
 * @brief The result of the multiplication is accumulated to the passed result buffer.
 * Multiplies a matrix by a "batched" vector (i.e. a matrix with a batch dimension composed by input vectors independent
 * from each other).
 *
 * @param[in]   lhs              Batched vector
 * @param[in]   rhs              Weights - input matrix (H(Rows)xW(Columns))
 * @param[in]   effective_bias   Bias + lhs_offset * kernel_sum term precalculated into a constant vector.
 * @param[out]  dst              Output
 * @param[in]   dst_multiplier   Multiplier for quantization
 * @param[in]   dst_shift        Shift for quantization
 * @param[in]   rhs_cols         Vector/matarix column length
 * @param[in]   rhs_rows         Row count of matrix
 * @param[in]   batches          Batch size
 * @param[in]   batch_offset     Number of timesteps between consecutive batches in input, see riscv_nn_lstm_step_s16.
 Note that the output is always stored with sequential batches.
 * @return                       The function returns <code>RISCV_NMSIS_NN_SUCCESS</code>

 */
riscv_nmsis_nn_status riscv_nn_vec_mat_mul_result_acc_s16(const int16_t *lhs,
                                                      const int8_t *rhs,
                                                      const int64_t *effective_bias,
                                                      int16_t *dst,
                                                      const int32_t dst_multiplier,
                                                      const int32_t dst_shift,
                                                      const int32_t rhs_cols,
                                                      const int32_t rhs_rows,
                                                      const int32_t batches,
                                                      const int32_t batch_offset);

/**
 * @brief s16 elementwise multiplication with s8 output
 * @param[in]       input_1_vect        pointer to input vector 1
 * @param[in]       input_2_vect        pointer to input vector 2
 * @param[in,out]   output              pointer to output vector
 * @param[in]       out_offset          output offset
 * @param[in]       out_mult            output multiplier
 * @param[in]       out_shift           output shift
 * @param[in]       block_size          number of samples per batch
 * @param[in]       batch_size          number of samples per batch
 * @param[in]       batch_offset        Number of timesteps between consecutive batches in output, see
 * riscv_nn_lstm_step_s8. Note that it is assumed that the input is stored with sequential batches.
 * @return          The function returns RISCV_NMSIS_NN_SUCCESS
 *
 * @details   Supported framework: TensorFlow Lite micro
 */
riscv_nmsis_nn_status riscv_elementwise_mul_s16_s8(const int16_t *input_1_vect,
                                               const int16_t *input_2_vect,
                                               int8_t *output,
                                               const int32_t out_offset,
                                               const int32_t out_mult,
                                               const int32_t out_shift,
                                               const int32_t block_size,
                                               const int32_t batch_size,
                                               const int32_t batch_offset);

/**
 * @brief s16 elementwise multiplication with s16 output
 * @param[in]       input_1_vect        pointer to input vector 1
 * @param[in]       input_2_vect        pointer to input vector 2
 * @param[in,out]   output              pointer to output vector
 * @param[in]       out_offset          output offset
 * @param[in]       out_mult            output multiplier
 * @param[in]       out_shift           output shift
 * @param[in]       block_size          number of samples per batch
 * @param[in]       batch_size          number of samples per batch
 * @param[in]       batch_offset        Number of timesteps between consecutive batches in output, see
 * riscv_nn_lstm_step_s16. Note that it is assumed that the input is stored with sequential batches.
 * @return          The function returns RISCV_NMSIS_NN_SUCCESS
 *
 * @details   Supported framework: TensorFlow Lite micro
 */
riscv_nmsis_nn_status riscv_elementwise_mul_s16_batch_offset(const int16_t *input_1_vect,
                                                         const int16_t *input_2_vect,
                                                         int16_t *output,
                                                         const int32_t out_offset,
                                                         const int32_t out_mult,
                                                         const int32_t out_shift,
                                                         const int32_t block_size,
                                                         const int32_t batch_size,
                                                         const int32_t batch_offset);

/**
 * @brief s16 elementwise multiplication. The result of the multiplication is accumulated to the passed result buffer.
 * @param[in]       input_1_vect        pointer to input vector 1
 * @param[in]       input_2_vect        pointer to input vector 2
 * @param[in]       input_1_offset      offset for input 1. Not used.
 * @param[in]       input_2_offset      offset for input 2. Not used.
 * @param[in,out]   output              pointer to output vector
 * @param[in]       out_offset          output offset. Not used.
 * @param[in]       out_mult            output multiplier
 * @param[in]       out_shift           output shift
 * @param[in]       out_activation_min  minimum value to clamp output to. Min: -32768
 * @param[in]       out_activation_max  maximum value to clamp output to. Max: 32767
 * @param[in]       block_size          number of samples
 * @return          The function returns RISCV_NMSIS_NN_SUCCESS
 *
 * @details   Supported framework: TensorFlow Lite micro
 */
riscv_nmsis_nn_status riscv_elementwise_mul_acc_s16(const int16_t *input_1_vect,
                                                const int16_t *input_2_vect,
                                                const int32_t input_1_offset,
                                                const int32_t input_2_offset,
                                                int16_t *output,
                                                const int32_t out_offset,
                                                const int32_t out_mult,
                                                const int32_t out_shift,
                                                const int32_t out_activation_min,
                                                const int32_t out_activation_max,
                                                const int32_t block_size);

#ifdef __cplusplus
}
#endif

#endif /* RISCV_NNSUPPORTFUNCTIONS_H */
