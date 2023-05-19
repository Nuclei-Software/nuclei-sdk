/*
 * SPDX-FileCopyrightText: Copyright 2020-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

/* ----------------------------------------------------------------------
 * Project:      NMSIS NN Library
 * Title:        riscv_nn_types.h
 * Description:  Public header file to contain the NMSIS-NN structs for the
 *               TensorFlowLite micro compliant functions
 *
 * $Date:        8 March 2023
 * $Revision:    V.2.5.0
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */

#ifndef _RISCV_NN_TYPES_H
#define _RISCV_NN_TYPES_H

#include <stdint.h>

/** Enum for specifying activation function types */
typedef enum
{
    RISCV_SIGMOID = 0, /**< Sigmoid activation function */
    RISCV_TANH = 1,    /**< Tanh activation function */
} riscv_nn_activation_type;

/** Function return codes */
typedef enum
{
    RISCV_NMSIS_NN_SUCCESS = 0,        /**< No error */
    RISCV_NMSIS_NN_ARG_ERROR = -1,     /**< One or more arguments are incorrect */
    RISCV_NMSIS_NN_NO_IMPL_ERROR = -2, /**<  No implementation available */
    RISCV_NMSIS_NN_SIZE_MISMATCH = -3,        /**< Size of matrices is not compatible with the operation */
} riscv_nmsis_nn_status;

/** NMSIS-NN object to contain the width and height of a tile */
typedef struct
{
    int32_t w; /**< Width */
    int32_t h; /**< Height */
} nmsis_nn_tile;

/** NMSIS-NN object used for the function context. */
typedef struct
{
    void *buf;    /**< Pointer to a buffer needed for the optimization */
    int32_t size; /**< Buffer size */
} nmsis_nn_context;

/** NMSIS-NN object to contain the dimensions of the tensors */
typedef struct
{
    int32_t n; /**< Generic dimension to contain either the batch size or output channels.
                     Please refer to the function documentation for more information */
    int32_t h; /**< Height */
    int32_t w; /**< Width */
    int32_t c; /**< Input channels */
} nmsis_nn_dims;

/** NMSIS-NN object to contain LSTM specific input parameters related to dimensions */
typedef struct
{
    int32_t max_time;
    int32_t num_inputs;
    int32_t num_batches;
    int32_t num_outputs;
} nmsis_nn_lstm_dims;

/** NMSIS-NN object for the per-channel quantization parameters */
typedef struct
{
    int32_t *multiplier; /**< Multiplier values */
    int32_t *shift;      /**< Shift values */
} nmsis_nn_per_channel_quant_params;

/** NMSIS-NN object for the per-tensor quantization parameters */
typedef struct
{
    int32_t multiplier; /**< Multiplier value */
    int32_t shift;      /**< Shift value */
} nmsis_nn_per_tensor_quant_params;

/** NMSIS-NN object for the quantized Relu activation */
typedef struct
{
    int32_t min; /**< Min value used to clamp the result */
    int32_t max; /**< Max value used to clamp the result */
} nmsis_nn_activation;

/** NMSIS-NN object for the convolution layer parameters */
typedef struct
{
    int32_t input_offset;  /**< Zero value for the input tensor */
    int32_t output_offset; /**< Zero value for the output tensor */
    nmsis_nn_tile stride;
    nmsis_nn_tile padding;
    nmsis_nn_tile dilation;
    nmsis_nn_activation activation;
} nmsis_nn_conv_params;

/** NMSIS-NN object for Depthwise convolution layer parameters */
typedef struct
{
    int32_t input_offset;  /**< Zero value for the input tensor */
    int32_t output_offset; /**< Zero value for the output tensor */
    int32_t ch_mult;       /**< Channel Multiplier. ch_mult * in_ch = out_ch */
    nmsis_nn_tile stride;
    nmsis_nn_tile padding;
    nmsis_nn_tile dilation;
    nmsis_nn_activation activation;
} nmsis_nn_dw_conv_params;
/** NMSIS-NN object for pooling layer parameters */
typedef struct
{
    nmsis_nn_tile stride;
    nmsis_nn_tile padding;
    nmsis_nn_activation activation;
} nmsis_nn_pool_params;

/** NMSIS-NN object for Fully Connected layer parameters */
typedef struct
{
    int32_t input_offset;  /**< Zero value for the input tensor */
    int32_t filter_offset; /**< Zero value for the filter tensor. Not used */
    int32_t output_offset; /**< Zero value for the output tensor */
    nmsis_nn_activation activation;
} nmsis_nn_fc_params;

