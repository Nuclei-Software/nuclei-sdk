// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_soc.h"

/* Define the interrupt handler name same as vector table in case download mode is flashxip. */
#define systimer_irq_handler     core_mtip_handler
#define systimer_sw_irq_handler  core_msip_handler

static uint32_t int0_cnt = 0;    /* msip timer interrupt test counter */
static uint32_t int1_cnt = 0;    /* mtip timer interrupt test counter */
unsigned int msip_trig_flag = 1; /* sw trigger systimer sw interrupt flag */

void systimer_irq_handler(void)
{
    int0_cnt++;
    printf("SysTimer IRQ handler %d\n\r", int0_cnt);
    uint64_t now = SysTimer_GetLoadValue();
    SysTimer_SetCompareValue(now + SOC_TIMER_FREQ / 2);
}

void systimer_sw_irq_handler(void)
{
    SysTimer_ClearSWIRQ();
    int1_cnt++;
    printf("SysTimer SW IRQ handler %d\n\r", int1_cnt);
    msip_trig_flag = 1;
}

void setup_timer()
{
    printf("init timer and start\n\r");
    uint64_t now = SysTimer_GetLoadValue();
    uint64_t then = now + SOC_TIMER_FREQ / 2;
    SysTimer_SetCompareValue(then);
}

#ifdef CFG_SIMULATION
#define LOOP_COUNT      2
#else
#define LOOP_COUNT      5
#endif

// This demo will show cpu working in clint interrupt mode not eclic interrupt mode
// Mainly test timer interrupt and software interrupt
// If you want to see how systimer working in ECLIC interrupt mode,
// please check demo_timer or demo_eclic example
// TODO Currently it only can build for evalsoc

int main(void)
{
    uint32_t returnCode;

    /* Initialize interrupt as CLINT interrupt mode, see MTVEC register description */
    CLINT_Interrupt_Init();

    returnCode = Core_Register_IRQ(SysTimer_IRQn, systimer_irq_handler); /* register system timer interrupt */

    __enable_irq(); /* enable global interrupt */

    setup_timer(); /* initialize timer */

    while (int0_cnt < LOOP_COUNT);
    __disable_core_irq(SysTimer_IRQn); /* Disable MTIP iterrupt */

    returnCode = Core_Register_IRQ(SysTimerSW_IRQn, systimer_sw_irq_handler); /* register system timer SW interrupt */

    do {
        if (msip_trig_flag == 1) {
            msip_trig_flag = 0;
            SysTimer_SetSWIRQ(); /* trigger timer sw interrupt */
            delay_1ms(200);
        }
    } while (int1_cnt < LOOP_COUNT); /* check test end condition */

    printf("SysTimer MTIP and MSIP CLINT interrupt test finish and pass\r\n");

    if (returnCode != 0) { /* Check return code for errors */
        return -1;
    }

    return 0;
}

