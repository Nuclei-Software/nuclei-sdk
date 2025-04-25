/*
 * FreeRTOS Kernel Port For Nuclei RISC-V Processor
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nuclei_sdk_soc.h"

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR                    char
#define portFLOAT                   float
#define portDOUBLE                  double
#define portLONG                    long
#define portSHORT                   short
#define portSTACK_TYPE              unsigned long
#define portBASE_TYPE               long
#define portPOINTER_SIZE_TYPE       unsigned long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;


/* TODO If you want to use openocd rtos-aware debug, you need to define OPENOCD_RTOS_AWARE macro */
//#define OPENOCD_RTOS_AWARE
#ifdef OPENOCD_RTOS_AWARE
#undef configTICK_TYPE_WIDTH_IN_BITS
#define configTICK_TYPE_WIDTH_IN_BITS TICK_TYPE_WIDTH_32_BITS
#endif

// configTICK_TYPE_WIDTH_IN_BITS
// see https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/03db672b8f45db24aa99f12051f7cf86746b9ed9/examples/template_configuration/FreeRTOSConfig.h#L124-L139
#if ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS )
typedef uint16_t TickType_t;
#define portMAX_DELAY           ( TickType_t )0xffff
#elif ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS )
typedef uint32_t TickType_t;
#define portMAX_DELAY           ( TickType_t )0xFFFFFFFFUL
#else
typedef uint64_t TickType_t;
#define portMAX_DELAY           ( TickType_t )0xFFFFFFFFFFFFFFFFULL
#endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH            ( -1 )
#define portTICK_PERIOD_MS          ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT          8
/*-----------------------------------------------------------*/

/* Scheduler utilities. */
#define portYIELD()                                                             \
    {                                                                               \
        /* Set a software interrupt(SWI) request to request a context switch. */    \
        SysTimer_SetSWIRQ();                                                        \
        /* Barriers are normally not required but do ensure the code is completely  \
        within the specified behaviour for the architecture. */                     \
        __RWMB();                                                                   \
    }

#define portEND_SWITCHING_ISR( xSwitchRequired )    if ( xSwitchRequired != pdFALSE ) portYIELD()
#define portYIELD_FROM_ISR( x )                     portEND_SWITCHING_ISR( x )
/*-----------------------------------------------------------*/


#define portSET_INTERRUPT_MASK_FROM_ISR()       ulPortRaiseBASEPRI()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)    vPortSetBASEPRI(x)
#define portDISABLE_INTERRUPTS()                vPortRaiseBASEPRI()
#define portENABLE_INTERRUPTS()                 vPortSetBASEPRI(0)

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site.  These are
not necessary for to use this port.  They are defined so the common demo files
(which build with all the ports) will build. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )      void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )            void vFunction( void *pvParameters )
/*-----------------------------------------------------------*/

/* Tickless idle/low power functionality. */
#ifndef portSUPPRESS_TICKS_AND_SLEEP
extern void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime);
#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime )       vPortSuppressTicksAndSleep( xExpectedIdleTime )
#endif
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

#ifdef configASSERT
extern void vPortValidateInterruptPriority(void);
#define portASSERT_IF_INTERRUPT_PRIORITY_INVALID()              vPortValidateInterruptPriority()
#endif

/* portNOP() is not required by this port. */
#define portNOP()                   __NOP()

#define portINLINE                  __inline

#ifndef portFORCE_INLINE
#define portFORCE_INLINE            inline __attribute__(( always_inline))
#endif

/* This variable should not be set in any of the FreeRTOS application
  only used internal of FreeRTOS Port code */
extern uint8_t uxMaxSysCallMTH;

/*-----------------------------------------------------------*/
portFORCE_INLINE static void vPortRaiseBASEPRI(void)
{
    ECLIC_SetMth(uxMaxSysCallMTH);
    __RWMB();
}

/*-----------------------------------------------------------*/

portFORCE_INLINE static uint8_t ulPortRaiseBASEPRI(void)
{
    uint8_t ulOriginalBASEPRI;

    ulOriginalBASEPRI = ECLIC_GetMth();
    ECLIC_SetMth(uxMaxSysCallMTH);
    __RWMB();

    /* This return might not be reached but is necessary to prevent compiler
    warnings. */
    return ulOriginalBASEPRI;
}
/*-----------------------------------------------------------*/

portFORCE_INLINE static void vPortSetBASEPRI(uint8_t ulNewMaskValue)
{
    ECLIC_SetMth(ulNewMaskValue);
    __RWMB();
}
/*-----------------------------------------------------------*/

