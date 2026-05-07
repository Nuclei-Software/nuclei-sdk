#include <stdlib.h>
#include "ctest.h"
#include "nuclei_sdk_soc.h"

#ifdef __riscv_atomic

#define INIT_VALUE  0x12345678
#define TEST_VALUE  0xDEADBEEF
CTEST(atomic, cas_w)
{
    uint32_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    ASSERT_EQUAL(__CAS_W((uint32_t*)pdata, INIT_VALUE, TEST_VALUE), INIT_VALUE);
}

CTEST(atomic, amoswap_w)
{
    uint32_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    ASSERT_EQUAL(__AMOSWAP_W((uint32_t*)pdata, TEST_VALUE), INIT_VALUE);
}

CTEST(atomic, amoadd_w)
{
    uint32_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint32_t result = __AMOADD_W((int32_t*)pdata, TEST_VALUE);
    uint32_t expected = INIT_VALUE;
    CTEST_LOG("AMOADD returned old value: 0x%x vs 0x%x", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, INIT_VALUE + TEST_VALUE);
}

CTEST(atomic, amoand_w)
{
    uint32_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint32_t result = __AMOAND_W((int32_t*)pdata, TEST_VALUE);
    uint32_t expected = INIT_VALUE;
    CTEST_LOG("AMOAND returned old value: 0x%x vs 0x%x", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, INIT_VALUE & TEST_VALUE);
}

CTEST(atomic, amoor_w)
{
    uint32_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint32_t result = __AMOOR_W((int32_t*)pdata, TEST_VALUE);
    uint32_t expected = INIT_VALUE;
    CTEST_LOG("AMOOR returned old value: 0x%x vs 0x%x", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, INIT_VALUE | TEST_VALUE);
}

CTEST(atomic, amoxor_w)
{
    uint32_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint32_t result = __AMOXOR_W((int32_t*)pdata, TEST_VALUE);
    uint32_t expected = INIT_VALUE;
    CTEST_LOG("AMOXOR returned old value: 0x%x vs 0x%x", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, INIT_VALUE ^ TEST_VALUE);
}

CTEST(atomic, amomaxu_w)
{
    uint32_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint32_t result = __AMOMAXU_W((uint32_t*)pdata, TEST_VALUE);
    uint32_t expected = INIT_VALUE;
    CTEST_LOG("AMOMAXU returned old value: %u vs %u", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, (uint32_t)(INIT_VALUE) > (uint32_t)(TEST_VALUE) ? INIT_VALUE : TEST_VALUE);
}

CTEST(atomic, amomax_w)
{
    uint32_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    int32_t result = __AMOMAX_W((int32_t*)pdata, TEST_VALUE);
    int32_t expected = INIT_VALUE;
    CTEST_LOG("AMOMAX returned old value: %d vs %d", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, (int32_t)(INIT_VALUE) > (int32_t)(TEST_VALUE) ? INIT_VALUE : TEST_VALUE);
}

CTEST(atomic, amominu_w)
{
    uint32_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint32_t result = __AMOMINU_W((uint32_t*)pdata, TEST_VALUE);
    uint32_t expected = INIT_VALUE;
    CTEST_LOG("AMOMINU returned old value: %u vs %u", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, (uint32_t)(INIT_VALUE) < (uint32_t)(TEST_VALUE) ? INIT_VALUE : TEST_VALUE);
}

CTEST(atomic, amomin_w)
{
    uint32_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    int32_t result = __AMOMIN_W((int32_t*)pdata, TEST_VALUE);
    int32_t expected = INIT_VALUE;
    CTEST_LOG("AMOMIN returned old value: %d vs %d", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, (int32_t)(INIT_VALUE) < (int32_t)(TEST_VALUE) ? INIT_VALUE : TEST_VALUE);
}

#if __RISCV_XLEN == 64
CTEST(atomic, cas_d)
{
    uint64_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    ASSERT_EQUAL(__CAS_D((uint64_t*)pdata, INIT_VALUE, TEST_VALUE), INIT_VALUE);
}

CTEST(atomic, amoswap_d)
{
    uint64_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    ASSERT_EQUAL(__AMOSWAP_D((uint64_t*)pdata, TEST_VALUE), INIT_VALUE);
}

CTEST(atomic, amoadd_d)
{
    uint64_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint64_t result = __AMOADD_D((int64_t*)pdata, TEST_VALUE);
    uint64_t expected = INIT_VALUE;
    CTEST_LOG("AMOADD returned old value: 0x%lx vs 0x%lx", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, INIT_VALUE + TEST_VALUE);
}

CTEST(atomic, amoand_d)
{
    uint64_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint64_t result = __AMOAND_D((int64_t*)pdata, TEST_VALUE);
    uint64_t expected = INIT_VALUE;
    CTEST_LOG("AMOAND returned old value: 0x%lx vs 0x%lx", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, INIT_VALUE & TEST_VALUE);
}

CTEST(atomic, amoor_d)
{
    uint64_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint64_t result = __AMOOR_D((int64_t*)pdata, TEST_VALUE);
    uint64_t expected = INIT_VALUE;
    CTEST_LOG("AMOOR returned old value: 0x%lx vs 0x%lx", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, INIT_VALUE | TEST_VALUE);
}

CTEST(atomic, amoxor_d)
{
    uint64_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint64_t result = __AMOXOR_D((int64_t*)pdata, TEST_VALUE);
    uint64_t expected = INIT_VALUE;
    CTEST_LOG("AMOXOR returned old value: 0x%lx vs 0x%lx", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, INIT_VALUE ^ TEST_VALUE);
}

CTEST(atomic, amomaxu_d)
{
    uint64_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint64_t result = __AMOMAXU_D((uint64_t*)pdata, TEST_VALUE);
    uint64_t expected = INIT_VALUE;
    CTEST_LOG("AMOMAXU returned old value: %lu vs %lu", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, (uint64_t)(INIT_VALUE) > (uint64_t)(TEST_VALUE) ? INIT_VALUE : TEST_VALUE);
}

CTEST(atomic, amomax_d)
{
    uint64_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    int64_t result = __AMOMAX_D((int64_t*)pdata, TEST_VALUE);
    int64_t expected = INIT_VALUE;
    CTEST_LOG("AMOMAX returned old value: %ld vs %ld", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, (int64_t)(INIT_VALUE) > (int64_t)(TEST_VALUE) ? INIT_VALUE : TEST_VALUE);
}

CTEST(atomic, amominu_d)
{
    uint64_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    uint64_t result = __AMOMINU_D((uint64_t*)pdata, TEST_VALUE);
    uint64_t expected = INIT_VALUE;
    CTEST_LOG("AMOMINU returned old value: %lu vs %lu", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, (uint64_t)(INIT_VALUE) < (uint64_t)(TEST_VALUE) ? INIT_VALUE : TEST_VALUE);
}

CTEST(atomic, amomin_d)
{
    uint64_t data = INIT_VALUE;
    unsigned long pdata = (unsigned long)(&data);
    int64_t result = __AMOMIN_D((int64_t*)pdata, TEST_VALUE);
    int64_t expected = INIT_VALUE;
    CTEST_LOG("AMOMIN returned old value: %ld vs %ld", result, expected);
    ASSERT_EQUAL(result, expected);
    ASSERT_EQUAL(data, (int64_t)(INIT_VALUE) < (int64_t)(TEST_VALUE) ? INIT_VALUE : TEST_VALUE);
}
#endif

#endif
