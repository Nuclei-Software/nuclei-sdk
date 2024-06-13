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
/******************************************************************************
 * @file     system_evalsoc.c
 * @brief    NMSIS Nuclei Core Device Peripheral Access Layer Source File for
 *           Nuclei Eval SoC which support Nuclei N/NX class cores
 * @version  V1.00
 * @date     22. Nov 2019
 ******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "nuclei_sdk_hal.h"

// TODO: This implementation contains many extra code controlled by macros
// which may be not suitable for your SoC, you can directly remove the code

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
/* ToDo: add here your necessary defines for device initialization
         following is an example for different system frequencies */
#ifndef SYSTEM_CLOCK
#define SYSTEM_CLOCK    (80000000UL)
#endif

/**
 * \defgroup  NMSIS_Core_SystemConfig       System Device Configuration
 * \brief Functions for system and clock setup available in system_<device>.c.
 * \details
 * Nuclei provides a template file **system_Device.c** that must be adapted by
 * the silicon vendor to match their actual device. As a <b>minimum requirement</b>,
 * this file must provide:
 *  -  A device-specific system configuration function, \ref SystemInit.
 *  -  Global c library \ref _premain_init and \ref _postmain_fini functions called right before calling main function.
 *     -  A global variable that contains the system frequency, \ref SystemCoreClock.
 *     -  A global eclic configuration initialization, \ref ECLIC_Init.
 *     -  A global exception and trap configuration initialization, \ref Trap_Init and \ref Exception_Init.
 *  -  Vendor customized interrupt, exception and nmi handling code, see \ref NMSIS_Core_IntExcNMI_Handling
 *
 * The file configures the device and, typically, initializes the oscillator (PLL) that is part
 * of the microcontroller device. This file might export other functions or variables that provide
 * a more flexible configuration of the microcontroller system.
 *
 * And this file also provided common interrupt, exception and NMI exception handling framework template,
 * Silicon vendor can customize these template code as they want.
 *
 * \note Please pay special attention to the static variable \c SystemCoreClock. This variable might be
 * used throughout the whole system initialization and runtime to calculate frequency/time related values.
 * Thus one must assure that the variable always reflects the actual system clock speed.
 *
 * \attention
 * Be aware that a value stored to \c SystemCoreClock during low level initialization (i.e. \c SystemInit()) might get
 * overwritten by C libray startup code and/or .bss section initialization.
 * Thus its highly recommended to call \ref SystemCoreClockUpdate at the beginning of the user \c main() routine.
 *
 * @{
 */

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)

/* for the following variables, see intexc_evalsoc.S and intexc_evalsoc_s.S */
/** default entry for s-mode non-vector irq entry */
extern void irq_entry_s(void);
/** default entry for s-mode exception entry */
extern void exc_entry_s(void);
/** default eclic interrupt or exception interrupt handler */
extern void default_intexc_handler(void);

#ifndef __ICCRISCV__
/** eclic s-mode software interrupt handler in eclic mode */
extern void eclic_ssip_handler(void) __WEAK;
/** eclic s-mode time interrupt handler in eclic mode */
extern void eclic_stip_handler(void) __WEAK;
#else
/** eclic s-mode software interrupt handler in eclic mode */
__WEAK __SUPERVISOR_INTERRUPT void eclic_ssip_handler(void)
{

}
/** eclic s-mode time interrupt handler in eclic mode */
__WEAK __SUPERVISOR_INTERRUPT __WEAK void eclic_stip_handler(void)
{

}
#endif

/* default s-mode exception handler, which user can modify it at your need */
static void system_default_exception_handler_s(unsigned long scause, unsigned long sp);

#ifndef __ICCRISCV__
#define __SMODE_VECTOR_ATTR   __attribute__((section (".text.vtable_s"), aligned(512)))
#else
#define __SMODE_VECTOR_ATTR   __attribute__((section (".sintvec"), aligned(512)))
#endif
// TODO: change the aligned(512) to match stvt alignment requirement according to your eclic max interrupt number
// TODO: place your interrupt handler into this vector table, important if your vector table is in flash
/**
 * \var unsigned long vector_table_s[SOC_INT_MAX]
 * \brief vector interrupt storing ISRs for supervisor mode
 * \details
 *  vector_table_s is hold by stvt register, the address must align according
 *  to actual interrupt numbers as below, now align to 512 bytes considering we put up to 128 interrupts here
 *  alignment must comply to table below if you increase or decrease vector interrupt number
 *  interrupt number      alignment
 *    0 to 16               64-byte
 *    17 to 32              128-byte
 *    33 to 64              256-byte
 *    65 to 128             512-byte
 *    129 to 256              1KB
 *    257 to 512              2KB
 *    513 to 1024             4KB
 */
