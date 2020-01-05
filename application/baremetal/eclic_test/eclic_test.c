// See LICENSE for license details.
#include <stdio.h>
#include "nuclei_sdk_hal.h"
#include "gpio.h"

/* define the interrupt handler name same as vector incase download mode is flashxip */
#define SOC_BUTTON_1_HANDLER eclic_irq49_handler
#define SOC_BUTTON_2_HANDLER eclic_irq50_handler

void wait_seconds(uint32_t seconds)
{
    uint64_t start_mtime, delta_mtime;
    uint64_t wait_ticks = ((uint64_t)seconds) * SOC_TIMER_FREQ;

    start_mtime = SysTimer_GetLoadValue();

    do {
        delta_mtime = SysTimer_GetLoadValue() - start_mtime;
    } while (delta_mtime < wait_ticks);
}

// Vector interrupt
__INTERRUPT void SOC_BUTTON_1_HANDLER(void)
{
    // save mepc,mcause,msubm enable interrupts
    SAVE_IRQ_CSR_CONTEXT();

    printf("%s", "----Begin button1 handler----Vector mode\r\n");

    // Green LED toggle
    gpio_toggle(GPIO, SOC_LED_GREEN_GPIO_MASK);

    // Clear the GPIO Pending interrupt by writing 1.
    gpio_clear_interrupt(GPIO, SOC_BUTTON_1_GPIO_OFS, GPIO_INT_RISE);

    wait_seconds(1); // Wait for a while

    printf("%s", "----End button1 handler\r\n");

    // disable interrupts,restore mepc,mcause,msubm
    RESTORE_IRQ_CSR_CONTEXT();
}

void SOC_BUTTON_2_HANDLER(void)
{
    printf("%s", "--------Begin button2 handler----NonVector mode\r\n");

    // Blue LED toggle
    gpio_toggle(GPIO, SOC_LED_BLUE_GPIO_MASK);

    // Clear pending interrupt of Button 2
    gpio_clear_interrupt(GPIO, SOC_BUTTON_2_GPIO_OFS, GPIO_INT_RISE);

    ECLIC_SetPendingIRQ(SOC_INT19_IRQn);

    wait_seconds(1); // Wait for a while

    printf("%s", "--------End button2 handler\r\n");
}

void eclic_irq19_handler(void)
{
    ECLIC_ClearPendingIRQ(SOC_INT19_IRQn);
    printf("1st level interrupt nesting hit\r\n");
    ECLIC_SetPendingIRQ(SOC_INT20_IRQn);
    wait_seconds(1);
    printf("1st level interrupt nesting end\r\n");
}

void eclic_irq20_handler(void)
{
    ECLIC_ClearPendingIRQ(SOC_INT20_IRQn);
    printf("2nd level interrupt nesting hit\r\n");
    wait_seconds(1);
    printf("2nd level interrupt nesting end\r\n");
}

void board_gpio_init(void)
{
    gpio_enable_input(GPIO, SOC_BUTTON_GPIO_MASK);
    gpio_set_pue(GPIO, SOC_BUTTON_GPIO_MASK, GPIO_BIT_ALL_ONE);

    gpio_enable_output(GPIO, SOC_LED_GPIO_MASK);
    gpio_write(GPIO, SOC_LED_GPIO_MASK, GPIO_BIT_ALL_ZERO);
    // Initialize the button as rising interrupt enabled
    gpio_enable_interrupt(GPIO, SOC_BUTTON_GPIO_MASK, GPIO_INT_RISE);
}

int main(int argc, char **argv)
{
    uint32_t returncode;
    // Set up the GPIOs such that the LED GPIO
    // can be used as both Inputs and Outputs.
    board_gpio_init();

    printf("start vector mode interrupt test\r\n");
    returncode = ECLIC_Register_IRQ(SOC_BUTTON_1_IRQn, ECLIC_VECTOR_INTERRUPT,
                                    ECLIC_POSTIVE_EDGE_TRIGGER, 2, 0,
                                    SOC_BUTTON_1_HANDLER);
    printf("start non-vector mode interrupt nesting test\r\n");
    returncode =
        ECLIC_Register_IRQ(SOC_INT19_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                           ECLIC_POSTIVE_EDGE_TRIGGER, 1, 0, eclic_irq19_handler);
    returncode =
        ECLIC_Register_IRQ(SOC_INT20_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                           ECLIC_POSTIVE_EDGE_TRIGGER, 2, 0, eclic_irq20_handler);
    returncode = ECLIC_Register_IRQ(
        SOC_BUTTON_2_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
        ECLIC_POSTIVE_EDGE_TRIGGER, 0, 0, SOC_BUTTON_2_HANDLER);

    // Enable interrupts in general.
    __enable_irq();

    while (1) {}

    return 0;
}
