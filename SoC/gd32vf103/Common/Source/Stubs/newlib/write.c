/* See LICENSE of license details. */
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <nuclei_sdk_hal.h>

#undef putchar

int putchar(int dat)
{
    if (dat == '\n') {
        usart_write(SOC_DEBUG_UART, '\r');
    }
    usart_write(SOC_DEBUG_UART, dat);
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