const unsigned long vector_table_s[SOC_INT_MAX] __SMODE_VECTOR_ATTR =
{
    (unsigned long)(default_intexc_handler),        /* 0: Reserved */
    (unsigned long)(default_intexc_handler),        /* 1: Reserved */
    (unsigned long)(default_intexc_handler),        /* 2: Reserved */

    (unsigned long)(eclic_ssip_handler),            /* 3: supervisor software interrupt in eclic mode */

    (unsigned long)(default_intexc_handler),        /* 4: Reserved */
    (unsigned long)(default_intexc_handler),        /* 5: Reserved */
    (unsigned long)(default_intexc_handler),        /* 6: Reserved */

    (unsigned long)(eclic_stip_handler),            /* 7: supervisor timer interrupt in eclic mode */

    (unsigned long)(default_intexc_handler),        /* 8: Reserved */
    (unsigned long)(default_intexc_handler),        /* 9: Reserved */
    (unsigned long)(default_intexc_handler),        /* 10: Reserved */
    (unsigned long)(default_intexc_handler),        /* 11: Reserved */

    (unsigned long)(default_intexc_handler),        /* 12: Reserved */
    (unsigned long)(default_intexc_handler),        /* 13: Reserved */
    (unsigned long)(default_intexc_handler),        /* 14: Reserved */
    (unsigned long)(default_intexc_handler),        /* 15: Reserved */

    (unsigned long)(default_intexc_handler),        /* 16: Reserved */
    (unsigned long)(default_intexc_handler),        /* 17: Reserved */
    (unsigned long)(default_intexc_handler),        /* 18: Reserved */
    (unsigned long)(default_intexc_handler),        /* 19: Interrupt 19 */

    (unsigned long)(default_intexc_handler),        /* 20: Interrupt 20 */
    (unsigned long)(default_intexc_handler),        /* 21: Interrupt 21 */
    (unsigned long)(default_intexc_handler),        /* 22: Interrupt 22 */
    (unsigned long)(default_intexc_handler),        /* 23: Interrupt 23 */

    (unsigned long)(default_intexc_handler),        /* 24: Interrupt 24 */
    (unsigned long)(default_intexc_handler),        /* 25: Interrupt 25 */
    (unsigned long)(default_intexc_handler),        /* 26: Interrupt 26 */
    (unsigned long)(default_intexc_handler),        /* 27: Interrupt 27 */

    (unsigned long)(default_intexc_handler),        /* 28: Interrupt 28 */
    (unsigned long)(default_intexc_handler),        /* 29: Interrupt 29 */
    (unsigned long)(default_intexc_handler),        /* 30: Interrupt 30 */
    (unsigned long)(default_intexc_handler),        /* 31: Interrupt 31 */

    (unsigned long)(default_intexc_handler),        /* 32: Interrupt 32 */
    (unsigned long)(default_intexc_handler),        /* 33: Interrupt 33 */
    (unsigned long)(default_intexc_handler),        /* 34: Interrupt 34 */
    (unsigned long)(default_intexc_handler),        /* 35: Interrupt 35 */

    (unsigned long)(default_intexc_handler),        /* 36: Interrupt 36 */
    (unsigned long)(default_intexc_handler),        /* 37: Interrupt 37 */
    (unsigned long)(default_intexc_handler),        /* 38: Interrupt 38 */
    (unsigned long)(default_intexc_handler),        /* 39: Interrupt 39 */

    (unsigned long)(default_intexc_handler),        /* 40: Interrupt 40 */
    (unsigned long)(default_intexc_handler),        /* 41: Interrupt 41 */
    (unsigned long)(default_intexc_handler),        /* 42: Interrupt 42 */
    (unsigned long)(default_intexc_handler),        /* 43: Interrupt 43 */

    (unsigned long)(default_intexc_handler),        /* 44: Interrupt 44 */
    (unsigned long)(default_intexc_handler),        /* 45: Interrupt 45 */
    (unsigned long)(default_intexc_handler),        /* 46: Interrupt 46 */
    (unsigned long)(default_intexc_handler),        /* 47: Interrupt 47 */

    (unsigned long)(default_intexc_handler),        /* 48: Interrupt 48 */
    (unsigned long)(default_intexc_handler),        /* 49: Interrupt 49 */
    (unsigned long)(default_intexc_handler),        /* 50: Interrupt 50 */
    (unsigned long)(default_intexc_handler),        /* 51: Interrupt 51 */

    (unsigned long)(default_intexc_handler),        /* 52: Interrupt 52 */
    (unsigned long)(default_intexc_handler),        /* 53: Interrupt 53 */
    (unsigned long)(default_intexc_handler),        /* 54: Interrupt 54 */
    (unsigned long)(default_intexc_handler),        /* 55: Interrupt 55 */

    (unsigned long)(default_intexc_handler),        /* 56: Interrupt 56 */
    (unsigned long)(default_intexc_handler),        /* 57: Interrupt 57 */
    (unsigned long)(default_intexc_handler),        /* 58: Interrupt 58 */
    (unsigned long)(default_intexc_handler),        /* 59: Interrupt 59 */

    (unsigned long)(default_intexc_handler),        /* 60: Interrupt 60 */
    (unsigned long)(default_intexc_handler),        /* 61: Interrupt 61 */
    (unsigned long)(default_intexc_handler),        /* 62: Interrupt 62 */
    (unsigned long)(default_intexc_handler),        /* 63: Interrupt 63 */
};
#endif
/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
/* ToDo: initialize SystemCoreClock with the system core clock frequency value
         achieved after system intitialization.
         This means system core clock frequency after call to SystemInit() */
/**
 * \brief      Variable to hold the system core clock value
 * \details
 * Holds the system core clock, which is the system clock frequency supplied to the SysTick
 * timer and the processor core clock. This variable can be used by debuggers to query the
 * frequency of the debug timer or to configure the trace clock speed.
 *
 * \attention
 * Compilers must be configured to avoid removing this variable in case the application
 * program is not using it. Debugging systems require the variable to be physically
 * present in memory so that it can be examined to configure the debugger.
 */
