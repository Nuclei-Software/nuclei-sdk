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
#define BENCH_DECLARE_VAR()     static volatile uint64_t _bc_sttcyc, _bc_endcyc, _bc_usecyc, _bc_sumcyc, _bc_lpcnt, _bc_ercd;

/** Initialize benchmark environment, need to called in before other BENCH_xxx macros are called */
#define BENCH_INIT()            printf("Benchmark initialized\n"); \
                                __prepare_bench_env(); \
                                _bc_ercd = 0; _bc_sumcyc = 0;

/** Reset benchmark sum cycle and use cycle for proc */
#define BENCH_RESET(proc)       _bc_sumcyc = 0; _bc_usecyc = 0; _bc_lpcnt = 0; _bc_ercd = 0;

/** Start to do benchmark for proc, and record start cycle, and reset error code */
#define BENCH_START(proc)       _bc_ercd = 0; \
                                _bc_sttcyc = READ_CYCLE();

/** Sample a benchmark for proc, and record this start -> sample cost cycle, and accumulate it to sum cycle */
#define BENCH_SAMPLE(proc)      _bc_endcyc = READ_CYCLE(); \
                                _bc_usecyc = _bc_endcyc - _bc_sttcyc; \
                                _bc_sumcyc += _bc_usecyc; _bc_lpcnt += 1;

/** Mark end of benchmark for proc, and calc used cycle, and print it */
#define BENCH_END(proc)         BENCH_SAMPLE(proc); \
                                printf("CSV, %s, %lu\n", #proc, (unsigned long)_bc_usecyc);

/** Mark stop of benchmark, start -> sample -> sample -> stop, and print the sum cycle of a proc */
#define BENCH_STOP(proc)        printf("CSV, %s, %lu\n", #proc, (unsigned long)_bc_sumcyc);

/** Show statistics of benchmark, format: STAT, proc, loopcnt, sumcyc */
#define BENCH_STAT(proc)        printf("STAT, %s, %lu, %lu\n", #proc, (unsigned long)_bc_lpcnt, (unsigned long)_bc_sumcyc);

/** Get benchmark use cycle */
#define BENCH_GET_USECYC()      (_bc_usecyc)

/** Get benchmark sum cycle */
#define BENCH_GET_SUMCYC()      (_bc_sumcyc)

/** Get benchmark loop count */
#define BENCH_GET_LPCNT()       (_bc_lpcnt)

/** Mark benchmark for proc is errored */
#define BENCH_ERROR(proc)       _bc_ercd = 1;
/** Show the status of the benchmark */
#define BENCH_STATUS(proc)      if (_bc_ercd) { \
                                    printf("ERROR, %s\n", #proc); \
                                } else { \
                                    printf("SUCCESS, %s\n", #proc); \
                                }
#else
#define BENCH_DECLARE_VAR()     static volatile uint64_t _bc_ercd, _bc_lpcnt;
#define BENCH_INIT()            _bc_ercd = 0; __prepare_bench_env();
#define BENCH_RESET(proc)
#define BENCH_START(proc)       _bc_ercd = 0;
#define BENCH_SAMPLE(proc)      _bc_lpcnt += 1;
#define BENCH_END(proc)
#define BENCH_STOP(proc)
#define BENCH_STAT(proc)
#define BENCH_GET_USECYC()      (0)
#define BENCH_GET_SUMCYC()      (0)
#define BENCH_GET_LPCNT()       (_bc_lpcnt)
#define BENCH_ERROR(proc)       _bc_ercd = 1;
#define BENCH_STATUS(proc)      if (_bc_ercd) { \
                                    printf("ERROR, %s\n", #proc); \
                                } else { \
                                    printf("SUCCESS, %s\n", #proc); \
                                }

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
