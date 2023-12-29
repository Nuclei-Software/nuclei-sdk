/* See LICENSE of license details. */
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <nuclei_sdk_hal.h>

#undef putchar

extern void __usart_putchar(uint32_t usart_periph, uint16_t data);

int putchar(int dat)
{
    if (dat == '\n') {
        __usart_putchar(SOC_DEBUG_UART, '\r');
    }
    __usart_putchar(SOC_DEBUG_UART, dat);
    return dat;
}

__WEAK ssize_t _write(int fd, const void* ptr, size_t len)
{
    if (!isatty(fd)) {
        return -1;
    }

    const uint8_t* writebuf = (const uint8_t*)ptr;
    for (size_t i = 0; i < len; i++) {
        putchar((int)writebuf[i]);
    }
    return len;
}

