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
 * \file     intexc_evalsoc.S
 * \brief    NMSIS Interrupt and Exception Handling Template File
 *  for Nuclei Eval SoC which support Nuclei N/NX class cores
 * \version  V1.00
 * \date     17 Dec 2019
 *
 ******************************************************************************/

#include "riscv_encoding.h"

/* TODO: Require Nuclei SDK >= 0.6.0, which introduced this cpufeature.h */
#include "cpufeature.h"

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
#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2)
#else
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
#endif
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
#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2)
#else
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
#endif
.endm

/**
 * \brief  Macro for context save in the RNMI/double trap handler
 * \details
 * Switch to the dedicated RNMI stack kept in CSR_MNSCRATCH (so the handler
 * does not rely on the possibly corrupted stack of the interrupted context)
 * and then save the caller saved registers.
 * \remarks
 * - The handler must NOT modify CSR_MNSCRATCH, otherwise the swap back in
 *   \ref RESTORE_RNMI_CONTEXT would not restore the interrupted sp.
 * - The GPR context is saved unconditionally instead of reusing
 *   \ref SAVE_CONTEXT, which becomes a no-op when the ECLIC hardware auto
 *   context save is enabled (ECLIC_HW_CTX_AUTO && CFG_HAS_ECLICV2). That
 *   hardware feature only saves/restores the context on the mtvec path via
 *   CSR_MTSP/CSR_POPXRET, and an RNMI is taken through CSR_MNVEC and returns
 *   with MNRET, so the hardware never saves nor restores anything here.
 */
.macro SAVE_RNMI_CONTEXT
    csrrw sp, CSR_MNSCRATCH, sp
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
 * \brief  Macro for context restore in the RNMI/double trap handler
 * \details
 * Restore the caller saved registers and then swap the interrupted sp back,
 * leaving the dedicated RNMI stack pointer in CSR_MNSCRATCH again.
 */
.macro RESTORE_RNMI_CONTEXT
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
    csrrw sp, CSR_MNSCRATCH, sp
.endm

/**
 * \brief  MNRET instruction
 * \details Return from an RNMI handler: restores the PC from CSR_MNEPC, the
 * privilege mode from mnstatus and sets mnstatus.NMIE.
 * \remarks The raw instruction encoding is used because the mnret mnemonic is
 * not supported by all toolchains.
 */
.macro MNRET
    .word 0x70200073
.endm

/**
 * \brief  Macro for save necessary CSRs to stack
 * \details
 * This macro store MCAUSE, MEPC, MSUBM to stack.
 */
.macro SAVE_CSR_CONTEXT
#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2)
#else
#ifdef CFG_HAS_CLIC
    /* Store CSR mcause to stack using pushmcause */
    csrrwi  x0, CSR_PUSHMCAUSE, 11
    /* Store CSR mepc to stack using pushmepc */
    csrrwi  x0, CSR_PUSHMEPC, 12
    /* Store CSR msub to stack using pushmsub */
    csrrwi  x0, CSR_PUSHMSUBM, 13
#else
    csrr x5, CSR_MEPC
    STORE x5,  12*REGBYTES(sp)
    csrr x5, CSR_MCAUSE
    STORE x5,  11*REGBYTES(sp)
#endif
#endif
.endm

/**
 * \brief  Macro for restore necessary CSRs from stack
 * \details
 * This macro restore MSUBM, MEPC, MCAUSE from stack.
 */
.macro RESTORE_CSR_CONTEXT
#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2)
#else
#ifdef CFG_HAS_CLIC
    LOAD x5,  13*REGBYTES(sp)
    csrw CSR_MSUBM, x5
#endif
    LOAD x5,  12*REGBYTES(sp)
    csrw CSR_MEPC, x5
    LOAD x5,  11*REGBYTES(sp)
    csrw CSR_MCAUSE, x5
#endif
.endm

