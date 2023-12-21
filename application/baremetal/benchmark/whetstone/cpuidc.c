#include <stdio.h>
#include "nuclei_sdk_soc.h"
#include "cpuidh.h"
#include "config.h"

volatile SPDP  theseSecs = 0.0;
volatile SPDP  startSecs = 0.0;
volatile SPDP  secs;

SPDP time()
{
    return (SPDP) __get_rv_time() / SOC_TIMER_FREQ;
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