volatile uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Clock Frequency (Core Clock) */

/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/

/**
 * \brief      Function to update the variable \ref SystemCoreClock
 * \details
 * Updates the variable \ref SystemCoreClock and must be called whenever the core clock is changed
 * during program execution. The function evaluates the clock register settings and calculates
 * the current core clock.
 */
void SystemCoreClockUpdate(void)             /* Get Core Clock Frequency */
{
    /* ToDo: add code to calculate the system frequency based upon the current
     *    register settings.
     * Note: This function can be used to retrieve the system core clock frequeny
     *    after user changed register settings.
     */
    SystemCoreClock = SYSTEM_CLOCK;
}

/**
 * \brief      Function to Initialize the system.
 * \details
 * Initializes the microcontroller system. Typically, this function configures the
 * oscillator (PLL) that is part of the microcontroller device. For systems
 * with a variable clock speed, it updates the variable \ref SystemCoreClock.
 * SystemInit is called from the file <b>startup<i>_device</i></b>.
 */
void SystemInit(void)
{
    /* ToDo: add code to initialize the system
     * Warn: do not use global variables because this function is called before
     * reaching pre-main. RW section maybe overwritten afterwards.
     */
    SystemCoreClock = SYSTEM_CLOCK;
}

/**
 * \defgroup  NMSIS_Core_IntExcNMI_Handling   Interrupt and Exception and NMI Handling
 * \brief Functions for interrupt, exception and nmi handle available in system_<device>.c.
 * \details
 * Nuclei provide a template for interrupt, exception and NMI handling. Silicon Vendor could adapat according
 * to their requirement. Silicon vendor could implement interface for different exception code and
 * replace current implementation.
 *
 * @{
 */
/** \brief Max exception handler number, don't include the NMI(0xFFF) one */
#define MAX_SYSTEM_EXCEPTION_NUM        26
/**
 * \brief      Store the exception handlers for each exception ID
 * \note
 * - This SystemExceptionHandlers are used to store all the handlers for all
 * the exception codes Nuclei N/NX core provided.
 * - Exception code 0 - 25, totally 26 exceptions are mapped to SystemExceptionHandlers[0:25]
 * - Exception for NMI is also re-routed to exception handling(exception code 0xFFF) in startup code configuration, the handler itself is mapped to SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM]
 */
static unsigned long SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM + 1];

/**
 * \brief      Store the exception handlers for each exception ID in supervisor mode
 * \note
 * - This SystemExceptionHandlers_S are used to store all the handlers for all
 * the exception codes Nuclei N/NX core provided.
 * - Exception code 0 - 11, totally 12 exceptions are mapped to SystemExceptionHandlers_S[0:11]
 * - The NMI (Non-maskable-interrupt) cannot be trapped to the supervisor-mode or user-mode for any configuration
 */
#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
static unsigned long SystemExceptionHandlers_S[MAX_SYSTEM_EXCEPTION_NUM];
#endif
/**
 * \brief      Exception Handler Function Typedef
 * \note
 * This typedef is only used internal in this system_<Device>.c file.
 * It is used to do type conversion for registered exception handler before calling it.
 */
typedef void (*EXC_HANDLER)(unsigned long cause, unsigned long sp);

/**
 * \brief      System Default Exception Handler
 * \details
 * This function provides a default exception and NMI handler for all exception ids.
 * By default, It will just print some information for debug, Vendor can customize it according to its requirements.
 * \param [in]  mcause    code indicating the reason that caused the trap in machine mode
 * \param [in]  sp        stack pointer
 */
static void system_default_exception_handler(unsigned long mcause, unsigned long sp)
{
    /* TODO: Uncomment this if you have implement NSDK_DEBUG function */
    NSDK_DEBUG("MCAUSE : 0x%lx\r\n", mcause);
    NSDK_DEBUG("MDCAUSE: 0x%lx\r\n", __RV_CSR_READ(CSR_MDCAUSE));
    NSDK_DEBUG("MEPC   : 0x%lx\r\n", __RV_CSR_READ(CSR_MEPC));
    NSDK_DEBUG("MTVAL  : 0x%lx\r\n", __RV_CSR_READ(CSR_MTVAL));
    NSDK_DEBUG("HARTID : %u\r\n", (unsigned int)__get_hart_id());
    Exception_DumpFrame(sp, PRV_M);
#if defined(SIMULATION_MODE)
    // directly exit if in SIMULATION
    extern void simulation_exit(int status);
    simulation_exit(1);
#else
    while (1);
#endif
}

/**
 * \brief      Initialize all the default core exception handlers
 * \details
 * The core exception handler for each exception id will be initialized to \ref system_default_exception_handler.
 * \note
 * Called in \ref _init function, used to initialize default exception handlers for all exception IDs
 * SystemExceptionHandlers contains NMI, but SystemExceptionHandlers_S not, because NMI can't be delegated to S-mode.
 */
static void Exception_Init(void)
{
    for (int i = 0; i < MAX_SYSTEM_EXCEPTION_NUM; i++) {
        SystemExceptionHandlers[i] = (unsigned long)system_default_exception_handler;
#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
        SystemExceptionHandlers_S[i] = (unsigned long)system_default_exception_handler_s;
#endif
    }
    SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM] = (unsigned long)system_default_exception_handler;
}

