/******************************************************************************
 * @file     matrix_utils.h
 * @brief    Public header file for NMSIS DSP Library
 * @version  V1.11.0
 * @date     30 May 2022
 * Target Processor: RISC-V cores
 ******************************************************************************/
/*
 * Copyright (c) 2010-2022 Arm Limited or its affiliates. All rights reserved.
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

 
#ifndef _MATRIX_UTILS_H_
#define _MATRIX_UTILS_H_

#include "riscv_math_types.h"
#include "riscv_math_memory.h"

#include "dsp/none.h"
#include "dsp/utils.h"

#ifdef   __cplusplus
extern "C"
{
#endif

#define ELEM(A,ROW,COL) &((A)->pData[(A)->numCols* (ROW) + (COL)])

#define SCALE_COL_T(T,CAST,A,ROW,v,i)        \
{                                       \
  int32_t _w;                            \
  T *data = (A)->pData;                 \
  const int32_t _numCols = (A)->numCols; \
  const int32_t nb = (A)->numRows - ROW;\
                                        \
  data += i + _numCols * (ROW);          \
                                        \
  for(_w=0;_w < nb; _w++)                  \
  {                                     \
     *data *= CAST v;                   \
     data += _numCols;                   \
  }                                     \
}

#define COPY_COL_T(T,A,ROW,COL,DST)               \
{                                                 \
    uint32_t _row;                                \
    T *_pb=DST;                                    \
    T *_pa = (A)->pData + ROW * (A)->numCols + COL;\
    for(_row = ROW; _row < (A)->numRows; _row ++) \
    {                                             \
         *_pb++ = *_pa;                             \
         _pa += (A)->numCols;                      \
    }                                             \
}

#if defined(RISCV_FLOAT16_SUPPORTED)

#define SWAP_ROWS_F16(A,COL,i,j)       \
{                                      \
  int32_t _w;                           \
  float16_t *dataI = (A)->pData;       \
  float16_t *dataJ = (A)->pData;       \
  const int32_t _numCols = (A)->numCols;\
  const int32_t nb = _numCols-(COL);    \
                                       \
  dataI += i*_numCols + (COL);          \
  dataJ += j*_numCols + (COL);          \
                                       \
  for(_w=0;_w < nb; _w++)                 \
  {                                    \
     float16_t tmp;                    \
     tmp = *dataI;                     \
     *dataI++ = *dataJ;                \
     *dataJ++ = tmp;                   \
  }                                    \
}

#define SCALE_ROW_F16(A,COL,v,i)       \
{                                      \
  int32_t _w;                           \
  float16_t *data = (A)->pData;        \
  const int32_t _numCols = (A)->numCols;\
  const int32_t nb = _numCols-(COL);    \
                                       \
  data += i*_numCols + (COL);           \
                                       \
  for(_w=0;_w < nb; _w++)                 \
  {                                    \
     *data++ *= (_Float16)v;           \
  }                                    \
}


#define MAC_ROW_F16(COL,A,i,v,B,j)                \
{                                                 \
  int32_t _w;                                      \
  float16_t *dataA = (A)->pData;                  \
  float16_t *dataB = (B)->pData;                  \
  const int32_t _numCols = (A)->numCols;           \
  const int32_t nb = _numCols-(COL);               \
                                                  \
  dataA += i*_numCols + (COL);                     \
  dataB += j*_numCols + (COL);                     \
                                                  \
  for(_w=0;_w < nb; _w++)                            \
  {                                               \
     *dataA++ += (_Float16)v * (_Float16)*dataB++;\
  }                                               \
}

#define MAS_ROW_F16(COL,A,i,v,B,j)                \
{                                                 \
  int32_t _w;                                      \
  float16_t *dataA = (A)->pData;                  \
  float16_t *dataB = (B)->pData;                  \
  const int32_t _numCols = (A)->numCols;           \
  const int32_t nb = _numCols-(COL);               \
                                                  \
  dataA += i*_numCols + (COL);                     \
  dataB += j*_numCols + (COL);                     \
                                                  \
  for(_w=0;_w < nb; _w++)                            \
  {                                               \
     *dataA++ -= (_Float16)v * (_Float16)*dataB++;\
  }                                               \
}

/* Functions with only a scalar version */
#define COPY_COL_F16(A,ROW,COL,DST) \
  COPY_COL_T(float16_t,A,ROW,COL,DST)

#define SCALE_COL_F16(A,ROW,v,i)        \
  SCALE_COL_T(float16_t,(_Float16),A,ROW,v,i)
  
#endif /* defined(RISCV_FLOAT16_SUPPORTED)*/

#define SWAP_ROWS_F32(A,COL,i,j)       \
{                                      \
  int32_t _w;                           \
  float32_t tmp;                       \
  float32_t *dataI = (A)->pData;       \
  float32_t *dataJ = (A)->pData;       \
  const int32_t _numCols = (A)->numCols;\
  const int32_t nb = _numCols - COL;    \
                                       \
  dataI += i*_numCols + (COL);          \
  dataJ += j*_numCols + (COL);          \
                                       \
                                       \
  for(_w=0;_w < nb; _w++)                 \
  {                                    \
     tmp = *dataI;                     \
     *dataI++ = *dataJ;                \
     *dataJ++ = tmp;                   \
  }                                    \
}

