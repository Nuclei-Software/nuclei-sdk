// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_soc.h"

/* NOTE: Make sure the interrupt function name for time interrupt and software time interrupt are
 * irqc_mtip_handler and irqc_msip_handler */

static volatile uint32_t int0_cnt = 0;    /* mtip timer interrupt test counter */
static volatile uint32_t int1_cnt = 0;    /* msip timer interrupt test counter */
volatile unsigned int msip_trig_flag = 1; /* sw trigger mtimer sw interrupt flag */

#define LOOP_COUNT      5

__INTERRUPT void irqc_mtip_handler(void)
{
    int0_cnt++;
    printf("MTimer IRQ handler %d\n\r", int0_cnt);
    uint32_t now = SysTimer_GetLoadValue();
    SysTimer_SetCompareValue(now + SOC_TIMER_FREQ / 10);
}

__INTERRUPT void irqc_msip_handler(void)
{
    SysTimer_ClearSWIRQ();
    int1_cnt++;
    printf("MTimer SW IRQ handler %d\n\r", int1_cnt);
    msip_trig_flag = 1;
}

void setup_timer()
{
    printf("init timer and start\n\r");
    uint32_t now = SysTimer_GetLoadValue();
    uint32_t then = now + SOC_TIMER_FREQ / 10;
    SysTimer_SetCompareValue(then);
}

int main(void)
{
    uint32_t returnCode;

    setup_timer(); /* initialize timer */

    // TODO you can register new handler only when your vector table is RW
    // otherwise please make sure pass NULL, and use the correct interrupt handler name defined
    // in system_<Device>.c
    returnCode = IRQC_Register_IRQ(SysTimer_IRQn, NULL); /* register system timer interrupt */

    __enable_irq(); /* enable global interrupt */

    while (int0_cnt < 5);
    IRQC_DisableIRQ(SysTimer_IRQn); /* Disable MTIP iterrupt */

    // TODO you can register new handler only when your vector table is RW
    returnCode = IRQC_Register_IRQ(SysTimerSW_IRQn, NULL); /* register system timer SW interrupt */

    do {
        if (msip_trig_flag == 1) {
            msip_trig_flag = 0;
            SysTimer_SetSWIRQ(); /* trigger timer sw interrupt */
            delay_1ms(10);
        }
    } while (int1_cnt < 5); /* check test end condition */

    printf("MTimer msip and mtip interrupt test finish and pass\r\n");

    if (returnCode != 0) { /* Check return code for errors */
        return -1;
    }

    return 0;
}