/**
 * \brief      Dump Exception Frame
 * \details
 * This function provided feature to dump exception frame stored in stack.
 * \param [in]  sp    stackpoint
 * \param [in]  mode  privileged mode to decide whether to dump msubm CSR
 */
void Exception_DumpFrame(unsigned long sp, uint8_t mode)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;

#ifndef __riscv_32e
    NSDK_DEBUG("ra: 0x%lx, tp: 0x%lx, t0: 0x%lx, t1: 0x%lx, t2: 0x%lx, t3: 0x%lx, t4: 0x%lx, t5: 0x%lx, t6: 0x%lx\n" \
           "a0: 0x%lx, a1: 0x%lx, a2: 0x%lx, a3: 0x%lx, a4: 0x%lx, a5: 0x%lx, a6: 0x%lx, a7: 0x%lx\n" \
           "cause: 0x%lx, epc: 0x%lx\n", exc_frame->ra, exc_frame->tp, exc_frame->t0, \
           exc_frame->t1, exc_frame->t2, exc_frame->t3, exc_frame->t4, exc_frame->t5, exc_frame->t6, \
           exc_frame->a0, exc_frame->a1, exc_frame->a2, exc_frame->a3, exc_frame->a4, exc_frame->a5, \
           exc_frame->a6, exc_frame->a7, exc_frame->cause, exc_frame->epc);
#else
    NSDK_DEBUG("ra: 0x%lx, tp: 0x%lx, t0: 0x%lx, t1: 0x%lx, t2: 0x%lx\n" \
           "a0: 0x%lx, a1: 0x%lx, a2: 0x%lx, a3: 0x%lx, a4: 0x%lx, a5: 0x%lx\n" \
           "cause: 0x%lx, epc: 0x%lx\n", exc_frame->ra, exc_frame->tp, exc_frame->t0, \
           exc_frame->t1, exc_frame->t2, exc_frame->a0, exc_frame->a1, exc_frame->a2, exc_frame->a3, \
           exc_frame->a4, exc_frame->a5, exc_frame->cause, exc_frame->epc);
#endif

    if (PRV_M == mode) {
        /* msubm is exclusive to machine mode */
        NSDK_DEBUG("msubm: 0x%lx\n", exc_frame->msubm);
    }
}

/**
 * \brief       Register an exception handler for exception code EXCn
 * \details
 * - For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will be registered into SystemExceptionHandlers[EXCn-1].
 * - For EXCn == NMI_EXCn, it will be registered into SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM].
 * \param [in]  EXCn    See \ref EXCn_Type
 * \param [in]  exc_handler     The exception handler for this exception code EXCn
 */
void Exception_Register_EXC(uint32_t EXCn, unsigned long exc_handler)
{
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        SystemExceptionHandlers[EXCn] = exc_handler;
    } else if (EXCn == NMI_EXCn) {
        SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM] = exc_handler;
    }
}

/**
 * \brief       Get current exception handler for exception code EXCn
 * \details
 * - For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will return SystemExceptionHandlers[EXCn-1].
 * - For EXCn == NMI_EXCn, it will return SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM].
 * \param [in]  EXCn    See \ref EXCn_Type
 * \return  Current exception handler for exception code EXCn, if not found, return 0.
 */
unsigned long Exception_Get_EXC(uint32_t EXCn)
{
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        return SystemExceptionHandlers[EXCn];
    } else if (EXCn == NMI_EXCn) {
        return SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM];
    } else {
        return 0;
    }
}

/**
 * \brief      Common NMI and Exception handler entry
 * \details
 * This function provided a command entry for NMI and exception. Silicon Vendor could modify
 * this template implementation according to requirement.
 * \param [in]  mcause    code indicating the reason that caused the trap in machine mode
 * \param [in]  sp        stack pointer
 * \remarks
 * - RISCV provided common entry for all types of exception. This is proposed code template
 *   for exception entry function, Silicon Vendor could modify the implementation.
 * - For the core_exception_handler template, we provided exception register function \ref Exception_Register_EXC
 *   which can help developer to register your exception handler for specific exception number.
 */
uint32_t core_exception_handler(unsigned long mcause, unsigned long sp)
{
    uint32_t EXCn = (uint32_t)(mcause & 0X00000fff);
    EXC_HANDLER exc_handler;

    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        exc_handler = (EXC_HANDLER)SystemExceptionHandlers[EXCn];
    } else if (EXCn == NMI_EXCn) {
        exc_handler = (EXC_HANDLER)SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM];
    } else {
        exc_handler = (EXC_HANDLER)system_default_exception_handler;
    }
    if (exc_handler != NULL) {
        exc_handler(mcause, sp);
    }
    return 0;
}

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
/**
 * \brief      Supervisor mode system Default Exception Handler
 * \details
 * This function provided a default supervisor mode exception and NMI handling code for all exception ids.
 * By default, It will just print some information for debug, Vendor can customize it according to its requirements.
 * \param [in]  scause    code indicating the reason that caused the trap in supervisor mode
 * \param [in]  sp        stack pointer
 */
static void system_default_exception_handler_s(unsigned long scause, unsigned long sp)
{
    /* TODO: Uncomment this if you have implement NSDK_DEBUG function */
    NSDK_DEBUG("SCAUSE : 0x%lx\r\n", scause);
    NSDK_DEBUG("SDCAUSE: 0x%lx\r\n", __RV_CSR_READ(CSR_SDCAUSE));
    NSDK_DEBUG("SEPC   : 0x%lx\r\n", __RV_CSR_READ(CSR_SEPC));
    NSDK_DEBUG("STVAL  : 0x%lx\r\n", __RV_CSR_READ(CSR_STVAL));
    Exception_DumpFrame(sp, PRV_S);
#if defined(SIMULATION_MODE)
    // directly exit if in SIMULATION
    extern void simulation_exit(int status);
    simulation_exit(1);
#else
    while (1);
#endif
}

