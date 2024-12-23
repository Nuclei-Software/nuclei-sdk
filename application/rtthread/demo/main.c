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

#define THREAD_PRIORITY 2
/* Reserve enough stack if rvv autovectorization enabled,
 * it will use stack to save and restore rvv registers,
 * which may corrupt stack, take care */
#define THREAD_STACK_SIZE 1024
#define THREAD_TIMESLICE  5
#define THREAD_NUM        5

/* Align stack when using static thread */
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread_stack[THREAD_NUM][THREAD_STACK_SIZE];
static struct rt_thread tid[THREAD_NUM];

/* Thread entry function */
static void thread_entry(void* parameter)
{
    rt_uint32_t count = 0;

    while (1) {
        rt_kprintf("thread %d count: %d\n", (rt_uint32_t)(unsigned long)parameter, count++);
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
