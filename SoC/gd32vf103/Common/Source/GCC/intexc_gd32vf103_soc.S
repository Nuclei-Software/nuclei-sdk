/*
 * Copyright (c) 2009-2019 Nuclei Limited. All rights reserved.
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
 * \file     intexc_Device.s
 * \brief    NMSIS Interrupt and Exception Handling Template File
 *           for Nuclei N/NX Class Device
 * \version  V1.00
 * \date     3 Dec 2019
 *
 * @note
 *
 ******************************************************************************/

#include "riscv_encoding.h"

/**
 * \brief  Global interrupt disabled
 * \details
 *  This function disable global interrupt.
 * \remarks
 *  - All the interrupt requests will be ignored by CPU.
*/
# Disable Interrupt
#
.macro DISABLE_MIE
    csrc CSR_MSTATUS, MSTATUS_MIE
.endm


/**
 * \brief  Macro for context save
 * \details
 * This macro save ABI defined caller saved registers in the stack.
 * \remarks
 * - This Macro could use to save context before you make context switch.
*/
/* Save caller registers */
.macro SAVE_CONTEXT

#ifdef __riscv_flen
    #if (__riscv_flen==64 )
      addi sp, sp, -20*REGBYTES - 20*FPREGBYTES
    #else
      addi sp, sp, -20*REGBYTES
    #endif /*__riscv_flen*/
#else
    addi sp, sp, -20*REGBYTES
#endif /*__riscv_flen*/

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
    STORE x16, 11*REGBYTES(sp)
    STORE x17, 12*REGBYTES(sp)
    STORE x28, 13*REGBYTES(sp)
    STORE x29, 14*REGBYTES(sp)
    STORE x30, 15*REGBYTES(sp)
    STORE x31, 16*REGBYTES(sp)
#endif /*__riscv_32e*/

#ifdef __riscv_flen
  #if (__riscv_flen == 64)
    FPSTORE f0, (20*REGBYTES + 0*FPREGBYTES)(sp)
    FPSTORE f1, (20*REGBYTES + 1*FPREGBYTES)(sp)
    FPSTORE f2, (20*REGBYTES + 2*FPREGBYTES)(sp)
    FPSTORE f3, (20*REGBYTES + 3*FPREGBYTES)(sp)
    FPSTORE f4, (20*REGBYTES + 4*FPREGBYTES)(sp)
    FPSTORE f5, (20*REGBYTES + 5*FPREGBYTES)(sp)
    FPSTORE f6, (20*REGBYTES + 6*FPREGBYTES)(sp)
    FPSTORE f7, (20*REGBYTES + 7*FPREGBYTES)(sp)
    FPSTORE f10, (20*REGBYTES + 8*FPREGBYTES)(sp)
    FPSTORE f11, (20*REGBYTES + 9*FPREGBYTES)(sp)
    FPSTORE f12, (20*REGBYTES + 10*FPREGBYTES)(sp)
    FPSTORE f13, (20*REGBYTES + 11*FPREGBYTES)(sp)
    FPSTORE f14, (20*REGBYTES + 12*FPREGBYTES)(sp)
    FPSTORE f15, (20*REGBYTES + 13*FPREGBYTES)(sp)
    FPSTORE f16, (20*REGBYTES + 14*FPREGBYTES)(sp)
    FPSTORE f17, (20*REGBYTES + 15*FPREGBYTES)(sp)
    FPSTORE f28, (20*REGBYTES + 16*FPREGBYTES)(sp)
    FPSTORE f29, (20*REGBYTES + 17*FPREGBYTES)(sp)
    FPSTORE f30, (20*REGBYTES + 18*FPREGBYTES)(sp)
    FPSTORE f31, (20*REGBYTES + 19*FPREGBYTES)(sp)
  #endif /*__riscv_flen*/
#endif /*__riscv_flen*/
.endm

/**
 * \brief  Macro for restore caller registers
 * \details
 * This macro restore ABI defined caller saved registers from stack.
 * \remarks
 * - You could use this Macro to restore context before you want return from interrupt or switch back to previous task.
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
    LOAD x16, 11*REGBYTES(sp)
    LOAD x17, 12*REGBYTES(sp)
    LOAD x28, 13*REGBYTES(sp)
    LOAD x29, 14*REGBYTES(sp)
    LOAD x30, 15*REGBYTES(sp)
    LOAD x31, 16*REGBYTES(sp)
#endif /*__riscv_32e*/