/**
 * \brief       Register an exception handler for exception code EXCn of supervisor mode
 * \details
 * -For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will be registered into SystemExceptionHandlers_S[EXCn-1].
 * -For EXCn == NMI_EXCn, The NMI (Non-maskable-interrupt) cannot be trapped to the supervisor-mode or user-mode for any
 *    configuration, so NMI won't be registered into SystemExceptionHandlers_S.
 * \param [in]  EXCn            See \ref EXCn_Type
 * \param [in]  exc_handler     The exception handler for this exception code EXCn
 */
void Exception_Register_EXC_S(uint32_t EXCn, unsigned long exc_handler)
{
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        SystemExceptionHandlers_S[EXCn] = exc_handler;
    }
}

/**
 * \brief       Get current exception handler for exception code EXCn of supervisor mode
 * \details
 * - For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will return SystemExceptionHandlers_S[EXCn-1].
 * \param [in]  EXCn    See \ref EXCn_Type
 * \return  Current exception handler for exception code EXCn, if not found, return 0.
 */
unsigned long Exception_Get_EXC_S(uint32_t EXCn)
{
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        return SystemExceptionHandlers[EXCn];
    } else {
        return 0;
    }
}

/**
 * \brief      common Exception handler entry of supervisor mode
 * \details
 * This function provided a supervisor mode common entry for exception. Silicon Vendor could modify
 * this template implementation according to requirement.
 * \param [in]  scause    code indicating the reason that caused the trap in supervisor mode
 * \param [in]  sp        stack pointer
 * \remarks
 * - RISCV provided supervisor mode common entry for all types of exception. This is proposed code template
 *   for exception entry function, Silicon Vendor could modify the implementation.
 * - For the core_exception_handler_s template, we provided exception register function \ref Exception_Register_EXC_S
 *   which can help developer to register your exception handler for specific exception number.
 */
uint32_t core_exception_handler_s(unsigned long scause, unsigned long sp)
{
    uint32_t EXCn = (uint32_t)(scause & 0X00000fff);
    EXC_HANDLER exc_handler;

    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        exc_handler = (EXC_HANDLER)SystemExceptionHandlers_S[EXCn];
    } else {
        exc_handler = (EXC_HANDLER)system_default_exception_handler_s;
    }
    if (exc_handler != NULL) {
        exc_handler(scause, sp);
    }
    return 0;
}
#endif

/** @} */ /* End of Doxygen Group NMSIS_Core_ExceptionAndNMI */

/** Banner Print for Nuclei SDK */
void SystemBannerPrint(void)
{
#if defined(NUCLEI_BANNER) && (NUCLEI_BANNER == 1)
    NSDK_DEBUG("Nuclei SDK Build Time: %s, %s\r\n", __DATE__, __TIME__);
#ifdef DOWNLOAD_MODE_STRING
    NSDK_DEBUG("Download Mode: %s\r\n", DOWNLOAD_MODE_STRING);
#endif
    NSDK_DEBUG("CPU Frequency %u Hz\r\n", (unsigned int)SystemCoreClock);
    NSDK_DEBUG("CPU HartID: %u\r\n", (unsigned int)__get_hart_id());
#endif
}

extern unsigned long vector_base[];
extern void irq_entry(void);
extern void exc_entry(void);

// Now this is done in Interrupt_Init
void ECLIC_Init(void)
{

}

/**
 * \brief initialize interrupt controller
 * \details
 * If ECLIC present, init it.
 * \remarks
 * This function previously was ECLIC_Init
 */
void Interrupt_Init(void)
{
    if (__RV_CSR_READ(CSR_MCFG_INFO) & MCFG_INFO_CLIC) {
        /* Set ECLIC vector interrupt base address to vector_base */
        __RV_CSR_WRITE(CSR_MTVT, (unsigned long)vector_base);
        /* Set ECLIC non-vector entry to irq_entry */
        __RV_CSR_WRITE(CSR_MTVT2, (unsigned long)irq_entry | 0x1);
        /* Set as CLIC interrupt mode */
        __RV_CSR_WRITE(CSR_MTVEC, (unsigned long)exc_entry | 0x3);
        /* Global Configuration about MTH and NLBits.
         * TODO: Please adapt it according to your system requirement.
         * This function is called in _init function */
#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
        ECLIC_SetMth(0);
        ECLIC_SetCfgNlbits(__ECLIC_INTCTLBITS);
#endif

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
        /*
         * Intialize ECLIC supervisor mode vector interrupt
         * base address stvt to vector_table_s
         */
        __RV_CSR_WRITE(CSR_STVT, (unsigned long)vector_table_s);
        /*
         * Set ECLIC supervisor mode non-vector entry to be controlled
         * by stvt2 CSR register.
         * Intialize supervisor mode ECLIC non-vector interrupt
         * base address stvt2 to irq_entry_s.
        */
        __RV_CSR_WRITE(CSR_STVT2, (unsigned long)irq_entry_s);
        __RV_CSR_SET(CSR_STVT2, 0x01);
        /*
         * Set supervisor exception entry stvec to exc_entry_s */
        __RV_CSR_WRITE(CSR_STVEC, (unsigned long)exc_entry_s);
#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
        /* Global Configuration about STH */
        ECLIC_SetSth(0);
#endif
#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
        EnableSUCCM();
#endif
#endif
    } else {
        /* Set as CLINT interrupt mode */
        __RV_CSR_WRITE(CSR_MTVEC, (unsigned long)exc_entry);
    }
}

