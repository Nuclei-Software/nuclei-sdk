/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 * Copyright (c) 2019-Present Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-26     Huaqi      the first version
 */

#include "nuclei_sdk_soc.h"
#include <rtthread.h>
#include <stdio.h>

#if !defined(__TEE_PRESENT) || (__TEE_PRESENT != 1)
/* __TEE_PRESENT should be defined in <Device>.h */
#error "This example require CPU TEE feature!"
#endif

#if !defined(__PMP_PRESENT) || (__PMP_PRESENT != 1)
/* __PMP_PRESENT should be defined in <Device>.h */
#error "This example require CPU PMP feature!"
#endif

#define THREAD_PRIORITY 2
#define THREAD_STACK_SIZE 512
#define THREAD_TIMESLICE 5
#define THREAD_NUM      5

/* Align stack when using static thread */
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread_stack[THREAD_NUM][THREAD_STACK_SIZE];
static struct rt_thread tid[THREAD_NUM];

/* Thread entry function */
static void thread_entry(void* parameter)
{
    rt_uint32_t count = 0;

    while (1) {
        rt_kprintf("thread %d count: %d\n", (rt_uint32_t)parameter, count++);
        rt_thread_mdelay(250);
    }
}

/* Thread demo */
int create_thread_demo(void)
{
    unsigned long i;
    for (i = 0; i < THREAD_NUM; i ++) {
        /* Create static threads */
        rt_thread_init(&tid[i], "thread", thread_entry, (void*)i, thread_stack[i],
                       THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    }

    /* Startup threads  */
    for (i = 0; i < THREAD_NUM; i ++) {
        rt_thread_startup(&tid[i]);
    }

    return 0;
}

int main(void)
{
    rt_uint32_t count = 0;

    create_thread_demo();

    while (1) {
        rt_kprintf("Main thread count: %d\n", count++);
        rt_thread_mdelay(500);
#ifdef CFG_SIMULATION
        if (count > 2) {
            // directly exit if in nuclei internally simulation
            SIMULATION_EXIT(0);
        }
#endif
    }
}

extern void entry(void);

#define SMODE_STACK_SIZE        2048
// Execute Hart ID
#define EXECUTE_HARTID          0

#if defined(__SSTC_PRESENT) && __SSTC_PRESENT == 1
#define SMODE_TIMER_IRQ         SysTimer_S_IRQn
#define SMODE_SWI_IRQ           SysTimerSW_S_IRQn
#else
#define SMODE_TIMER_IRQ         SysTimer_IRQn
#define SMODE_SWI_IRQ           SysTimerSW_IRQn
#endif


/* Create a stack for supervisor mode execution */
uint8_t smode_stack[SMODE_STACK_SIZE] __attribute__((aligned(16)));

uintptr_t smode_sp = (uintptr_t) (smode_stack + sizeof(smode_stack));

int main_entry(void)
{
    CSR_MCFGINFO_Type mcfg;
    mcfg.d = __RV_CSR_READ(CSR_MCFG_INFO);

    if ((mcfg.b.tee & mcfg.b.clic) == 0) {
        printf("INFO: TEE and ECLIC feature are required to run this SMode RT-Thread Demo\n");
        return 0;
    }

    // set pmp, S mode can access all address range
    pmp_config pmp_cfg = {
        /* M mode grants S and U mode with full permission of the whole address range */
        .protection = PMP_L | PMP_R | PMP_W | PMP_X,
        /* Memory region range 2^__RISCV_XLEN bytes */
        .order = __RISCV_XLEN,
        /* Initial base address is 0 */
        .base_addr = 0,
    };

    __set_PMPENTRYx(0, &pmp_cfg);
    // before drop to S Mode, specifies in which privilege mode the interrupt should be taken
    ECLIC_SetModeIRQ(SMODE_TIMER_IRQ, PRV_S);
    ECLIC_SetModeIRQ(SMODE_SWI_IRQ, PRV_S);
#if defined(__SSTC_PRESENT) && __SSTC_PRESENT == 1
    /* Disable S-mode access some system timer registers */
    SysTimer_DisableSAccess();
    /* Enable CY,TM,IR in CSR_MCOUNTEREN to allow S-mode access cycle,time,instret csr */
    SysTimer_EnableSSTC();
    printf("Set ECLIC Timer S-Mode Interrupt and Software Timer S-Mode Interrupt to be executed in S-Mode\r\n");
#else
    printf("Set ECLIC Timer M-Mode Interrupt and Software Timer M-Mode Interrupt to be executed in S-Mode\r\n");
    SysTimer_EnableSAccess();
#endif

    printf("Drop to S-Mode to prepare RT-Thread Environment\r\n");
    /* Drop to S mode */
    __switch_mode(PRV_S, smode_sp, entry);
    // Should never return here
    return 1;
}
