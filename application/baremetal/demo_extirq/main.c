// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_hal.h"

// TODO: must place this irqc_uart0_handler to vector_base table
// defined in system_<Device>.c
// If you have other new interrupt handlers, similar steps need to be done

// vector mode interrupt
__INTERRUPT void irqc_uart0_handler(void)
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

int main(void)
{
    IRQC_Register_IRQ(UART0_IRQn, (void *)irqc_uart0_handler);

    // Enable interrupts in general.
    __enable_irq();

    // Enable uart0 receive interrupt
    uart_enable_rxint(SOC_DEBUG_UART);
    printf("You can press any key now to trigger uart receive interrupt\n");
    // Wait uart0 press any key
    while (1);

    return 0;
}
