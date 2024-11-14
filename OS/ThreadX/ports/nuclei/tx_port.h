/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Port Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */
/*                                                                        */
/*    tx_port.h                                          RISC-V64/GNU     */
/*                                                           6.2.1        */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains data type definitions that make the ThreadX      */
/*    real-time kernel function identically on a variety of different     */
/*    processor architectures.  For example, the size or number of bits   */
/*    in an "int" data type vary between microprocessor architectures and */
/*    even C compilers for the same microprocessor.  ThreadX does not     */
/*    directly use native C data types.  Instead, ThreadX creates its     */
/*    own special types that can be mapped to actual data types by this   */
/*    file to guarantee consistency in the interface and functionality.   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/

#ifndef TX_PORT_H
#define TX_PORT_H

#include <stdint.h>
#include <string.h>

#ifndef TXM_MODULE
#include <nuclei_sdk_soc.h>
#endif

/* Determine if the optional ThreadX user define file should be used.  */

#ifdef TX_INCLUDE_USER_DEFINE_FILE


/* Yes, include the user defines in tx_user.h. The defines in this file may
   alternately be defined on the command line.  */

#include "tx_user.h"
#endif


/* Define compiler library include files.  */


/* Define ThreadX basic types for this port.  */

#define VOID                                    void
typedef char                                    CHAR;
typedef unsigned char                           UCHAR;
typedef int                                     INT;
typedef unsigned int                            UINT;
typedef long                                    LONG;
typedef unsigned long                           ULONG;
typedef unsigned long long                      ULONG64;
typedef short                                   SHORT;
typedef unsigned short                          USHORT;
#define ULONG64_DEFINED

#define ALIGN_TYPE_DEFINED
#define ALIGN_TYPE                              ULONG

/* Define the priority levels for ThreadX.  Legal values range
   from 32 to 1024 and MUST be evenly divisible by 32.  */

#ifndef TX_MAX_PRIORITIES
#define TX_MAX_PRIORITIES                       32
#endif


/* Define the minimum stack for a ThreadX thread on this processor. If the size supplied during
   thread creation is less than this value, the thread create call will return an error.  */

#ifndef TX_MINIMUM_STACK
#define TX_MINIMUM_STACK                        1024        /* Minimum stack size for this port  */
#endif


/* Define the max heap size allocated for ThreadX in bytes, change it to bigger or smaller according to your environment */
// This can be used in your application code as memory pool size
#ifndef TX_HEAP_SIZE
#define TX_HEAP_SIZE                            10240       /* Max heap size in bytes allocated for threadx  */
#endif


/* Define the system timer thread's default stack size and priority.  These are only applicable
   if TX_TIMER_PROCESS_IN_ISR is not defined.  */

#ifndef TX_TIMER_THREAD_STACK_SIZE
#define TX_TIMER_THREAD_STACK_SIZE              1024        /* Default timer thread stack size  */
#endif

#ifndef TX_TIMER_THREAD_PRIORITY
#define TX_TIMER_THREAD_PRIORITY                0           /* Default timer thread priority    */
#endif


/* Define various constants for the ThreadX RISC-V port.  */

#define TX_INT_DISABLE                          0x00000000  /* Disable interrupts value */
#define TX_INT_ENABLE                           0x00000008  /* Enable interrupt value   */


/* Define the clock source for trace event entry time stamp. The following two item are port specific.
   For example, if the time source is at the address 0x0a800024 and is 16-bits in size, the clock
   source constants would be:

#define TX_TRACE_TIME_SOURCE                    *((ULONG *) 0x0a800024)
#define TX_TRACE_TIME_MASK                      0x0000FFFFUL

*/

#ifndef TX_TRACE_TIME_SOURCE
#define TX_TRACE_TIME_SOURCE                    ++_tx_trace_simulated_time
#endif
#ifndef TX_TRACE_TIME_MASK
#define TX_TRACE_TIME_MASK                      0xFFFFFFFFUL
#endif


/* Define the port specific options for the _tx_build_options variable. This variable indicates
   how the ThreadX library was built.  */

#define TX_PORT_SPECIFIC_BUILD_OPTIONS          0


/* Define the in-line initialization constant so that modules with in-line
   initialization capabilities can prevent their initialization from being
   a function call.  */

#define TX_INLINE_INITIALIZATION


/* Determine whether or not stack checking is enabled. By default, ThreadX stack checking is
   disabled. When the following is defined, ThreadX thread stack checking is enabled.  If stack
   checking is enabled (TX_ENABLE_STACK_CHECKING is defined), the TX_DISABLE_STACK_FILLING
   define is negated, thereby forcing the stack fill which is necessary for the stack checking
   logic.  */

#ifdef TX_ENABLE_STACK_CHECKING
#undef TX_DISABLE_STACK_FILLING
#endif



/* Define the TX_THREAD control block extensions for this port. The main reason
   for the multiple macros is so that backward compatibility can be maintained with
   existing ThreadX kernel awareness modules.  */

