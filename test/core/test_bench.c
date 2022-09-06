#include <stdlib.h>
#include <string.h>
#include "ctest.h"
#include "nuclei_sdk_soc.h"
#include "nmsis_bench.h"


BENCH_DECLARE_VAR();

unsigned long test_mem[32];

CTEST(bench, bench)
{
    BENCH_INIT();
    BENCH_START(memset);
    memset(test_mem, 0xa5, sizeof(test_mem));
    BENCH_END(memset);
    BENCH_STATUS();
    BENCH_ERROR();
    BENCH_STATUS();
}
// Declare HPMCOUNTER3 and HPMCOUNTER4
HPM_DECLARE_VAR(3);
HPM_DECLARE_VAR(4);

// Define HPMEVENT3 and HPMEVENT4
// HPMEVENT3 value macro: event_sel: 0, event_idx: 8, m/s/uevent_enable: 0xf
// means Select the instruction commit events, record conditional branch event for all M/S/U mode
#define HPM_EVENT3      HPM_EVENT(0, 8, 0xf)

// HPMEVENT4 value macro: event_sel: 0, event_idx: 4, m/s/uevent_enable: 0x1
// means Select the instruction commit events, record integer store instruction (includes SC) event for only U mode
#define HPM_EVENT4      HPM_EVENT(0, 4, 0x1)

CTEST(bench, hpm)
{
    // Init HPM bench, only need to do it once
    HPM_INIT();

    // start to record hpm3 and hpm4
    HPM_START(3, memset, HPM_EVENT3);
    HPM_START(4, memset, HPM_EVENT4);
    memset(test_mem, 0x5a, sizeof(test_mem));
    // finish record and print hpm value
    HPM_END(3, memset, HPM_EVENT3);
    HPM_END(4, memset, HPM_EVENT4);
}

