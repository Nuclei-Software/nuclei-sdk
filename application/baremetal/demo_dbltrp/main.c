/*
 * Copyright (c) 2026 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
/******************************************************************************
 * @file     main.c
 * @brief    Nuclei Smdbltrp double trap demo
 * @version  V1.00
 * @date     2026
 ******************************************************************************/

#include <stdio.h>
#include "nuclei_sdk_soc.h"

#if defined(__DBLTRP_PRESENT) && (__DBLTRP_PRESENT == 1)

/*
 * Real double trap demo.
 *
 * A double trap happens when a trap is taken while the hart is already in an
 * M-mode trap handler, ie. mstatus.MDT is already 1 (it is set by hardware when
 * the previous trap was taken). Instead of manually pre-setting MDT, this demo
 * builds the real situation:
 *
 *   1. main() executes an illegal instruction -> 1st trap, M-mode trap entry,
 *      hardware sets mstatus.MDT = 1, mmode_exception_handler() runs.
 *   2. mmode_exception_handler() executes a second illegal instruction while
 *      MDT is still 1 -> this is an *unexpected trap* (double trap). Because
 *      Smrnmi is implemented and mnstatus.NMIE is 1, the hart jumps to the RNMI
 *      handler (CSR_MNVEC, set up by RNMI_Init). mncause records the *original*
 *      culprit exception (mncause[MXLEN-1] = 0, exccode = 2 illegal instruction),
 *      NOT the double-trap code 16 (16 belongs to mcause, used when an S-mode
 *      double trap is delivered to M-mode).
 *   3. core_rnmi_handler() reports it, skips the faulting instruction and
 *      clears MDT, then MNRET resumes the outer M-mode exception handler.
 *   4. mmode_exception_handler() skips the first illegal instruction and returns,
 *      main() resumes normally.
 */

static volatile uint32_t exc_count = 0;
static volatile uint32_t rnmi_count = 0;
static volatile unsigned long except_mcause = 0;
static volatile unsigned long rnmi_mncause = 0;

/* RNMI handler: only reached on a genuine double trap (a trap taken while
 * mstatus.MDT is already 1) or on an RNMI interrupt. Overrides the weak default
 * one in system_evalsoc.c.
 *
 * NOTE the printf() calls below are acceptable for this demo only. This handler
 * runs on the dedicated 2 KB RNMI stack and an RNMI can interrupt printf()
 * itself, so a re-entered newlib stdout lock or a stack overflow is possible in
 * the general case. A production RNMI handler should only touch a pre-formatted
 * buffer or plain memory, and leave the console output to a deferred context. */
void core_rnmi_handler(unsigned long mncause, unsigned long mnepc,
                       unsigned long mnstatus, unsigned long sp)
{
    CSR_MNSTATUS_Type mnst;
    unsigned long exccode;

    (void)sp;
    mnst.d = mnstatus;
    exccode = mncause & MCAUSE_CAUSE;
    rnmi_count++;
    rnmi_mncause = mncause;

    /*
     * MNRET returns to M-mode here, and per the Smdbltrp spec MNRET only clears
     * MDT when the new privilege mode is not M, so clear it explicitly. It must
     * be 0 before mstatus.MIE can be enabled again.
     */
    __clear_MDT();

    printf("[RNMI] %s caught: mncause=0x%lx (interrupt=%lu, exccode=%lu), "
           "mnepc=0x%lx, MNPP=%lu(%c), NMIE=%d\r\n",
           (mncause & MCAUSE_INTR) ? "RNMI interrupt" : "double trap",
           mncause, (unsigned long)((mncause & MCAUSE_INTR) ? 1 : 0), exccode,
           mnepc, (unsigned long)mnst.b.mnpp,
           (mnst.b.mnpp == 3) ? 'M' : (mnst.b.mnpp == 1) ? 'S' : 'U',
           (int)mnst.b.nmie);

    /* Skip the faulting illegal instruction, MNRET resumes the interrupted
     * M-mode exception handler right after it. The +4 assumes a 32-bit
     * instruction, which is true for the 0x00000000 encoding used below; a
     * compressed instruction (e.g. an illegal 16-bit encoding) would need the
     * instruction length decoded from the encoding instead of a fixed 4. */
    __set_MNEPC(mnepc + 4);
}

