/* See LICENSE of license details. */
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "nuclei_sdk_hal.h"

// #define UART_AUTO_ECHO

__WEAK ssize_t _read(int fd, void* ptr, size_t len)
{
    if (fd != STDIN_FILENO) {
        return -1;
    }

    uint8_t* readbuf = (uint8_t*)ptr;
    readbuf[0] = usart_read(SOC_DEBUG_UART);
#ifdef UART_AUTO_ECHO
    usart_write(SOC_DEBUG_UART, (int)readbuf[0]);
#endif
    return 1;
}
