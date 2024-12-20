#include "riscv_encoding.h"

#ifndef __riscv_32e
#define portRegNum          30
#else
#define portRegNum          14
#endif

#define portCONTEXT_SIZE    ( portRegNum * REGBYTES )

    .extern _tx_thread_current_ptr
    .extern _tx_thread_execute_ptr


.section    .text

/*
 * VOID   _tx_thread_schedule(VOID);
 */
    .globl _tx_thread_schedule

/* Start the first task.  This also clears the bit that indicates the FPU is
    in use in case the FPU was used before the scheduler was started - which
    would otherwise result in the unnecessary leaving of space in the stack
    for lazy saving of FPU registers. */
.align 3
.type _tx_thread_schedule, @function
_tx_thread_schedule:
    /* Setup Interrupt Stack using
       The stack that was used by main()
       before the scheduler is started is
       no longer required after the scheduler is started.
       Interrupt stack pointer is stored in CSR_MSCRATCH */
    la t0, _sp
    csrw CSR_MSCRATCH, t0
    /* Enable interrupt to wait _tx_thread_execute_ptr is not null */
    csrsi CSR_MSTATUS, MSTATUS_MIE
_tx_thread_schedule_loop:
    la a0, _tx_thread_execute_ptr   /* Pick current thread to a0 */
    LOAD a0, 0(a0)
    beqz a0, _tx_thread_schedule_loop
_tx_thread_schedule_ready:
    /* Disable interrupt to load first thread */
    csrci CSR_MSTATUS, MSTATUS_MIE
    la a1, _tx_thread_current_ptr  /* _tx_thread_current_ptr = _tx_thread_execute_ptr */
    STORE a0, 0(a1)
    LOAD sp, 2  * REGBYTES(a0)      /* Read sp from _tx_thread_execute_ptr -> tx_thread_stack_ptr */

    /* Pop PC from stack and set MEPC */
    LOAD t0,  0  * REGBYTES(sp)
    csrw CSR_MEPC, t0
    /* Pop mstatus from stack and set it */
    LOAD t0,  (portRegNum - 1)  * REGBYTES(sp)
    csrw CSR_MSTATUS, t0
    /* Interrupt still disable here */
    /* Restore Registers from Stack */
    LOAD x1,  1  * REGBYTES(sp)    /* RA */
    LOAD x5,  2  * REGBYTES(sp)
    LOAD x6,  3  * REGBYTES(sp)
    LOAD x7,  4  * REGBYTES(sp)
    LOAD x8,  5  * REGBYTES(sp)
    LOAD x9,  6  * REGBYTES(sp)
    LOAD x10, 7  * REGBYTES(sp)
    LOAD x11, 8  * REGBYTES(sp)
    LOAD x12, 9  * REGBYTES(sp)
    LOAD x13, 10 * REGBYTES(sp)
    LOAD x14, 11 * REGBYTES(sp)
    LOAD x15, 12 * REGBYTES(sp)
#ifndef __riscv_32e
    LOAD x16, 13 * REGBYTES(sp)
    LOAD x17, 14 * REGBYTES(sp)
    LOAD x18, 15 * REGBYTES(sp)
    LOAD x19, 16 * REGBYTES(sp)
    LOAD x20, 17 * REGBYTES(sp)
    LOAD x21, 18 * REGBYTES(sp)
    LOAD x22, 19 * REGBYTES(sp)
    LOAD x23, 20 * REGBYTES(sp)
    LOAD x24, 21 * REGBYTES(sp)
    LOAD x25, 22 * REGBYTES(sp)
    LOAD x26, 23 * REGBYTES(sp)
    LOAD x27, 24 * REGBYTES(sp)
    LOAD x28, 25 * REGBYTES(sp)
    LOAD x29, 26 * REGBYTES(sp)
    LOAD x30, 27 * REGBYTES(sp)
    LOAD x31, 28 * REGBYTES(sp)
#endif

    addi sp, sp, portCONTEXT_SIZE

    mret

    .size _tx_thread_schedule, . - _tx_thread_schedule

