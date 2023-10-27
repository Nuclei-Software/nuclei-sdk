/******************************************************************************
 * @file     riscv_math.h
 * @brief    Public header file for NMSIS DSP Library
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

/**
   \mainpage NMSIS DSP Software Library
   *
   * \section intro Introduction
   *
   * This user manual describes the NMSIS DSP software library,
   * a suite of common compute processing functions for use on RISC-V processor
   * based devices.
   *
   * The library is divided into a number of functions each covering a specific category:
   * - \ref groupMath "Basic math functions"
   * - \ref groupFastMath "Fast math functions"
   * - \ref groupCmplxMath "Complex math functions"
   * - \ref groupFilters "Filtering functions"
   * - \ref groupMatrix "Matrix functions"
   * - \ref groupTransforms "Transform functions"
   * - \ref groupController "Motor control functions"
   * - \ref groupStats "Statistical functions"
   * - \ref groupSupport "Support functions"
   * - \ref groupInterpolation "Interpolation functions"
   * - \ref groupSVM "Support Vector Machine functions (SVM)"
   * - \ref groupBayes "Bayes classifier functions"
   * - \ref groupDistance "Distance functions"
   * - \ref groupQuaternionMath "Quaternion functions"
   * - \ref groupWindow "Window functions"
   *
   * The library has generally separate functions for operating on 8-bit integers, 16-bit integers,
   * 32-bit integer and 32-bit floating-point values and 64-bit floating-point values.
   *
   * The library is providing vectorized versions of most algorithms for Helium
   * and of most f32 algorithms for Neon.
   *
   * When using a vectorized version, provide a little bit of padding after the end of
   * a buffer (3 words) because the vectorized code may read a little bit after the end
   * of a buffer. You don't have to modify your buffers but just ensure that the
   * end of buffer + padding is not outside of a memory region.
   *
   * NMSIS-DSP pack is containing an optional project : The NMSIS-DSP
   * Compute Graph. The documentation for this project is available
   * on the <a href="https://github.com/RISCV-software/NMSIS-DSP/blob/main/ComputeGraph/README.md">NMSIS-DSP github repository.</a>
   *
   * A Python wrapper is also available with a Python API as close as possible
   * to the C one. It can be used to start developing and testing an algorithm with NumPy and
   * SciPy before writing the C version. Is is available on <a href="https://pypi.org/project/nmsisdsp/">PyPI.org</a>.
   * It can be installed with : pip install nmsisdsp
   *
   *
   * \section using Using the Library
   *
   * The library is released in source form. It is strongly advised to compile the library using -Ofast to
   * have the best performances.
   *
   * \note Please refer to [NMSIS-DSP](../../../dsp/index.html)
   *
   * \section example Examples
   *
   * The library ships with a number of examples which demonstrate how to use the library functions.
   *
   * Toolchain Support
   * -----------------
   *
   * The library has been developed and tested with nuclei riscv gcc toolchain.
   *
   * Building the Library
   * --------------------
   *
   * In NMSIS repo, it contains a Makefile to rebuild libraries on nuclei riscv gcc toolchain in the <code>NMSIS/</code> folder.
   * * In *NMSIS* folder, you can run `make gen_dsp_lib` to build and install DSP library into **NMSIS/Library/DSP/GCC** folder.
   *
   * Preprocessor Macros
   * -------------------
   *
   * Each library project have different preprocessor macros controlled via CMakeLists.txt.
   *
   *
   */


/**
 * @defgroup groupExamples Examples
 */




#ifndef _RISCV_MATH_H
#define _RISCV_MATH_H


#include "riscv_math_types.h"
#include "riscv_math_memory.h"

#include "dsp/none.h"
#include "dsp/utils.h"

#include "dsp/basic_math_functions.h"
#include "dsp/interpolation_functions.h"
#include "dsp/bayes_functions.h"
#include "dsp/matrix_functions.h"
#include "dsp/complex_math_functions.h"
#include "dsp/statistics_functions.h"
#include "dsp/controller_functions.h"
#include "dsp/support_functions.h"
#include "dsp/distance_functions.h"
#include "dsp/svm_functions.h"
#include "dsp/fast_math_functions.h"
#include "dsp/transform_functions.h"
#include "dsp/filtering_functions.h"
#include "dsp/quaternion_math_functions.h"
#include "dsp/window_functions.h"

#if defined (RISCV_FLOAT16_SUPPORTED)
#include "riscv_math_f16.h"
#endif /* defined (RISCV_FLOAT16_SUPPORTED) */



#ifdef   __cplusplus
extern "C"
{
#endif




//#define TABLE_SPACING_Q31     0x400000
//#define TABLE_SPACING_Q15     0x80





#ifdef   __cplusplus
}
#endif


#endif /* _RISCV_MATH_H */

/**
 *
 * End of file.
 */
