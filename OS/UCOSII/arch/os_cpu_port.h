#ifndef _OS_CPU_PORT_H_
#define _OS_CPU_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nuclei_sdk_soc.h"

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure RTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR                    char
#define portFLOAT                   float
#define portDOUBLE		            double
#define portLONG		            long
#define portSHORT		            short
#define portSTACK_TYPE	            unsigned long
#define portBASE_TYPE	            long
#define portPOINTER_SIZE_TYPE       unsigned long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

/* RISC-V TIMER is 64-bit long */
typedef uint64_t TickType_t;
#define portMAX_DELAY               ( TickType_t )0xFFFFFFFFFFFFFFFFULL
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portTICK_PERIOD_MS          ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT          8
/*-----------------------------------------------------------*/

/* Scheduler utilities. */
#define portYIELD() 															\
{																				\
	/* Set a software interrupt(SWI) request to request a context switch. */    \
	SysTimer_SetSWIRQ();                                                        \
	/* Barriers are normally not required but do ensure the code is completely	\
	within the specified behaviour for the architecture. */						\
	__RWMB();										                            \
	__FENCE_I();										                        \
}

/* Critical section management. */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

#define portDISABLE_INTERRUPTS()				vPortRaiseBASEPRI()
#define portENABLE_INTERRUPTS()					vPortSetBASEPRI(0)
#define portENTER_CRITICAL()					vPortEnterCritical()
#define portEXIT_CRITICAL()						vPortExitCritical()

/*-----------------------------------------------------------*/

/* portNOP() is not required by this port. */
#define portNOP()   __NOP()

#define portINLINE	__inline

#ifndef portFORCE_INLINE
	#define portFORCE_INLINE inline __attribute__(( always_inline))
#endif

/* This variable should not be set in any of the RTOS application
  only used internal of RTOS Port code */
extern uint8_t uxMaxSysCallMTH;

/*-----------------------------------------------------------*/
portFORCE_INLINE static void vPortRaiseBASEPRI( void )
{
    ECLIC_SetMth(uxMaxSysCallMTH);
    __RWMB();
    __FENCE_I();
}

/*-----------------------------------------------------------*/
portFORCE_INLINE static uint8_t ulPortRaiseBASEPRI( void )
{
    uint8_t ulOriginalBASEPRI;

    ulOriginalBASEPRI = ECLIC_GetMth();
    ECLIC_SetMth(uxMaxSysCallMTH);
    __RWMB();
    __FENCE_I();

	/* This return might not be reached but is necessary to prevent compiler
	warnings. */
	return ulOriginalBASEPRI;
}
/*-----------------------------------------------------------*/

portFORCE_INLINE static void vPortSetBASEPRI( uint8_t ulNewMaskValue )
{
    ECLIC_SetMth(ulNewMaskValue);
    __RWMB();
    __FENCE_I();
}
/*-----------------------------------------------------------*/

#define portMEMORY_BARRIER()        __asm volatile( "" ::: "memory" )

#ifdef __cplusplus
}
#endif

#endif
