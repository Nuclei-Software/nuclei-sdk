/* See LICENSE of license details. */
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "nuclei_sdk_hal.h"

// #define UART_AUTO_ECHO

#undef getchar

int getchar(void)
{
    int dat;

    dat = (int)uart_read(SOC_DEBUG_UART);
#ifdef UART_AUTO_ECHO
    uart_write(SOC_DEBUG_UART, (uint8_t)dat);
#endif
    return dat;
}

__WEAK ssize_t _read(int fd, void* ptr, size_t len)
{
    if (fd != STDIN_FILENO) {
        return -1;
    }
    ssize_t cnt = 0;
    uint8_t* readbuf = (uint8_t*)ptr;
    for (cnt = 0; cnt < len; cnt ++) {
        readbuf[cnt] = getchar();
    }

    return cnt;
}
