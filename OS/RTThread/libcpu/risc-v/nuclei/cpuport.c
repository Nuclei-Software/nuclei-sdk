/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 * Copyright (c) 2019-Present Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020/03/26     Huaqi        Nuclei RISC-V Core porting code.
 */

#include <rthw.h>
#include <rtthread.h>
#include <stdio.h>

#include "cpuport.h"

#ifndef BOOT_HARTID
#define EXECUTE_HARTID  0
#else
#define EXECUTE_HARTID  BOOT_HARTID
#endif


#define SYSTICK_TICK_CONST                          (SOC_TIMER_FREQ / RT_TICK_PER_SECOND)

#ifndef configKERNEL_INTERRUPT_PRIORITY
#define configKERNEL_INTERRUPT_PRIORITY         0
#endif

#ifndef configMAX_SYSCALL_INTERRUPT_PRIORITY
// See function prvCheckMaxSysCallPrio and prvCalcMaxSysCallMTH
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    255
#endif

#ifdef SMODE_RTOS
#define SysTick_Handler     eclic_stip_handler
extern void eclic_ssip_handler(void);
#define portINITIAL_XSTATUS ( SSTATUS_SPP | SSTATUS_SPIE | MSTATUS_FS_INITIAL | MSTATUS_VS_INITIAL)

#if defined(__SSTC_PRESENT) && __SSTC_PRESENT == 1
#define SMODE_TIMER_IRQ         SysTimer_S_IRQn
#define SMODE_SWI_IRQ           SysTimerSW_S_IRQn
#define SMODE_SET_SWI()         SysTimer_SetSWIRQ_S()
#define SMODE_CLR_SWI()         SysTimer_ClearSWIRQ_S()
#define SMODE_TICK_CONFIG()     SysTick_Config_S(SYSTICK_TICK_CONST)
#define SMODE_TICK_RELOAD()     SysTick_Reload_S(SYSTICK_TICK_CONST)
#else
#define SMODE_TIMER_IRQ         SysTimer_IRQn
#define SMODE_SWI_IRQ           SysTimerSW_IRQn
#define SMODE_SET_SWI()         SysTimer_SetHartSWIRQ(EXECUTE_HARTID)
#define SMODE_CLR_SWI()         SysTimer_ClearHartSWIRQ(EXECUTE_HARTID)
#define SMODE_TICK_CONFIG()     SysTick_HartConfig(SYSTICK_TICK_CONST, EXECUTE_HARTID)
#define SMODE_TICK_RELOAD()     SysTick_HartReload(SYSTICK_TICK_CONST, EXECUTE_HARTID)
#endif

#else
#define SysTick_Handler     eclic_mtip_handler
#define portINITIAL_XSTATUS ( MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_FS_INITIAL | MSTATUS_VS_INITIAL)
#endif

volatile rt_ubase_t  rt_interrupt_from_thread = 0;
volatile rt_ubase_t  rt_interrupt_to_thread   = 0;
volatile rt_ubase_t rt_thread_switch_interrupt_flag = 0;
void SysTick_Handler(void);

