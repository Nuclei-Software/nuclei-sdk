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
 * \file     startup_demosoc.S
 * \brief    NMSIS Nuclei N/NX Class Core based Core Device Startup File for
 *  Nuclei Demo SoC which support Nuclei N/NX class cores
 * \version  V1.00
 * \date     17. Dec 2019
 *
 ******************************************************************************/

#include "riscv_encoding.h"

.macro DECLARE_INT_HANDLER  INT_HDL_NAME
#if defined(__riscv_xlen) && (__riscv_xlen == 32)
    .word \INT_HDL_NAME
#else
    .dword \INT_HDL_NAME
#endif
.endm

    /*
     * Put the interrupt vectors in this section according to vector remapped or not:
     * .vtable: vector table's LMA and VMA are the same, it is not remapped
     * .vtable_ilm: vector table's LMA and VMA are different, it is remapped, and
     *              VECTOR_TABLE_REMAPPED need to be defined
     */
#if defined(VECTOR_TABLE_REMAPPED)
    .section .vtable_ilm
#else
    .section .vtable
#endif

    .weak  eclic_msip_handler
    .weak  eclic_mtip_handler
    .weak  eclic_irq19_handler
    .weak  eclic_irq20_handler
    .weak  eclic_irq21_handler
    .weak  eclic_irq22_handler
    .weak  eclic_irq23_handler
    .weak  eclic_irq24_handler
    .weak  eclic_irq25_handler
    .weak  eclic_irq26_handler
    .weak  eclic_irq27_handler
    .weak  eclic_irq28_handler
    .weak  eclic_irq29_handler
    .weak  eclic_irq30_handler
    .weak  eclic_irq31_handler
    .weak  eclic_irq32_handler
    .weak  eclic_irq33_handler
    .weak  eclic_irq34_handler
    .weak  eclic_irq35_handler
    .weak  eclic_irq36_handler
    .weak  eclic_irq37_handler
    .weak  eclic_irq38_handler
    .weak  eclic_irq39_handler
    .weak  eclic_irq40_handler
    .weak  eclic_irq41_handler
    .weak  eclic_irq42_handler
    .weak  eclic_irq43_handler
    .weak  eclic_irq44_handler
    .weak  eclic_irq45_handler
    .weak  eclic_irq46_handler
    .weak  eclic_irq47_handler
    .weak  eclic_irq48_handler
    .weak  eclic_irq49_handler
    .weak  eclic_irq50_handler
    .weak  eclic_irq51_handler
    .weak  eclic_irq52_handler
    .weak  eclic_irq53_handler
    .weak  eclic_irq54_handler
    .weak  eclic_irq55_handler
    .weak  eclic_irq56_handler
    .weak  eclic_irq57_handler
    .weak  eclic_irq58_handler
    .weak  eclic_irq59_handler
    .weak  eclic_irq60_handler
    .weak  eclic_irq61_handler
    .weak  eclic_irq62_handler
    .weak  eclic_irq63_handler
    .weak  eclic_irq64_handler
    .weak  eclic_irq65_handler
    .weak  eclic_irq66_handler
    .weak  eclic_irq67_handler
    .weak  eclic_irq68_handler

    .globl vector_base
    .type vector_base, @object
vector_base:
#ifndef VECTOR_TABLE_REMAPPED
    j _start                                                /* 0: Reserved, Jump to _start when reset for vector table not remapped cases.*/
    .align LOG_REGBYTES                                     /*    Need to align 4 byte for RV32, 8 Byte for RV64 */
#else
    DECLARE_INT_HANDLER     default_intexc_handler          /* 0: Reserved, default handler for vector table remapped cases */
