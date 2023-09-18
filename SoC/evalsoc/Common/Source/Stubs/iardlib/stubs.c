#include "nuclei_sdk_hal.h"

// #define UART_AUTO_ECHO

// uncomment it if you want to print via IAR Breakpoint(C-SPY emulated IO)
// You need to set Library Configuration -> Stdout/Stderr -> Via IAR Breakpoint
// #define DEBUG_IAR_BREAKPOINT

// For more iar dlib stub function

#ifndef DEBUG_IAR_BREAKPOINT
// By default print via uart io
int putchar(int ch)
{
    if (ch == '\n') {
        uart_write(SOC_DEBUG_UART, '\r');
    }
    uart_write(SOC_DEBUG_UART, ch);

    return ch;
}

int getchar(void)
{
    int dat;

    dat = (int)uart_read(SOC_DEBUG_UART);
#ifdef UART_AUTO_ECHO
    uart_write(SOC_DEBUG_UART, (uint8_t)dat);
#endif
    return dat;
}

#endif
