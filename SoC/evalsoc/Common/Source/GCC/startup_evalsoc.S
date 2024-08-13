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
/******************************************************************************
 * \file     startup_evalsoc.S
 * \brief    NMSIS Nuclei N100 Core based Core Device Startup File for
 *  Nuclei Eval SoC which support Nuclei N/NX class cores
 * \version  V1.00
 * \date     17. Dec 2019
 *
 ******************************************************************************/

#include "riscv_encoding.h"

#include "cpufeature.h"

    .section .text.init
    .globl _start
    .type _start, @function
    .globl _reset_vector
    .type _reset_vector, @function

_reset_vector:
    .option push
    .option norvc
    .option arch,-zcmt,-zcmp,-zcb,-zca
    j _start

#ifdef CFG_HAS_EXCP
    j exc_entry
#endif
    .option pop
/**
 * Reset Handler called on controller reset
 */
_start:
    /* ===== Startup Stage 1 ===== */
    /* Disable Global Interrupt */
    csrc CSR_MSTATUS, MSTATUS_MIE

    /* Initialize GP and TP and jump table base when zcmt enabled */
    .option push
    .option norelax
    la gp, __global_pointer$
    la tp, __tls_base
#if defined(__riscv_zcmt)
    la t0, __jvt_base$
    csrw CSR_JVT, t0
#endif
    .option pop

    /* Set correct sp for current cpu */
    la sp, _sp

    /* mtvec and mtvt are readonly CSRs for N100, no need to initialize it */

    /* ===== Startup Stage 2 ===== */

    /* No FPU for N100 */

    /* Enable mcycle and minstret counter */
    csrci CSR_MCOUNTINHIBIT, 0x5

#if defined(SMP_CPU_CNT) && (SMP_CPU_CNT > 1)
    csrr a0, CSR_MHARTID
    li a1, BOOT_HARTID
    bne a0, a1, __skip_init
#endif

__init_common:
    /* ===== Startup Stage 3 ===== */
    /*
     * Load text section from CODE ROM to CODE RAM
     * when text LMA is different with VMA
     */
    la a0, _text_lma
    la a1, _text
    /* If text LMA and VMA are equal
     * then no need to copy text section */
    beq a0, a1, 2f
    la a2, _etext
    bgeu a1, a2, 2f

1:
    /* Load code section if necessary */
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, 1b
2:
    /* Load data section */
    la a0, _data_lma
    la a1, _data
    /* If data vma=lma, no need to copy */
    beq a0, a1, 2f
    la a2, _edata
    bgeu a1, a2, 2f
1:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, 1b
2:
    /* Clear bss section */
    la a0, __bss_start
    la a1, _end
    bgeu a0, a1, 2f
1:
    sw zero, (a0)
    addi a0, a0, 4
    bltu a0, a1, 1b
2:

    .size _start, . - _start

.globl _start_premain
.type _start_premain, @function
_start_premain:
    /*
     * Call vendor defined SystemInit to
     * initialize the micro-controller system
     * SystemInit will just be called by boot cpu
     */
    call SystemInit

    /*
     * Call C/C++ constructor start up code,
     * __libc_fini is defined in linker script,
     * so register_fini function will be called
     * and will run atexit (__libc_fini_array)
     * to do previous call atexit function
     */
    call __libc_init_array

    /* do pre-init steps before main */
    /* _premain_init will be called by each cpu
     * please make sure the implementation of __premain_int
     * considered this
     */
    call _premain_init

    /* ===== Call SMP Main Function  ===== */
    /* argc = argv = 0 */
    li a0, 0
    li a1, 0
#ifdef RTOS_RTTHREAD
    // Call entry function when using RT-Thread
    call entry
#else
    call main
#endif
    /* do post-main steps after main
     * this function will be called by each cpu */
    call _postmain_fini

1:
    wfi
    j 1b

    .size _start_premain, . - _start_premain