#define portMEMORY_BARRIER()                            __asm volatile( "" ::: "memory" )

#if ( configNUMBER_OF_CORES == 1 )
    extern UBaseType_t uxCriticalNesting;
    #define portGET_CRITICAL_NESTING_COUNT()            ( uxCriticalNesting )
    #define portSET_CRITICAL_NESTING_COUNT( x )         ( uxCriticalNesting = ( x ) )
    #define portINCREMENT_CRITICAL_NESTING_COUNT()      ( uxCriticalNesting++ )
    #define portDECREMENT_CRITICAL_NESTING_COUNT()      ( uxCriticalNesting-- )
    /* Critical section management. */
    extern void vPortEnterCritical(void);
    extern void vPortExitCritical(void);

    #define portENTER_CRITICAL()                        vPortEnterCritical()
    #define portEXIT_CRITICAL()                         vPortExitCritical()
#else
    /* Return the core ID on which the code is running. */
    #define portGET_CORE_ID()                           __get_hart_index()

    /* Set the interrupt mask. */
    #define portSET_INTERRUPT_MASK()                    ulPortRaiseBASEPRI()

    /* Clear the interrupt mask. */
    #define portCLEAR_INTERRUPT_MASK( x )               vPortSetBASEPRI(x)

    /* Request the core ID x to yield. */
    #define portYIELD_CORE( x )              \
    {                                                                               \
        /* Set a software interrupt(SWI) to core x  to request a context switch. */ \
        SysTimer_SetHartSWIRQ(x);                                                   \
        /* Barriers are normally not required but do ensure the code is completely  \
        within the specified behaviour for the architecture. */                     \
        __RWMB();                                                                   \
    }

    #define portRTOS_SPINLOCK_COUNT                     2
    /* Multi-core */
    #define portMAX_CORE_COUNT                          16

    typedef volatile uint32_t spin_lock_t;
    extern spin_lock_t hw_sync_locks[portRTOS_SPINLOCK_COUNT];
    extern void vPortRecursiveLock(unsigned long ulLockNum, spin_lock_t *pxSpinLock, BaseType_t uxAcquire);

    /* Acquire the TASK lock. TASK lock is a recursive lock.
     * It should be able to be locked by the same core multiple times. */
    #define portGET_TASK_LOCK()                         vPortRecursiveLock( 1, &hw_sync_locks[1], pdTRUE )

    /* Release the TASK lock. If a TASK lock is locked by the same core multiple times,
     * it should be released as many times as it is locked. */
    #define portRELEASE_TASK_LOCK()                     vPortRecursiveLock( 1, &hw_sync_locks[1], pdFALSE )

    /* Acquire the ISR lock. ISR lock is a recursive lock.
     * It should be able to be locked by the same core multiple times. */
   #define portGET_ISR_LOCK()                           vPortRecursiveLock( 0, &hw_sync_locks[0], pdTRUE )

    /* Release the ISR lock. If a ISR lock is locked by the same core multiple times, \
     * it should be released as many times as it is locked. */
    #define portRELEASE_ISR_LOCK()                      vPortRecursiveLock( 0, &hw_sync_locks[0], pdFALSE )

    extern void vTaskEnterCritical( void );
    extern void vTaskExitCritical( void );
    extern UBaseType_t vTaskEnterCriticalFromISR( void );
    extern void vTaskExitCriticalFromISR( UBaseType_t uxSavedInterruptStatus );
    #define portENTER_CRITICAL()                        vTaskEnterCritical()
    #define portEXIT_CRITICAL()                         vTaskExitCritical()
    #define portENTER_CRITICAL_FROM_ISR()               vTaskEnterCriticalFromISR()
    #define portEXIT_CRITICAL_FROM_ISR( x )             vTaskExitCriticalFromISR( x )

    /* Critical nesting count management. */
    extern UBaseType_t uxCriticalNestings[ configNUMBER_OF_CORES ];
    #define portGET_CRITICAL_NESTING_COUNT()            ( uxCriticalNestings[ portGET_CORE_ID() ] )
    #define portSET_CRITICAL_NESTING_COUNT( x )         ( uxCriticalNestings[ portGET_CORE_ID() ] = ( x ) )
    #define portINCREMENT_CRITICAL_NESTING_COUNT()      ( uxCriticalNestings[ portGET_CORE_ID() ]++ )
    #define portDECREMENT_CRITICAL_NESTING_COUNT()      ( uxCriticalNestings[ portGET_CORE_ID() ]-- )

#endif /* if ( configNUMBER_OF_CORES > 1 ) */

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