#define SCALE_ROW_F32(A,COL,v,i)       \
{                                      \
  int32_t _w;                           \
  float32_t *data = (A)->pData;        \
  const int32_t _numCols = (A)->numCols;\
  const int32_t nb = _numCols - COL;    \
                                       \
  data += i*_numCols + (COL);           \
                                       \
  for(_w=0;_w < nb; _w++)                 \
  {                                    \
     *data++ *= v;                     \
  }                                    \
}


#define MAC_ROW_F32(COL,A,i,v,B,j)     \
{                                      \
  int32_t _w;                           \
  float32_t *dataA = (A)->pData;       \
  float32_t *dataB = (B)->pData;       \
  const int32_t _numCols = (A)->numCols;\
  const int32_t nb = _numCols-(COL);    \
                                       \
  dataA = dataA + i*_numCols + (COL);   \
  dataB = dataB + j*_numCols + (COL);   \
                                       \
  for(_w=0;_w < nb; _w++)                 \
  {                                    \
     *dataA++ += v* *dataB++;          \
  }                                    \
}

#define MAS_ROW_F32(COL,A,i,v,B,j)     \
{                                      \
  int32_t _w;                           \
  float32_t *dataA = (A)->pData;       \
  float32_t *dataB = (B)->pData;       \
  const int32_t _numCols = (A)->numCols;\
  const int32_t nb = _numCols-(COL);    \
                                       \
  dataA = dataA + i*_numCols + (COL);   \
  dataB = dataB + j*_numCols + (COL);   \
                                       \
  for(_w=0;_w < nb; _w++)                 \
  {                                    \
     *dataA++ -= v* *dataB++;          \
  }                                    \
}



/* Functions _with only a scalar version */

#define COPY_COL_F32(A,ROW,COL,DST) \
  COPY_COL_T(float32_t,A,ROW,COL,DST)

#define COPY_COL_F64(A,ROW,COL,DST) \
  COPY_COL_T(float64_t,A,ROW,COL,DST)

#define SWAP_COLS_F32(A,COL,i,j)               \
{                                              \
  int32_t _w;                                  \
  float32_t *data = (A)->pData;                \
  const int32_t _numCols = (A)->numCols;       \
  for(_w=(COL);_w < _numCols; _w++)                \
  {                                            \
     float32_t tmp;                            \
     tmp = data[_w*_numCols + i];                \
     data[_w*_numCols + i] = data[_w*_numCols + j];\
     data[_w*_numCols + j] = tmp;                \
  }                                            \
}

#define SCALE_COL_F32(A,ROW,v,i)        \
  SCALE_COL_T(float32_t,,A,ROW,v,i)

#define SWAP_ROWS_F64(A,COL,i,j)       \
{                                      \
  int32_t _w;                           \
  float64_t *dataI = (A)->pData;       \
  float64_t *dataJ = (A)->pData;       \
  const int32_t _numCols = (A)->numCols;\
  const int32_t nb = _numCols-(COL);    \
                                       \
  dataI += i*_numCols + (COL);          \
  dataJ += j*_numCols + (COL);          \
                                       \
  for(_w=0;_w < nb; _w++)                 \
  {                                    \
     float64_t tmp;                    \
     tmp = *dataI;                     \
     *dataI++ = *dataJ;                \
     *dataJ++ = tmp;                   \
  }                                    \
}

#define SWAP_COLS_F64(A,COL,i,j)               \
{                                              \
  int32_t _w;                                  \
  float64_t *data = (A)->pData;                \
  const int32_t _numCols = (A)->numCols;       \
  for(_w=(COL);_w < _numCols; _w++)                \
  {                                            \
     float64_t tmp;                            \
     tmp = data[_w*_numCols + i];                \
     data[_w*_numCols + i] = data[_w*_numCols + j];\
     data[_w*_numCols + j] = tmp;                \
  }                                            \
}

#define SCALE_ROW_F64(A,COL,v,i)       \
{                                      \
  int32_t _w;                           \
  float64_t *data = (A)->pData;        \
  const int32_t _numCols = (A)->numCols;\
  const int32_t nb = _numCols-(COL);    \
                                       \
  data += i*_numCols + (COL);           \
                                       \
  for(_w=0;_w < nb; _w++)                 \
  {                                    \
     *data++ *= v;                     \
  }                                    \
}

#define SCALE_COL_F64(A,ROW,v,i)        \
  SCALE_COL_T(float64_t,,A,ROW,v,i)

#define MAC_ROW_F64(COL,A,i,v,B,j)      \
{                                       \
  int32_t _w;                           \
  float64_t *dataA = (A)->pData;        \
  float64_t *dataB = (B)->pData;        \
  const int32_t _numCols = (A)->numCols;\
  const int32_t nb = _numCols-(COL);     \
                                        \
  dataA += i*_numCols + (COL);           \
  dataB += j*_numCols + (COL);           \
                                        \
  for(_w=0;_w < nb; _w++)                  \
  {                                     \
     *dataA++ += v* *dataB++;           \
  }                                     \
}

#define MAS_ROW_F64(COL,A,i,v,B,j)      \
{                                       \
  int32_t _w;                           \
  float64_t *dataA = (A)->pData;        \
  float64_t *dataB = (B)->pData;        \
  const int32_t _numCols = (A)->numCols;\
  const int32_t nb = _numCols-(COL);     \
                                        \
  dataA += i*_numCols + (COL);           \
  dataB += j*_numCols + (COL);           \
                                        \
  for(_w=0;_w < nb; _w++)                  \
  {                                     \
     *dataA++ -= v* *dataB++;           \
  }                                     \
}

#ifdef   __cplusplus
}
#endif

#endif /* ifndef _MATRIX_UTILS_H_ */
