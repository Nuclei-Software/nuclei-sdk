#include "nuclei_sdk_soc.h"
#include "gprof_api.h"

// TODO uncomment below if you are using RTOS, and you need to implement check the TODO in this file
//#define USING_RTOS

// TODO comment below if you dont want to use systimer interrupt as period sample interrupt
// or this systimer interrupt is already used by other tasks such as RTOS or some other program
// or you can add gprof_sample(pc) in your own period timer interrupt function
#define SAMPLE_USING_SYSTIMER

extern void gprof_sample(unsigned long pc);

#ifdef USING_RTOS
// TODO: If you are using RTOS, please take care that the system timer interrupt is already used by tick interrupt
//       so you need to call gprof_sample() function by yourself

#error "You must implement a period timer interrupt run in PROF_HZ and call gprof_sample when USING_RTOS"

#else

// Sample in period interrupt, defined in PROF_HZ
#define SAMPLE_TIMER_TICKS     (SOC_TIMER_FREQ / PROF_HZ)

#ifdef SAMPLE_USING_SYSTIMER
// timer interrupt handler
// vector mode interrupt
__INTERRUPT void eclic_mtip_handler(void)
{
    /* get current pc which is interrupted by interrupt via mepc */
    unsigned long pc = (unsigned long) (__RV_CSR_READ(CSR_MEPC));
    // Reload Timer Interrupt
    SysTick_Reload(SAMPLE_TIMER_TICKS);

    gprof_sample(pc);
}
#endif

// setup timer
static void setup_gprof_timer(void)
{
#ifdef SAMPLE_USING_SYSTIMER
    SysTick_Config(SAMPLE_TIMER_TICKS);

    // initialize timer interrupt as vector interrupt
    ECLIC_Register_IRQ(SysTimer_IRQn, ECLIC_VECTOR_INTERRUPT,
            ECLIC_LEVEL_TRIGGER, 1, 0, eclic_mtip_handler);
    // Enable IRQ
    __enable_irq();
#else
    // TODO if you don't use rtos or SAMPLE_USING_SYSTIMER
    // you should setup your own gprof period timer interrupt here
#endif
}

static void stop_gprof_timer(void)
{
#ifdef SAMPLE_USING_SYSTIMER
    ECLIC_DisableIRQ(SysTimer_IRQn);
    // Disable IRQ
    __disable_irq();
#else
    // TODO if you don't use rtos or SAMPLE_USING_SYSTIMER
    // you should disable your sample interrupt here if it is not used by other case
#endif
}
#endif

/* Stop profiling */
void gprof_off(void)
{
#ifndef USING_RTOS
    stop_gprof_timer();
#endif
}

/* Start profiling */
void gprof_on(void)
{
#ifndef USING_RTOS
    setup_gprof_timer();
#endif
}
