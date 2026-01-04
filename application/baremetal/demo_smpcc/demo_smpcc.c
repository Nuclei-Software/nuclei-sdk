// See LICENSE for license details.
#include <stdio.h>
#include <string.h>

#include "nuclei_sdk_soc.h"
#include "nmsis_bench.h"

#if !defined(__SMPCC_PRESENT) || (__SMPCC_PRESENT != 1)
/* __SMPCC_PRESENT should be defined in <Device>.h */
#error "This example requires CPU SMPCC feature!"
#endif

BENCH_DECLARE_VAR();

#ifndef MAX_L2_SIZE_KB
/** \brief The max size of L2 Cache in KB
 * \remarks Here we assume the L2 Cache size is not larger than 1MB */
#define MAX_L2_SIZE_KB (1 * 1024)
#endif
/* The unit of MAX_L2_SIZE_BYTE and MIN_L2_SIZE_BYTE is byte */
#define MAX_L2_SIZE_BYTE (MAX_L2_SIZE_KB * 1024)
#define MIN_L2_SIZE_BYTE (64 * 1024) // 64KB

#ifndef MAX_TEST_SIZE
/** \brief The max size of memory to traverse
 * \remarks
 *  - Ensure that the memory size should larger than L1 Cache size
 *    and not larger than L2 Cache size.
 *  - Only use 25% L2 Cache size as test size. */
#define MAX_TEST_SIZE (MAX_L2_SIZE_BYTE >> 2)
#endif

#define CHECK_POWER_OF_2(x) ((x) && !((x) & ((x) - 1)))
#define PREHEAT_NUM 10 /*!< The number of preheat iterations. */

/* The start address of test memory will also be used as CLM start address.
 * So it should be aligned to the L2 cache size. */
uint8_t ddr_mem[MAX_TEST_SIZE] __attribute__((aligned(MAX_L2_SIZE_BYTE)));

static char *cvt_size(uint32_t size)
{
    static char buf[32];
    char units[] = {'B', 'K', 'M', 'G'};
    int i = 0;
    while (size >= 1024 && i < 3) {
        size >>= 10;
        i++;
    }
    snprintf(buf, sizeof(buf), "%u %c%s", size, units[i], i > 0 ? "B" : "");
    return buf;
}

static int show_smpcc_info(void)
{
    int cc_size = 0;
    printf("SMP & Cluster Cache Information:\r\n");
    printf("    SMP_CFG:");
    printf(" CC_PRESENT=%d", SMPCC_IsCCachePresent());
    printf(" SMP_NUM=%d", SMPCC_GetCoreNum());
    printf(" IOCP_NUM=%d", SMPCC_GetIOCPNum());
    printf(" PMON_NUM=%d", SMPCC_GetPMONNum());
    printf("\r\n");
    if (SMPCC_IsCCachePresent()) {
        uint32_t set = SMPCC_GetCCacheSetNum();
        uint32_t way = SMPCC_GetCCacheWayNum();
        uint32_t lsize = SMPCC_GetCCacheLineSize();
        cc_size = set * way * lsize;
        printf("    L2CACHE:");
        printf(" %s", cvt_size(cc_size));
        printf("(set=%d,", set);
        printf("way=%d,", way);
        printf("lsize=%d,", lsize);
        printf("ecc=%d)\r\n", SMPCC_IsCCacheSupportECC());
    }
    return cc_size;
}

static uint8_t traverse_step = 0;
static size_t traverse_size = 0;
/**
 * \brief Traverse memory for Cluster Cache or CLM access test
 * \remarks 
 * The `traverse_size` and `step` are global variables
 * which are set once in `main` and keep constant in all tests.
 * The `noinline` attribute is used to prevent inlining. When this function
 * is inlined, the MACRO `PREHEAT_NUM` may not take effect. */
static __attribute__((noinline)) void mem_traverse(const uint8_t *base)
{
    register uint8_t tmp;
    volatile const uint8_t *vbase = (volatile const uint8_t *)base;
    for (size_t i = 0; i < traverse_size; i += traverse_step) {
        tmp = vbase[i];
    }
    __RWMB();
}

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
static void CCache2CLM(unsigned long addr, uint32_t way_msk)
{
    SMPCC_DisableCCache();
    int32_t res = MFlushInvalCCache();
    if (res != SMPCC_CMD_xCMD_RESULT_SUCCESS) {
        printf("MFlushInvalCCache Failed!\r\n");
    }
    SMPCC_SetCLMNWays(addr, way_msk);
    SMPCC_EnableCLM();
}

