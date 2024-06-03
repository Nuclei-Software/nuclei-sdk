// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_soc.h"

// NOTE: both timer and software time interrupt are vector interrupts
// Not support interrupt nesting

// 100ms
#define TIMER_TICKS             (SOC_TIMER_FREQ / 10)

static volatile uint32_t int_check_cnt = 0;

// setup timer
void setup_timer(void)
{
    printf("Initialize timer and start timer interrupt periodically\n\r");
    SysTick_Config(TIMER_TICKS);
}

// timer interrupt handler
// vector mode interrupt
__INTERRUPT void irqc_mtip_handler(void)
{
    static uint32_t int_t_cnt = 0;    /* timer interrupt counter */

    printf("-------------------\r\n");
    printf("[IN TIMER INTERRUPT]timer interrupt hit %d times\r\n", int_t_cnt++);

    printf("[IN TIMER INTERRUPT]trigger software interrupt\r\n");
    printf("[IN TIMER INTERRUPT]software interrupt will run when timer interrupt finished\r\n");
    // trigger software interrupt
    SysTimer_SetSWIRQ();

    // Reload Timer Interrupt
    SysTick_Reload(TIMER_TICKS);

    printf("[IN TIMER INTERRUPT]timer interrupt end\r\n");
}

// timer software interrupt handler
// vector mode interrupt
__INTERRUPT void irqc_msip_handler(void)
{
    static uint32_t int_sw_cnt = 0;   /* software interrupt counter */

    SysTimer_ClearSWIRQ();

    printf("[IN SOFTWARE INTERRUPT]software interrupt hit %d times\r\n", int_sw_cnt++);
    printf("[IN SOFTWARE INTERRUPT]software interrupt end\r\n");
    int_check_cnt ++;
}

#define RUN_LOOPS   20
int main(void)
{
    int32_t returnCode;

    // initialize timer
    setup_timer();

    // initialize software interrupt as vector interrupt
    returnCode = IRQC_Register_IRQ(SysTimerSW_IRQn, (void *)irqc_msip_handler);

    // inital timer interrupt as vector interrupt
    returnCode = IRQC_Register_IRQ(SysTimer_IRQn, (void *)irqc_mtip_handler);

    // Enable interrupts in general.
    __enable_irq();

    // Wait for timer interrupt and software interrupt
    // triggered periodically
    while (int_check_cnt < RUN_LOOPS);
    __disable_irq();
    printf("IRQC Demo finished sucessfully in %d loops\n", RUN_LOOPS);
    return 0;
}