#define TX_THREAD_EXTENSION_0
#define TX_THREAD_EXTENSION_1
#ifdef  TX_ENABLE_IAR_LIBRARY_SUPPORT
#define TX_THREAD_EXTENSION_2               VOID    *tx_thread_module_instance_ptr;         \
                                            VOID    *tx_thread_module_entry_info_ptr;       \
                                            ULONG   tx_thread_module_current_user_mode;     \
                                            ULONG   tx_thread_module_user_mode;             \
                                            ULONG   tx_thread_module_saved_lr;              \
                                            VOID    *tx_thread_module_kernel_stack_start;   \
                                            VOID    *tx_thread_module_kernel_stack_end;     \
                                            ULONG   tx_thread_module_kernel_stack_size;     \
                                            VOID    *tx_thread_module_stack_ptr;            \
                                            VOID    *tx_thread_module_stack_start;          \
                                            VOID    *tx_thread_module_stack_end;            \
                                            ULONG   tx_thread_module_stack_size;            \
                                            VOID    *tx_thread_module_reserved;             \
                                            VOID    *tx_thread_iar_tls_pointer;
#else
#define TX_THREAD_EXTENSION_2               VOID    *tx_thread_module_instance_ptr;         \
                                            VOID    *tx_thread_module_entry_info_ptr;       \
                                            ULONG   tx_thread_module_current_user_mode;     \
                                            ULONG   tx_thread_module_user_mode;             \
                                            ULONG   tx_thread_module_saved_lr;              \
                                            VOID    *tx_thread_module_kernel_stack_start;   \
                                            VOID    *tx_thread_module_kernel_stack_end;     \
                                            ULONG   tx_thread_module_kernel_stack_size;     \
                                            VOID    *tx_thread_module_stack_ptr;            \
                                            VOID    *tx_thread_module_stack_start;          \
                                            VOID    *tx_thread_module_stack_end;            \
                                            ULONG   tx_thread_module_stack_size;            \
                                            VOID    *tx_thread_module_reserved;
#endif
#ifndef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
#define TX_THREAD_EXTENSION_3
#else
#define TX_THREAD_EXTENSION_3           unsigned long long  tx_thread_execution_time_total; \
                                        unsigned long long  tx_thread_execution_time_last_start;
#endif


/* Define the port extensions of the remaining ThreadX objects.  */

#define TX_BLOCK_POOL_EXTENSION
#define TX_BYTE_POOL_EXTENSION
#define TX_MUTEX_EXTENSION
#define TX_EVENT_FLAGS_GROUP_EXTENSION          VOID    *tx_event_flags_group_module_instance; \
                                                VOID   (*tx_event_flags_group_set_module_notify)(struct TX_EVENT_FLAGS_GROUP_STRUCT *group_ptr);

#define TX_QUEUE_EXTENSION                      VOID    *tx_queue_module_instance; \
                                                VOID   (*tx_queue_send_module_notify)(struct TX_QUEUE_STRUCT *queue_ptr);

#define TX_SEMAPHORE_EXTENSION                  VOID    *tx_semaphore_module_instance; \
                                                VOID   (*tx_semaphore_put_module_notify)(struct TX_SEMAPHORE_STRUCT *semaphore_ptr);

#define TX_TIMER_EXTENSION                      VOID    *tx_timer_module_instance; \
                                                VOID   (*tx_timer_module_expiration_function)(ULONG id);


/* Define the user extension field of the thread control block.  Nothing
   additional is needed for this port so it is defined as white space.  */

#ifndef TX_THREAD_USER_EXTENSION
#define TX_THREAD_USER_EXTENSION
#endif



/* Define the macros for processing extensions in tx_thread_create, tx_thread_delete,
   tx_thread_shell_entry, and tx_thread_terminate.  */

#define TX_THREAD_CREATE_EXTENSION(thread_ptr)
#define TX_THREAD_DELETE_EXTENSION(thread_ptr)
#define TX_THREAD_COMPLETED_EXTENSION(thread_ptr)
#define TX_THREAD_TERMINATED_EXTENSION(thread_ptr)


/* Define the ThreadX object creation extensions for the remaining objects.  */

#define TX_BLOCK_POOL_CREATE_EXTENSION(pool_ptr)
#define TX_BYTE_POOL_CREATE_EXTENSION(pool_ptr)
#define TX_EVENT_FLAGS_GROUP_CREATE_EXTENSION(group_ptr)
#define TX_MUTEX_CREATE_EXTENSION(mutex_ptr)
#define TX_QUEUE_CREATE_EXTENSION(queue_ptr)
#define TX_SEMAPHORE_CREATE_EXTENSION(semaphore_ptr)
#define TX_TIMER_CREATE_EXTENSION(timer_ptr)


/* Define the ThreadX object deletion extensions for the remaining objects.  */

#define TX_BLOCK_POOL_DELETE_EXTENSION(pool_ptr)
#define TX_BYTE_POOL_DELETE_EXTENSION(pool_ptr)
#define TX_EVENT_FLAGS_GROUP_DELETE_EXTENSION(group_ptr)
#define TX_MUTEX_DELETE_EXTENSION(mutex_ptr)
#define TX_QUEUE_DELETE_EXTENSION(queue_ptr)
#define TX_SEMAPHORE_DELETE_EXTENSION(semaphore_ptr)
#define TX_TIMER_DELETE_EXTENSION(timer_ptr)


