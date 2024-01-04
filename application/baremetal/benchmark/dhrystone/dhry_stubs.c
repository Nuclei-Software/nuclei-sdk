#include "nuclei_sdk_soc.h"

void reset_cycle(void)
{
    __set_rv_cycle(0);
}

void reset_instret(void)
{
    __set_rv_instret(0);
}

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
    return SysTimer_GetLoadValue() / SOC_TIMER_FREQ;
}

