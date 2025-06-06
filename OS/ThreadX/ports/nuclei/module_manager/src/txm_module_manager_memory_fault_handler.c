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
#include "tx_thread.h"
#include "txm_module.h"


/* Define the user's fault notification callback function pointer.  This is
   setup via the txm_module_manager_memory_fault_notify API.  */

VOID    (*_txm_module_manager_fault_notify)(TX_THREAD *, TXM_MODULE_INSTANCE *);


/* Define a macro that can be used to allocate global variables useful to
   store information about the last fault. This macro is defined in
   txm_module_port.h and is usually populated in the assembly language
   fault handling prior to the code calling _txm_module_manager_memory_fault_handler.  */

TXM_MODULE_MANAGER_FAULT_INFO


VOID  _txm_module_manager_memory_fault_handler(VOID)
{

TXM_MODULE_INSTANCE     *module_instance_ptr;
TX_THREAD               *thread_ptr;

    /* Pickup the current thread.  */
    thread_ptr =  _tx_thread_current_ptr;

    /* Initialize the module instance pointer to NULL.  */
    module_instance_ptr =  TX_NULL;

    /* Is there a thread?  */
    if (thread_ptr)
    {
        /* Pickup the module instance.  */
        module_instance_ptr =  thread_ptr -> tx_thread_module_instance_ptr;

        /* Terminate the current thread.  */
        _tx_thread_terminate(_tx_thread_current_ptr);
    }

    /* Determine if there is a user memory fault notification callback.  */
    if (_txm_module_manager_fault_notify)
    {
        /* Yes, call the user's notification memory fault callback.  */
        (_txm_module_manager_fault_notify)(thread_ptr, module_instance_ptr);
    }
}