#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
/**
 * \brief  Initialize a specific IRQ and register the handler
 * \details
 * This function set vector mode, trigger mode and polarity, interrupt level and priority,
 * assign handler for specific IRQn.
 * \param [in]  IRQn        NMI interrupt handler address
 * \param [in]  shv         \ref ECLIC_NON_VECTOR_INTERRUPT means non-vector mode, and \ref ECLIC_VECTOR_INTERRUPT is vector mode
 * \param [in]  trig_mode   see \ref ECLIC_TRIGGER_Type
 * \param [in]  lvl         interupt level
 * \param [in]  priority    interrupt priority
 * \param [in]  handler     interrupt handler, if NULL, handler will not be installed
 * \return       -1 means invalid input parameter. 0 means successful.
 * \remarks
 * - This function use to configure specific eclic interrupt and register its interrupt handler and enable its interrupt.
 * - If the vector table is placed in read-only section(FLASHXIP mode), handler could not be installed
 */
int32_t ECLIC_Register_IRQ(IRQn_Type IRQn, uint8_t shv, ECLIC_TRIGGER_Type trig_mode, uint8_t lvl, uint8_t priority, void* handler)
{
    if ((IRQn > SOC_INT_MAX) || (shv > ECLIC_VECTOR_INTERRUPT) \
        || (trig_mode > ECLIC_NEGTIVE_EDGE_TRIGGER)) {
        return -1;
    }

    /* set interrupt vector mode */
    ECLIC_SetShvIRQ(IRQn, shv);
    /* set interrupt trigger mode and polarity */
    ECLIC_SetTrigIRQ(IRQn, trig_mode);
    /* set interrupt level */
    ECLIC_SetLevelIRQ(IRQn, lvl);
    /* set interrupt priority */
    ECLIC_SetPriorityIRQ(IRQn, priority);
    if (handler != NULL) {
        /* set interrupt handler entry to vector table */
        ECLIC_SetVector(IRQn, (rv_csr_t)handler);
    }
    /* enable interrupt */
    ECLIC_EnableIRQ(IRQn);
    return 0;
}
#endif

#if (defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)) && (defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1))
/**
 * \brief  Initialize a specific IRQ and register the handler for supervisor mode
 * \details
 * This function set vector mode, trigger mode and polarity, interrupt level and priority,
 * assign handler for specific IRQn.
 * \param [in]  IRQn        NMI interrupt handler address
 * \param [in]  shv         \ref ECLIC_NON_VECTOR_INTERRUPT means non-vector mode, and \ref ECLIC_VECTOR_INTERRUPT is vector mode
 * \param [in]  trig_mode   see \ref ECLIC_TRIGGER_Type
 * \param [in]  lvl         interupt level
 * \param [in]  priority    interrupt priority
 * \param [in]  handler     interrupt handler, if NULL, handler will not be installed
 * \return       -1 means invalid input parameter. 0 means successful.
 * \remarks
 * - This function use to configure specific eclic S-mode interrupt and register its interrupt handler and enable its interrupt.
 * - If the vector table is placed in read-only section (FLASHXIP mode), handler could not be installed.
 */
int32_t ECLIC_Register_IRQ_S(IRQn_Type IRQn, uint8_t shv, ECLIC_TRIGGER_Type trig_mode, uint8_t lvl, uint8_t priority, void* handler)
{
    if ((IRQn > SOC_INT_MAX) || (shv > ECLIC_VECTOR_INTERRUPT) \
        || (trig_mode > ECLIC_NEGTIVE_EDGE_TRIGGER)) {
        return -1;
    }

    /* set interrupt vector mode */
    ECLIC_SetShvIRQ_S(IRQn, shv);
    /* set interrupt trigger mode and polarity */
    ECLIC_SetTrigIRQ_S(IRQn, trig_mode);
    /* set interrupt level */
    ECLIC_SetLevelIRQ_S(IRQn, lvl);
    /* set interrupt priority */
    ECLIC_SetPriorityIRQ_S(IRQn, priority);
    if (handler != NULL) {
        /* set interrupt handler entry to vector table */
        ECLIC_SetVector_S(IRQn, (rv_csr_t)handler);
    }
    /* enable interrupt */
    ECLIC_EnableIRQ_S(IRQn);
    return 0;
}
#endif

// NOTE: FALLBACK_DEFAULT_ECLIC_BASE/FALLBACK_DEFAULT_SYSTIMER_BASE macros are removed
// No longer support for cpu without iregion feature

#ifndef CFG_IREGION_BASE_ADDR
/** Nuclei RISC-V CPU IRegion Base Address Probed, you should avoid to use it in your application code, please use __IREGION_BASEADDR if you want */
volatile unsigned long CpuIRegionBase = 0xFFFFFFFF;
#endif

#define CLINT_MSIP(base, hartid)    (*(volatile uint32_t *)((uintptr_t)((base) + ((hartid) * 4))))
#define SMP_CTRLREG(base, ofs)      (*(volatile uint32_t *)((uintptr_t)((base) + (ofs))))

