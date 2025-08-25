
/*
 * FreeRTOS Kernel Port For Nuclei RISC-V Processor
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#include "riscv_encoding.h"

#ifndef __riscv_32e
#define portRegNum          32
#else
#define portRegNum          14
#endif

#define portCONTEXT_SIZE    ( portRegNum * REGBYTES )

.section    .text.entry
.align 8

// If you want to use SMP freertos
// configNUMBER_OF_CORES must be defined to the number of cores in your system.
// If you define configNUMBER_OF_CORES using -D option, you must make sure that
// asm option must contains this macro defined
#ifndef configNUMBER_OF_CORES
#define configNUMBER_OF_CORES   1
#endif

.extern xPortTaskSwitch
#if ( configNUMBER_OF_CORES == 1 )
.extern pxCurrentTCB
#else
.extern pxCurrentTCBs
#endif
.global prvPortStartFirstTask

/**
 * \brief  Global interrupt disabled
 * \details
 *  This function disable global interrupt.
 * \remarks
 *  - All the interrupt requests will be ignored by CPU.
 */
.macro DISABLE_MIE
    csrc CSR_MSTATUS, MSTATUS_MIE
.endm

/**
 * \brief  Macro for context save
 * \details
 * This macro save ABI defined caller saved registers in the stack.
 * \remarks
 * - This Macro could use to save context when you enter to interrupt
 * or exception
*/
/* Save caller registers */
.macro SAVE_CONTEXT
    csrrw sp, CSR_MSCRATCHCSWL, sp
    /* Allocate stack space for context saving */
#ifndef __riscv_32e
    addi sp, sp, -20*REGBYTES
#else
    addi sp, sp, -14*REGBYTES
#endif /* __riscv_32e */

    STORE x1, 0*REGBYTES(sp)
    STORE x4, 1*REGBYTES(sp)
    STORE x5, 2*REGBYTES(sp)
    STORE x6, 3*REGBYTES(sp)
    STORE x7, 4*REGBYTES(sp)
    STORE x10, 5*REGBYTES(sp)
    STORE x11, 6*REGBYTES(sp)
    STORE x12, 7*REGBYTES(sp)
    STORE x13, 8*REGBYTES(sp)
    STORE x14, 9*REGBYTES(sp)
    STORE x15, 10*REGBYTES(sp)
#ifndef __riscv_32e
    STORE x16, 14*REGBYTES(sp)
    STORE x17, 15*REGBYTES(sp)
    STORE x28, 16*REGBYTES(sp)
    STORE x29, 17*REGBYTES(sp)
    STORE x30, 18*REGBYTES(sp)
    STORE x31, 19*REGBYTES(sp)
#endif /* __riscv_32e */
.endm

/**
 * \brief  Macro for restore caller registers
 * \details
 * This macro restore ABI defined caller saved registers from stack.
 * \remarks
 * - You could use this macro to restore context before you want return
 * from interrupt or exeception
 */
/* Restore caller registers */
.macro RESTORE_CONTEXT
    LOAD x1, 0*REGBYTES(sp)
    LOAD x4, 1*REGBYTES(sp)
    LOAD x5, 2*REGBYTES(sp)
    LOAD x6, 3*REGBYTES(sp)
    LOAD x7, 4*REGBYTES(sp)
    LOAD x10, 5*REGBYTES(sp)
    LOAD x11, 6*REGBYTES(sp)
    LOAD x12, 7*REGBYTES(sp)
    LOAD x13, 8*REGBYTES(sp)
    LOAD x14, 9*REGBYTES(sp)
    LOAD x15, 10*REGBYTES(sp)
#ifndef __riscv_32e
    LOAD x16, 14*REGBYTES(sp)
    LOAD x17, 15*REGBYTES(sp)
    LOAD x28, 16*REGBYTES(sp)
    LOAD x29, 17*REGBYTES(sp)
    LOAD x30, 18*REGBYTES(sp)
    LOAD x31, 19*REGBYTES(sp)

    /* De-allocate the stack space */
    addi sp, sp, 20*REGBYTES
#else
    /* De-allocate the stack space */
    addi sp, sp, 14*REGBYTES
