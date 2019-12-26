/**/
#include "riscv_encoding.h"

#********************************************************************************************************
#                                          PUBLIC FUNCTIONS
#********************************************************************************************************

    .extern  OSRunning                               # External references
    .extern  OSPrioCur
    .extern  OSPrioHighRdy
    .extern  OSTCBCur
    .extern  OSTCBHighRdy
    .extern  OSIntEnter
    .extern  OSIntExit
    .extern  OSTimeTick
    .extern  OSTaskSwHook


    .global  OSStartHighRdy                          # Functions declared in this file
    .global  OS_CPU_SR_Save
    .global  OS_CPU_SR_Restore
    .global  OSCtxSw
    .global  OSIntCtxSw
    .global  OSTickISR



#********************************************************************************************************
#                                               EQUATES
#********************************************************************************************************

    .equ  RISCV_MSTATUS_MIE,         0x08

    .equ  RISCV_MIE_MSIE,            0x08            # M Soft Interrupt bit

    .equ  RISCV_PRCI_BASE_ADDR,      0x44000000


#********************************************************************************************************
#                                     CODE GENERATION DIRECTIVES
#********************************************************************************************************

.section .text


#********************************************************************************************************
#                                 CRITICAL SECTION METHOD 3 FUNCTIONS
#
# Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
#              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
#              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
#              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
#              into the CPU's status register.
#
# Prototypes :     OS_CPU_SR  OS_CPU_SR_Save(void);
#                  void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
#
#
# Note(s)    : 1) These functions are used in general like this:
#
#                 void Task (void *p_arg)
#                 {
#                 #if OS_CRITICAL_METHOD == 3          /* Allocate storage for CPU status register */
#                     OS_CPU_SR  cpu_sr;
#                 #endif
#
#                          :
#                          :
#                     OS_ENTER_CRITICAL();             /* cpu_sr = OS_CPU_SaveSR();                */
#                          :
#                          :
#                     OS_EXIT_CRITICAL();              /* OS_CPU_RestoreSR(cpu_sr);                */
#                          :
#                          :
#                 }
#********************************************************************************************************
    .align 8
OS_CPU_SR_Save:
# Save the Machine status register
    csrr   a0, mstatus

# Disable global interupt
//RISCV_MSTATUS_MIE = 0x8 (0b1000) it is the bit of MIE in mstatus
    csrci mstatus,0x8
    ret

OS_CPU_SR_Restore:
# Restore the Machine status register previous state
    csrw   mstatus, a0
    ret

#********************************************************************************************************
#                                         START MULTITASKING
#                                      void OSStartHighRdy(void)
#
# Note(s) : 1) OSStartHighRdy() MUST:
#              a) Call OSTaskSwHook() then,
#              b) Set OSRunning to TRUE,
#              c) Set OSTCBHighRdy->OSTCBStkPtr = SP
#              d) Restore x1-x31; There is no need to restore x0 since it is always zero.
#              e) Enable interrupts (tasks will run with interrupts enabled).
#              f) Switch to highest priority task.
#********************************************************************************************************

.macro STORE_CTX_REG
    addi sp,sp,-32*REGBYTES
    STORE     ra,   0 * REGBYTES(sp)
    STORE     t0,   4 * REGBYTES(sp)
    STORE     t1,   5 * REGBYTES(sp)
    STORE     t2,   6 * REGBYTES(sp)
    STORE     s0,   7 * REGBYTES(sp)
    STORE     s1,   8 * REGBYTES(sp)
    STORE     a0,   9 * REGBYTES(sp)
    STORE     a1,  10 * REGBYTES(sp)
    STORE     a2,  11 * REGBYTES(sp)
    STORE     a3,  12 * REGBYTES(sp)
    STORE     a4,  13 * REGBYTES(sp)
    STORE     a5,  14 * REGBYTES(sp)
#ifndef __riscv_32e
    STORE     a6,  15 * REGBYTES(sp)
    STORE     a7,  16 * REGBYTES(sp)
    STORE     s2,  17 * REGBYTES(sp)
    STORE     s3,  18 * REGBYTES(sp)
    STORE     s4,  19 * REGBYTES(sp)
    STORE     s5,  20 * REGBYTES(sp)
    STORE     s6,  21 * REGBYTES(sp)
    STORE     s7,  22 * REGBYTES(sp)
    STORE     s8,  23 * REGBYTES(sp)
    STORE     s9,  24 * REGBYTES(sp)
    STORE     s10, 25 * REGBYTES(sp)
    STORE     s11, 26 * REGBYTES(sp)
    STORE     t3,  27 * REGBYTES(sp)
    STORE     t4,  28 * REGBYTES(sp)
    STORE     t5,  29 * REGBYTES(sp)
    STORE     t6,  30 * REGBYTES(sp)
