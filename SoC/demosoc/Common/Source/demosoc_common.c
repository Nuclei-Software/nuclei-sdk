#include "nuclei_sdk_soc.h"

__STATIC_FORCEINLINE uint64_t get_timer_freq(void)
{
    return (uint64_t)SOC_TIMER_FREQ;
}

uint32_t measure_cpu_freq(uint32_t n)
{
    uint32_t start_mcycle, delta_mcycle;
    uint32_t start_mtime, delta_mtime;
    uint64_t mtime_freq = get_timer_freq();

    // Don't start measuruing until we see an mtime tick
    uint32_t tmp = (uint32_t)SysTimer_GetLoadValue();
    do {
        start_mtime = (uint32_t)SysTimer_GetLoadValue();
        start_mcycle = __RV_CSR_READ(CSR_MCYCLE);
    } while (start_mtime == tmp);

    do {
        delta_mtime = (uint32_t)SysTimer_GetLoadValue() - start_mtime;
        delta_mcycle = __RV_CSR_READ(CSR_MCYCLE) - start_mcycle;
    } while (delta_mtime < n);

    return (delta_mcycle / delta_mtime) * mtime_freq
           + ((delta_mcycle % delta_mtime) * mtime_freq) / delta_mtime;
}

uint32_t get_cpu_freq(void)
{
    uint32_t cpu_freq;

    // warm up
    measure_cpu_freq(1);
    // measure for real
    cpu_freq = measure_cpu_freq(100);

    return cpu_freq;
}

/**
 * \brief      delay a time in milliseconds
 * \details
 *             provide API for delay
 * \param[in]  count: count in milliseconds
 * \remarks
 */
void delay_1ms(uint32_t count)
{
    uint64_t start_mtime, delta_mtime;
    uint64_t delay_ticks = (SOC_TIMER_FREQ * (uint64_t)count) / 1000;

    start_mtime = SysTimer_GetLoadValue();

    do {
        delta_mtime = SysTimer_GetLoadValue() - start_mtime;
    } while (delta_mtime < delay_ticks);
}

#if defined(SIMULATION_MODE)
void simulation_exit(int status)
{
#if SIMULATION_MODE == SIMULATION_MODE_XLSPIKE
    // pass exit status via rxfifo register
    UART0->RXFIFO = status;
    uart_write(UART0, 4);
#elif SIMULATION_MODE == SIMULATION_MODE_QEMU
    #define QEMU_VIRT_TEST_BASE 0x100000
    #define QEMU_SIG_EXIT      0x3333
    REG32(QEMU_VIRT_TEST_BASE) = (status << 16) | QEMU_SIG_EXIT;
#endif
}
#endif