#endif /* __riscv_32e */
    csrrw sp, CSR_MSCRATCHCSWL, sp
.endm

/**
 * \brief  Macro for save necessary CSRs to stack
 * \details
 * This macro store MCAUSE, MEPC, MSUBM to stack.
 */
.macro SAVE_CSR_CONTEXT
    /* Store CSR mcause to stack using pushmcause */
    csrrwi  x0, CSR_PUSHMCAUSE, 11
    /* Store CSR mepc to stack using pushmepc */
    csrrwi  x0, CSR_PUSHMEPC, 12
    /* Store CSR msub to stack using pushmsub */
    csrrwi  x0, CSR_PUSHMSUBM, 13
.endm

/**
 * \brief  Macro for restore necessary CSRs from stack
 * \details
 * This macro restore MSUBM, MEPC, MCAUSE from stack.
 */
.macro RESTORE_CSR_CONTEXT
    LOAD x5,  13*REGBYTES(sp)
    csrw CSR_MSUBM, x5
    LOAD x5,  12*REGBYTES(sp)
    csrw CSR_MEPC, x5
    LOAD x5,  11*REGBYTES(sp)
    csrw CSR_MCAUSE, x5
.endm

/**
 * \brief  Exception/NMI Entry
 * \details
 * This function provide common entry functions for exception/nmi.
 * \remarks
 * This function provide a default exception/nmi entry.
 * ABI defined caller save register and some CSR registers
 * to be saved before enter interrupt handler and be restored before return.
 */
.section .text.trap
/* In CLIC mode, the exeception entry must be 64bytes aligned */
.align 6
.global exc_entry
.type exc_entry, @function
exc_entry:
    /* Save the caller saving registers (context) */
    SAVE_CONTEXT
    /* Save the necessary CSR registers */
    SAVE_CSR_CONTEXT

    /*
     * Set the exception handler function arguments
     * argument 1: mcause value
     * argument 2: current stack point(SP) value
     */
    csrr a0, mcause
    mv a1, sp
    /*
     * TODO: Call the exception handler function
     * By default, the function template is provided in
     * system_Device.c, you can adjust it as you want
     */
    call core_exception_handler

    /* Restore the necessary CSR registers */
    RESTORE_CSR_CONTEXT
    /* Restore the caller saving registers (context) */
    RESTORE_CONTEXT

    /* Return to regular code */
    mret

    .size exc_entry, . - exc_entry

/**
 * \brief  Non-Vector Interrupt Entry
 * \details
 * This function provide common entry functions for handling
 * non-vector interrupts
 * \remarks
 * This function provide a default non-vector interrupt entry.
 * ABI defined caller save register and some CSR registers need
 * to be saved before enter interrupt handler and be restored before return.
 */
.section      .text.irq
/* In CLIC mode, the interrupt entry must be 4bytes aligned */
.align 2
.global irq_entry
.type irq_entry, @function
/* This label will be set to MTVT2 register */
irq_entry:
    /* Save the caller saving registers (context) */
    SAVE_CONTEXT
    /* Save the necessary CSR registers */
    SAVE_CSR_CONTEXT

    /* This special CSR read/write operation, which is actually
     * claim the CLIC to find its pending highest ID, if the ID
     * is not 0, then automatically enable the mstatus.MIE, and
     * jump to its vector-entry-label, and update the link register
     */
    csrrw ra, CSR_JALMNXTI, ra

    /* Critical section with interrupts disabled */
    DISABLE_MIE

    /* Restore the necessary CSR registers */
    RESTORE_CSR_CONTEXT
    /* Restore the caller saving registers (context) */
    RESTORE_CONTEXT

    /* Return to regular code */
    mret

    .size irq_entry, . - irq_entry

/* Default Handler for Exceptions / Interrupts */
.global default_intexc_handler
.type default_intexc_handler, @function
Undef_Handler:
default_intexc_handler:
1:
    j 1b

    .size default_intexc_handler, . - default_intexc_handler

/* Start the first task.  This also clears the bit that indicates the FPU is
    in use in case the FPU was used before the scheduler was started - which
    would otherwise result in the unnecessary leaving of space in the stack
    for lazy saving of FPU registers. */
