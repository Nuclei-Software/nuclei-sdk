#include "nuclei_sdk_soc.h"

long csr_cycle(void)
{
    return __get_rv_cycle();
}

long csr_instret(void)
{
    return __get_rv_instret();
}

long time(void)
{
    return __get_rv_time() / SOC_TIMER_FREQ;
}