#endif

.endm

.macro LOAD_CTX_REG
    LOAD     ra,   0 * REGBYTES(sp)
    LOAD     t0,   4 * REGBYTES(sp)
    LOAD     t1,   5 * REGBYTES(sp)
    LOAD     t2,   6 * REGBYTES(sp)
    LOAD     s0,   7 * REGBYTES(sp)
    LOAD     s1,   8 * REGBYTES(sp)
    LOAD     a0,   9 * REGBYTES(sp)
    LOAD     a1,  10 * REGBYTES(sp)
    LOAD     a2,  11 * REGBYTES(sp)
    LOAD     a3,  12 * REGBYTES(sp)
    LOAD     a4,  13 * REGBYTES(sp)
    LOAD     a5,  14 * REGBYTES(sp)
#ifndef __riscv_32e
    LOAD     a6,  15 * REGBYTES(sp)
    LOAD     a7,  16 * REGBYTES(sp)
    LOAD     s2,  17 * REGBYTES(sp)
    LOAD     s3,  18 * REGBYTES(sp)
    LOAD     s4,  19 * REGBYTES(sp)
    LOAD     s5,  20 * REGBYTES(sp)
    LOAD     s6,  21 * REGBYTES(sp)
    LOAD     s7,  22 * REGBYTES(sp)
    LOAD     s8,  23 * REGBYTES(sp)
    LOAD     s9,  24 * REGBYTES(sp)
    LOAD     s10, 25 * REGBYTES(sp)
    LOAD     s11, 26 * REGBYTES(sp)
    LOAD     t3,  27 * REGBYTES(sp)
    LOAD     t4,  28 * REGBYTES(sp)
    LOAD     t5,  29 * REGBYTES(sp)
    LOAD     t6,  30 * REGBYTES(sp)
#endif
    addi sp,sp,32*REGBYTES
.endm


.global exc_entry
.align 6
exc_entry:

    STORE_CTX_REG

    csrr t0,mstatus
    STORE t0,1*REGBYTES(sp)

    //call xlen_t trap_handler(mcause,mepc,sp);
    csrr a0,mcause
    mv a1,sp
    jal core_exception_handler

    LOAD t0,1*REGBYTES(sp)
    csrw mstatus,t0

    LOAD_CTX_REG

    mret

.globl eclic_mtip_handler
eclic_mtip_handler:

    STORE_CTX_REG

    csrr t0,mstatus
    STORE t0,1*REGBYTES(sp)


    csrr a0,mcause
    csrr a1,mepc
    mv a2,sp
    jal timer_irq_handler
    csrw mepc,a0

    LOAD t0,1*REGBYTES(sp)
    csrw mstatus,t0

    LOAD_CTX_REG

    mret

.align 3
.global task_switch
task_switch:
    //restore sp when trap occur ,it's important
    mv sp,a0

    //then store sp
    la t0,OSTCBCur
    LOAD t1,0(t0)
    STORE sp,0(t1)

    jal OSTaskSwHook

    //load OSTCBCur
    la t0,OSTCBHighRdy
    LOAD t1,0(t0)
    la t0,OSTCBCur
    STORE t1,0(t0)

    //load OSPrioCur
    la t0,OSPrioHighRdy //uint8
    lbu t1,0(t0)
    la t0,OSPrioCur //uint8
    sb t1,0(t0)

    //load new sp
    la     t0, OSTCBHighRdy
    LOAD   t1, 0(t0)
    LOAD   sp, 0(t1) //load sp

    //restore context
    LOAD t0,31*REGBYTES(sp)
    csrw mepc,t0
    LOAD t0,1*REGBYTES(sp)
    csrs mstatus,t0

    LOAD_CTX_REG

    mret

    .global cpu_sr_set
    .align 4
cpu_sr_set:
    csrr a0,mstatus
    csrci mstatus,0x8
    ret

    .global cpu_sr_reset
cpu_sr_reset:
    csrw mstatus,a0
    ret


.align 3
//this function don't set mstauts. So the first task should let user to enable global interrupt
OSStartHighRdy:

    //call OSTaskSwHook
    jal OSTaskSwHook

    //OSRunning=1
    li t0, 0x01
    la t1, OSRunning
    sb t0, 0(t1)

    la   t0, OSTCBHighRdy
    LOAD t1, 0(t0)
    LOAD sp, 0(t1) //load sp

    LOAD t0,31*REGBYTES(sp)
    csrw mepc,t0

    jal TickSetup

    LOAD_CTX_REG
    //run task
    mret

    .align 4

OSCtxSw:
    j  scheduler_ecall
    ret

//never be used!
// delete it in OSIntexit
OSIntCtxSw:

    j scheduler_ecall//task_switch

    .end
