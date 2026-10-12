/*
 * Copyright (c) 2023 Nuclei Limited. All rights reserved.
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
 * \version  V1.0.0
 * \date     15 Sept 2023
 *
 ******************************************************************************/

#include "riscv_encoding.h"
#include "cpufeature.h"

DISABLE_MIE MACRO
  csrci CSR_MSTATUS, MSTATUS_MIE
  ENDM

SAVE_CONTEXT MACRO
#if defined(ECLIC_HW_CTX_AUTO) && defined(CFG_HAS_ECLICV2)
#else
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
  ENDM

RESTORE_CONTEXT MACRO
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
  ENDM

/* RNMI context save/restore: switch to the dedicated RNMI stack held in
 * CSR_MNSCRATCH and then save/restore the GPR context.
 * NOTE: SAVE_CONTEXT/RESTORE_CONTEXT are NOT reused on purpose, they become a
 * no-op when ECLIC_HW_CTX_AUTO && CFG_HAS_ECLICV2 is set, because that hardware
 * feature only saves/restores the context on the mtvec path via
 * CSR_MTSP/CSR_POPXRET. An RNMI is taken through CSR_MNVEC and returns with
 * MNRET, so the hardware saves nothing here and the software must do it. */
SAVE_RNMI_CONTEXT MACRO
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
  ENDM

RESTORE_RNMI_CONTEXT MACRO
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
  ENDM

/* MNRET: return from an RNMI handler, raw encoding of the MNRET instruction */
MNRET MACRO
    DC32 0x70200073
  ENDM

SAVE_CSR_CONTEXT MACRO
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
  ENDM

RESTORE_CSR_CONTEXT MACRO
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
  ENDM

    PUBWEAK exc_entry, irq_entry, early_exc_entry, default_intexc_handler
    PUBWEAK Undef_Handler
    EXTERN core_exception_handler
#if defined(CFG_HAS_ISA_DBLTRP)
    PUBWEAK rnmi_entry
    EXTERN core_rnmi_handler
#endif

    SECTION `.text`:CODE:NOROOT(2)
    CODE

    ALIGN 6
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

    ALIGN 2
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

#if defined(CFG_HAS_ISA_DBLTRP)
    /* Resumable NMI / Double Trap Entry, address programmed into CSR_MNVEC */
    ALIGN 2
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
    call core_rnmi_handler

    /* Restore the context and switch back to the interrupted stack */
    RESTORE_RNMI_CONTEXT

    /* Return to the interrupted context using MNRET */
    MNRET
#endif /* #if defined(CFG_HAS_ISA_DBLTRP) */

default_intexc_handler:
Undef_Handler:
    j Undef_Handler

    ALIGN 6
early_exc_entry:
    j early_exc_entry

    END
