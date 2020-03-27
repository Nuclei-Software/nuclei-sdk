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
#define THREAD_STACK_SIZE 396
#define THREAD_TIMESLICE 5

/*　Align stack when using static thread　*/
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread_stack[2][THREAD_STACK_SIZE];
static struct rt_thread tid1, tid2;

/* Thread entry function */
static void thread_entry(void *parameter)
{
    rt_uint32_t count = 0;

    while (1) {
        rt_kprintf("thread %d count: %d\n", (rt_uint32_t)parameter, count++);
        rt_thread_mdelay(500);
    }
}

/* Thread demo */
int create_thread_demo(void)
{
    /* Create static threads */
    rt_thread_init(&tid1, "thread1", thread_entry, (void *)0, thread_stack[0],
                   THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);

    rt_thread_init(&tid2, "thread2", thread_entry, (void *)1, thread_stack[1],
                   THREAD_STACK_SIZE, THREAD_PRIORITY - 1, THREAD_TIMESLICE);

    /* Startup threads  */
    rt_thread_startup(&tid1);
    rt_thread_startup(&tid2);

    return 0;
}

int main(void)
{
    rt_uint32_t count = 0;

    create_thread_demo();

    while (1) {
        rt_kprintf("Main thread count: %d\n", count++);
        rt_thread_mdelay(1000);
    }
}
