/*
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

#ifndef __NMSIS_BENCH__
#define __NMSIS_BENCH__

/*!
 * @file     nmsis_bench.h
 * @brief    benchmark and helper related API for Nuclei N/NX Core
 */

#ifdef __cplusplus
 extern "C" {
#endif

#include "core_feature_base.h"
#include <stdio.h>

/**
 * \defgroup NMSIS_Core_Bench_Helpers   NMSIS Bench and Test Related Helper Functions
 * \ingroup  NMSIS_Core
 * \brief    Functions that used to do benchmark and test suite.
 * \details
 *
 * NMSIS benchmark and test related helper functions are provided to help do benchmark
 * and test case pass/fail assertion.
 *
 * If you want to do calculate cpu cycle cost of a process, you can use BENCH_xxx macros
 * defined in this.
 *
 * In a single c source code file, you should include `nmsis_bench.h`, and then you should place `BENCH_DECLARE_VAR();`
 * before call other BENCH_xxx macros. If you want to start to do benchmark, you should only call `BENCH_INIT();`
 * once in your source code, and then place `BENCH_START(proc_name);` and `BENCH_END(proc_name)` before
 * and after the process you want to measure. You can refer to `<nuclei-sdk>/application/baremetal/demo_dsp`
 * for how to use it.
 *
 * If you want to disable the benchmark calculation, you can place `#define DISABLE_NMSIS_BENCH`
 * before include `nmsis_bench.h`
 * 
 * If in your c test source code, you can add `NMSIS_TEST_PASS();` and `NMSIS_TEST_FAIL();` to mark c test
 * is pass or fail.
 *
 * @{
 */

/**
 * \brief   Prepare benchmark environment
 * \details
 * Prepare benchmark required environment, such as turn on necessary units
 * like vpu, cycle, instret counters, hpm counters
 */
__STATIC_FORCEINLINE void __prepare_bench_env(void)
{
#ifdef __riscv_vector
    __RV_CSR_SET(CSR_MSTATUS, MSTATUS_VS);
#endif
    __enable_all_counter();
}

#ifndef READ_CYCLE
/** Read run cycle of cpu */
#define READ_CYCLE              __get_rv_cycle
#endif

#ifndef DISABLE_NMSIS_BENCH

/** Declare benchmark required variables, need to be placed above all BENCH_xxx macros in each c source code if BENCH_xxx used */
#define BENCH_DECLARE_VAR()     static volatile uint64_t _bc_sttcyc, _bc_endcyc, _bc_usecyc, _bc_ercd;

/** Initialize benchmark environment, need to called in before other BENCH_xxx macros are called */
#define BENCH_INIT()            printf("Benchmark initialized\n"); \
                                __prepare_bench_env(); \
                                _bc_ercd = 0;

/** Start to do benchmark for proc, and record start cycle */
#define BENCH_START(proc)       _bc_ercd = 0; \
                                _bc_sttcyc = READ_CYCLE();

/** Mark end of benchmark for proc, and calc used cycle, and print it */
#define BENCH_END(proc)         _bc_endcyc = READ_CYCLE(); \
                                _bc_usecyc = _bc_endcyc - _bc_sttcyc; \
                                printf("CSV, %s, %lu\n", #proc, _bc_usecyc);

/** Mark benchmark for proc is errored */
#define BENCH_ERROR(proc)       _bc_ercd = 1;
/** Show the status of the benchmark */
#define BENCH_STATUS(proc)      if (_bc_ercd) { \
                                    printf("ERROR, %s\n", #proc); \
                                } else { \
                                    printf("SUCCESS, %s\n", #proc); \
                                }
#else
#define BENCH_DECLARE_VAR()     static volatile uint64_t _bc_ercd;
#define BENCH_INIT()            _bc_ercd = 0; __prepare_bench_env();
#define BENCH_START(proc)       _bc_ercd = 0;
#define BENCH_END(proc)
#define BENCH_ERROR(proc)       _bc_ercd = 1;
#define BENCH_STATUS(proc)      if (_bc_ercd) { \
                                    printf("ERROR, %s\n", #proc); \
                                } else { \
                                    printf("SUCCESS, %s\n", #proc); \
                                }

#endif

// High performance monitor bench helpers
#ifndef DISABLE_NMSIS_HPM
/** Declare high performance monitor counter idx benchmark required variables, need to be placed above all HPM_xxx macros in each c source code if HPM_xxx used */
#define HPM_DECLARE_VAR(idx)    static volatile uint64_t __hpm_sttcyc##idx, __hpm_endcyc##idx, __hpm_usecyc##idx, __hpm_val##idx;

#define HPM_SEL_ENABLE(ena)         (ena << 28)
#define HPM_SEL_EVENT(sel, idx)     ((sel) | (idx << 4))

/** Construct a event variable to be set(sel -> event_sel, idx -> event_idx, ena -> m/s/u_enable) */
#define HPM_EVENT(sel, idx, ena)    (HPM_SEL_ENABLE(ena) | HPM_SEL_EVENT(sel, idx))

/** Initialize high performance monitor environment, need to called in before other HPM_xxx macros are called */
#define HPM_INIT()              printf("High performance monitor initialized\n"); \
                                __prepare_bench_env();

/** Start to do high performance benchmark for proc, and record start hpm counter */
#define HPM_START(idx, proc, event)                  \
                                __hpm_val##idx = (event);                                   \
                                __set_hpm_event(idx, __hpm_val##idx);                       \
                                __set_hpm_counter(idx, 0);                                  \
                                __hpm_sttcyc##idx = __get_hpm_counter(idx);

/** Mark end of high performance benchmark for proc, and calc used hpm counter value */
#define HPM_END(idx, proc, event)                    \
                                __hpm_endcyc##idx = __get_hpm_counter(idx);                 \
                                __hpm_usecyc##idx = __hpm_endcyc##idx - __hpm_sttcyc##idx;  \
                                printf("HPM%d:0x%x, %s, %lu\n", idx, event, #proc, __hpm_usecyc##idx);

#else
#define HPM_DECLARE_VAR(idx)
#define HPM_EVENT(sel, idx, ena)
#define HPM_INIT()
#define HPM_START(idx, proc, event)
#define HPM_END(idx, proc, event)
#endif

// NMSIS Helpers
#ifndef DISABLE_NMSIS_HELPER
/** Mark test or application passed */
#define NMSIS_TEST_PASS()      printf("\nNMSIS_TEST_PASS\n");
/** Mark test or application failed */
#define NMSIS_TEST_FAIL()      printf("\nNMSIS_TEST_FAIL\n");
#else
#define NMSIS_TEST_PASS()
#define NMSIS_TEST_FAIL()
#endif

/** @} */ /* End of Doxygen Group NMSIS_Core_Bench_Helpers */
#ifdef __cplusplus
}
#endif
#endif /* __NMSIS_BENCH__ */
