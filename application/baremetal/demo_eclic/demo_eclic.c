// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_soc.h"

// If define SWIRQ_INTLEVEL_HIGHER equals 1 the software interrupt will have a higher interrupt level.
// the software interrupt will run during timer interrupt.
// If define SWIRQ_INTLEVEL_HIGHER equals 0 the software interrupt will have a lower interrupt level.
// the software interrupt will run after timer interrupt.
#define SWIRQ_INTLEVEL_HIGHER   1

#define HIGHER_INTLEVEL         2
#define LOWER_INTLEVEL          1

#define TIMER_TICKS             (2 * SOC_TIMER_FREQ)

// setup timer
void setup_timer(void)
{
    printf("Initialize timer and start timer interrupt periodly\n\r");
    SysTick_Config(TIMER_TICKS);
}

// timer interrupt handler
// non-vector mode interrupt
void eclic_mtip_handler(void)
{
    static uint32_t int_t_cnt = 0;    /* timer interrupt counter */
    printf("-------------------\r\n");
    printf("[IN TIMER INTERRUPT]timer interrupt hit %d times\r\n", int_t_cnt++);

    printf("[IN TIMER INTERRUPT]trigger software interrupt\r\n");
#if SWIRQ_INTLEVEL_HIGHER == 1
    printf("[IN TIMER INTERRUPT]software interrupt will run during timer interrupt\r\n");
#else
    printf("[IN TIMER INTERRUPT]software interrupt will run when timer interrupt finished\r\n");
#endif
    // trigger software interrupt
    SysTimer_SetSWIRQ();

    // Reload Timer Interrupt
    SysTick_Reload(TIMER_TICKS);

    printf("[IN TIMER INTERRUPT]timer interrupt end\r\n");
}

// timer software interrupt handler
// vector mode interrupt
__INTERRUPT void eclic_msip_handler(void)
{
    static uint32_t int_sw_cnt = 0;   /* software interrupt counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT();

    SysTimer_ClearSWIRQ();

    printf("[IN SOFTWARE INTERRUPT]software interrupt hit %d times\r\n", int_sw_cnt++);
    printf("[IN SOFTWARE INTERRUPT]software interrupt end\r\n");
    
    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT();
}

int main(int argc, char **argv)
{
    uint8_t timer_intlevel, swirq_intlevel;
    int32_t returnCode;

    // Set TIMER Interrupt and Software Interrupt Level
    // According to the macro SWIRQ_INTLEVEL_HIGHER setting
#if SWIRQ_INTLEVEL_HIGHER == 1
    timer_intlevel = LOWER_INTLEVEL;
    swirq_intlevel = HIGHER_INTLEVEL;
#else
    timer_intlevel = HIGHER_INTLEVEL;
    swirq_intlevel = LOWER_INTLEVEL;
#endif

    // initialize timer
    setup_timer();

    // initialize software interrupt as vector interrupt
    returnCode = ECLIC_Register_IRQ(SysTimerSW_IRQn, ECLIC_VECTOR_INTERRUPT,
                    ECLIC_LEVEL_TRIGGER, swirq_intlevel, 0, eclic_msip_handler);

    // inital timer interrupt as non-vector interrupt
    returnCode = ECLIC_Register_IRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                    ECLIC_LEVEL_TRIGGER, timer_intlevel, 0, eclic_mtip_handler);

    // Enable interrupts in general.
    __enable_irq();

    // Wait for timer interrupt and software interrupt
    // triggered periodly
    while (1);
    return 0;
}
