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
/**   Module                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef TXM_MODULE
#define TXM_MODULE
#endif

#ifndef TX_SOURCE_CODE
#define TX_SOURCE_CODE
#endif


/* Include necessary system files.  */

#include "txm_module.h"
#include "tx_thread.h"

/* Define the global module entry pointer from the start thread of the module.  */

TXM_MODULE_THREAD_ENTRY_INFO    *_txm_module_entry_info;


/* Define the dispatch function pointer used in the module implementation.  */

ULONG                           (*_txm_module_kernel_call_dispatcher)(ULONG kernel_request, ULONG param_1, ULONG param_2, ULONG param3);


/* Define the startup code that clears the uninitialized global data and sets up the
   preset global variables.  */

extern VOID _gcc_setup(char *code_addr, char *data_addr);


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_thread_shell_entry                       Nuclei RISC-V  */
/*                                                           6.4.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function calls the specified entry function of the thread. It  */
/*    also provides a place for the thread's entry function to return.    */
/*    If the thread returns, this function places the thread in a         */
/*    "COMPLETED" state.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                        Pointer to current thread         */
/*    thread_info                       Pointer to thread entry info      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gcc_setup                        cstartup initialization           */
/*    thread_entry                      Thread's entry function           */
/*    tx_thread_resume                  Resume the module callback thread */
/*    _txm_module_thread_system_suspend Module thread suspension routine  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Initial thread stack frame                                          */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

extern char __FLASH_segment_start__[];
extern char __RAM_segment_start__[];
extern char _GLOBAL_OFFSET_TABLE_[];
extern char __got_load_start__[], __got_end__[];

extern char __data_load_start__[], __data_start__[], __data_end__[];
extern char __bss_start__[], __bss_end__[];

VOID _gcc_setup(char *code_addr, char *data_addr)
{
    char *start, *load_data_start, *bss_start;
    unsigned long *got_items;
    unsigned long i, size, load_data_size, bss_size;
    unsigned long FLASH_segment_start = (unsigned long)__FLASH_segment_start__;
    unsigned long data_start = (unsigned long)__data_start__;

    start = code_addr + ((unsigned long)_GLOBAL_OFFSET_TABLE_ - (unsigned long)__FLASH_segment_start__);
    size = (unsigned long)__got_end__ - (unsigned long)__got_load_start__;
    load_data_start = code_addr + ((unsigned long)__data_load_start__ - (unsigned long)__FLASH_segment_start__);
    load_data_size = (unsigned long)__data_end__ - (unsigned long)__data_start__;
    bss_start = data_addr + ((unsigned long)__bss_start__ - (unsigned long)__data_start__);
    bss_size = (unsigned long)__bss_end__ - (unsigned long)__bss_start__;

    // Modify the GOT table to reference new code and data addresses
    // TODO: CODE IN ROM is not working, since we cannot modify the GOT table in ROM
    got_items = (unsigned long *) start;
    for (i = 0; i < size / sizeof(unsigned long); i += 1) {
        if (got_items[i] == 0) {
            got_items[i] = 0;
        } else if (got_items[i] >= (unsigned long)data_start) {
            got_items[i] = (unsigned long)data_addr + (got_items[i] - (unsigned long)data_start);
        } else {
            got_items[i] = (unsigned long)code_addr + (got_items[i] - (unsigned long)FLASH_segment_start);
        }
    }

    // Load data segment
    for (i = 0; i < load_data_size; i += 1) {
        data_addr[i] = load_data_start[i];
    }

    // Zero init bss segment
    for (i = 0; i < bss_size; i += 1) {
        bss_start[i] = 0;
    }
}

VOID  _txm_module_thread_shell_entry(TX_THREAD *thread_ptr, TXM_MODULE_THREAD_ENTRY_INFO *thread_info)
{

#ifndef TX_DISABLE_NOTIFY_CALLBACKS
    VOID            (*entry_exit_notify)(TX_THREAD *, UINT);
#endif
    VOID (*new_gcc_setup)(char *, char *);

    /* Determine if this is the start thread.  If so, we must prepare the module for
       execution.  If not, simply skip the C startup code.  */
    if (thread_info -> txm_module_thread_entry_info_start_thread)
    {
        /* Initialize the C environment.  */
        new_gcc_setup = (void (*)(char *, char *))((unsigned long)(thread_info -> txm_module_thread_entry_info_module -> txm_module_instance_shell_entry_function) - ((unsigned long)_txm_module_thread_shell_entry - (unsigned long)_gcc_setup));
        new_gcc_setup(thread_info -> txm_module_thread_entry_info_code_base_address, thread_info -> txm_module_thread_entry_info_data_base_address);

        /* Save the entry info pointer, for later use.  */
        _txm_module_entry_info =  thread_info;

        /* Save the kernel function dispatch address. This is used to make all resident calls from
           the module.  */
        _txm_module_kernel_call_dispatcher =  thread_info -> txm_module_thread_entry_info_kernel_call_dispatcher;

        /* Ensure that we have a valid pointer.  */
        while (!_txm_module_kernel_call_dispatcher)
        {
            /* Loop here, if an error is present getting the dispatch function pointer!
               An error here typically indicates the resident portion of _tx_thread_schedule
               is not supporting the trap to obtain the function pointer.   */
        }

        /* Resume the module's callback thread, already created in the manager.  */
        _txe_thread_resume(thread_info -> txm_module_thread_entry_info_callback_request_thread);
    }

#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Pickup the entry/exit application callback routine.  */
    entry_exit_notify =  thread_info -> txm_module_thread_entry_info_exit_notify;

    /* Determine if an application callback routine is specified.  */
    if (entry_exit_notify != TX_NULL)
    {

        /* Yes, notify application that this thread has been entered!  */
        (entry_exit_notify)(thread_ptr, TX_THREAD_ENTRY);
    }
#endif

    /* Call current thread's entry function.  */
    (thread_info -> txm_module_thread_entry_info_entry) (thread_info -> txm_module_thread_entry_info_parameter);

    /* Suspend thread with a "completed" state.  */


#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Pickup the entry/exit application callback routine again.  */
    entry_exit_notify =  thread_info -> txm_module_thread_entry_info_exit_notify;

    /* Determine if an application callback routine is specified.  */
    if (entry_exit_notify != TX_NULL)
    {

        /* Yes, notify application that this thread has exited!  */
        (entry_exit_notify)(thread_ptr, TX_THREAD_EXIT);
    }
#endif

    /* Call actual thread suspension routine.  */
    _txm_module_thread_system_suspend(thread_ptr);

#ifdef TX_SAFETY_CRITICAL

    /* If we ever get here, raise safety critical exception.  */
    TX_SAFETY_CRITICAL_EXCEPTION(__FILE__, __LINE__, 0);
#endif
}
