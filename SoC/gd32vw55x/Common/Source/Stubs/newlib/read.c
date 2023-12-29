/* See LICENSE of license details. */
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "nuclei_sdk_hal.h"

// #define UART_AUTO_ECHO

extern void __usart_putchar(uint32_t usart_periph, uint16_t data);
extern int __usart_getchar(uint32_t usart_periph);

#undef getchar

int getchar(void)
{
    int dat;

    dat = (int)__usart_getchar(SOC_DEBUG_UART);
#ifdef UART_AUTO_ECHO
    __usart_putchar(SOC_DEBUG_UART, (uint16_t)dat);
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
        /* Return partial buffer if we get EOL */
        if (readbuf[cnt] == '\n') {
            return cnt;
        }
    }

    return cnt;
}
