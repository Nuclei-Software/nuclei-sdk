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

HPM_DECLARE_VAR(3);
HPM_DECLARE_VAR(4);

#define HPM_EVENT3      HPM_EVENT(0, 8, 0xf)
#define HPM_EVENT4      HPM_EVENT(0, 4, 0x1)

CTEST(bench, hpm)
{
    HPM_INIT();
    HPM_START(3, memset, HPM_EVENT3);
    HPM_START(4, memset, HPM_EVENT4);
    memset(test_mem, 0x5a, sizeof(test_mem));
    HPM_END(3, memset, HPM_EVENT3);
    HPM_END(4, memset, HPM_EVENT4);
}

