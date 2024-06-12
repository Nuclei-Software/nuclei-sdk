#include <stdio.h>
#include <stdint.h>

#include "nuclei_sdk_soc.h"

#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
#else
#error "This example require CPU ECLIC feature"
#endif

#if defined(__SYSTIMER_PRESENT) && (__SYSTIMER_PRESENT == 1)
#else
#error "This example require CPU System Timer feature"
#endif

#if defined(SIMULATION_MODE) && (SIMULATION_MODE == SIMULATION_MODE_XLSPIKE)
#define RECORD_START()          UART0->RXFIFO = 1
#define RECORD_END()            UART0->RXFIFO = 2
#else
#define RECORD_START()
#define RECORD_END()
#endif

int main(void)
{
    volatile uint64_t start, end;

    __disable_irq();
    // need to adapt the tick according to your SoC
    SysTick_Config(1);
    SysTimer_Start();

    start = __get_rv_cycle();
    RECORD_START();
    // Should not enter interrupt handler due to irq disabled
    __WFI();
    RECORD_END();
    end = __get_rv_cycle();

    printf("CSV, WFI Start/End, %lu/%lu\n", (unsigned long)start, (unsigned long)end);
    printf("CSV, WFI Cost, %lu\n", (unsigned long)(end - start));

    return 0;
}
