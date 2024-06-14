#include "nuclei_sdk_soc.h"

__STATIC_FORCEINLINE uint64_t get_timer_freq(void)
{
    return (uint64_t)SOC_TIMER_FREQ;
}

// optimize measure_cpu_freq function with Os/O0
// to get a correct cpu frequency, which
// is important for flashxip linker script
#if defined ( __GNUC__ )
#pragma GCC push_options
#pragma GCC optimize ("Os")
#elif defined ( __ICCRISCV__ )
#pragma optimize=medium
#endif
uint32_t measure_cpu_freq(uint32_t n)
{
#if defined(__TIMER_PRESENT) && (__TIMER_PRESENT == 1) && defined(__PMON_PRESENT) && (__PMON_PRESENT == 1)
    uint32_t start_mcycle, delta_mcycle;
    uint32_t start_mtime, delta_mtime;
    uint64_t mtime_freq = get_timer_freq();

    // Don't start measuruing until we see an mtime tick
    uint32_t tmp = (uint32_t)SysTimer_GetLoadValue();
    do {
        start_mtime = (uint32_t)SysTimer_GetLoadValue();
        start_mcycle = __get_rv_cycle();
    } while (start_mtime == tmp);

    do {
        delta_mtime = (uint32_t)SysTimer_GetLoadValue() - start_mtime;
        delta_mcycle = __get_rv_cycle() - start_mcycle;
    } while (delta_mtime < n);

    return (delta_mcycle / delta_mtime) * mtime_freq
           + ((delta_mcycle % delta_mtime) * mtime_freq) / delta_mtime;
#else
    // When system timer not exist, just return 1000000Hz
    #warning "measure_cpu_freq function require timer and performance monitor present, if you are using this, it will not work"
    return 1000000;
#endif
}
#if defined ( __GNUC__ )
#pragma GCC pop_options
#elif defined ( __ICCRISCV__ )
#endif

uint32_t get_cpu_freq(void)
{
    uint32_t cpu_freq;

    // warm up
    measure_cpu_freq(1);
    // measure for real
#ifdef CFG_SIMULATION
    cpu_freq = measure_cpu_freq(5);
#else
    cpu_freq = measure_cpu_freq(100);
#endif

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
#if defined(__TIMER_PRESENT) && (__TIMER_PRESENT == 1)
    uint64_t start_mtime, delta_mtime;
    uint64_t delay_ticks = (SOC_TIMER_FREQ * (uint64_t)count) / 1000;

    start_mtime = SysTimer_GetLoadValue();

    do {
        delta_mtime = SysTimer_GetLoadValue() - start_mtime;
    } while (delta_mtime < delay_ticks);
#else
    #warning "delay_1ms function require timer present, if you are using this, it will not work"
#endif
}

void simulation_exit(int status)
{
    // Both xlspike and qemu will write RXFIFO to make it works for xlspike even SIMU=qemu
    // workaround for fix cycle model exit with some message not print
    for (int i = 0; i < 10; i ++) {
        // print '\0' instead of '\r' for qemu simulation in ide
        uart_write(UART0, '\0');
    }
    uart_write(UART0, '\n');
    // pass exit status via rxfifo register
    SIMULATION_EXIT(status);
#if defined(SIMULATION_MODE)
#if SIMULATION_MODE == SIMULATION_MODE_QEMU
    #define QEMU_VIRT_TEST_BASE 0x100000
    #define QEMU_SIG_EXIT      0x3333
    REG32(QEMU_VIRT_TEST_BASE) = (status << 16) | QEMU_SIG_EXIT;
#endif
#endif
}

