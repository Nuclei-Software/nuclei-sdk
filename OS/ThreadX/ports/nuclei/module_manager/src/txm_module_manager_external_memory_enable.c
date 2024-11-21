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
#include "tx_mutex.h"
#include "tx_queue.h"
#include "tx_thread.h"
#include "txm_module.h"


UINT  _txm_module_manager_external_memory_enable(TXM_MODULE_INSTANCE *module_instance,
                                                 VOID *start_address,
                                                 ULONG length,
                                                 UINT attributes)
{
#ifdef TXM_MODULE_MANAGER_PMP

    /* Return success.  */
    return(TX_SUCCESS);

#else
    /* Return success.  */
    return(TX_SUCCESS);

#endif
}
