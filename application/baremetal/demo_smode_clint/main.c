// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_hal.h"

#if !defined(__PMP_PRESENT) || (__PMP_PRESENT != 1)
/* __PMP_PRESENT should be defined in <Device>.h */
#warning "__PMP_PRESENT is not defined or equal to 1, please check!"
#error "This example require CPU PMP feature!"
#endif

#if !defined(__SMODE_PRESENT) || (__SMODE_PRESENT == 0)
/* __SMODE_PRESENT should be defined in <Device>.h */
#warning "__SMODE_PRESENT is not defined or equal to 0, please check!"
#error "This example require CPU SMODE feature!"
#endif

#if !defined(__SSTC_PRESENT) || (__SSTC_PRESENT == 0)
/* __SSTC_PRESENT should be defined in <Device>.h */
#warning "__SSTC_PRESENT is not defined or equal to 0, please check!"
#error "This example require CPU SSTC feature!"
#endif

// 2048 is enough
#define SMODE_STACK_SIZE        2048
// 100ms
#define TIMER_TICKS             (SOC_TIMER_FREQ / 10)

/* Create a stack for supervisor mode execution */
uint8_t smode_stack[SMODE_STACK_SIZE] __attribute__((aligned(16)));

uintptr_t smode_sp = (uintptr_t) (smode_stack + sizeof(smode_stack));

/* Define the interrupt handler name same as vector table in case download mode is flashxip. */
#define systimer_irq_handler     core_stip_handler
#define systimer_sw_irq_handler  core_ssip_handler

static uint32_t int0_cnt = 0;    /* stip timer interrupt test counter */
static uint32_t int1_cnt = 0;    /* ssip timer interrupt test counter */
unsigned int ssip_trig_flag = 1; /* sw trigger systimer sw interrupt flag */

void systimer_irq_handler(void)
{
    int0_cnt++;
    printf("SysTimer IRQ handler %d\n\r", int0_cnt);
    SysTick_Reload_S(TIMER_TICKS);
}

void systimer_sw_irq_handler(void)
{
    /* To clear the software interrupt in CLINT interrupt mode, we need to clear 
     * the SSIP bit field in SIP register */
    __RV_CSR_CLEAR(CSR_SIP, 1 << IRQ_S_SOFT);
    int1_cnt++;
    printf("SysTimer SW IRQ handler %d\n\r", int1_cnt);
    ssip_trig_flag = 1;
}

void setup_timer_s(void)
{
    printf("init timer and start\n\r");
    SysTick_Config_S(TIMER_TICKS);
}

void print_sp_judge_privilege_mode(void)
{
    uintptr_t sp;

    __asm__ volatile("add  %0, x0, sp" :"=r"(sp));

    printf("Current sp is 0x%lx, ", (unsigned long)sp);

    if ( ((uint8_t *)sp <= &(smode_stack[SMODE_STACK_SIZE - 1])) && ((uint8_t *)sp >= &(smode_stack[0])) ) {
        printf("so it is in Supervisor Mode!\r\n");
    } else {
        printf("so it is in Machine Mode!\r\n");
    }
}

#ifdef CFG_SIMULATION
#define LOOP_COUNT      2
#else
#define LOOP_COUNT      5
#endif

#if defined(__PMP_PRESENT) && (__PMP_PRESENT == 1)
static void supervisor_mode_entry_point(void)
{
    int32_t returnCode = 0;

    printf("[IN S-MODE ENTRY POINT] Hello Supervisor Mode!!!\r\n");
    print_sp_judge_privilege_mode();

    /* Enable S-mode interrupt overall */
    __enable_irq_s();

    /* Register and enable SysTimer_S_IRQn interrupt */
    returnCode = Core_Register_IRQ_S(SysTimer_S_IRQn, systimer_irq_handler); /* register system timer interrupt */

    setup_timer_s(); /* initialize timer */

    while (int0_cnt < LOOP_COUNT);
    __disable_core_irq_s(SysTimer_S_IRQn); /* Disable STIP interrupt */

    /* Register and enable SysTimerSW_S_IRQn interrupt */
    returnCode = Core_Register_IRQ_S(SysTimerSW_S_IRQn, systimer_sw_irq_handler); /* register system timer SW interrupt */

    do {
        if (ssip_trig_flag == 1) {
            ssip_trig_flag = 0;
            /* To trigger the software interrupt in CLINT interrupt mode, we need to set the
             * SSIP bit field in SIP register */
            __RV_CSR_SET(CSR_SIP, 1 << IRQ_S_SOFT);
            delay_1ms(200);
        }
    } while (int1_cnt < LOOP_COUNT); /* check test end condition */
    __disable_core_irq_s(SysTimerSW_S_IRQn); /* Disable SSIP interrupt */

    printf("SysTimer STIP and SSIP CLINT interrupt test finish and pass\r\n");

#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(0);
#endif
    while(1);
}
#endif /* #if defined(__PMP_PRESENT) && (__PMP_PRESENT == 1) */

int main(int argc, char** argv)
{
    // Check SSTC via CSR MCFGINFO register
    CSR_MCFGINFO_Type mcfg;
    mcfg.d = __RV_CSR_READ(CSR_MCFG_INFO);
    if (mcfg.b.sstc == 0) {
        printf("SSTC is not present, so will not run this example!\n");
        return 0;
    }

    /* Initialize interrupt as CLINT interrupt mode, see MTVEC register description */
    CLINT_Interrupt_Init();

#if defined(__PMP_PRESENT) && (__PMP_PRESENT == 1)
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
    /* Delegate S-mode sw interrupt and timer interrupt to S-mode */
    __set_mideleg(MIDELEG_STIE | MIDELEG_SSIE);

    SysTimer_EnableSSTC();

    printf("Drop to S-Mode now\n\r");
    /* Drop to S mode */
    __switch_mode(PRV_S, smode_sp, supervisor_mode_entry_point);
#else
    printf("[ERROR]__PMP_PRESENT & __PMP_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}
