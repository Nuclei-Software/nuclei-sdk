#include "riscv_encoding.h"
#include "cpufeature.h"

#ifndef __riscv_32e
#define portRegNum          32
#else
#define portRegNum          14
#endif

#define portCONTEXT_SIZE    ( portRegNum * REGBYTES )

    .extern _tx_thread_current_ptr
    .extern _tx_thread_execute_ptr
    .extern _tx_find_ready_thread


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
#if defined(SMP_CPU_CNT) && (SMP_CPU_CNT > 1)
    /* get correct sp for each cpu
     * each stack size is __STACK_SIZE
     * defined in linker script */
    lui t1, %hi(__STACK_SIZE)
    addi t1, t1, %lo(__STACK_SIZE)
    la t0, _sp
    csrr a0, CSR_MHARTID
    andi a0, a0, 0xFF
    li a1, 0
1:
    beq a0, a1, 2f
    sub t0, t0, t1
    addi a1, a1, 1
    j 1b
2:
#else
    la t0, _sp // TODO need to consider SMP case
#endif
    csrw CSR_MSCRATCH, t0
    /* Request _tx_find_ready_thread to set the current thread. */
    li a0, 1
    call _tx_find_ready_thread

    /* Increment the run count for this thread.  */
    /* _tx_thread_current_ptr[coreid] -> tx_thread_run_count++;  */
    LOAD  t0, 1 * REGBYTES(a0)
    addi  t0, t0, 1
    STORE t0, 1 * REGBYTES(a0)

    LOAD sp,  2 * REGBYTES(a0)      /* Read sp from selected thread -> tx_thread_stack_ptr */
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
    /* Store last pc to thread stack */
    csrr t0, CSR_MEPC
    STORE t0, 0(sp)

    /* Push additional registers */

#if defined(SMP_CPU_CNT) && (SMP_CPU_CNT > 1)
    la t0, _tx_thread_current_ptr
    csrr t1, CSR_MHARTID
    slli t1, t1, LOG_REGBYTES
    add t0, t0, t1
#else
    la t0, _tx_thread_current_ptr
#endif
    LOAD t0, 0(t0)
    /* If _tx_thread_current_ptr[coreid] is null, no sp need to be saved */
    beqz t0, _tx_thread_save_switch
    /* Store sp to task stack to _tx_thread_current_ptr -> tx_thread_stack_ptr */
    STORE sp, 2 * REGBYTES(t0)

_tx_thread_save_switch:
    jal PortThreadSwitch

    /* PortThreadSwitch has claimed and published the selected thread in
       _tx_thread_current_ptr[coreid].  Restore that exact thread instead of
       reloading _tx_thread_execute_ptr, which may be changed by another hart. */
#if defined(SMP_CPU_CNT) && (SMP_CPU_CNT > 1)
    la t0, _tx_thread_current_ptr
    csrr t1, CSR_MHARTID
    slli t1, t1, LOG_REGBYTES
    add t0, t0, t1
#else
    la t0, _tx_thread_current_ptr
#endif
    LOAD t0, 0(t0)
    /* If _tx_thread_current_ptr[coreid] is null, no sp need to be restored */
    beqz t0, _tx_thread_restore_switch
    LOAD sp, 2 * REGBYTES(t0)
_tx_thread_restore_switch:

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