static void CCachePerformanceMonitorTest()
{
    struct {
        uint8_t event;
        char *name;
    } event_list[] = {
        {SMPCC_PMON_EVENT_DATA_READ_COUNT, "Data Read Count"},
        {SMPCC_PMON_EVENT_DATA_READ_HIT_COUNT, "Data Read Hit Count"},
        {SMPCC_PMON_EVENT_DATA_READ_MISS_COUNT, "Data Read Miss Count"},
    };
    unsigned long event_count[sizeof(event_list)];
    int pmon_num = SMPCC_GetPMONNum();
    if (pmon_num == 0) {
        printf("No PMON available!\r\n");
        return;
    }
    int event_num = sizeof(event_list) / sizeof(event_list[0]);
    /* Choose the minimum of PMON number and event number */
    int loop = pmon_num > event_num ? event_num : pmon_num;
    /* First test with CCache flushed */
    MFlushInvalDCacheCCache();
    for (int i = 0; i < loop; i++) {
        SMPCC_ClearPMONCount(i);
        /* Use boot hart ID as SMPCC performance monitor client ID for demo */
        SMPCC_SetPMONEventSelect(i, BOOT_HARTID, event_list[i].event);
    }
    mem_traverse(ddr_mem);
    for (int i = 0; i < loop; i++) {
        event_count[i] = SMPCC_GetPMONCount(i);
    }
    printf("Test with CCache flushed:\r\n");
    for (int i = 0; i < loop; i++) {
        printf("CSV, %s, %lu\r\n", event_list[i].name, event_count[i]);
    }
    /* Test again, this time with CCache pre-filled */
    /* Invalidate data in L1 DCache to accurately measure CCache miss/hit rates */
    MInvalDCache();
    for (int i = 0; i < pmon_num; i++) {
        SMPCC_ClearPMONCount(i);
        SMPCC_SetPMONEventSelect(i, BOOT_HARTID, event_list[i].event);
    }
    mem_traverse(ddr_mem);
    for (int i = 0; i < loop; i++) {
        event_count[i] = SMPCC_GetPMONCount(i);
    }
    printf("Test with CCache pre-filled:\r\n");
    for (int i = 0; i < loop; i++) {
        printf("CSV, %s, %lu\r\n", event_list[i].name, event_count[i]);
    }
}

// Declare HPMCOUNTER3
HPM_DECLARE_VAR(3);
// Declare HPMCOUNTER4
HPM_DECLARE_VAR(4);

// NOTE: The following L2 Cache related HPM events only supported by PMUv2
// Select the L2 Cache read hit count, record L2 Cache read hit event for all M/S/U mode
#define HPM_EVENT3      HPM_EVENT(EVENT_SEL_TYPE_3, EVENT_TYPE_3_L2_CACHE_READ, MSU_EVENT_ENABLE)
// Select the L2 Cache read miss count, record L2 Cache read miss event for all M/S/U mode
#define HPM_EVENT4      HPM_EVENT(EVENT_SEL_TYPE_3, EVENT_TYPE_3_L2_CACHE_READ_MISS, MSU_EVENT_ENABLE)
static void CCachePerformanceMonitorTestWithPMUv2() {
#if defined(__HPM_PRESENT) && (__HPM_PRESENT == 1) && (!defined(DISABLE_NMSIS_HPM))
    HPM_INIT();
    /* First test with L2 Cache flushed */
    printf("Test with L2 Cache flushed:\r\n");
    MFlushInvalDCacheCCache();
    HPM_START(3, L2_read_count, HPM_EVENT3);
    HPM_START(4, L2_read_miss, HPM_EVENT4);
    mem_traverse(ddr_mem);
    HPM_END(4, L2_read_miss, HPM_EVENT4);
    HPM_END(3, L2_read_count, HPM_EVENT3);
    /* Test again, this time with L2 Cache pre-filled */
    /* Invalidate data in L1 DCache to accurately measure L2 cache miss/hit rates */
    MInvalDCache();
    printf("Test with L2 Cache pre-filled:\r\n");
    HPM_START(3, L2_read_count, HPM_EVENT3);
    HPM_START(4, L2_read_miss, HPM_EVENT4);
    mem_traverse(ddr_mem);
    HPM_END(4, L2_read_miss, HPM_EVENT4);
    HPM_END(3, L2_read_count, HPM_EVENT3);
#else
    printf("CFG_HAS_HPM is not defined in cpufeature.h. Please check the "
           "consistency between cpufeature.h and the hardware.\r\n");
#endif /* #if defined(__HPM_PRESENT) && (__HPM_PRESENT == 1) && (!defined(DISABLE_NMSIS_HPM)) */
}
#endif /* #if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1) */

