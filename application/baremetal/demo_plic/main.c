// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_hal.h"

#if !defined(__PLIC_PRESENT) || (__PLIC_PRESENT == 0)
/* __PLIC_PRESENT should be defined in <Device>.h */
#warning "__PLIC_PRESENT is not defined or equal to 0, please check!"
#endif

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


int main(int argc, char **argv)
{
#if defined(__PLIC_PRESENT) && (__PLIC_PRESENT == 1)
    int32_t returnCode;

    // Initialize interrupt mode to clint/plic mode
    PLIC_Interrupt_Init();

    // register plic uart0 interrupt
    // when register plic interrupt, please PLIC_xxx_IRQn
    returnCode = PLIC_Register_IRQ(PLIC_UART0_IRQn, 1, plic_uart0_handler);

    // Enable interrupts in general.
    __enable_irq();

    // Enable uart0 receive interrupt
    uart_enable_rxint(SOC_DEBUG_UART);
    printf("You can press any key now to trigger uart receive interrupt\n");
    // Wait uart0 press any key
    while (1);
#else
    printf("[ERROR]__PLIC_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}