void __sync_harts(void) __attribute__((section(".text.init")));
/**
 * \brief Synchronize all harts
 * \details
 * This function is used to synchronize all the harts,
 * especially to wait the boot hart finish initialization of
 * data section, bss section and c runtines initialization
 * This function must be placed in .text.init section, since
 * section initialization is not ready, global variable
 * and static variable should be avoid to use in this function,
 * and avoid to call other functions
 */
void __sync_harts(void)
{
// Only do synchronize when SMP_CPU_CNT is defined and number > 0
// TODO: If you don't need to support SMP, you can directly remove code in it
#if defined(SMP_CPU_CNT) && (SMP_CPU_CNT > 1)
    unsigned long hartid = __get_hart_id();
    unsigned long tmr_hartid = __get_hart_index();
    unsigned long clint_base, irgb_base, smp_base;
    unsigned long mcfg_info;

    // NOTE: we should avoid to use global variable such as CpuIRegionBase before smp cpu are configured
    mcfg_info = __RV_CSR_READ(CSR_MCFG_INFO);
    // Assume IREGION feature present
    if (mcfg_info & MCFG_INFO_IREGION_EXIST) { // IRegion Info present
        // clint base = system timer base + 0x1000
        irgb_base = (__RV_CSR_READ(CSR_MIRGB_INFO) >> 10) << 10;
        clint_base = irgb_base + IREGION_TIMER_OFS + 0x1000;
        smp_base = irgb_base + IREGION_SMP_OFS;
    } else {
        // Should not enter to here if iregion feature present
        while(1);
    }
    // Enable SMP
    SMP_CTRLREG(smp_base, 0xc) = 0xFFFFFFFF;
    // Enaable L2, disable cluster local memory
    if (SMP_CTRLREG(smp_base, 0x4) & 0x1) {
        SMP_CTRLREG(smp_base, 0x10) = 0x1;
        SMP_CTRLREG(smp_base, 0xd8) = 0x0;
    }
    __SMP_RWMB();

    // pre-condition: interrupt must be disabled, this is done before calling this function
    // BOOT_HARTID is defined <Device.h>
    if (hartid == BOOT_HARTID) { // boot hart
        // clear msip pending
        for (int i = 0; i < SMP_CPU_CNT; i ++) {
            CLINT_MSIP(clint_base, i) = 0;
        }
        __SMP_RWMB();
    } else {
        // Set machine software interrupt pending to 1
        CLINT_MSIP(clint_base, tmr_hartid) = 1;
        __SMP_RWMB();
        // wait for pending bit cleared by boot hart
        while (CLINT_MSIP(clint_base, tmr_hartid) == 1);
    }
#endif
}

/**
 * \brief do the init for trap
 * \details
 */
static void Trap_Init(void)
{
}

/**
 * \brief early init function before main
 * \details
 * This function is executed right before main function.
 * For RISC-V gnu toolchain, _init function might not be called
 * by __libc_init_array function, so we defined a new function
 * to do initialization.
 */
void _premain_init(void)
{
    // TODO to make it possible for configurable boot hartid
    unsigned long hartid = __get_hart_id();
    unsigned long mcfginfo = __RV_CSR_READ(CSR_MCFG_INFO);

    /* TODO: Add your own initialization code here, called before main */
    // TODO This code controlled by macros RUNMODE_* are only used internally by Nuclei
    // You can remove it if you don't want it
    // No need to use in your code
#if defined(RUNMODE_ILM_EN)
    // Only disable ilm when it is present
    if (mcfginfo & MCFG_INFO_ILM) {
#if RUNMODE_ILM_EN == 0
        __RV_CSR_CLEAR(CSR_MILM_CTL, MILM_CTL_ILM_EN);
#else
        __RV_CSR_SET(CSR_MILM_CTL, MILM_CTL_ILM_EN);
#endif
    }
#endif

#if defined(RUNMODE_DLM_EN)
    // Only disable dlm when it is present
    if (mcfginfo & MCFG_INFO_DLM) {
#if RUNMODE_DLM_EN == 0
        __RV_CSR_CLEAR(CSR_MDLM_CTL, MDLM_CTL_DLM_EN);
#else
        __RV_CSR_SET(CSR_MDLM_CTL, MDLM_CTL_DLM_EN);
#endif
    }
#endif

#if defined(RUNMODE_LDSPEC_EN)
#if RUNMODE_LDSPEC_EN == 1
    __RV_CSR_SET(CSR_MMISC_CTL, MMISC_CTL_LDSPEC_ENABLE);
#else
    __RV_CSR_CLEAR(CSR_MMISC_CTL, MMISC_CTL_LDSPEC_ENABLE);
#endif
#endif

    /* __ICACHE_PRESENT and __DCACHE_PRESENT are defined in evalsoc.h */
    // For our internal cpu testing, they want to set evalsoc __ICACHE_PRESENT/__DCACHE_PRESENT to be 1
    // __CCM_PRESENT is still default to 0 in evalsoc.h, since it is used in core_feature_eclic.h to register interrupt, if set to 1, it might cause exception
    // but in the cpu, icache or dcache might not exist due to cpu configuration, so here
    // we need to check whether icache/dcache really exist, if yes, then turn on it
#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1)
    if (ICachePresent()) { // Check whether icache real present or not
        EnableICache();
    }
#endif
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1)
    if (DCachePresent()) { // Check whether dcache real present or not
        EnableDCache();
    }
