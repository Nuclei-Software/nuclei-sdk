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


VOID  _txm_module_manager_alignment_adjust(TXM_MODULE_PREAMBLE *module_preamble,
                                           ULONG *code_size,
                                           ULONG *code_alignment,
                                           ULONG *data_size,
                                           ULONG *data_alignment)
{
#ifdef TXM_MODULE_MANAGER_PMP

#else


#endif
}