.type prvPortStartFirstTask, @function
.align 3
prvPortStartFirstTask:
    /* Setup Interrupt Stack using
       The stack that was used by main()
       before the scheduler is started is
       no longer required after the scheduler is started.
       Interrupt stack pointer is stored in CSR_MSCRATCH */
#if defined(SMP_CPU_CNT) && (SMP_CPU_CNT > 1)
    /* get correct sp for each cpu
     * each stack size is __STACK_SIZE
     * defined in linker script */
    lui t1, %hi(__STACK_SIZE)
    addi t1, t1, %lo(__STACK_SIZE)
    la t0, _sp
    csrr a0, CSR_MHARTID
    andi a0, a0, 0xFF
    li a1, 0
1:
    beq a0, a1, 2f
    sub t0, t0, t1
    addi a1, a1, 1
    j 1b
2:
#else
    la t0, _sp // TODO need to consider SMP case
#endif
    csrw CSR_MSCRATCH, t0

#if ( configNUMBER_OF_CORES == 1 )
    LOAD t0, pxCurrentTCB           /* Load pxCurrentTCB. */
#else
    la t0, pxCurrentTCBs            /* Load pxCurrentTCBs[core] */
    csrr t1, CSR_MHARTID
    slli t1, t1, LOG_REGBYTES
    add t0, t0, t1
    LOAD t0, 0(t0)
#endif
    LOAD sp, 0x0(t0)                /* Read sp from first TCB member */

    /* Pop PC from stack and set MEPC */
    LOAD t0,  0  * REGBYTES(sp)
    csrw CSR_MEPC, t0
    /* Pop mstatus from stack and set it */
    LOAD t0,  (portRegNum - 1)  * REGBYTES(sp)
    csrw CSR_MSTATUS, t0
    /* Interrupt still disable here */
    /* Restore Registers from Stack */
    LOAD x1,  1  * REGBYTES(sp)    /* RA */
    LOAD x5,  2  * REGBYTES(sp)
    LOAD x6,  3  * REGBYTES(sp)
    LOAD x7,  4  * REGBYTES(sp)
    LOAD x8,  5  * REGBYTES(sp)
    LOAD x9,  6  * REGBYTES(sp)
    LOAD x10, 7  * REGBYTES(sp)
    LOAD x11, 8  * REGBYTES(sp)
    LOAD x12, 9  * REGBYTES(sp)
    LOAD x13, 10 * REGBYTES(sp)
    LOAD x14, 11 * REGBYTES(sp)
    LOAD x15, 12 * REGBYTES(sp)
#ifndef __riscv_32e
    LOAD x16, 13 * REGBYTES(sp)
    LOAD x17, 14 * REGBYTES(sp)
    LOAD x18, 15 * REGBYTES(sp)
    LOAD x19, 16 * REGBYTES(sp)
    LOAD x20, 17 * REGBYTES(sp)
    LOAD x21, 18 * REGBYTES(sp)
    LOAD x22, 19 * REGBYTES(sp)
    LOAD x23, 20 * REGBYTES(sp)
    LOAD x24, 21 * REGBYTES(sp)
    LOAD x25, 22 * REGBYTES(sp)
    LOAD x26, 23 * REGBYTES(sp)
    LOAD x27, 24 * REGBYTES(sp)
    LOAD x28, 25 * REGBYTES(sp)
    LOAD x29, 26 * REGBYTES(sp)
    LOAD x30, 27 * REGBYTES(sp)
    LOAD x31, 28 * REGBYTES(sp)
#endif

    addi sp, sp, portCONTEXT_SIZE

    mret

    .size prvPortStartFirstTask, . - prvPortStartFirstTask

.align 2
.global eclic_msip_handler
.type eclic_msip_handler, @function
eclic_msip_handler:
#if defined(__riscv_flen)
    addi sp, sp, -(2 * REGBYTES)
    STORE t0, 0 * REGBYTES(sp)
    STORE t1, 1 * REGBYTES(sp)
_save_fp_stack:
    csrr t0, CSR_MSTATUS
    li t1, MSTATUS_FS_DIRTY
    and t0,t0,t1