#endif

    /* Do fence and fence.i to make sure previous ilm/dlm/icache/dcache control done */
    __RWMB();
    __FENCE_I();

    // BOOT_HARTID is defined <Device.h> and also controlled by BOOT_HARTID in conf/evalsoc/build.mk
#ifndef CFG_IREGION_BASE_ADDR       // Need to probe the cpu iregion base address
    if (hartid == BOOT_HARTID) { // only done in boot hart
        // IREGION INFO MUST BE AFTER L1/L2 Cache enabled and SMP enabled if SMP present
        CpuIRegionBase = (__RV_CSR_READ(CSR_MIRGB_INFO) >> 10) << 10;
    } else {
        // wait for correct iregion base addr is set by boot hart
        while (CpuIRegionBase == 0xFFFFFFFF);
    }
#endif

#if defined(RUNMODE_L2_EN)
    if ((mcfginfo & (0x1 << 11)) && SMP_CTRLREG(__SMPCC_BASEADDR, 0x4) & 0x1 ) { // L2 Cache present
#if RUNMODE_L2_EN == 1
        // Enable L2, disable cluster local memory
        SMP_CTRLREG(__SMPCC_BASEADDR, 0x10) = 0x1;
        SMP_CTRLREG(__SMPCC_BASEADDR, 0xd8) = 0x0;
        __SMP_RWMB();
#else
        // Disable L2, enable cluster local memory
        SMP_CTRLREG(__SMPCC_BASEADDR, 0x10) = 0x0;
        // use as clm or cache, when l2 disable, the affect to ddr is the same, l2 is really disabled
        SMP_CTRLREG(__SMPCC_BASEADDR, 0xd8) = 0;//0xFFFFFFFF;
        __SMP_RWMB();
#endif
    }
#endif

#if defined(RUNMODE_BPU_EN)
#if RUNMODE_BPU_EN == 1
    __RV_CSR_SET(CSR_MMISC_CTL, MMISC_CTL_BPU);
#else
    __RV_CSR_CLEAR(CSR_MMISC_CTL, MMISC_CTL_BPU);
#endif
#endif

    if (hartid == BOOT_HARTID) { // only required for boot hartid
        // TODO implement get_cpu_freq function to get real cpu clock freq in HZ or directly give the real cpu HZ
        // TODO you can directly give the correct cpu frequency here, if you know it without call get_cpu_freq function
        SystemCoreClock = get_cpu_freq();
        uart_init(SOC_DEBUG_UART, 115200);
        /* Display banner after UART initialized */
        SystemBannerPrint();
        /* Initialize exception default handlers */
        Exception_Init();
        /* Interrupt initialization */
        Interrupt_Init();
        Trap_Init();
        // TODO: internal usage for Nuclei
#ifdef RUNMODE_CONTROL
        NSDK_DEBUG("Current RUNMODE=%s, ilm:%d, dlm %d, icache %d, dcache %d, ccm %d\n", \
            RUNMODE_STRING, RUNMODE_ILM_EN, RUNMODE_DLM_EN, \
            RUNMODE_IC_EN, RUNMODE_DC_EN, RUNMODE_CCM_EN);
        // ILM and DLM need to be present
        if (mcfginfo & 0x180 == 0x180) {
            NSDK_DEBUG("CSR: MILM_CTL 0x%x, MDLM_CTL 0x%x\n", \
                __RV_CSR_READ(CSR_MILM_CTL), __RV_CSR_READ(CSR_MDLM_CTL));
        }
        // I/D cache need to be present
        if (mcfginfo & 0x600) {
            NSDK_DEBUG("CSR: MCACHE_CTL 0x%x\n", __RV_CSR_READ(CSR_MCACHE_CTL));
        }
        NSDK_DEBUG("CSR: MMISC_CTL 0x%x\n", __RV_CSR_READ(CSR_MMISC_CTL));
#endif
    } else {
        /* Interrupt initialization */
        Interrupt_Init();
    }
}

/**
 * \brief finish function after main
 * \param [in]  status     status code return from main
 * \details
 * This function is executed right after main function.
 * For RISC-V gnu toolchain, _fini function might not be called
 * by __libc_fini_array function, so we defined a new function
 * to do initialization
 */
void _postmain_fini(int status)
{
    /* TODO: Add your own finishing code here, called after main */
    extern void simulation_exit(int status);
    simulation_exit(status);
}

/**
 * \brief _init function called in __libc_init_array()
 * \details
 * This `__libc_init_array()` function is called during startup code,
 * user need to implement this function, otherwise when link it will
 * error init.c:(.text.__libc_init_array+0x26): undefined reference to `_init'
 * \note
 * Please use \ref _premain_init function now
 */
void _init(void)
{
    /* Don't put any code here, please use _premain_init now */
}

/**
 * \brief _fini function called in __libc_fini_array()
 * \details
 * This `__libc_fini_array()` function is called when exit main.
 * user need to implement this function, otherwise when link it will
 * error fini.c:(.text.__libc_fini_array+0x28): undefined reference to `_fini'
 * \note
 * Please use \ref _postmain_fini function now
 */
void _fini(void)
{
    /* Don't put any code here, please use _postmain_fini now */
}

/** @} */ /* End of Doxygen Group NMSIS_Core_SystemConfig */