/**
 * \brief  Exception/NMI Entry
 * \details
 * This function provide common entry functions for exception/nmi and interrupt of plic mode.
 * \remarks
 * This function provide a default exception/nmi entry.
 * ABI defined caller save register and some CSR registers
 * to be saved before enter interrupt handler and be restored before return.
 */
.section .text.trap
/* In CLIC mode, the exeception entry must be 64bytes aligned */
.align 6
# gnu let .weak override .globl, but llvm will show warning
# see https://reviews.llvm.org/D90108
.weak exc_entry
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
#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2)
    csrrwi x0, CSR_POPXRET, 0
#else
    mret
#endif

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
# gnu let .weak override .globl, but llvm will show warning
# see https://reviews.llvm.org/D90108
.weak irq_entry
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
#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2)
    csrrwi x0, CSR_POPXRET, 0
#else
    mret
#endif
    .size irq_entry, . - irq_entry

#if defined(CFG_HAS_ISA_DBLTRP)
/**
 * \brief  Resumable NMI / Double Trap Entry
 * \details
 * This function provides the default entry for a resumable NMI (RNMI) and for
 * an unexpected/double trap in M-mode. The address of this entry is programmed
 * into CSR_MNVEC by \ref _premain_init when the Smrnmi/Smdbltrp feature is
 * implemented.
 * \remarks
 * - On entry the CPU has saved the interrupted state into mnepc/mncause and
 *   mnstatus, and has cleared mnstatus.NMIE. The handler must return with the
 *   MNRET instruction, which restores the PC from mnepc, the privilege mode
 *   from mnstatus and sets mnstatus.NMIE back to 1.
 * - A dedicated per-hart RNMI stack is used: its top address is preloaded into
 *   CSR_MNSCRATCH by RNMI_Init() and sp is swapped with mnscratch on entry and
 *   exit, so the handler does not rely on the (possibly corrupted) stack of
 *   the interrupted context. The handler must NOT modify CSR_MNSCRATCH.
 * - The GPR context is always saved in software, even when the ECLIC hardware
 *   auto context save is enabled: that hardware feature is tied to the mtvec
 *   path, which an RNMI taken through CSR_MNVEC does not go through.
 * - The MNRET encoding (0x70200073) is used instead of the mnret mnemonic
 *   because mnret is not supported by all toolchains.
 */
.section .text.rnmi
.align 2
# gnu let .weak override .globl, but llvm will show warning
# see https://reviews.llvm.org/D90108
.weak rnmi_entry
.type rnmi_entry, @function
rnmi_entry:
    /* Switch to the dedicated RNMI stack and save the context */
    SAVE_RNMI_CONTEXT

    /*
     * Set the RNMI handler function arguments
     * argument 1: mncause value
     * argument 2: mnepc value
     * argument 3: mnstatus value
     * argument 4: current stack point(SP) value
     */
    csrr a0, CSR_MNCAUSE
    csrr a1, CSR_MNEPC
    csrr a2, CSR_MNSTATUS
    mv   a3, sp
    /*
     * TODO: Call the RNMI handler function
     * By default, the function template is provided in
     * system_Device.c, you can adjust it as you want
     */
    call core_rnmi_handler

    /* Restore the context and switch back to the interrupted stack */
    RESTORE_RNMI_CONTEXT

    /* Return to the interrupted context using MNRET */
    MNRET

    .size rnmi_entry, . - rnmi_entry
#endif /* #if defined(CFG_HAS_ISA_DBLTRP) */

/* Restore the default section, the code below belongs to the irq entry as
 * before the rnmi entry was introduced */
.section .text.irq

/* Default Handler for Exceptions / Interrupts */
# gnu let .weak override .globl, but llvm will show warning
# see https://reviews.llvm.org/D90108
.weak default_intexc_handler
.type default_intexc_handler, @function
Undef_Handler:
default_intexc_handler:
1:
    j 1b

    .size default_intexc_handler, . - default_intexc_handler