struct rt_hw_stack_frame {
    rt_ubase_t epc;        /* epc - epc    - program counter                     */
    rt_ubase_t ra;         /* x1  - ra     - return address for jumps            */
    rt_ubase_t t0;         /* x5  - t0     - temporary register 0                */
    rt_ubase_t t1;         /* x6  - t1     - temporary register 1                */
    rt_ubase_t t2;         /* x7  - t2     - temporary register 2                */
    rt_ubase_t s0_fp;      /* x8  - s0/fp  - saved register 0 or frame pointer   */
    rt_ubase_t s1;         /* x9  - s1     - saved register 1                    */
    rt_ubase_t a0;         /* x10 - a0     - return value or function argument 0 */
    rt_ubase_t a1;         /* x11 - a1     - return value or function argument 1 */
    rt_ubase_t a2;         /* x12 - a2     - function argument 2                 */
    rt_ubase_t a3;         /* x13 - a3     - function argument 3                 */
    rt_ubase_t a4;         /* x14 - a4     - function argument 4                 */
    rt_ubase_t a5;         /* x15 - a5     - function argument 5                 */
#ifndef __riscv_32e
    rt_ubase_t a6;         /* x16 - a6     - function argument 6                 */
    rt_ubase_t a7;         /* x17 - s7     - function argument 7                 */
    rt_ubase_t s2;         /* x18 - s2     - saved register 2                    */
    rt_ubase_t s3;         /* x19 - s3     - saved register 3                    */
    rt_ubase_t s4;         /* x20 - s4     - saved register 4                    */
    rt_ubase_t s5;         /* x21 - s5     - saved register 5                    */
    rt_ubase_t s6;         /* x22 - s6     - saved register 6                    */
    rt_ubase_t s7;         /* x23 - s7     - saved register 7                    */
    rt_ubase_t s8;         /* x24 - s8     - saved register 8                    */
    rt_ubase_t s9;         /* x25 - s9     - saved register 9                    */
    rt_ubase_t s10;        /* x26 - s10    - saved register 10                   */
    rt_ubase_t s11;        /* x27 - s11    - saved register 11                   */
    rt_ubase_t t3;         /* x28 - t3     - temporary register 3                */
    rt_ubase_t t4;         /* x29 - t4     - temporary register 4                */
    rt_ubase_t t5;         /* x30 - t5     - temporary register 5                */
    rt_ubase_t t6;         /* x31 - t6     - temporary register 6                */
#endif
    rt_ubase_t xstatus;    /*              - m/s status register             */
};

/**
 * This function will initialize thread stack
 *
 * @param tentry the entry of thread
 * @param parameter the parameter of entry
 * @param stack_addr the beginning stack address
 * @param texit the function will be called when thread exit
 *
 * @return stack address
 */
rt_uint8_t* rt_hw_stack_init(void*       tentry,
                             void*       parameter,
                             rt_uint8_t* stack_addr,
                             void*       texit)
{
    struct rt_hw_stack_frame* frame;
    rt_uint8_t*         stk;
    int                i;

    stk  = stack_addr + sizeof(rt_ubase_t);
    stk  = (rt_uint8_t*)RT_ALIGN_DOWN((rt_ubase_t)stk, REGBYTES);
    stk -= sizeof(struct rt_hw_stack_frame);

    frame = (struct rt_hw_stack_frame*)stk;

    for (i = 0; i < sizeof(struct rt_hw_stack_frame) / sizeof(rt_ubase_t); i++) {
        ((rt_ubase_t*)frame)[i] = 0xdeadbeef;
    }

    frame->ra      = (rt_ubase_t)texit;
    frame->a0      = (rt_ubase_t)parameter;
    frame->epc     = (rt_ubase_t)tentry;

    frame->xstatus = portINITIAL_XSTATUS;

    return stk;
}

/*
 * void rt_hw_context_switch_interrupt(rt_ubase_t from, rt_ubase_t to);
 */
void rt_hw_context_switch_interrupt(rt_ubase_t from, rt_ubase_t to)
{
    if (rt_thread_switch_interrupt_flag == 0) {
        rt_interrupt_from_thread = from;
    }

    rt_interrupt_to_thread = to;
    rt_thread_switch_interrupt_flag = 1;
    /* Set a software interrupt(SWI) request to request a context switch. */
#ifdef SMODE_RTOS
    SMODE_SET_SWI();
#else
    SysTimer_SetSWIRQ();
#endif
    /* Barriers are normally not required but do ensure the code is completely
        within the specified behaviour for the architecture. */
    __RWMB();
}

void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to)
{
    rt_hw_context_switch_interrupt(from, to);
}

/** shutdown CPU */
void rt_hw_cpu_shutdown()
{
    rt_uint32_t level;
    rt_kprintf("shutdown...\n");

    level = rt_hw_interrupt_disable();
    while (level) {
        RT_ASSERT(0);
    }
}