#ifdef __riscv_flen
  #if (__riscv_flen==64)
    /* Restore fp caller registers */
    FPLOAD f0, (20*REGBYTES + 0*FPREGBYTES)(sp)
    FPLOAD f1, (20*REGBYTES + 1*FPREGBYTES)(sp)
    FPLOAD f2, (20*REGBYTES + 2*FPREGBYTES)(sp)
    FPLOAD f3, (20*REGBYTES + 3*FPREGBYTES)(sp)
    FPLOAD f4, (20*REGBYTES + 4*FPREGBYTES)(sp)
    FPLOAD f5, (20*REGBYTES + 5*FPREGBYTES)(sp)
    FPLOAD f6, (20*REGBYTES + 6*FPREGBYTES)(sp)
    FPLOAD f7, (20*REGBYTES + 7*FPREGBYTES)(sp)
    FPLOAD f10, (20*REGBYTES + 8*FPREGBYTES)(sp)
    FPLOAD f11, (20*REGBYTES + 9*FPREGBYTES)(sp)
    FPLOAD f12, (20*REGBYTES + 10*FPREGBYTES)(sp)
    FPLOAD f13, (20*REGBYTES + 11*FPREGBYTES)(sp)
    FPLOAD f14, (20*REGBYTES + 12*FPREGBYTES)(sp)
    FPLOAD f15, (20*REGBYTES + 13*FPREGBYTES)(sp)
    FPLOAD f16, (20*REGBYTES + 14*FPREGBYTES)(sp)
    FPLOAD f17, (20*REGBYTES + 15*FPREGBYTES)(sp)
    FPLOAD f28, (20*REGBYTES + 16*FPREGBYTES)(sp)
    FPLOAD f29, (20*REGBYTES + 17*FPREGBYTES)(sp)
    FPLOAD f30, (20*REGBYTES + 18*FPREGBYTES)(sp)
    FPLOAD f31, (20*REGBYTES + 19*FPREGBYTES)(sp)
  #endif /*__riscv_flen*/
#endif /*__riscv_flen*/


#ifdef __riscv_flen
  #if(__riscv_flen == 64 )
    addi sp, sp, 20*REGBYTES  + 20*FPREGBYTES
  #else
    addi sp, sp, 20*REGBYTES
  #endif /*__riscv_flen*/
#else
    /* De-allocate the stack space */
    addi sp, sp, 20*REGBYTES
#endif /*__riscv_flen*/
.endm

/**
 * \brief  Macro for restore caller registers
 * \details
 * This macro restore ABI defined call saved registers except X5 from stack.
 * \remarks
 * - This Macro is used to restore context before return from interrupt or switch back to previous task.
 */
.macro RESTORE_CONTEXT_EXCPT_X5
    LOAD x1,  0*REGBYTES(sp)
    LOAD x6,  2*REGBYTES(sp)
    LOAD x7,  3*REGBYTES(sp)
    LOAD x10, 4*REGBYTES(sp)
    LOAD x11, 5*REGBYTES(sp)
    LOAD x12, 6*REGBYTES(sp)
    LOAD x13, 7*REGBYTES(sp)
    LOAD x14, 8*REGBYTES(sp)
    LOAD x15, 9*REGBYTES(sp)
#ifndef __riscv_32e
    LOAD x16, 10*REGBYTES(sp)
    LOAD x17, 11*REGBYTES(sp)
    LOAD x28, 12*REGBYTES(sp)
    LOAD x29, 13*REGBYTES(sp)
    LOAD x30, 14*REGBYTES(sp)
    LOAD x31, 15*REGBYTES(sp)
#endif /*__riscv_32e*/
.endm


/**
 * \brief  Macro for restore X5
 * \details
 * This macro restore X5 from stack.
 * \remarks
 * - This macro is used to restore X5 before return from interrupt or switch back to previous task.
 */
.macro RESTORE_CONTEXT_ONLY_X5
    LOAD x5,  1*REGBYTES(sp)
.endm


