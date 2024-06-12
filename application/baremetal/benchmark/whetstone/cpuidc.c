#include <stdio.h>
#include "nuclei_sdk_soc.h"
#include "cpuidh.h"
#include "config.h"

volatile SPDP  theseSecs = 0.0;
volatile SPDP  startSecs = 0.0;
volatile SPDP  secs;

SPDP time()
{
#if defined(__SYSTIMER_PRESENT) && (__SYSTIMER_PRESENT == 1)
    return (SPDP) SysTimer_GetLoadValue() / SOC_TIMER_FREQ;
#else
#error "This example require CPU System Timer feature"
#endif
}

void getSecs()
{
    theseSecs = time() ;
    return;
}

void start_time()
{
    getSecs();
    startSecs = theseSecs;
    return;
}

void end_time()
{
    getSecs();
    secs = theseSecs - startSecs;
    return;
}