.align 2
.global eclic_msip_handler
.type eclic_msip_handler, @function
eclic_msip_handler:
    addi sp, sp, -portCONTEXT_SIZE
    STORE x1,  1  * REGBYTES(sp)    /* RA */
    STORE x5,  2  * REGBYTES(sp)
    STORE x6,  3  * REGBYTES(sp)
    STORE x7,  4  * REGBYTES(sp)
    STORE x8,  5  * REGBYTES(sp)
    STORE x9,  6  * REGBYTES(sp)
    STORE x10, 7  * REGBYTES(sp)
    STORE x11, 8  * REGBYTES(sp)
    STORE x12, 9  * REGBYTES(sp)
    STORE x13, 10 * REGBYTES(sp)
    STORE x14, 11 * REGBYTES(sp)
    STORE x15, 12 * REGBYTES(sp)
#ifndef __riscv_32e
    STORE x16, 13 * REGBYTES(sp)
    STORE x17, 14 * REGBYTES(sp)
    STORE x18, 15 * REGBYTES(sp)
    STORE x19, 16 * REGBYTES(sp)
    STORE x20, 17 * REGBYTES(sp)
    STORE x21, 18 * REGBYTES(sp)
    STORE x22, 19 * REGBYTES(sp)
    STORE x23, 20 * REGBYTES(sp)
    STORE x24, 21 * REGBYTES(sp)
    STORE x25, 22 * REGBYTES(sp)
    STORE x26, 23 * REGBYTES(sp)
    STORE x27, 24 * REGBYTES(sp)
    STORE x28, 25 * REGBYTES(sp)
    STORE x29, 26 * REGBYTES(sp)
    STORE x30, 27 * REGBYTES(sp)
    STORE x31, 28 * REGBYTES(sp)
#endif
    /* Push mstatus to stack */
    csrr t0, CSR_MSTATUS
    STORE t0,  (portRegNum - 1)  * REGBYTES(sp)

    /* Push additional registers */

    /* Store sp to task stack to _tx_thread_current_ptr -> tx_thread_stack_ptr*/
    la t0, _tx_thread_current_ptr
    LOAD t0, 0(t0)
    STORE sp, 2 * REGBYTES(t0)

    csrr t0, CSR_MEPC
    STORE t0, 0(sp)

    jal PortThreadSwitch

    /* Switch task context to _tx_thread_execute_ptr */
    la t0, _tx_thread_execute_ptr
    LOAD t0, 0(t0)
    LOAD sp, 2 * REGBYTES(t0)

    /* Pop PC from stack and set MEPC */
    LOAD t0,  0  * REGBYTES(sp)
    csrw CSR_MEPC, t0
    /* Pop additional registers */

    /* Pop mstatus from stack and set it */
    LOAD t0,  (portRegNum - 1)  * REGBYTES(sp)
    csrw CSR_MSTATUS, t0
    /* Interrupt still disable here */
    /* Restore Registers from Stack */
    LOAD x1,  1  * REGBYTES(sp)    /* RA */
    LOAD x5,  2  * REGBYTES(sp)
    LOAD x6,  3  * REGBYTES(sp)
    LOAD x7,  4  * REGBYTES(sp)
    LOAD x8,  5  * REGBYTES(sp)
    LOAD x9,  6  * REGBYTES(sp)
    LOAD x10, 7  * REGBYTES(sp)
    LOAD x11, 8  * REGBYTES(sp)
    LOAD x12, 9  * REGBYTES(sp)
    LOAD x13, 10 * REGBYTES(sp)
    LOAD x14, 11 * REGBYTES(sp)
    LOAD x15, 12 * REGBYTES(sp)
#ifndef __riscv_32e
    LOAD x16, 13 * REGBYTES(sp)
    LOAD x17, 14 * REGBYTES(sp)
    LOAD x18, 15 * REGBYTES(sp)
    LOAD x19, 16 * REGBYTES(sp)
    LOAD x20, 17 * REGBYTES(sp)
    LOAD x21, 18 * REGBYTES(sp)
    LOAD x22, 19 * REGBYTES(sp)
    LOAD x23, 20 * REGBYTES(sp)
    LOAD x24, 21 * REGBYTES(sp)
    LOAD x25, 22 * REGBYTES(sp)
    LOAD x26, 23 * REGBYTES(sp)
    LOAD x27, 24 * REGBYTES(sp)
    LOAD x28, 25 * REGBYTES(sp)
    LOAD x29, 26 * REGBYTES(sp)
    LOAD x30, 27 * REGBYTES(sp)
    LOAD x31, 28 * REGBYTES(sp)
#endif

    addi sp, sp, portCONTEXT_SIZE
    mret

    .size eclic_msip_handler, . - eclic_msip_handler
