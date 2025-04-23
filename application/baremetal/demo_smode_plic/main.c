
// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_hal.h"

#if !defined(__PMP_PRESENT) || (__PMP_PRESENT != 1)
/* __PMP_PRESENT should be defined in <Device>.h */
#warning "__PMP_PRESENT is not defined or equal to 1, please check!"
#warning "This example require CPU PMP feature!"
#endif

#if !defined(__PLIC_PRESENT) || (__PLIC_PRESENT == 0)
/* __PLIC_PRESENT should be defined in <Device>.h */
#warning "__PLIC_PRESENT is not defined or equal to 0, please check!"
#warning "This example require CPU PLIC feature!"
#endif

// 2048 is enough
#define SMODE_STACK_SIZE        2048

/* Create a stack for supervisor mode execution */
uint8_t smode_stack[SMODE_STACK_SIZE] __attribute__((aligned(16)));

uintptr_t smode_sp = (uintptr_t) (smode_stack + sizeof(smode_stack));
static volatile uint32_t int_check_cnt = 0;

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

#if defined(__PLIC_PRESENT) && (__PLIC_PRESENT == 1)
// plic uart0 interrupt handler
// plic claim and complete is done in system_mmode_extirq_handler
void plic_uart0_handler(void)
{
    int32_t status = uart_get_status(SOC_DEBUG_UART);
    printf("Enter uart0 interrupt, ");
    if (status & UART_IP_RXIP_MASK) {
        unsigned char c_get;
        // Clear rx pending
        uart_clear_status(SOC_DEBUG_UART, UART_IP_RXIP_MASK);
        c_get = uart_read(SOC_DEBUG_UART);
        printf("you just typed: %c\n", c_get);
    } else {
        printf("\n");
    }
}
#endif

#if (defined(__PLIC_PRESENT) && (__PLIC_PRESENT == 1)) && (defined(__PMP_PRESENT) && (__PMP_PRESENT == 1))
static void supervisor_mode_entry_point(void)
{
    // setup timer and software interrupt , then register the S mode irq
    uint8_t timer_intlevel, swirq_intlevel = 0;
    int32_t returnCode = 0;

    printf("[IN S-MODE ENTRY POINT] Hello Supervisor Mode!!!\r\n");
    print_sp_judge_privilege_mode();

    // register plic uart0 interrupt
    // when register plic interrupt, please PLIC_xxx_IRQn
    returnCode = PLIC_Register_IRQ_S(PLIC_UART0_IRQn, 1, plic_uart0_handler);

    // Enable interrupts in general.
    __enable_irq_s();
    // Enable uart0 receive interrupt
    uart_enable_rxint(SOC_DEBUG_UART);
    printf("You can press any key now to trigger uart receive interrupt\n");


    // Wait uart0 press any key
    while (1);


#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(0);
#endif
    while(1);
}
#endif

int main(int argc, char** argv)
{
#if (defined(__PLIC_PRESENT) && (__PLIC_PRESENT == 1)) && (defined(__PMP_PRESENT) && (__PMP_PRESENT == 1))
    int32_t returnCode;
    CSR_MCFGINFO_Type mcfg;

    // Do PLIC present check via CSR MCFGINFO register
    // Just to confirm whether PLIC really present
    mcfg.d = __RV_CSR_READ(CSR_MCFG_INFO);
    if (mcfg.b.plic == 0) {
        printf("PLIC is not present in this CPU, please check!\n");
        return 0;
    }

    // Initialize interrupt mode to clint/plic mode
    PLIC_Interrupt_Init();

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
    __set_mideleg((1 << IRQ_S_EXT));

    // Enable interrupts in general.
    __enable_irq();

    printf("Drop to S-Mode now\n\r");
    /* Drop to S mode */
    __switch_mode(PRV_S, smode_sp, supervisor_mode_entry_point);
#else
    printf("[ERROR]__PMP_PRESENT & __PLIC_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}
