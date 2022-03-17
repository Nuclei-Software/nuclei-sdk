#include <stdint.h>
#include <nuclei_sdk_hal.h>

int metal_tty_putc(int c)
{
    if (c == '\n') {
        usart_write(SOC_DEBUG_UART, '\r');
    }
    usart_write(SOC_DEBUG_UART, (uint8_t)c);
    return 0;
}

int metal_tty_getc(void)
{
    int c = (int)usart_read(SOC_DEBUG_UART);
    return c;
}

void exit(int fd)
{
    while (1) {
        __WFI();
    }
}

// workaround for newlibc required
// __libc_fini_array and __libc_init_array function
__WEAK void __libc_fini_array(void)
{
}

// required by libncrt_2.0.0 in RISC-V GNU Toolchain 2022.06
extern void __SEGGER_RTL_init_heap(void *ptr, size_t size);
extern char __heap_start[];
extern char __heap_end[];

__WEAK void __libc_init_array(void)
{
    size_t heapsz = (size_t)__heap_end - (size_t)__heap_start;
    __SEGGER_RTL_init_heap((void *)__heap_start, heapsz);
}