#if defined(__riscv_vector)
    bne t0, t1, _save_vector_stack
#else
    bne t0, t1, _save_integer_stack
#endif
    LOAD t0, 0 * REGBYTES(sp)
    LOAD t1, 1 * REGBYTES(sp)
    addi sp, sp, (2 * REGBYTES)

    /* Save fp registers and fcsr and 3 reserved reg space, make sure 16 bytes aligned */
    addi sp, sp, -(5 * REGBYTES)
    STORE t0,   0 * REGBYTES(sp)
    csrr  t0, CSR_FCSR
    STORE t0,   1 * REGBYTES(sp)
    LOAD t0, 0 * REGBYTES(sp)
    addi sp, sp, (1 * REGBYTES)
    addi sp, sp, -(32 * FPREGBYTES)
    FPSTORE f0 , 0  * FPREGBYTES(sp)
    FPSTORE f1 , 1  * FPREGBYTES(sp)
    FPSTORE f2 , 2  * FPREGBYTES(sp)
    FPSTORE f3 , 3  * FPREGBYTES(sp)
    FPSTORE f4 , 4  * FPREGBYTES(sp)
    FPSTORE f5 , 5  * FPREGBYTES(sp)
    FPSTORE f6 , 6  * FPREGBYTES(sp)
    FPSTORE f7 , 7  * FPREGBYTES(sp)
    FPSTORE f8 , 8  * FPREGBYTES(sp)
    FPSTORE f9 , 9  * FPREGBYTES(sp)
    FPSTORE f10, 10 * FPREGBYTES(sp)
    FPSTORE f11, 11 * FPREGBYTES(sp)
    FPSTORE f12, 12 * FPREGBYTES(sp)
    FPSTORE f13, 13 * FPREGBYTES(sp)
    FPSTORE f14, 14 * FPREGBYTES(sp)
    FPSTORE f15, 15 * FPREGBYTES(sp)
    FPSTORE f16, 16 * FPREGBYTES(sp)
    FPSTORE f17, 17 * FPREGBYTES(sp)
    FPSTORE f18, 18 * FPREGBYTES(sp)
    FPSTORE f19, 19 * FPREGBYTES(sp)
    FPSTORE f20, 20 * FPREGBYTES(sp)
    FPSTORE f21, 21 * FPREGBYTES(sp)
    FPSTORE f22, 22 * FPREGBYTES(sp)
    FPSTORE f23, 23 * FPREGBYTES(sp)
    FPSTORE f24, 24 * FPREGBYTES(sp)
    FPSTORE f25, 25 * FPREGBYTES(sp)
    FPSTORE f26, 26 * FPREGBYTES(sp)
    FPSTORE f27, 27 * FPREGBYTES(sp)
    FPSTORE f28, 28 * FPREGBYTES(sp)
    FPSTORE f29, 29 * FPREGBYTES(sp)
    FPSTORE f30, 30 * FPREGBYTES(sp)
    FPSTORE f31, 31 * FPREGBYTES(sp)
#endif
#if defined(__riscv_vector)
    addi sp, sp, -(2 * REGBYTES)
    STORE t0, 0 * REGBYTES(sp)
    STORE t1, 1 * REGBYTES(sp)
