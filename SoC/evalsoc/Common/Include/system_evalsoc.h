/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
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
/*******************************************************************************
 * @file     system_evalsoc.h
 * @brief    NMSIS Nuclei N/NX Device Peripheral Access Layer Header File for
 *           Device <Device>
 * @version  V1.00
 * @date     17. Dec 2019
 ******************************************************************************/

#ifndef __SYSTEM_EVALSOC_H__   /* ToDo: replace '<Device>' with your device name */
#define __SYSTEM_EVALSOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern volatile uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock) */

typedef struct EXC_Frame {
    unsigned long ra;                /* ra: x1, return address for jump */
    unsigned long tp;                /* tp: x4, thread pointer */
    unsigned long t0;                /* t0: x5, temporary register 0 */
    unsigned long t1;                /* t1: x6, temporary register 1 */
    unsigned long t2;                /* t2: x7, temporary register 2 */
    unsigned long a0;                /* a0: x10, return value or function argument 0 */
    unsigned long a1;                /* a1: x11, return value or function argument 1 */
    unsigned long a2;                /* a2: x12, function argument 2 */
    unsigned long a3;                /* a3: x13, function argument 3 */
    unsigned long a4;                /* a4: x14, function argument 4 */
    unsigned long a5;                /* a5: x15, function argument 5 */
    unsigned long cause;             /* cause: machine/supervisor mode cause csr register */
    unsigned long epc;               /* epc: machine/ supervisor mode exception program counter csr register */
    unsigned long reserved0;         /* reserved */
#ifndef __riscv_32e
    unsigned long a6;                /* a6: x16, function argument 6 */
    unsigned long a7;                /* a7: x17, function argument 7 */
    unsigned long t3;                /* t3: x28, temporary register 3 */
    unsigned long t4;                /* t4: x29, temporary register 4 */
    unsigned long t5;                /* t5: x30, temporary register 5 */
    unsigned long t6;                /* t6: x31, temporary register 6 */
#endif
} EXC_Frame_Type;

/**
 * \brief Setup the microcontroller system.
 * \details
 * Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit(void);

/**
 * \brief  Update SystemCoreClock variable.
 * \details
 * Updates the SystemCoreClock with current core Clock retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate(void);

extern uint32_t core_exception_handler(unsigned long mcause, unsigned long sp);

#if defined(__Vendor_EXCEPTION) && (__Vendor_EXCEPTION == 0)
/**
 * \brief Dump Exception Frame
 */
void Exception_DumpFrame(unsigned long sp, uint8_t mode);

/**
 * \brief Register an exception handler for exception code EXCn
 */
extern void Exception_Register_EXC(uint32_t EXCn, unsigned long exc_handler);

/**
 * \brief Get current exception handler for exception code EXCn
 */
extern unsigned long Exception_Get_EXC(uint32_t EXCn);
#endif

/**
 * \brief Initialize eclic config
 */
extern void Interrupt_Init(void);

/**
 * \brief  Initialize a specific IRQ and register the handler
 * \details
 * This function set handler for specific IRQn and enable it.
 */
extern int32_t IRQC_Register_IRQ(IRQn_Type IRQn, void* handler);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_EVALSOC_H__ */