/* First level M-mode exception handler for the illegal instruction in main().
 * While it runs, mstatus.MDT is 1 (set by hardware on the 1st trap), so the
 * second illegal instruction below is a real double trap. */
static void mmode_exception_handler(unsigned long mcause, unsigned long sp)
{
    EXC_Frame_Type *frame = (EXC_Frame_Type *)sp;

    exc_count++;
    except_mcause = mcause;
    printf("[M-mode exc] mcause=0x%lx (%s), mepc=0x%lx, MDT=%d\r\n",
           mcause,
           ((mcause & MCAUSE_CAUSE) == CAUSE_ILLEGAL_INSTRUCTION) ?
               "illegal instruction" : "?",
           (unsigned long)frame->epc, (int)__get_MDT());

    printf("[M-mode exc] execute a second illegal instruction while MDT=%d, "
           "this is a real double trap...\r\n", (int)__get_MDT());
    /* Second trap while MDT is already 1 -> unexpected trap -> RNMI handler */
    __asm volatile(".word 0x00000000");
    printf("[M-mode exc] resumed after the RNMI handler\r\n");

    /* Skip the 1st illegal instruction in main(), the saved mepc slot is
     * restored by exc_entry before mret. The +4 assumes the 0x00000000 (32-bit)
     * encoding used above instead of a compressed instruction. */
    frame->epc += 4;
}

#endif /* #if defined(__DBLTRP_PRESENT) && (__DBLTRP_PRESENT == 1) */

int main(void)
{
    printf("\r\nSmdbltrp double trap demo\r\n");

#if defined(__DBLTRP_PRESENT) && (__DBLTRP_PRESENT == 1)
    /* Make sure the running CPU really implements the feature, otherwise the
     * second trap below would enter the critical-error state instead of being
     * delivered to the RNMI handler */
    if (!IINFO_IsSmdbltrpSupported()) {
        printf("Smdbltrp is not supported in this CPU, will not run this example!\r\n");
        return 0;
    }

    printf("before: MDT=%d, NMIE=%d\r\n", (int)__get_MDT(), (int)__get_NMIE());

    Exception_Register_EXC(IlleIns_EXCn, (unsigned long)mmode_exception_handler);

    printf("execute the first illegal instruction in M-mode...\r\n");
    /* 1st trap -> M-mode exception entry -> mmode_exception_handler */
    __asm volatile(".word 0x00000000");

    printf("back in main: exc_count=%u, rnmi_count=%u, MDT=%d, NMIE=%d\r\n",
           (unsigned)exc_count, (unsigned)rnmi_count,
           (int)__get_MDT(), (int)__get_NMIE());

    if (exc_count == 1 && rnmi_count == 1 &&
        (except_mcause & MCAUSE_CAUSE) == CAUSE_ILLEGAL_INSTRUCTION &&
        /* double trap: not an interrupt (mncause[MXLEN-1] == 0) and mncause
         * holds the original culprit cause, ie. the illegal instruction, NOT 16 */
        (rnmi_mncause & MCAUSE_INTR) == 0 &&
        (rnmi_mncause & MCAUSE_CAUSE) == CAUSE_ILLEGAL_INSTRUCTION) {
        printf("[SUCCESS] Test Smdbltrp double trap passed!\r\n");
    } else {
        printf("[FAIL] exc_count=%u, rnmi_count=%u, except_mcause=0x%lx, "
               "rnmi_mncause=0x%lx\r\n",
               (unsigned)exc_count, (unsigned)rnmi_count,
               except_mcause, rnmi_mncause);
    }
#else
    /* Build with XLCFG_DBLTRP=0: the feature is not compiled in/initialized,
     * so just skip the demo instead of triggering a double trap */
    printf("XLCFG_DBLTRP is not enabled (__DBLTRP_PRESENT=0), will not run this example!\r\n");
    printf("Please build this example with XLCFG_DBLTRP=1\r\n");
#endif

    return 0;
}