_save_vector_stack:
    csrr t0, CSR_MSTATUS
    li t1, MSTATUS_VS_DIRTY
    and t0,t0,t1
    bne t0, t1, _save_integer_stack
    LOAD t0, 0 * REGBYTES(sp)
    LOAD t1, 1 * REGBYTES(sp)
    addi sp, sp, (2 * REGBYTES)

    /* Save vector registers and vtype/vl/vstart/vcsr */
    addi sp, sp, -(6 * REGBYTES)
    STORE t0,   0 * REGBYTES(sp)
    STORE t1,   1 * REGBYTES(sp)
    csrr  t0, CSR_VSTART
    STORE t0,   2 * REGBYTES(sp)
    csrr  t0, CSR_VTYPE
    STORE t0,   3 * REGBYTES(sp)
    csrr  t0, CSR_VL
    STORE t0,   4 * REGBYTES(sp)
    csrr  t0, CSR_VCSR
    STORE t0,   5 * REGBYTES(sp)

    mv t1, sp
    csrr  t0, CSR_VLENB
    slli t0, t0, 5
    /* t1 is the new temp stack where t0, t1 saved */
    sub t1, t1, t0
    LOAD t0, 0 * REGBYTES(sp)
    STORE t0,   0 * REGBYTES(t1)
    LOAD t0, 1 * REGBYTES(sp)
    STORE t0,   1 * REGBYTES(t1)
    mv sp, t1
    /* new sp -> t1 to save vector registers */
    addi t1, t1, (2 * REGBYTES)
    vsetvli t0, x0, e8, m8, ta, ma
    vse8.v v0, (t1)
    add t1, t1, t0
    vse8.v v8, (t1)
    add t1, t1, t0
    vse8.v v16, (t1)
    add t1, t1, t0
    vse8.v v24, (t1)
    LOAD t0, 0 * REGBYTES(sp)
    LOAD t1, 1 * REGBYTES(sp)
    addi sp, sp, (2 * REGBYTES)
#endif

#if defined(__riscv_flen) || defined(__riscv_vector)
    addi sp, sp, -(2 * REGBYTES)
    STORE t0, 0 * REGBYTES(sp)
    STORE t1, 1 * REGBYTES(sp)
_save_integer_stack:
    LOAD t0, 0 * REGBYTES(sp)
    LOAD t1, 1 * REGBYTES(sp)
    addi sp, sp, (2 * REGBYTES)
#endif
    addi sp, sp, -portCONTEXT_SIZE
    STORE x1,  1  * REGBYTES(sp)    /* RA */
    STORE x5,  2  * REGBYTES(sp)
    STORE x6,  3  * REGBYTES(sp)
    STORE x7,  4  * REGBYTES(sp)
    STORE x8,  5  * REGBYTES(sp)
    STORE x9,  6  * REGBYTES(sp)
    STORE x10, 7  * REGBYTES(sp)
    STORE x11, 8  * REGBYTES(sp)
    STORE x12, 9  * REGBYTES(sp)
    STORE x13, 10 * REGBYTES(sp)
    STORE x14, 11 * REGBYTES(sp)
    STORE x15, 12 * REGBYTES(sp)
#ifndef __riscv_32e
    STORE x16, 13 * REGBYTES(sp)
    STORE x17, 14 * REGBYTES(sp)
    STORE x18, 15 * REGBYTES(sp)
    STORE x19, 16 * REGBYTES(sp)
    STORE x20, 17 * REGBYTES(sp)
    STORE x21, 18 * REGBYTES(sp)
    STORE x22, 19 * REGBYTES(sp)
    STORE x23, 20 * REGBYTES(sp)
    STORE x24, 21 * REGBYTES(sp)
    STORE x25, 22 * REGBYTES(sp)
    STORE x26, 23 * REGBYTES(sp)
    STORE x27, 24 * REGBYTES(sp)
    STORE x28, 25 * REGBYTES(sp)
    STORE x29, 26 * REGBYTES(sp)
    STORE x30, 27 * REGBYTES(sp)
    STORE x31, 28 * REGBYTES(sp)
#endif
    /* Push mstatus to stack */
    csrr t0, CSR_MSTATUS
    STORE t0,  (portRegNum - 1)  * REGBYTES(sp)

    /* Push additional registers */

    /* Store sp to task stack */
#if ( configNUMBER_OF_CORES == 1 )
    LOAD t0, pxCurrentTCB           /* Load pxCurrentTCB. */
#else
    la t0, pxCurrentTCBs            /* Load pxCurrentTCBs[core] */
    csrr t1, CSR_MHARTID
    slli t1, t1, LOG_REGBYTES
    add t0, t0, t1
    LOAD t0, 0(t0)
#endif
    STORE sp, 0(t0)

    csrr t0, CSR_MEPC
    STORE t0, 0(sp)
    jal xPortTaskSwitch

    /* Switch task context */
#if ( configNUMBER_OF_CORES == 1 )
    LOAD t0, pxCurrentTCB           /* Load pxCurrentTCB. */