int main(void)
{
    /* Only run on BOOT_HARTID */
    if (BOOT_HARTID != __get_hart_id()) {
        return 0;
    }
    CSR_MCFGINFO_Type mcfg;
    mcfg.d = __RV_CSR_READ(CSR_MCFG_INFO);
    if (!mcfg.b.smp) {
        printf("SMP feature is not supported!\r\n");
        return 0;
    }

    /* Disable all prefetch to accurately measure L2 cache miss/hit rates */
    uint32_t *pfl1dctrl1 = (uint32_t *)(__IINFO_BASEADDR + 0x100);
    *pfl1dctrl1 = 0; /* Disable all prefetch */

    SMP_VER_Type smp_ver = SMPCC_GetVersion();
    printf("SMPCC version is %d.%d.%d\r\n", smp_ver.b.maj_ver,
           smp_ver.b.min_ver, smp_ver.b.mic_ver);

    /* Part 1: Show SMPCC information */
    int cc_size = show_smpcc_info();
    /* The following demos need L2 cache presence and has a proper size */
    /* L2 cache size should be a power of 2 and larger than 64KB but not larger than MAX_L2_SIZE */
    if (!CHECK_POWER_OF_2(cc_size)) {
        printf("L2 cache size is not a power of 2!\r\n");
        return 0;
    } else if (cc_size < MIN_L2_SIZE_BYTE) {
        printf("There is no L2 cache or you may run on QEMU!\r\n");
        return 0;
    } else if (cc_size > MAX_L2_SIZE_BYTE) {
        printf("L2 cache is larger than %d KB!\r\n", MAX_L2_SIZE_KB);
        printf("You can set MAX_L2_SIZE_KB=%d to rebuild this demo and try again.\r\n", cc_size >> 10);
        return 0;
    }

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
    if (DOWNLOAD_MODE != DOWNLOAD_MODE_DDR) {
        printf("Please set DOWNLOAD=ddr to rebuild this demo and try again.\r\n");
        return 0;
    }

    /* Use L2 cache line size as step for memory traverse */
    traverse_step = SMPCC_GetCCacheLineSize();
    traverse_size = cc_size >> 2;

    /* Part 2: Show Difference between w/ or w/o CCache */
    /* Disable CCache
     * NOTE: Remember to flush and invalidate the L2 cache after disabling CCache */
    if (SMPCC_IsCCacheEnabled()) {
        SMPCC_DisableCCache();
        MFlushInvalCCache();
    }
    for (int i = 0; i < PREHEAT_NUM; i++)
        mem_traverse(ddr_mem);
    BENCH_START(mem_traverse_wo_ccache);
    mem_traverse(ddr_mem);
    BENCH_END(mem_traverse_wo_ccache);
    /* Enable CCache */
    SMPCC_EnableCCache();
    for (int i = 0; i < PREHEAT_NUM; i++)
        mem_traverse(ddr_mem);
    BENCH_START(mem_traverse_with_ccache);
    mem_traverse(ddr_mem);
    BENCH_END(mem_traverse_with_ccache);

    /* Part 3: Show how to change workmode between CCache and CLM */
    int clm_ways = SMPCC_GetCCacheWayNum() >> 2;
    int way_msk = (1 << clm_ways) - 1;
    CCache2CLM((unsigned long)&ddr_mem[0], way_msk);
    BENCH_START(mem_traverse_with_clm);
    mem_traverse(ddr_mem);
    BENCH_END(mem_traverse_with_clm);
    SMPCC_SetCLMNoWay(); /* Use CLM as Cluster Cache */

    /* Part 4: CCache performance monitor demo */
    /* 0x94 is the offset for performance configuration register */
    const uint32_t *performance_cfg1 = (const uint32_t *)(__IINFO_BASEADDR + 0x94);
    uint32_t hpm_exist = *performance_cfg1 & 0x1;      /* Bit 0: HPM existence flag */
    uint32_t hpm_ver = (*performance_cfg1 & 0xC000) >> 14;  /* Bits 15:14: HPM version */
    if (hpm_exist && hpm_ver == 2) {  /* Check if HPMv2 is available */
        printf("Cluster Cache performance has been monitored by PMUv2!\r\n");
        CCachePerformanceMonitorTestWithPMUv2();
    } else {
        CCachePerformanceMonitorTest();
    }
#else
    printf("__CCM_PRESENT is not defined or __CCM_PRESENT is 0!\r\n");
#endif
    printf("End of SMPCC demo!\r\n");
    return 0;
}