#endif
    DECLARE_INT_HANDLER     default_intexc_handler          /* 1: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 2: Reserved */
    DECLARE_INT_HANDLER     eclic_msip_handler              /* 3: Machine software interrupt */

    DECLARE_INT_HANDLER     default_intexc_handler          /* 4: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 5: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 6: Reserved */
    DECLARE_INT_HANDLER     eclic_mtip_handler              /* 7: Machine timer interrupt */

    DECLARE_INT_HANDLER     default_intexc_handler          /* 8: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 9: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 10: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 11: Reserved */

    DECLARE_INT_HANDLER     default_intexc_handler          /* 12: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 13: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 14: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 15: Reserved */

    DECLARE_INT_HANDLER     default_intexc_handler          /* 16: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 17: Reserved */
    DECLARE_INT_HANDLER     default_intexc_handler          /* 18: Reserved */
    DECLARE_INT_HANDLER     eclic_irq19_handler             /* 19: Interrupt 19 */

    DECLARE_INT_HANDLER     eclic_irq20_handler             /* 20: Interrupt 20 */
    DECLARE_INT_HANDLER     eclic_irq21_handler             /* 21: Interrupt 21 */
    DECLARE_INT_HANDLER     eclic_irq22_handler             /* 22: Interrupt 22 */
    DECLARE_INT_HANDLER     eclic_irq23_handler             /* 23: Interrupt 23 */

    DECLARE_INT_HANDLER     eclic_irq24_handler             /* 24: Interrupt 24 */
    DECLARE_INT_HANDLER     eclic_irq25_handler             /* 25: Interrupt 25 */
    DECLARE_INT_HANDLER     eclic_irq26_handler             /* 26: Interrupt 26 */
    DECLARE_INT_HANDLER     eclic_irq27_handler             /* 27: Interrupt 27 */

    DECLARE_INT_HANDLER     eclic_irq28_handler             /* 28: Interrupt 28 */
    DECLARE_INT_HANDLER     eclic_irq29_handler             /* 29: Interrupt 29 */
    DECLARE_INT_HANDLER     eclic_irq30_handler             /* 30: Interrupt 30 */
    DECLARE_INT_HANDLER     eclic_irq31_handler             /* 31: Interrupt 31 */

    DECLARE_INT_HANDLER     eclic_irq32_handler             /* 32: Interrupt 32 */
    DECLARE_INT_HANDLER     eclic_irq33_handler             /* 33: Interrupt 33 */
    DECLARE_INT_HANDLER     eclic_irq34_handler             /* 34: Interrupt 34 */
    DECLARE_INT_HANDLER     eclic_irq35_handler             /* 35: Interrupt 35 */

    DECLARE_INT_HANDLER     eclic_irq36_handler             /* 36: Interrupt 36 */
    DECLARE_INT_HANDLER     eclic_irq37_handler             /* 37: Interrupt 37 */
    DECLARE_INT_HANDLER     eclic_irq38_handler             /* 38: Interrupt 38 */
    DECLARE_INT_HANDLER     eclic_irq39_handler             /* 39: Interrupt 39 */

    DECLARE_INT_HANDLER     eclic_irq40_handler             /* 40: Interrupt 40 */
    DECLARE_INT_HANDLER     eclic_irq41_handler             /* 41: Interrupt 41 */
    DECLARE_INT_HANDLER     eclic_irq42_handler             /* 42: Interrupt 42 */
    DECLARE_INT_HANDLER     eclic_irq43_handler             /* 43: Interrupt 43 */

    DECLARE_INT_HANDLER     eclic_irq44_handler             /* 44: Interrupt 44 */
    DECLARE_INT_HANDLER     eclic_irq45_handler             /* 45: Interrupt 45 */
    DECLARE_INT_HANDLER     eclic_irq46_handler             /* 46: Interrupt 46 */
    DECLARE_INT_HANDLER     eclic_irq47_handler             /* 47: Interrupt 47 */

    DECLARE_INT_HANDLER     eclic_irq48_handler             /* 48: Interrupt 48 */
    DECLARE_INT_HANDLER     eclic_irq49_handler             /* 49: Interrupt 49 */
    DECLARE_INT_HANDLER     eclic_irq50_handler             /* 50: Interrupt 50 */
    DECLARE_INT_HANDLER     eclic_irq51_handler             /* 51: Interrupt 51 */

    DECLARE_INT_HANDLER     eclic_irq52_handler             /* 52: Interrupt 52 */
    DECLARE_INT_HANDLER     eclic_irq53_handler             /* 53: Interrupt 53 */
    DECLARE_INT_HANDLER     eclic_irq54_handler             /* 54: Interrupt 54 */
    DECLARE_INT_HANDLER     eclic_irq55_handler             /* 55: Interrupt 55 */

    DECLARE_INT_HANDLER     eclic_irq56_handler             /* 56: Interrupt 56 */
    DECLARE_INT_HANDLER     eclic_irq57_handler             /* 57: Interrupt 57 */
    DECLARE_INT_HANDLER     eclic_irq58_handler             /* 58: Interrupt 58 */
    DECLARE_INT_HANDLER     eclic_irq59_handler             /* 59: Interrupt 59 */

    DECLARE_INT_HANDLER     eclic_irq60_handler             /* 60: Interrupt 60 */
    DECLARE_INT_HANDLER     eclic_irq61_handler             /* 61: Interrupt 61 */
    DECLARE_INT_HANDLER     eclic_irq62_handler             /* 62: Interrupt 62 */
    DECLARE_INT_HANDLER     eclic_irq63_handler             /* 63: Interrupt 63 */

    DECLARE_INT_HANDLER     eclic_irq64_handler             /* 64: Interrupt 64 */
    DECLARE_INT_HANDLER     eclic_irq65_handler             /* 65: Interrupt 65 */
    DECLARE_INT_HANDLER     eclic_irq66_handler             /* 66: Interrupt 66 */
    DECLARE_INT_HANDLER     eclic_irq67_handler             /* 67: Interrupt 67 */

    DECLARE_INT_HANDLER     eclic_irq68_handler             /* 68: Interrupt 68 */

    .section .init

    .globl _start
    .type _start, @function