#else
    la t0, pxCurrentTCBs            /* Load pxCurrentTCBs[core] */
    csrr t1, CSR_MHARTID
    slli t1, t1, LOG_REGBYTES
    add t0, t0, t1
    LOAD t0, 0(t0)
#endif
    LOAD sp, 0x0(t0)                /* Read sp from first TCB member */

    /* Pop PC from stack and set MEPC */
    LOAD t0,  0  * REGBYTES(sp)
    csrw CSR_MEPC, t0
    /* Pop additional registers */

    /* Pop mstatus from stack and set it */
    LOAD t0,  (portRegNum - 1)  * REGBYTES(sp)
    csrw CSR_MSTATUS, t0
    /* Interrupt still disable here */
    /* Restore Registers from Stack */
    LOAD x1,  1  * REGBYTES(sp)    /* RA */
    LOAD x5,  2  * REGBYTES(sp)
    LOAD x6,  3  * REGBYTES(sp)
    LOAD x7,  4  * REGBYTES(sp)
    LOAD x8,  5  * REGBYTES(sp)
    LOAD x9,  6  * REGBYTES(sp)
    LOAD x10, 7  * REGBYTES(sp)
    LOAD x11, 8  * REGBYTES(sp)
    LOAD x12, 9  * REGBYTES(sp)
    LOAD x13, 10 * REGBYTES(sp)
    LOAD x14, 11 * REGBYTES(sp)
    LOAD x15, 12 * REGBYTES(sp)
#ifndef __riscv_32e
    LOAD x16, 13 * REGBYTES(sp)
    LOAD x17, 14 * REGBYTES(sp)
    LOAD x18, 15 * REGBYTES(sp)
    LOAD x19, 16 * REGBYTES(sp)
    LOAD x20, 17 * REGBYTES(sp)
    LOAD x21, 18 * REGBYTES(sp)
    LOAD x22, 19 * REGBYTES(sp)
    LOAD x23, 20 * REGBYTES(sp)
    LOAD x24, 21 * REGBYTES(sp)
    LOAD x25, 22 * REGBYTES(sp)
    LOAD x26, 23 * REGBYTES(sp)
    LOAD x27, 24 * REGBYTES(sp)
    LOAD x28, 25 * REGBYTES(sp)
    LOAD x29, 26 * REGBYTES(sp)
    LOAD x30, 27 * REGBYTES(sp)
    LOAD x31, 28 * REGBYTES(sp)
#endif

    addi sp, sp, portCONTEXT_SIZE

#if defined(__riscv_vector)
    /* Pop vector stack if needed */
    addi sp, sp, -(2 * REGBYTES)
    STORE t0, 0 * REGBYTES(sp)
    STORE t1, 1 * REGBYTES(sp)
_restore_vector_stack:
    csrr t0, CSR_MSTATUS
    li t1, MSTATUS_VS_DIRTY
    and t0,t0,t1
#if defined(__riscv_flen)
    bne t0, t1, _restore_fp_stack
#else
    bne t0, t1, _restore_done
#endif
    LOAD t0, 0 * REGBYTES(sp)
    LOAD t1, 1 * REGBYTES(sp)
    addi sp, sp, (2 * REGBYTES)

    /* pop vector registers and vtype/vl/vstart/vcsr */
    addi sp, sp, -(3 * REGBYTES)
    STORE t0,   0 * REGBYTES(sp)
    STORE t1,   1 * REGBYTES(sp)
    STORE t2,   2 * REGBYTES(sp)
    mv t2, sp
    /* temp regs are stored in t2 stack top */
    addi sp, sp, (3 * REGBYTES)
    vsetvli t0, x0, e8, m8, ta, ma
    vle8.v v0, (sp)
    add sp, sp, t0
    vle8.v v8, (sp)
    add sp, sp, t0
    vle8.v v16, (sp)
    add sp, sp, t0
    vle8.v v24, (sp)
    add sp, sp, t0

    LOAD t0,   2 * REGBYTES(sp)
    LOAD t1,   1 * REGBYTES(sp)
    vsetvl x0, t0, t1
    LOAD t0,   0 * REGBYTES(sp)
    csrw  CSR_VSTART, t0
    LOAD t0,   3 * REGBYTES(sp)
    csrw  CSR_VCSR, t0
    addi sp, sp, (4 * REGBYTES)

    LOAD t0,   0 * REGBYTES(t2)
    LOAD t1,   1 * REGBYTES(t2)
    LOAD t2,   2 * REGBYTES(t2)