/** NMSIS-NN object for SVDF layer parameters */
typedef struct
{
    int32_t rank;
    int32_t input_offset;  /**< Zero value for the input tensor */
    int32_t output_offset; /**< Zero value for the output tensor */
    nmsis_nn_activation input_activation;
    nmsis_nn_activation output_activation;
} nmsis_nn_svdf_params;

/** NMSIS-NN object for Softmax s16 layer parameters */
typedef struct
{
    const int16_t *exp_lut;
    const int16_t *one_by_one_lut;
} nmsis_nn_softmax_lut_s16;

/** LSTM guard parameters */
typedef struct
{
    int32_t input_variance;
    int32_t forget_variance;
    int32_t cell_variance;
    int32_t output_variance;
} nmsis_nn_lstm_guard_params;

/** LSTM scratch buffer container */
typedef struct
{
    int16_t *input_gate;
    int16_t *forget_gate;
    int16_t *cell_gate;
    int16_t *output_gate;
} nmsis_nn_lstm_context;

/** Quantized clip value for cell and projection of LSTM input. Zero value means no clipping. */
typedef struct
{
    int16_t cell;
    int8_t projection;
} nmsis_nn_lstm_clip_params;

/** NMSIS-NN object for quantization parameters */
typedef struct
{
    int32_t multiplier; /**< Multiplier value */
    int32_t shift;      /**< Shift value */
} nmsis_nn_scaling;

/** NMSIS-NN norm layer coefficients */
typedef struct
{
    int16_t *input_weight;
    int16_t *forget_weight;
    int16_t *cell_weight;
    int16_t *output_weight;
} nmsis_nn_layer_norm;

/** Parameters for integer LSTM, as defined in TFLM */
typedef struct
{
    int32_t time_major; /**< Nonzero (true) if first row of data is timestamps for input */
    nmsis_nn_scaling input_to_input_scaling;
    nmsis_nn_scaling input_to_forget_scaling;
    nmsis_nn_scaling input_to_cell_scaling;
    nmsis_nn_scaling input_to_output_scaling;
    nmsis_nn_scaling recurrent_to_input_scaling;
    nmsis_nn_scaling recurrent_to_forget_scaling;
    nmsis_nn_scaling recurrent_to_cell_scaling;
    nmsis_nn_scaling recurrent_to_output_scaling;
    nmsis_nn_scaling cell_to_input_scaling;
    nmsis_nn_scaling cell_to_forget_scaling;
    nmsis_nn_scaling cell_to_output_scaling;
    nmsis_nn_scaling projection_scaling;
    nmsis_nn_scaling hidden_scaling;
    nmsis_nn_scaling layer_norm_input_scaling;  /**< layer normalization for input layer */
    nmsis_nn_scaling layer_norm_forget_scaling; /**< layer normalization for forget gate */
    nmsis_nn_scaling layer_norm_cell_scaling;   /**< layer normalization for cell */
    nmsis_nn_scaling layer_norm_output_scaling; /**< layer normalization for outpus layer */

    int32_t cell_state_shift;
    int32_t hidden_offset;
    int32_t output_state_offset;

    nmsis_nn_lstm_clip_params clip;
    nmsis_nn_lstm_guard_params guard;
    nmsis_nn_layer_norm layer_norm;

    /* Effective bias is precalculated as bias + zero_point * weight.
    Only applicable to when input/output are s8 and weights are s16 */
    const int32_t *i2i_effective_bias; /**< input to input effective bias */
    const int32_t *i2f_effective_bias; /**< input to forget gate effective bias */
    const int32_t *i2c_effective_bias; /**< input to cell effective bias */
    const int32_t *i2o_effective_bias; /**< input to output effective bias */

    const int32_t *r2i_effective_bias; /**< recurrent gate to input effective bias */
    const int32_t *r2f_effective_bias; /**< recurrent gate to forget gate effective bias */
    const int32_t *r2c_effective_bias; /**< recurrent gate to cell effective bias */
    const int32_t *r2o_effective_bias; /**< recurrent gate to output effective bias */

    const int32_t *projection_effective_bias;

    /* Not precalculated bias */
    const int32_t *input_gate_bias;
    const int32_t *forget_gate_bias;
    const int32_t *cell_gate_bias;
    const int32_t *output_gate_bias;

    /* Activation min and max */
    nmsis_nn_activation activation;

} nmsis_nn_lstm_params;

#endif // _RISCV_NN_TYPES_H
