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
 * \file     startup_evalsoc.c
 * \brief    NMSIS Nuclei N/NX Class Core based Core Device Startup File for
 *  Nuclei Eval SoC which support Nuclei N/NX class cores using IAR compiler
 * \version  V1.0.0
 * \date     19 Dec 2023
 *
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "evalsoc.h"

extern void _premain_init(void);

#ifndef IAR_DATA_INIT
#define IAR_DATA_INIT _GLUE(__iar_data_init, _DLIB_ELF_INIT_INTERFACE_VERSION)
#endif

extern void IAR_DATA_INIT(void);

// TODO: Currently only single core is supported
int __low_level_init(void)
{
    __disable_interrupt();

    /* Enable mcycle and minstret counter */
    __RV_CSR_CLEAR(CSR_MCOUNTINHIBIT, 0x5);

    /* Call IAR Internal data initial function */
    IAR_DATA_INIT();

    /*
     * You can place it before calling IAR_DATA_INIT
     * if in this SystemInit function use no global variable.
     * Call vendor defined SystemInit to
     * initialize the micro-controller system
     * SystemInit will just be called by boot cpu
     */
    SystemInit();

    /* Get CPU frequency and initialize uart for print */
    _premain_init();

#ifdef RTOS_RTTHREAD
    /* Directly jump to rtthread startup process, no longer return */
    extern int rtthread_startup(void);
    rtthread_startup();
#endif

    /* No need to call it again, since it is initialized */
    return 0;
}
