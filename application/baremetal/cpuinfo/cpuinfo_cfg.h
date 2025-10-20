#ifndef __CPUINFO_CFG_H__
#define __CPUINFO_CFG_H__

/* NOTE: You can customize your own printf function here */
#include <stdio.h>
#define CIF_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)

typedef unsigned long addr_t;
#define CIF_PRIxADDR "lx"

#endif
