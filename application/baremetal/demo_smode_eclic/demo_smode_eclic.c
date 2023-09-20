// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_soc.h"

#if !defined(__TEE_PRESENT) || (__TEE_PRESENT != 1)
/* __TEE_PRESENT should be defined in <Device>.h */
#warning "__TEE_PRESENT is not defined or equal to 1, please check!"
#endif
// If define SWIRQ_INTLEVEL_HIGHER equals 1 the software interrupt will have a higher interrupt level.
// the software interrupt will run during timer interrupt.
// If define SWIRQ_INTLEVEL_HIGHER equals 0 the software interrupt will have a lower interrupt level.
// the software interrupt will run after timer interrupt.
#define SWIRQ_INTLEVEL_HIGHER   0

#define HIGHER_INTLEVEL         2
#define LOWER_INTLEVEL          1

// 100ms
#define TIMER_TICKS             (SOC_TIMER_FREQ / 10)

// 2048 is enough
#define SMODE_STACK_SIZE        2048

// Execute Hart ID
#define EXECUTE_HARTID          0

/* Create a stack for supervisor mode execution */
uint8_t smode_stack[SMODE_STACK_SIZE] __attribute__((aligned(16)));

uintptr_t smode_sp = (uintptr_t) (smode_stack + sizeof(smode_stack));
static volatile uint32_t int_check_cnt = 0;

void print_sp_judge_privilege_mode(void)
{
    uintptr_t sp;

    __asm__ volatile("add  %0, x0, sp" :"=r"(sp));

    printf("Current sp is %p, ", sp);

    if ( ((uint8_t *)sp <= &(smode_stack[SMODE_STACK_SIZE - 1])) && ((uint8_t *)sp >= &(smode_stack[0])) ) {
        printf("so it is in Supervisor Mode!\r\n");
    } else {
        printf("so it is in Machine Mode!\r\n");
    }
}

// setup timer
void setup_timer(void)
{
    printf("Initialize timer and start timer interrupt periodically\n\r");
    SysTick_HartConfig(TIMER_TICKS, EXECUTE_HARTID);
}

// timer interrupt s-mode handler
// non-vector mode interrupt
void eclic_stip_handler(void)
{
    static uint32_t int_t_cnt = 0;    /* timer interrupt counter */

    print_sp_judge_privilege_mode();
    printf("-------------------\r\n");
    printf("[IN S-MODE TIMER INTERRUPT]timer interrupt hit %d times\r\n", int_t_cnt++);

    printf("[IN S-MODE TIMER INTERRUPT]trigger software interrupt\r\n");
#if SWIRQ_INTLEVEL_HIGHER == 1
    printf("[IN S-MODE TIMER INTERRUPT]software interrupt will run during timer interrupt\r\n");
#else
    printf("[IN S-MODE TIMER INTERRUPT]software interrupt will run when timer interrupt finished\r\n");
#endif
    // trigger software interrupt
    SysTimer_SetHartSWIRQ(EXECUTE_HARTID);

    // Reload Timer Interrupt
    SysTick_HartReload(TIMER_TICKS, EXECUTE_HARTID);

    printf("[IN S-MODE TIMER INTERRUPT]timer interrupt end\r\n");
}

// timer software interrupt S-mode handler
// vector mode interrupt
__SUPERVISOR_INTERRUPT void eclic_ssip_handler(void)
{
    static uint32_t int_sw_cnt = 0;   /* software interrupt counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT_S();

    SysTimer_ClearHartSWIRQ(EXECUTE_HARTID);

    printf("[IN S-MODE SOFTWARE INTERRUPT]software interrupt hit %d times\r\n", int_sw_cnt++);
    print_sp_judge_privilege_mode();
    printf("[IN S-MODE SOFTWARE INTERRUPT]software interrupt end\r\n");

    int_check_cnt ++;
    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT_S();
}

#define RUN_LOOPS   20
#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
static void supervisor_mode_entry_point(void)
{
    // setup timer and software interrupt , then register the S mode irq
    uint8_t timer_intlevel, swirq_intlevel = 0;
    int32_t returnCode = 0;

    printf("[IN S-MODE ENTRY POINT] Hello Supervisor Mode!!!\r\n");
    print_sp_judge_privilege_mode();
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
    returnCode = ECLIC_Register_IRQ_S(SysTimerSW_IRQn, ECLIC_VECTOR_INTERRUPT,
                                        ECLIC_LEVEL_TRIGGER, swirq_intlevel, 0, (void *)eclic_ssip_handler);

    // inital timer interrupt as non-vector interrupt
    returnCode = ECLIC_Register_IRQ_S(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                        ECLIC_LEVEL_TRIGGER, timer_intlevel, 0, (void *)eclic_stip_handler);

    // Enable interrupts in general.
    __enable_irq_s();

    // Wait for timer interrupt and software interrupt triggered periodically
    while (int_check_cnt < RUN_LOOPS);
    __disable_irq_s();
    printf("ECLIC S-Mode Demo finished sucessfully in %d loops\n", RUN_LOOPS);
    while(1);
}
#endif

int main(int argc, char** argv)
{
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

    print_sp_judge_privilege_mode();
#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
    // before drop to S Mode, specifies in which privilege mode the interrupt should be taken
    ECLIC_SetModeIRQ(SysTimerSW_IRQn, PRV_S);
    ECLIC_SetModeIRQ(SysTimer_IRQn, PRV_S);

    printf("Drop to S-Mode now\n\r");
    /* Drop to S mode */
    __switch_mode(PRV_S, smode_sp, supervisor_mode_entry_point);
    while (1);
#else
    printf("[ERROR]__TEE_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}