/**
 * \brief  Macro for save MEPC and MSTATUS to stack
 * \details
 * This macro store MEPC and MSTATUS to stack.
 * \remarks
 * - This macro for enter exception handling to save the MEPC and MSTATUS.
 */
.macro SAVE_EPC_STATUS
    csrr x5, CSR_MEPC
    STORE x5,  16*REGBYTES(sp)
    csrr x5, CSR_MSTATUS
    STORE x5,  17*REGBYTES(sp)
    csrr x5, CSR_MSUBM
    STORE x5,  18*REGBYTES(sp)
.endm

/**
 * \brief  Macro for restore MEPC and MSTATUS from stack
 * \details
 * This macro restore MEPC and MSTATUS from stack.
 * \remarks
 * - This macro for return from exception handling to restore the MEPC and MSTATUS.
 */
.macro RESTORE_EPC_STATUS
    LOAD x5,  16*REGBYTES(sp)
    csrw CSR_MEPC, x5
    LOAD x5,  17*REGBYTES(sp)
    csrw CSR_MSTATUS, x5
    LOAD x5,  18*REGBYTES(sp)
    csrw CSR_MSUBM, x5
.endm


/**
 * \brief  exception handler enter and return
 * \details
 * This function provide default exception handler enter and return flow.
 * \remarks
 * This function provide a default exception entry.
 * ABI defined caller save register and some CSR registers
 * need save before enter exception handler and restore before return.
 */
.section .text.trap
/* In CLIC mode, the exeception entry must be 64bytes aligned */
.align 6
.global exc_entry
.weak exc_entry
exc_entry:
    /* Allocate the stack space */
    //addi sp, sp, -19*REGBYTES

    /* Save the caller saving registers (context) */
    SAVE_CONTEXT
    /* Save the MEPC/MSTATUS/MSUBM registers */
    SAVE_EPC_STATUS

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

    /* Restore the MEPC/MSTATUS/MSUBM registers */
    RESTORE_EPC_STATUS
    /* Restore the caller saving registers (context) */
    RESTORE_CONTEXT

    /* De-allocate the stack space */
    //addi sp, sp, 19*REGBYTES
    /* Return to regular code */
    mret

/**
 * \brief  Interrupt handler enter and return
 * \details
 * This function provide how to handler a interrupt.
 * \remarks
 * This function provide a default interrupt entry.
 * ABI defined caller save register and some CSR registers need
 * save before enter exception handler and restore before return.
 */
/* IRQ entry point */
.section      .text.irq
/* In CLIC mode, the interrupt entry must be 4bytes aligned */
.align 2
.global irq_entry
.weak irq_entry
/* This label will be set to MTVT2 register */
irq_entry:
    /* Allocate the stack space */
    //addi sp, sp, -19*REGBYTES

    /* Save ABI defined caller saved register */
    SAVE_CONTEXT

    /* This special CSR read operation, which is actually use mcause as operand to directly store it to memory */
    csrrwi  x0, CSR_PUSHMCAUSE, 17
    /* This special CSR read operation, which is actually use mepc as operand to directly store it to memory */
    csrrwi  x0, CSR_PUSHMEPC, 18
    /* This special CSR read operation, which is actually use Msubm as operand to directly store it to memory */
    csrrwi  x0, CSR_PUSHMSUBM, 19

    /* This special CSR read/write operation, which is actually claim the CLIC to find its pending highest
     * ID, if the ID is not 0, then automatically enable the mstatus.MIE, and jump to its vector-entry-label, and
     * update the link register
     */
    csrrw ra, CSR_JALMNXTI, ra

    //RESTORE_CONTEXT_EXCPT_X5

    /* Critical section with interrupts disabled */
    DISABLE_MIE # Disable interrupts

    LOAD x5,  19*REGBYTES(sp)
    csrw CSR_MSUBM, x5
    LOAD x5,  18*REGBYTES(sp)
    csrw CSR_MEPC, x5
    LOAD x5,  17*REGBYTES(sp)
    csrw CSR_MCAUSE, x5

    RESTORE_CONTEXT


    /* Return to regular code */
    mret

/* Default Handler for Exceptions / Interrupts */
.global default_intexc_handler
.weak default_intexc_handler
Undef_Handler:
default_intexc_handler:
1:
	j 1b
