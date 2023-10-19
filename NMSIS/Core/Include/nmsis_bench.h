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
                                printf("CSV, %s, %lu\n", #proc, (uint32_t)_bc_usecyc);

/** Mark stop of benchmark, start -> sample -> sample -> stop, and print the sum cycle of a proc */
#define BENCH_STOP(proc)        printf("CSV, %s, %lu\n", #proc, (uint32_t)_bc_sumcyc);

/** Show statistics of benchmark, format: STAT, proc, loopcnt, sumcyc */
#define BENCH_STAT(proc)        printf("STAT, %s, %lu, %lu\n", #proc, (uint32_t)_bc_lpcnt, (uint32_t)_bc_sumcyc);

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

// High performance monitor bench helpers
#ifndef DISABLE_NMSIS_HPM

/* Events type select */
#define EVENT_SEL_INSTRUCTION_COMMIT                                               0
#define EVENT_SEL_MEMORY_ACCESS                                                    1

/* Instruction commit events idx define*/
#define EVENT_INSTRUCTION_COMMIT_CYCLE_COUNT                                       1
#define EVENT_INSTRUCTION_COMMIT_RETIRED_COUNT                                     2
/* Integer load instruction (includes LR) */
#define EVENT_INSTRUCTION_COMMIT_INTEGER_LOAD                                      3
/* Integer store instruction (includes SC) */
#define EVENT_INSTRUCTION_COMMIT_INTEGER_STORE                                     4
/* Atomic memory operation (do not include LR and SC) */
#define EVENT_INSTRUCTION_COMMIT_ATOMIC_MEMORY_OPERATION                           5
/* System instruction */
#define EVENT_INSTRUCTION_COMMIT_SYSTEM                                            6
/* Integer computational instruction (excluding multiplication/division/remainder) */
#define EVENT_INSTRUCTION_COMMIT_INTEGER_COMPUTATIONAL                             7
#define EVENT_INSTRUCTION_COMMIT_CONDITIONAL_BRANCH                                8
#define EVENT_INSTRUCTION_COMMIT_TAKEN_CONDITIONAL_BRANCH                          9
#define EVENT_INSTRUCTION_COMMIT_JAL                                               10
#define EVENT_INSTRUCTION_COMMIT_JALR                                              11
#define EVENT_INSTRUCTION_COMMIT_RETURN                                            12
/* Control transfer instruction (CBR+JAL+JALR) */
#define EVENT_INSTRUCTION_COMMIT_CONTROL_TRANSFER                                  13
/* 14 Reseved */
#define EVENT_INSTRUCTION_COMMIT_INTEGER_MULTIPLICATION                            15
/* Integer division/remainder instruction */
#define EVENT_INSTRUCTION_COMMIT_INTEGER_DIVISION_REMAINDER                        16
#define EVENT_INSTRUCTION_COMMIT_FLOATING_POINT_LOAD                               17
#define EVENT_INSTRUCTION_COMMIT_FLOATING_POINT_STORE                              18
/* Floating-point addition/subtraction */
#define EVENT_INSTRUCTION_COMMIT_FLOATING_POINT_ADDITION_SUBTRACTION               19
#define EVENT_INSTRUCTION_COMMIT_FLOATING_POINT_MULTIPLICATION                     20
/* Floating-point fused multiply-add (FMADD, FMSUB, FNMSUB, FNMADD) */
#define EVENT_INSTRUCTION_COMMIT_FLOATING_POINT_FUSED_MULTIPLY_ADD_SUB             21
#define EVENT_INSTRUCTION_COMMIT_FLOATING_POINT_DIVISION_OR_SQUARE_ROOT            22
#define EVENT_INSTRUCTION_COMMIT_OTHER_FLOATING_POINT_INSTRUCTION                  23
#define EVENT_INSTRUCTION_COMMIT_CONDITIONAL_BRANCH_PREDICTION_FAIL                24
#define EVENT_INSTRUCTION_COMMIT_JAL_PREDICTION_FAIL                               25
#define EVENT_INSTRUCTION_COMMIT_JALR_PREDICTION_FAIL                              26

/* Memory access events idx define*/
#define EVENT_MEMORY_ACCESS_ICACHE_MISS                                            1
#define EVENT_MEMORY_ACCESS_DCACHE_MISS                                            2
#define EVENT_MEMORY_ACCESS_ITLB_MISS                                              3
#define EVENT_MEMORY_ACCESS_DTLB_MISS                                              4
#define EVENT_MEMORY_ACCESS_MAIN_DTLB_MISS                                         5

/* Enable the corresponding performance monitor counter increment for events in Machine/Supervisor/User Mode */
#define MSU_EVENT_ENABLE                                                           0x0F
#define MEVENT_EN                                                                  0x08
#define SEVENT_EN                                                                  0x02
#define UEVENT_EN                                                                  0x01

/** Declare high performance monitor counter idx benchmark required variables, need to be placed above all HPM_xxx macros in each c source code if HPM_xxx used */
#define HPM_DECLARE_VAR(idx)    static volatile uint64_t __hpm_sttcyc##idx, __hpm_endcyc##idx, __hpm_usecyc##idx, __hpm_sumcyc##idx, __hpm_lpcnt##idx, __hpm_val##idx;

#define HPM_SEL_ENABLE(ena)         (ena << 28)
#define HPM_SEL_EVENT(sel, idx)     ((sel) | (idx << 4))

/** Construct a event variable to be set(sel -> event_sel, idx -> event_idx, ena -> m/s/u_enable) */
#define HPM_EVENT(sel, idx, ena)    (HPM_SEL_ENABLE(ena) | HPM_SEL_EVENT(sel, idx))

/** Initialize high performance monitor environment, need to called in before other HPM_xxx macros are called */
#define HPM_INIT()              printf("High performance monitor initialized\n"); \
                                __prepare_bench_env();

/** Reset high performance benchmark for proc using counter which index is idx */
#define HPM_RESET(idx, proc, event)                  __hpm_sumcyc##idx = 0; __hpm_lpcnt##idx = 0;

/** Start to do high performance benchmark for proc, and record start hpm counter */
#define HPM_START(idx, proc, event)                  \
                                __hpm_val##idx = (event);                                   \
                                __set_hpm_event(idx, __hpm_val##idx);                       \
                                __set_hpm_counter(idx, 0);                                  \
                                __hpm_sttcyc##idx = __get_hpm_counter(idx);

/** Do high performance benchmark sample for proc, and sum it into sum counter */
#define HPM_SAMPLE(idx, proc, event)                 \
                                __hpm_endcyc##idx = __get_hpm_counter(idx);                 \
                                __hpm_usecyc##idx = __hpm_endcyc##idx - __hpm_sttcyc##idx;  \
                                __hpm_sumcyc##idx += __hpm_usecyc##idx;                     \
                                __hpm_lpcnt##idx += 1;

/** Mark end of high performance benchmark for proc, and calc used hpm counter value */
#define HPM_END(idx, proc, event)                    \
                                HPM_SAMPLE(idx, proc, event);                               \
                                printf("HPM%d:0x%x, %s, %lu\n", idx, event, #proc, (uint32_t)__hpm_usecyc##idx);

/** Mark stop of hpm benchmark, start -> sample -> sample -> stop, and print the sum cycle of a proc */
#define HPM_STOP(idx, proc, event)                   \
                                printf("HPM%d:0x%x, %s, %lu\n", idx, event, #proc, (uint32_t)__hpm_sumcyc##idx);

/** Show statistics of hpm benchmark, format: STATHPM#idx:event, proc, loopcnt, sumcyc */
#define HPM_STAT(idx, proc, event)                   \
                                printf("STATHPM%d:0x%x, %s, %lu, %lu\n", idx, event, #proc, (uint32_t)__hpm_lpcnt##idx, (uint32_t)__hpm_sumcyc##idx);

/** Get hpm benchmark use cycle for counter idx */
#define HPM_GET_USECYC(idx)             (__hpm_usecyc##idx)

/** Get hpm benchmark sum cycle for counter idx */
#define HPM_GET_SUMCYC(idx)             (__hpm_sumcyc##idx)

/** Get hpm benchmark loop count for counter idx */
#define HPM_GET_LPCNT(idx)              (__hpm_lpcnt##idx)

#else
#define HPM_DECLARE_VAR(idx)
#define HPM_EVENT(sel, idx, ena)
#define HPM_INIT()
#define HPM_RESET(idx, proc, event)
#define HPM_START(idx, proc, event)
#define HPM_SAMPLE(idx, proc, event)
#define HPM_END(idx, proc, event)
#define HPM_STOP(idx, proc, event)
#define HPM_STAT(idx, proc, event)
#define HPM_GET_USECYC(idx)        (0)
#define HPM_GET_SUMCYC(idx)        (0)
#define HPM_GET_LPCNT(idx)         (1)
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
