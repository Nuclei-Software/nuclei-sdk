/*
 * FreeRTOS Kernel V11.2.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * Copyright (c) 2019-Present Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
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
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */

#include <stdio.h>
#include <stdlib.h>

#include "nuclei_sdk_soc.h"

/* The period of the example software timer, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainSOFTWARE_TIMER_PERIOD_MS    pdMS_TO_TICKS(500)
#define TASKDLYMS                       pdMS_TO_TICKS(15)
#define mainQUEUE_LENGTH                (1)

static void prvSetupHardware(void);
static void vExampleTimerCallback(TimerHandle_t xTimer);

/* The queue used by the queue send and queue receive tasks. */
static QueueHandle_t xQueue = NULL;

static TaskHandle_t StartTask1_Handler;
static TaskHandle_t StartTask2_Handler;

static TaskHandle_t TaskHandles[configNUMBER_OF_CORES];

#if configNUMBER_OF_CORES > 1
#define ENTER_CRITICAL()    taskENTER_CRITICAL()
#define EXIT_CRITICAL()     taskEXIT_CRITICAL()
#else
#define ENTER_CRITICAL()    taskENTER_CRITICAL()
#define EXIT_CRITICAL()     taskEXIT_CRITICAL()
#endif

void prvSetupHardware(void)
{

}

void start_task(void* pvParameters);

long main(void)
{
    CSR_MCFGINFO_Type mcfg_info;

#if defined(CPU_SERIES) && CPU_SERIES == 100
    mcfg_info.b.clic = 1;
#else
    mcfg_info.d = __RV_CSR_READ(CSR_MCFG_INFO);
#endif

    if (0 == mcfg_info.b.clic) {
        printf("ECLIC is not present, will not run this example!\r\n");
        return 0;
    }

    TimerHandle_t xExampleSoftwareTimer = NULL;
    unsigned long i, prio, param;

    /* Configure the system ready to run the demo.  The clock configuration
    can be done here if it was not done before main() was called. */
    prvSetupHardware();

    xQueue = xQueueCreate(/* The number of items the queue can hold. */
                 mainQUEUE_LENGTH,
                 /* The size of each item the queue holds. */
                 sizeof(uint32_t));

    if (xQueue == NULL) {
        printf("Unable to create xQueue due to low memory.\n");
        while (1);
    }

    for (i = 0; i < configNUMBER_OF_CORES; i++) {
        prio = (i % configMAX_PRIORITIES);
        param = (i << 16) + prio;
        xTaskCreate((TaskFunction_t)start_task, (const char*)"start_task",
                    (uint16_t)256, (void*)param, (UBaseType_t)prio,
                    (TaskHandle_t*)&TaskHandles[i]);
        if (TaskHandles[i] == NULL) {
            printf("Unable to create task %d due to low memory.\n", i);
            while (1);
        }
    }

    xExampleSoftwareTimer =
        xTimerCreate((const char*)"ExTimer", mainSOFTWARE_TIMER_PERIOD_MS,
                     pdTRUE, (void*)0, vExampleTimerCallback);

    xTimerStart(xExampleSoftwareTimer, 0);

#if configNUMBER_OF_CORES > 1
    printf("Startup FreeRTOS SMP on hartid %d\r\n", __get_hart_id());
#else
    printf("Startup FreeRTOS on hartid %d\r\n", __get_hart_id());
#endif

    vTaskStartScheduler();

    printf("OS should never run to here\r\n");

    while (1);
}


void smp_main(void)
{
    /* BOOT_HARTID is defined as the local hart id inside the current cluster, compare against
     * the local hart id returned by __get_local_hart_id(), not the hart index */
    if (__get_local_hart_id() == BOOT_HARTID ) {
        main();
    } else {
        xPortStartScheduler();
    }
}

void start_task(void* pvParameters)
{
    int cnt = 0;

    ENTER_CRITICAL();
    printf("Enter to task %u\r\n", (unsigned long)(pvParameters) >> 16);
    EXIT_CRITICAL();

    while (1) {
        ENTER_CRITICAL();
        printf("task %u prio %u is running %d on hart %d.....\r\n",  (unsigned long)(pvParameters) >> 16, \
            (unsigned long)pvParameters & 0xFF, cnt++, __get_hart_id());
        EXIT_CRITICAL();
        vTaskDelay(TASKDLYMS);
    }
}

static void vExampleTimerCallback(TimerHandle_t xTimer)
{
    /* The timer has expired.  Count the number of times this happens.  The
    timer that calls this function is an auto re-load timer, so it will
    execute periodically. */
    static int cnt = 0;
    ENTER_CRITICAL();
    printf("timers Callback %d on hart %d\r\n", cnt++, __get_hart_id());
    EXIT_CRITICAL();
}

void vApplicationTickHook(void)
{
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* The RTOS tick hook function is enabled by setting configUSE_TICK_HOOK to
    1 in FreeRTOSConfig.h.

    "Give" the semaphore on every 500th tick interrupt. */

    /* If xHigherPriorityTaskWoken is pdTRUE then a context switch should
    normally be performed before leaving the interrupt (because during the
    execution of the interrupt a task of equal or higher priority than the
    running task was unblocked).  The syntax required to context switch from
    an interrupt is port dependent, so check the documentation of the port you
    are using.

    In this case, the function is running in the context of the tick interrupt,
    which will automatically check for the higher priority task to run anyway,
    so no further action is required. */
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
    /* The malloc failed hook is enabled by setting
    configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

    Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
    ENTER_CRITICAL();
    printf("malloc failed on hart %d\n", __get_hart_id());
    EXIT_CRITICAL();
    while (1);
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
    /* Run time stack overflow checking is performed if
    configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected.  pxCurrentTCB can be
    inspected in the debugger if the task name passed into this function is
    corrupt. */
    ENTER_CRITICAL();
    printf("Stack Overflow on hart %d\n", __get_hart_id());
    EXIT_CRITICAL();
    while (1);
}
/*-----------------------------------------------------------*/

extern UBaseType_t uxCriticalNesting;
void vApplicationIdleHook(void)
{
    // volatile size_t xFreeStackSpace;
    /* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
    FreeRTOSConfig.h.

    This function is called on each cycle of the idle task.  In this case it
    does nothing useful, other than report the amount of FreeRTOS heap that
    remains unallocated. */
    /* By now, the kernel has allocated everything it is going to, so
    if there is a lot of heap remaining unallocated then
    the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
    reduced accordingly. */
    __WFI(); // Enter to low power
}

void vApplicationPassiveIdleHook(void)
{
    __WFI(); // Enter to low power
}
