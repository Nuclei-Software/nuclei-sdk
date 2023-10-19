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
 * \file     intexc_evalsoc_s.S
 * \brief    NMSIS supervisor-mode Interrupt and Exception Handling Template File
 *  for Nuclei Eval SoC which support Nuclei N/NX class cores
 * \version  V1.0.0
 * \date     15 Sept 2023
 *
 ******************************************************************************/

#include "riscv_encoding.h"

DISABLE_SIE MACRO
  csrci CSR_SSTATUS, MSTATUS_SIE
  ENDM

SAVE_CONTEXT MACRO
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

RESTORE_CONTEXT MACRO
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

  ENDM

SAVE_CSR_CONTEXT_S MACRO
    /* Store CSR scause to stack using pushscause */
    csrrwi  x0, CSR_PUSHSCAUSE, 11
    /* Store CSR sepc to stack using pushsepc */
    csrrwi  x0, CSR_PUSHSEPC, 12
  ENDM

RESTORE_CSR_CONTEXT_S MACRO
    LOAD x5,  12*REGBYTES(sp)
    csrw CSR_SEPC, x5
    LOAD x5,  11*REGBYTES(sp)
    csrw CSR_SCAUSE, x5
  ENDM

    PUBWEAK exc_entry_s, irq_entry_s
    EXTERN core_exception_handler_s

    SECTION `.text`:CODE:NOROOT(2)
    CODE

    ALIGN 6
exc_entry_s:
    /* Save the caller saving registers (context) */
    SAVE_CONTEXT
    /* Save the necessary CSR registers */
    SAVE_CSR_CONTEXT_S

    /*
     * Set the exception handler function arguments
     * argument 1: scause value
     * argument 2: current stack point(SP) value
     */
    csrr a0, scause
    mv a1, sp
    /*
     * TODO: Call the exception handler function
     * By default, the function template is provided in
     * system_Device.c, you can adjust it as you want
     */
    call core_exception_handler_s

    /* Restore the necessary CSR registers */
    RESTORE_CSR_CONTEXT_S
    /* Restore the caller saving registers (context) */
    RESTORE_CONTEXT

    /* Return to regular code */
    sret

    ALIGN 2
irq_entry_s:
    /* Save the caller saving registers (context) */
    SAVE_CONTEXT
    /* Save the necessary CSR registers */
    SAVE_CSR_CONTEXT_S

    /* This special CSR read/write operation, which is actually
     * claim the CLIC to find its pending highest ID, if the ID
     * is not 0, then automatically enable the sstatus.SIE, and
     * jump to its vector-entry-label, and update the link register
     */
    csrrw ra, CSR_JALSNXTI, ra

    /* Critical section with interrupts disabled */
    DISABLE_SIE

    /* Restore the necessary CSR registers */
    RESTORE_CSR_CONTEXT_S
    /* Restore the caller saving registers (context) */
    RESTORE_CONTEXT

    /* Return to regular code */
    sret

    END
