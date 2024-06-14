// See LICENSE for license details.
#ifndef _NUCLEI_SDK_HAL_H
#define _NUCLEI_SDK_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board_nuclei_fpga_eval.h"

#define SOC_DEBUG_UART      UART0

#ifndef DISABLE_NSDK_DEBUG
#define NSDK_DEBUG(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#else
#define NSDK_DEBUG(fmt, ...)
#endif

#ifndef NUCLEI_BANNER
#define NUCLEI_BANNER       1
#endif

#ifdef __cplusplus
}
#endif
#endif