/* Define ThreadX interrupt lockout and restore macros for protection on
   access of critical kernel information.  The restore interrupt macro must
   restore the interrupt posture of the running thread prior to the value
   present prior to the disable macro.  In most cases, the save area macro
   is used to define a local function save area for the disable and restore
   macros.  */

#ifdef TX_DISABLE_INLINE

#define TX_INTERRUPT_SAVE_AREA                  register ULONG interrupt_save;

#define TX_DISABLE                              interrupt_save = _tx_thread_interrupt_control(TX_INT_DISABLE);
#define TX_RESTORE                              _tx_thread_interrupt_control(interrupt_save);

#else

#define TX_INTERRUPT_SAVE_AREA                  ULONG interrupt_save;
/* Atomically read mstatus into interrupt_save and clear bit 3 of mstatus.  */
#define TX_DISABLE                              {__asm__ ("csrrci %0, mstatus, 0x08" : "=r" (interrupt_save) : );};
/* We only care about mstatus.mie (bit 3), so mask interrupt_save and write to mstatus.  */
#define TX_RESTORE                              {register ULONG __tempmask = interrupt_save & 0x08; \
                                                __asm__ ("csrrs x0, mstatus, %0 \n\t" : : "r" (__tempmask) : );};

#endif

#ifndef TXM_MODULE
static inline void _tx_thread_system_return(void)
{
        /* Set a software interrupt(SWI) request to request a context switch. */
        SysTimer_SetSWIRQ();
        /* Barriers are normally not required but do ensure the code is completely
        within the specified behaviour for the architecture. */
        __RWMB();
}
#endif

#define THREAD_INITIAL_MSTATUS      (MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_FS_INITIAL | MSTATUS_VS_INITIAL)

struct thread_stack_frame {
    unsigned long epc;        /* epc - epc    - program counter                     */
    unsigned long ra;         /* x1  - ra     - return address for jumps            */
    unsigned long t0;         /* x5  - t0     - temporary register 0                */
    unsigned long t1;         /* x6  - t1     - temporary register 1                */
    unsigned long t2;         /* x7  - t2     - temporary register 2                */
    unsigned long s0_fp;      /* x8  - s0/fp  - saved register 0 or frame pointer   */
    unsigned long s1;         /* x9  - s1     - saved register 1                    */
    unsigned long a0;         /* x10 - a0     - return value or function argument 0 */
    unsigned long a1;         /* x11 - a1     - return value or function argument 1 */
    unsigned long a2;         /* x12 - a2     - function argument 2                 */
    unsigned long a3;         /* x13 - a3     - function argument 3                 */
    unsigned long a4;         /* x14 - a4     - function argument 4                 */
    unsigned long a5;         /* x15 - a5     - function argument 5                 */
#ifndef __riscv_32e
    unsigned long a6;         /* x16 - a6     - function argument 6                 */
    unsigned long a7;         /* x17 - s7     - function argument 7                 */
    unsigned long s2;         /* x18 - s2     - saved register 2                    */
    unsigned long s3;         /* x19 - s3     - saved register 3                    */
    unsigned long s4;         /* x20 - s4     - saved register 4                    */
    unsigned long s5;         /* x21 - s5     - saved register 5                    */
    unsigned long s6;         /* x22 - s6     - saved register 6                    */
    unsigned long s7;         /* x23 - s7     - saved register 7                    */
    unsigned long s8;         /* x24 - s8     - saved register 8                    */
    unsigned long s9;         /* x25 - s9     - saved register 9                    */
    unsigned long s10;        /* x26 - s10    - saved register 10                   */
    unsigned long s11;        /* x27 - s11    - saved register 11                   */
    unsigned long t3;         /* x28 - t3     - temporary register 3                */
    unsigned long t4;         /* x29 - t4     - temporary register 4                */
    unsigned long t5;         /* x30 - t5     - temporary register 5                */
    unsigned long t6;         /* x31 - t6     - temporary register 6                */
#endif
    unsigned long mstatus;    /*              - machine status register             */
};


/* Define the interrupt lockout macros for each ThreadX object.  */

#define TX_BLOCK_POOL_DISABLE                   TX_DISABLE
#define TX_BYTE_POOL_DISABLE                    TX_DISABLE
#define TX_EVENT_FLAGS_GROUP_DISABLE            TX_DISABLE
#define TX_MUTEX_DISABLE                        TX_DISABLE
#define TX_QUEUE_DISABLE                        TX_DISABLE
#define TX_SEMAPHORE_DISABLE                    TX_DISABLE


/* Define the version ID of ThreadX.  This may be utilized by the application.  */

#ifdef TX_THREAD_INIT
CHAR                            _tx_version_id[] =
                                    "Copyright (c) Microsoft Corporation. All rights reserved. * ThreadX Nuclei RISC-V Version 6.4.0 *";
#else
extern  CHAR                    _tx_version_id[];
#endif

#endif
