/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Module Manager                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE

#include "tx_api.h"
#include "txm_module.h"

VOID _txm_module_manager_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(TX_THREAD *, TXM_MODULE_INSTANCE *))
{
    struct thread_stack_frame *frame;
    TXM_MODULE_THREAD_ENTRY_INFO *thread_entry_info;
    uint8_t *stk;
    int i;

    stk  = thread_ptr -> tx_thread_stack_end;
    // thread_ptr -> tx_thread_stack_ptr stored thread_entry_info
    thread_entry_info = (TXM_MODULE_THREAD_ENTRY_INFO *)(thread_ptr -> tx_thread_stack_ptr);
    /* https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-cc.adoc */
    /* 32-bit boundary for ilp32e, and 128-bit boundary for others */
#ifndef __riscv_32e
    stk  = (uint8_t *)(((unsigned long)stk) & (~(unsigned long)(16 - 1)));
#else
    stk  = (uint8_t *)(((unsigned long)stk) & (~(unsigned long)(4 - 1)));
#endif
    stk -= sizeof(struct thread_stack_frame);

    frame = (struct thread_stack_frame*)stk;

    for (i = 0; i < sizeof(struct thread_stack_frame) / sizeof(unsigned long); i++) {
        ((unsigned long*)frame)[i] = 0xdeadbeef;
    }

    frame->a0      = (unsigned long)thread_entry_info->txm_module_thread_entry_info_thread;
    frame->a1      = (unsigned long)thread_entry_info;

    frame->epc     = (unsigned long)function_ptr;
    frame->mstatus = THREAD_INITIAL_MSTATUS;

    thread_ptr -> tx_thread_stack_ptr = stk;
    __FENCE_I();
}