void xPortTaskSwitch(void)
{
    /* Clear Software IRQ, A MUST */
#ifdef SMODE_RTOS
    SMODE_CLR_SWI();
#else
    SysTimer_ClearSWIRQ();
#endif
    rt_thread_switch_interrupt_flag = 0;
    // make from thread to be to thread
    // If there is another swi interrupt triggered by other harts
    // not through rt_hw_context_switch or rt_hw_context_switch_interrupt
    // the task switch should just do a same task save and restore
    rt_interrupt_from_thread = rt_interrupt_to_thread;
}

void vPortSetupTimerInterrupt(void)
{
#ifdef SMODE_RTOS

#if defined(__TEE_PRESENT) && __TEE_PRESENT == 1
    SMODE_TICK_CONFIG();
    ECLIC_DisableIRQ_S(SMODE_TIMER_IRQ);
    ECLIC_SetLevelIRQ_S(SMODE_TIMER_IRQ, configKERNEL_INTERRUPT_PRIORITY);
    ECLIC_SetShvIRQ_S(SMODE_TIMER_IRQ, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_SetVector_S(SMODE_TIMER_IRQ, (rv_csr_t)SysTick_Handler);
    ECLIC_EnableIRQ_S(SMODE_TIMER_IRQ);

    /* Set SWI interrupt level to lowest level/priority, SysTimerSW as Vector Interrupt */
    ECLIC_SetShvIRQ_S(SMODE_SWI_IRQ, ECLIC_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ_S(SMODE_SWI_IRQ, configKERNEL_INTERRUPT_PRIORITY);
    ECLIC_SetVector_S(SMODE_SWI_IRQ, (rv_csr_t)eclic_ssip_handler);
    ECLIC_EnableIRQ_S(SMODE_SWI_IRQ);
#else
    #error "TEE feature is required for RT-Thread S-Mode support"
#endif

#else
    /* Make SWI and SysTick the lowest priority interrupts. */
    /* Stop and clear the SysTimer. SysTimer as Non-Vector Interrupt */
    SysTick_Config(SYSTICK_TICK_CONST);
    ECLIC_DisableIRQ(SysTimer_IRQn);
    ECLIC_SetLevelIRQ(SysTimer_IRQn, configKERNEL_INTERRUPT_PRIORITY);
    ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_EnableIRQ(SysTimer_IRQn);

    /* Set SWI interrupt level to lowest level/priority, SysTimerSW as Vector Interrupt */
    ECLIC_SetShvIRQ(SysTimerSW_IRQn, ECLIC_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ(SysTimerSW_IRQn, configKERNEL_INTERRUPT_PRIORITY);
    ECLIC_EnableIRQ(SysTimerSW_IRQn);

#endif
}


#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#ifndef RT_HEAP_SIZE
#warning RT_HEAP_SIZE is not defined in rtconfig.h, using default 2048
#define RT_HEAP_SIZE 2048
#endif
static uint32_t rt_heap[RT_HEAP_SIZE];  // heap default size: 4K(1024 * 4)
RT_WEAK void* rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void* rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
    /* OS Tick Configuration */
    vPortSetupTimerInterrupt();

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif

    rt_hw_interrupt_disable();
}

/* This is the timer interrupt service routine. */
void SysTick_Handler(void)
{
    // Reload timer
#ifdef SMODE_RTOS
    SMODE_TICK_RELOAD();
#else
    SysTick_Reload(SYSTICK_TICK_CONST);
#endif

    /* enter interrupt */
    rt_interrupt_enter();

    /* tick increase */
    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}


void rt_hw_console_output(const char* str)
{
    rt_size_t size = 0;

    size = rt_strlen(str);
    for (int i = 0; i < size; i ++) {
        putchar(str[i]);
    }
}

char rt_hw_console_getchar(void)
{
    char ch = -1;
    ch = (char)getchar();
    return ch;
}

rt_base_t rt_hw_interrupt_disable(void)
{
    return __RV_CSR_READ_CLEAR(CSR_XSTATUS, XSTATUS_XIE);
}

void rt_hw_interrupt_enable(rt_base_t level)
{
    __RV_CSR_WRITE(CSR_XSTATUS, level);
}