/**
 * Reset Handler called on controller reset
 */
_start:
    /* ===== Startup Stage 1 ===== */
    /* Disable Global Interrupt */
    csrc CSR_MSTATUS, MSTATUS_MIE

    /* Initialize GP and Stack Pointer SP */
    .option push
    .option norelax
    la gp, __global_pointer$
    la tp, __tls_base
    .option pop
    la sp, _sp

    /*
     * Set the the NMI base mnvec to share
     * with mtvec by setting CSR_MMISC_CTL
     * bit 9 NMI_CAUSE_FFF to 1
     */
    li t0, MMISC_CTL_NMI_CAUSE_FFF
    csrs CSR_MMISC_CTL, t0

    /*
     * Intialize ECLIC vector interrupt
     * base address mtvt to vector_base
     */
    la t0, vector_base
    csrw CSR_MTVT, t0

    /*
     * Set ECLIC non-vector entry to be controlled
     * by mtvt2 CSR register.
     * Intialize ECLIC non-vector interrupt
     * base address mtvt2 to irq_entry.
     */
    la t0, irq_entry
    csrw CSR_MTVT2, t0
    csrs CSR_MTVT2, 0x1

    /*
     * Set Exception Entry MTVEC to early_exc_entry
     * Due to settings above, Exception and NMI
     * will share common entry.
     * This early_exc_entry is only used during early
     * boot stage before main
     */
    la t0, early_exc_entry
    csrw CSR_MTVEC, t0

    /* Set the interrupt processing mode to ECLIC mode */
    li t0, 0x3f
    csrc CSR_MTVEC, t0
    csrs CSR_MTVEC, 0x3

    /* ===== Startup Stage 2 ===== */

#if defined(__riscv_flen) && __riscv_flen > 0
    /* Enable FPU */
    li t0, MSTATUS_FS
    csrs mstatus, t0
    csrw fcsr, x0
#endif

    /* Enable mcycle and minstret counter */
    csrci CSR_MCOUNTINHIBIT, 0x5

    /* ===== Startup Stage 3 ===== */
    /*
     * Load code section from FLASH to ILM
     * when code LMA is different with VMA
     */
    la a0, _ilm_lma
    la a1, _ilm
    /* If the ILM phy-address same as the logic-address, then quit */
    beq a0, a1, 2f
    la a2, _eilm
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

    /*
     * Call vendor defined SystemInit to
     * initialize the micro-controller system
     */
    call SystemInit

    /* Call global constructors */
    la a0, __libc_fini_array
    call atexit
    /* Call C/C++ constructor start up code */
    call __libc_init_array

    /* do pre-init steps before main */
    call _premain_init

    /*
     * When all initialization steps done
     * set exception entry to correct exception
     * entry and jump to main.
     */
    la t0, exc_entry
    csrw CSR_MTVEC, t0

    /* ===== Call Main Function  ===== */
    /* argc = argv = 0 */
    li a0, 0
    li a1, 0

#ifdef RTOS_RTTHREAD
    // Call entry function when using RT-Thread
    call entry
#else
    call main
#endif
    /* do post-main steps after main */
    call _postmain_fini

1:
    j 1b

/* Early boot exception entry before main */
.align 6
.global early_exc_entry
early_exc_entry:
    wfi
    j early_exc_entry
