#include <stdlib.h>
#include "ctest.h"
#include "nuclei_sdk_soc.h"

static void test_loop(int loop)
{
    for (volatile int i = 0; i < loop; i ++) {

    }
}

__attribute__((optimize("-Os"))) static int test_cmpopretz(void *a0, int mode, int *addr)
{
    if (mode == 2) {
        test_loop((int)a0);
    }
    return 0;
}

CTEST(arch_zc, zcmp)
{
    int ret;

    ret = test_cmpopretz((void *)0x01234567, 3, (int *)0x87654321);
    ASSERT_EQUAL(ret, 0);
    ret = test_cmpopretz((void *)0x01234567, 2, (int *)0x87654321);
    ASSERT_EQUAL(ret, 0);
}