#endif

#if defined(__riscv_flen)
    addi sp, sp, -(2 * REGBYTES)
    STORE t0, 0 * REGBYTES(sp)
    STORE t1, 1 * REGBYTES(sp)
_restore_fp_stack:
    csrr t0, CSR_MSTATUS
    li t1, MSTATUS_FS_DIRTY
    and t0,t0,t1
    bne t0, t1, _restore_done
    LOAD t0, 0 * REGBYTES(sp)
    LOAD t1, 1 * REGBYTES(sp)
    addi sp, sp, (2 * REGBYTES)

    /* Restore fp registers and fcsr and 3 reserved reg space */
    FPLOAD f0 , 0  * FPREGBYTES(sp)
    FPLOAD f1 , 1  * FPREGBYTES(sp)
    FPLOAD f2 , 2  * FPREGBYTES(sp)
    FPLOAD f3 , 3  * FPREGBYTES(sp)
    FPLOAD f4 , 4  * FPREGBYTES(sp)
    FPLOAD f5 , 5  * FPREGBYTES(sp)
    FPLOAD f6 , 6  * FPREGBYTES(sp)
    FPLOAD f7 , 7  * FPREGBYTES(sp)
    FPLOAD f8 , 8  * FPREGBYTES(sp)
    FPLOAD f9 , 9  * FPREGBYTES(sp)
    FPLOAD f10, 10 * FPREGBYTES(sp)
    FPLOAD f11, 11 * FPREGBYTES(sp)
    FPLOAD f12, 12 * FPREGBYTES(sp)
    FPLOAD f13, 13 * FPREGBYTES(sp)
    FPLOAD f14, 14 * FPREGBYTES(sp)
    FPLOAD f15, 15 * FPREGBYTES(sp)
    FPLOAD f16, 16 * FPREGBYTES(sp)
    FPLOAD f17, 17 * FPREGBYTES(sp)
    FPLOAD f18, 18 * FPREGBYTES(sp)
    FPLOAD f19, 19 * FPREGBYTES(sp)
    FPLOAD f20, 20 * FPREGBYTES(sp)
    FPLOAD f21, 21 * FPREGBYTES(sp)
    FPLOAD f22, 22 * FPREGBYTES(sp)
    FPLOAD f23, 23 * FPREGBYTES(sp)
    FPLOAD f24, 24 * FPREGBYTES(sp)
    FPLOAD f25, 25 * FPREGBYTES(sp)
    FPLOAD f26, 26 * FPREGBYTES(sp)
    FPLOAD f27, 27 * FPREGBYTES(sp)
    FPLOAD f28, 28 * FPREGBYTES(sp)
    FPLOAD f29, 29 * FPREGBYTES(sp)
    FPLOAD f30, 30 * FPREGBYTES(sp)
    FPLOAD f31, 31 * FPREGBYTES(sp)
    addi sp, sp, (32 * FPREGBYTES)

    addi sp, sp, -(1 * REGBYTES)
    STORE t0,   0 * REGBYTES(sp)
    LOAD  t0,   1 * REGBYTES(sp)
    csrw  CSR_FCSR, t0
    LOAD t0,   0 * REGBYTES(sp)
    addi sp, sp, (5 * REGBYTES)
#endif

#if defined(__riscv_flen) || defined(__riscv_vector)
    addi sp, sp, -(2 * REGBYTES)
    STORE t0, 0 * REGBYTES(sp)
    STORE t1, 1 * REGBYTES(sp)
_restore_done:
    LOAD t0, 0 * REGBYTES(sp)
    LOAD t1, 1 * REGBYTES(sp)
    addi sp, sp, (2 * REGBYTES)
#endif
    mret

    .size eclic_msip_handler, . - eclic_msip_handler
