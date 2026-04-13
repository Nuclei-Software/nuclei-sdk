// See LICENSE for license details.
#include <stdio.h>
#include <stdlib.h>

#include "nuclei_sdk_soc.h"

/**
 * \brief Mask for injecting a single-bit ECC error.
 * \details 
 * The ECC code is 7 bits wide for 32-bit data and 8 bits wide for 64-bit data.
 * To inject a single-bit error, the mask must be a one-hot value (i.e., exactly one bit set).
 * This mask is used both in 32-bit and 64-bit cases, so the error bit must be within
 * positions 0 to 6 (inclusive).
 * This default mask (0x1U) targets the least significant ECC bit.
 */
#define SINGLEBIT_ERROR_MASK (0x1U)

/**
 * \brief Mask for injecting a double-bit ECC error.
 * \details
 * This macro has similar restriction as SINGLEBIT_ERROR_MASK, the two error bits must be
 * within positions 0 to 6. 
 */
#define DOUBLEBIT_ERROR_MASK (0x3U)

/**
 * If you can determine the hardware configuration of tag bits,
 * you can define the corresponding macros to skip dynamic calculation.
 *
 * For example:
 *
 * An ICache has 512 sets, each cache line has 64 Bytes, and the
 * physical address (PASIZE) is 32 bits. Then the ICache tag bits
 * is 17 bits. tag bits = 32 - log2(512) - log2(64)
 *
 * +-------------------------------------------------+
 * | tag bits: 17 | set bits: 9 | cache line bits: 6 |
 * +-------------------------------------------------+
 *
 * Then we can define the macro as:
 *
 * #define ICACHE_TAGBITS (17)
 *
 * Then the dynamic calculation will not be performed.
 *
 */

/* Enter the corresponding cache tag bits and uncomment the macro
 * to skip the dynamic calculation. */
// #define ICACHE_TAGBITS (<Enter value here>)
// #define DCACHE_TAGBITS (<Enter value here>)
// #define CCACHE_TAGBITS (<Enter value here>)

#ifndef ICACHE_TAGBITS
#define ICACHE_TAGBITS  get_ic_tagbits()
#endif
#ifndef DCACHE_TAGBITS
#define DCACHE_TAGBITS  get_dc_tagbits()
#endif
#ifndef CCACHE_TAGBITS
#define CCACHE_TAGBITS  get_cc_tagbits()
#endif

uint32_t pa_size; /*!< Physical address size */
typedef enum {
    ECC_ERR_INJ_MODE_NOT_EXIST = 0, /*!< ECC not exist */
    ECC_ERR_INJ_MODE_DIRECT_WRITE,  /*!< ECC exist and support error injection direct write mode */
    ECC_ERR_INJ_MODE_XOR,           /*!< ECC exist and support error injection XOR mode */
    ECC_ERR_INJ_MODE_MAX            /*!< Max of ECC Error injection mode */
} ECC_ERR_INJ_MODE;
ECC_ERR_INJ_MODE inj_mode = __ECC_CODE_INJ; /*!< Initialize ECC Error injection mode with CPU CFG */
enum ECC_MEM_Type {
    ECC_MEM_TYPE_ICACHE = 0,
    ECC_MEM_TYPE_DCACHE,
    ECC_MEM_TYPE_TLB,
    ECC_MEM_TYPE_ILM,
    ECC_MEM_TYPE_DLM,
    ECC_MEM_TYPE_CCACHE,
    ECC_MEM_TYPE_MAX
} test_mem_type = ECC_MEM_TYPE_MAX;   /*!< The memory type of ECC functionality test */
volatile int inst_acc_fault_flag = 0; /*!< Instruction access fault flag */
volatile int load_fault_flag = 0;     /*!< Load access fault flag */

/**
 * \brief Simple test function that just returns
 * \remarks Used to test ECC error injection functionality. The function is aligned
 * to cache line boundaries (128 bytes) to prevent error injection from affecting
 * other code when the processor pre-fetches instructions.
 */
void __attribute__((naked, aligned(128))) test_code(void)
{
    __ASM volatile("ret;"
                   ".balign 128");
}

/**
 * \brief Test data array
 * \remarks This data block is 128 bytes in size and aligned to 128-byte boundaries.
 * It follows the same design principle as test_code to prevent interference during error testing.
 */
uint32_t __attribute__((aligned(128))) test_data[32] = {0};


static uint32_t get_ic_tagbits()
{
    uint32_t ic_info = __RV_CSR_READ(CSR_MICFG_INFO);
    uint32_t set_bits = (ic_info & 0xF) + 3;
    uint32_t lsize_bits = ((ic_info >> 7) & 0x7) + 2;
    rv_csr_t mcfg = __RV_CSR_READ(CSR_MCFG_INFO);
    uint32_t off_bits = set_bits + lsize_bits;
    if (!!(mcfg & MCFG_INFO_PLIC)) {
        /*
         * With MMU enabled, virtual and physical addresses share the same
         * lower 12 bits (since page size is 4KB). The ICache uses these
         * bits directly for indexing. PLIC implies MMU is present.
         */
        off_bits = 12;
    }
    return pa_size - off_bits;
}

static uint32_t get_dc_tagbits()
{
    uint32_t dc_info = __RV_CSR_READ(CSR_MDCFG_INFO);
    uint32_t set_bits = (dc_info & 0xF) + 3;
    uint32_t lsize_bits = ((dc_info >> 7) & 0x7) + 2;
    uint32_t off_bits = set_bits + lsize_bits;
    return pa_size - off_bits;
}

static uint32_t get_cc_tagbits()
{
    uint32_t set_bits = SMPCC->CC_CFG.b.cc_set;
    uint32_t lsize_bits = SMPCC->CC_CFG.b.cc_lsize + 2;
    uint32_t off_bits = set_bits + lsize_bits;
    return pa_size - off_bits;
}

/**
 * \brief Generate the error injected ecc code according to the golden
 * ecc code and the error injection mask.
 */
static inline uint8_t gen_ecc_err_code(uint32_t msk, uint8_t golden)
{
    return inj_mode == ECC_ERR_INJ_MODE_DIRECT_WRITE ? (msk ^ golden) : msk;
}

void inst_access_fault_handler(unsigned long mcause, unsigned long sp)
{
    inst_acc_fault_flag += 1;
    switch (test_mem_type) {
        case (ECC_MEM_TYPE_ILM):
            if (ECC_IsILMDoubleBitErrorOccured()) {
                ECC_DisableILMECCExcp();
                ECC_ClearILMDoubleBitError();
            }
            break;
        case (ECC_MEM_TYPE_ICACHE):
            if (ECC_IsICacheDoubleBitErrorOccured()) {
                ECC_DisableICacheECCExcp();
                ECC_ClearICacheDoubleBitError();
            }
            break;
        case (ECC_MEM_TYPE_CCACHE):
            if (SMPCC_GetFatalErrCount()) {
                SMPCC_DisableCCacheECCExcp();
                SMPCC_ClearFatalErrCount();
            }
        default:
            break;
    }
}

void load_fault_handler(unsigned long mcause, unsigned long sp)
{
    load_fault_flag += 1;
    switch (test_mem_type) {
        case (ECC_MEM_TYPE_DLM):
            if (ECC_IsDLMDoubleBitErrorOccured()) {
                ECC_DisableDLMECCExcp();
                ECC_ClearDLMDoubleBitError();
            }
            break;
        case (ECC_MEM_TYPE_DCACHE):
            if (ECC_IsDCacheDoubleBitErrorOccured()) {
                ECC_DisableDCacheECCExcp();
                ECC_ClearDCacheDoubleBitError();
            }
            break;
        case (ECC_MEM_TYPE_CCACHE):
            if (SMPCC_GetFatalErrCount()) {
                SMPCC_DisableCCacheECCExcp();
                SMPCC_ClearFatalErrCount();
            }
        default:
            break;
    }
}

#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
static int32_t __attribute__((aligned(4096))) ic_ecc_err_inj_demo();
static int32_t __attribute__((aligned(4096))) cc_ecc_err_inj_demo();
static void setup_nc_region(void)
{
    pma_config pma_cfg = {
        .region_base = 0,
        .region_size = 0x1000,
        .region_type = PMA_REGION_TYPE_NC,
        .region_enable = PMA_REGION_ENA,
    };

    pma_cfg.region_base = (unsigned long)&ic_ecc_err_inj_demo;
    PMA_SetRegion(0, &pma_cfg);
    pma_cfg.region_base = (unsigned long)&cc_ecc_err_inj_demo;
    PMA_SetRegion(1, &pma_cfg);
}

static int32_t __attribute__((aligned(4096))) ic_ecc_err_inj_demo()
{
    if (!ECC_IsICacheSupportECC()) {
        printf("ICache not present or ICache not support ECC, will not run ICache ECC demo.\r\n");
        return EXIT_SUCCESS;
    }
    test_mem_type = ECC_MEM_TYPE_ICACHE;

    ECC_EnableICacheECC();
    ECC_EnableICacheECCCheck();

    /* ------------- Calculate ICache Data RAM golden ecc code ------------- */
#if __RISCV_XLEN == 32
    uint32_t code = *(const uint32_t *)test_code;
    uint8_t ecc_code_calc = ECC_GenerateECCCodeU32(code);
#elif __RISCV_XLEN == 64
    uint64_t code = *(const uint64_t *)test_code;
    uint8_t ecc_code_calc = ECC_GenerateECCCodeU64(code);
#endif

    /* ------------- ICache Data RAM single bit error test ------------- */
    uint8_t sgl_ecc_code = gen_ecc_err_code(SINGLEBIT_ERROR_MASK, ecc_code_calc);
    ECC_ICacheDRamErrInject(sgl_ecc_code, (void *)test_code);
    test_code();

    if (ECC_IsICacheSingleBitErrorOccured()) {
        ECC_ClearICacheSingleBitError();
        printf("ECC single bit error has occured on ICache Data RAM!\r\n");
    } else {
        printf("[ERROR]: ECC single bit error hasn't occured on ICache Data RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of ICache Data RAM single bit error test ---------- */

    /* ------------- ICache Data RAM double bit error test ------------- */
    uint8_t dbl_ecc_code = gen_ecc_err_code(DOUBLEBIT_ERROR_MASK, ecc_code_calc);
    inst_acc_fault_flag = 0;
    ECC_ICacheDRamErrInject(dbl_ecc_code, (void *)test_code);
    ECC_EnableICacheECCExcp();
    test_code();

    if (inst_acc_fault_flag == 1) {
        printf("ECC double bit error has occured on ICache Data RAM!\r\n");
    } else {
        printf("[ERROR]: ECC double bit error hasn't occured on ICache Data RAM!\r\n");
        return EXIT_FAILURE;
    }

    ECC_ICacheErrRestore((void *)test_code);
    ECC_EnableICacheECCExcp();
    test_code();

    if (inst_acc_fault_flag == 2) {
        printf("[ERROR]: ECC double bit error occured again on ICache Data RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of ICache Data RAM double bit error test ---------- */

    /* ------------- Calculate ICache TAG RAM golden ecc code ------------- */
    /**
     * Tag value structure: total 32 bits
     * high -> low
     * +-------------------------------------------------------------+
     * | 0 extended | 1bit: lock | 1bit: valid | tag_bits: tag_value |
     * +-------------------------------------------------------------+
     */
    uint32_t tag_bits = ICACHE_TAGBITS;
    uint32_t off_bits = pa_size - tag_bits;
    uint32_t tag_value = ((unsigned long)test_code >> off_bits) |
                         (3UL << ICACHE_TAGBITS); // set valid and lock
    ecc_code_calc = ECC_GenerateECCCodeU32(tag_value);

    /* ------------- ICache Tag RAM single bit error test ------------- */
    sgl_ecc_code = gen_ecc_err_code(SINGLEBIT_ERROR_MASK, ecc_code_calc);
    ECC_ICacheTRamErrInject(sgl_ecc_code, (void *)test_code);
    test_code();

    if (ECC_IsICacheSingleBitErrorOccured()) {
        ECC_ClearICacheSingleBitError();
        printf("ECC single bit error has occured on ICache Tag RAM!\r\n");
    } else {
        printf("[ERROR]: ECC single bit error hasn't occured on ICache Tag RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of ICache Tag RAM single bit error test ---------- */


    /* ------------- ICache Tag RAM double bit error test ------------- */
    dbl_ecc_code = gen_ecc_err_code(DOUBLEBIT_ERROR_MASK, ecc_code_calc);
    inst_acc_fault_flag = 0;
    ECC_ICacheTRamErrInject(dbl_ecc_code, (void *)test_code);
    ECC_EnableICacheECCExcp();
    test_code();

    if (inst_acc_fault_flag == 1) {
        printf("ECC double bit error has occured on ICache Tag RAM!\r\n");
    } else {
        printf("[ERROR]: ECC double bit error hasn't occured on ICache Tag RAM!\r\n");
        return EXIT_FAILURE;
    }

    ECC_ICacheErrRestore((void *)test_code);
    ECC_EnableICacheECCExcp();
    test_code();

    if (inst_acc_fault_flag == 2) {
        printf("[ERROR]: ECC double bit error occured again on ICache Tag RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of ICache Tag RAM double bit error test ---------- */

    return EXIT_SUCCESS;
}

static int32_t dc_ecc_err_inj_demo()
{
    if (!ECC_IsDCacheSupportECC()) {
        printf("DCache not present or DCache not support ECC, will not run DCache ECC demo.\r\n");
        return EXIT_SUCCESS;
    }
    test_mem_type = ECC_MEM_TYPE_DCACHE;

    ECC_EnableDCacheECC();
    ECC_EnableDCacheECCCheck();

    /* ------------- Calculate DCache Data RAM golden ecc code ------------- */
    uint32_t data = *(const uint32_t *)test_data;
    uint8_t ecc_code_calc = ECC_GenerateECCCodeU32(data);

    /* ------------- DCache Data RAM single bit error test ------------- */
    uint8_t sgl_ecc_code = gen_ecc_err_code(SINGLEBIT_ERROR_MASK, ecc_code_calc);
    ECC_DCacheDRamErrInject(sgl_ecc_code, test_data);
    __LW(test_data);
    __RWMB();

    if (ECC_IsDCacheSingleBitErrorOccured()) {
        ECC_ClearDCacheSingleBitError();
        printf("ECC single bit error has occured on DCache Data RAM!\r\n");
    } else {
        printf("[ERROR]: ECC single bit error hasn't occured on DCache Data RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of DCache Data RAM single bit error test ---------- */

    /* ------------- DCache Data RAM double bit error test ------------- */
    uint8_t dbl_ecc_code = gen_ecc_err_code(DOUBLEBIT_ERROR_MASK, ecc_code_calc);
    load_fault_flag = 0;
    ECC_DCacheDRamErrInject(dbl_ecc_code, test_data);
    ECC_EnableDCacheECCExcp();
    __LW(test_data);
    __RWMB();

    if (load_fault_flag == 1) {
        printf("ECC double bit error has occured on DCache Data RAM!\r\n");
    } else {
        printf("[ERROR]: ECC double bit error hasn't occured on DCache Data RAM!\r\n");
        return EXIT_FAILURE;
    }

    ECC_DCacheErrRestore(test_data);
    ECC_EnableDCacheECCExcp();
    __LW(test_data);
    __RWMB();

    if (load_fault_flag == 2) {
        printf("[ERROR]: ECC double bit error occured again on DCache Data RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of DCache Data RAM double bit error test ---------- */

    /* ------------- Calculate DCache TAG RAM golden ecc code ------------- */
    /**
     * Tag value structure: total 32 bits
     * high -> low
     * +------------------------------------------------------------------------------------------+
     * | 0 extended | 1bit: shared | 1bit: lock | 1bit: dirty | 1bit: valid | tag_bits: tag_value |
     * +------------------------------------------------------------------------------------------+
     */
    uint32_t tag_bits = DCACHE_TAGBITS;
    uint32_t off_bits = pa_size - tag_bits;
    uint32_t tag_value = ((unsigned long)test_data >> off_bits) |
                         (5UL << tag_bits); // set valid and lock
    ecc_code_calc = ECC_GenerateECCCodeU32(tag_value);

    /* ------------- DCache Tag RAM single bit error test ------------- */
    sgl_ecc_code = gen_ecc_err_code(SINGLEBIT_ERROR_MASK, ecc_code_calc);
    ECC_DCacheTRamErrInject(sgl_ecc_code, test_data);
    __LW(test_data);
    __RWMB();

    if (ECC_IsDCacheSingleBitErrorOccured()) {
        ECC_ClearDCacheSingleBitError();
        printf("ECC single bit error has occured on DCache Tag RAM!\r\n");
    } else {
        printf("[ERROR]: ECC single bit error hasn't occured on DCache Tag RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of DCache Tag RAM single bit error test ---------- */

    /* ------------- DCache Tag RAM double bit error test ------------- */
    dbl_ecc_code = gen_ecc_err_code(DOUBLEBIT_ERROR_MASK, ecc_code_calc);
    load_fault_flag = 0;
    ECC_DCacheTRamErrInject(dbl_ecc_code, test_data);
    ECC_EnableDCacheECCExcp();
    __LW(test_data);
    __RWMB();

    if (load_fault_flag == 1) {
        printf("ECC double bit error has occured on DCache Tag RAM!\r\n");
    } else {
        printf("[ERROR]: ECC double bit error hasn't occured on DCache Tag RAM!\r\n");
        return EXIT_FAILURE;
    }

    ECC_DCacheErrRestore(test_data);
    ECC_EnableDCacheECCExcp();
    __LW(test_data);
    __RWMB();

    if (load_fault_flag == 2) {
        printf("[ERROR]: ECC double bit error occured again on DCache Tag RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of DCache Tag RAM double bit error test ---------- */

    return EXIT_SUCCESS;
}

static int32_t __attribute__((aligned(4096))) cc_ecc_err_inj_demo()
{
    if (!SMPCC_IsCCacheSupportECC()) {
        printf("CCache not support ECC, will not run CCache ECC demo.\r\n");
        return EXIT_SUCCESS;
    }
    test_mem_type = ECC_MEM_TYPE_CCACHE;

    SMPCC_EnableCCache();
    SMPCC_EnableCCacheECC();
    SMPCC_EnableCCacheECCCheck();

    /* ------------- Calculate CCache Data RAM golden ecc code ------------- */
    uint64_t data = *(const uint64_t *)test_data;
    uint8_t ecc_code_calc = ECC_GenerateECCCodeU64(data);

    /* ------------- CCache Data RAM single bit error test ------------- */
    uint8_t sgl_ecc_code = gen_ecc_err_code(SINGLEBIT_ERROR_MASK, ecc_code_calc);
    SMPCC_CCacheDramErrInject(sgl_ecc_code, test_data);
    __LW(test_data);
    __RWMB();

    if (SMPCC_GetRecvErrCount()) {
        SMPCC_ClearRecvErrCount();
        printf("ECC single bit error has occured on CCache Data RAM!\r\n");
    } else {
        printf("[ERROR]: ECC single bit error hasn't occured on CCache Data RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of CCache Data RAM single bit error test ---------- */

    /* ------------- CCache Data RAM double bit error test ------------- */
    uint8_t dbl_ecc_code = gen_ecc_err_code(DOUBLEBIT_ERROR_MASK, ecc_code_calc);
    load_fault_flag = 0;
    SMPCC_ClearFatalErrCount();
    SMPCC_CCacheDramErrInject(dbl_ecc_code, test_data);
    SMPCC_EnableCCacheECCExcp();
    __LW(test_data);
    __RWMB();

    if (load_fault_flag == 1) {
        printf("ECC double bit error has occured on CCache Data RAM!\r\n");
    } else {
        printf("[ERROR]: ECC double bit error hasn't occured on CCache Data RAM!\r\n");
        return EXIT_FAILURE;
    }

    SMPCC_CCacheErrRestore(test_data);
    SMPCC_EnableCCacheECCExcp();
    __LW(test_data);
    __RWMB();

    if (load_fault_flag == 2) {
        printf("[ERROR]: ECC double bit error occured again on CCache Data RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of CCache Data RAM double bit error test ---------- */

    /* ------------- Calculate CCache TAG RAM golden ecc code ------------- */
    /**
     * Tag value structure: total 32 bits
     * high -> low
     * +---------------------------------------------------------------------------+
     * | 0 extended | 1bit: dirty | 1bit: lock | tag_bits: tag_value | 1bit: valid |
     * +---------------------------------------------------------------------------+
     */
    uint32_t tag_bits = DCACHE_TAGBITS;
    uint32_t off_bits = pa_size - tag_bits;
    uint32_t tag_value =
        1U | ((unsigned long)test_data >> (off_bits - 1)) |
        (3U << (tag_bits + 1)); // set valid, lock and dirty bits to 1
    ecc_code_calc = ECC_GenerateECCCodeU32(tag_value);

    /* ------------- CCache Tag RAM single bit error test ------------- */
    sgl_ecc_code = gen_ecc_err_code(SINGLEBIT_ERROR_MASK, ecc_code_calc);
    SMPCC_ClearRecvErrCount();
    SMPCC_CCacheTramErrInject(sgl_ecc_code, test_data);
    __LW(test_data);
    __RWMB();

    if (SMPCC_GetRecvErrCount()) {
        SMPCC_ClearRecvErrCount();
        printf("ECC single bit error has occured on CCache Tag RAM!\r\n");
    } else {
        printf("[ERROR]: ECC single bit error hasn't occured on CCache Tag RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of CCache Tag RAM single bit error test ---------- */

    /* ------------- CCache Tag RAM double bit error test ------------- */
    dbl_ecc_code = gen_ecc_err_code(DOUBLEBIT_ERROR_MASK, ecc_code_calc);
    load_fault_flag = 0;
    SMPCC_ClearFatalErrCount();
    SMPCC_CCacheTramErrInject(dbl_ecc_code, test_data);
    SMPCC_EnableCCacheECCExcp();
    __LW(test_data);
    __RWMB();

    if (load_fault_flag == 1) {
        printf("ECC double bit error has occured on CCache Tag RAM!\r\n");
    } else {
        printf("[ERROR]: ECC double bit error hasn't occured on CCache Tag RAM!\r\n");
        return EXIT_FAILURE;
    }

    SMPCC_CCacheErrRestore(test_code);
    SMPCC_EnableCCacheECCExcp();
    test_code();

    if (load_fault_flag == 2) {
        printf("[ERROR]: ECC double bit error occured again on CCache Tag RAM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of CCache Tag RAM double bit error test ---------- */

    return EXIT_SUCCESS;
}
#endif /* #if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1) */

static int32_t ilm_ecc_err_inj_demo()
{
    if (!ECC_IsILMSupportECC()) {
        return EXIT_SUCCESS;
    }
    test_mem_type = ECC_MEM_TYPE_ILM;

    ECC_EnableILMECC();
    ECC_EnableILMECCCheck();

    /* ------------- Calculate ILM golden ecc code ------------- */
#if __RISCV_XLEN == 32
        uint32_t code = *(const uint32_t *)test_code;
        uint8_t ecc_code_calc = ECC_GenerateECCCodeU32(code);
#elif __RISCV_XLEN == 64
        uint64_t code = *(const uint64_t *)test_code;
        uint8_t ecc_code_calc = ECC_GenerateECCCodeU64(code);
#endif

    /* ------------- ILM single bit error test ------------- */
    uint8_t sgl_ecc_code = gen_ecc_err_code(SINGLEBIT_ERROR_MASK, ecc_code_calc);
    ECC_ILMErrInject(sgl_ecc_code, (void *)test_code);
    test_code();

    if (ECC_IsILMSingleBitErrorOccured()) {
        ECC_ClearILMSingleBitError();
        printf("ECC single bit error has occured on ILM!\r\n");
    } else {
        printf("[ERROR]: ECC single bit error hasn't occured on ILM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of ILM single bit error test ---------- */

    /* ------------- ILM double bit error test ------------- */
    uint8_t dbl_ecc_code = gen_ecc_err_code(DOUBLEBIT_ERROR_MASK, ecc_code_calc);
    inst_acc_fault_flag = 0;
    ECC_ILMErrInject(dbl_ecc_code, (void *)test_code);
    ECC_EnableILMECCExcp();
    test_code();

    if (inst_acc_fault_flag == 1) {
        printf("ECC double bit error has occured on ILM!\r\n");
    } else {
        printf("[ERROR]: ECC double bit error hasn't occured on ILM!\r\n");
        return EXIT_FAILURE;
    }

    ECC_ILMErrRestore(test_code);
    ECC_EnableILMECCExcp();
    test_code();

    if (inst_acc_fault_flag == 2) {
        printf("[ERROR]: ECC double bit error occured again on ILM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of ILM double bit error test ---------- */

    return EXIT_SUCCESS;
}

static int32_t dlm_ecc_err_inj_demo() {
    if (!ECC_IsDLMSupportECC()) {
        return EXIT_SUCCESS;
    }
    test_mem_type = ECC_MEM_TYPE_DLM;

    ECC_EnableDLMECC();
    ECC_EnableDLMECCCheck();

    /* ------------- Calculate DLM golden ecc code ------------- */
    uint32_t data = *(const uint32_t *)test_data;
    uint8_t ecc_code_calc = ECC_GenerateECCCodeU32(data);

    /* ------------- DLM single bit error test ------------- */
    uint8_t sgl_ecc_code = gen_ecc_err_code(SINGLEBIT_ERROR_MASK, ecc_code_calc);
    ECC_DLMErrInject(sgl_ecc_code, test_data);
    __LW(test_data);
    __RWMB();

    if (ECC_IsDLMSingleBitErrorOccured()) {
        ECC_ClearDLMSingleBitError();
        printf("ECC single bit error has occured on DLM!\r\n");
    } else {
        printf("[ERROR]: ECC single bit error hasn't occured on DLM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of DLM single bit error test ---------- */

    /* ------------- DLM double bit error test ------------- */
    uint8_t dbl_ecc_code = gen_ecc_err_code(DOUBLEBIT_ERROR_MASK, ecc_code_calc);
    load_fault_flag = 0;
    ECC_DLMErrInject(dbl_ecc_code, test_data);
    ECC_EnableDLMECCExcp();
    __LW(test_data);
    __RWMB();

    if (load_fault_flag == 1) {
        printf("ECC double bit error has occured on DLM!\r\n");
    } else {
        printf("[ERROR]: ECC double bit error hasn't occured on DLM!\r\n");
        return EXIT_FAILURE;
    }

    ECC_DLMErrRestore(test_data);
    ECC_EnableDLMECCExcp();
    __LW(test_data);
    __RWMB();

    if (load_fault_flag == 2) {
        printf("[ERROR]: ECC double bit error occured again on DLM!\r\n");
        return EXIT_FAILURE;
    }
    /* ---------- end of DLM double bit error test ---------- */

    return EXIT_SUCCESS;
}

static int32_t run_ecc_err_inj_demo(int32_t cc_demo)
{
    int32_t res = EXIT_SUCCESS;
    switch (DOWNLOAD_MODE) {
        case DOWNLOAD_MODE_SRAM:
        case DOWNLOAD_MODE_DDR:
            if (IINFO->ecc_inj_way & (1U << 31)) {
                printf("[WARNING]: Precise ECC injection is not supported!\r\n");
                break;
            }
#if defined(__CCM_PRESENT) && (__CCM_PRESENT == 1)
            setup_nc_region();
            res |= ic_ecc_err_inj_demo();
            res |= dc_ecc_err_inj_demo();
            if (cc_demo) {
                res |= cc_ecc_err_inj_demo();
            }
#else
            printf("__CCM_PRESENT is not defined or __CCM_PRESENT is 0!\r\n");
#endif
            break;
        case DOWNLOAD_MODE_ILM:
            res |= ilm_ecc_err_inj_demo();
            res |= dlm_ecc_err_inj_demo();
            break;
        case DOWNLOAD_MODE_FLASH:
        case DOWNLOAD_MODE_FLASHXIP:
        default:
            printf("Unsupported download mode! Only sram/ddr/ilm is supported.\r\n");
            break;
    }
    return res;
}

int main(void)
{
    int32_t res = EXIT_SUCCESS;
    int32_t cc_ecc_support = SMPCC_IsCCachePresent() && SMPCC_IsCCacheSupportECC();

    if (ECC_IsGlobalSupportECC()) {
        pa_size = *(uint32_t *)__IINFO_MPASIZE_ADDR;
        /* Check PA size read from hardware is equal to the CPU PA size */
        if (pa_size != __CPU_PA_SIZE) {
            printf("[WARNING]: CPU PA size is not equal to the CFG_PA_SIZE\r\n");
        }

        printf("ECC supported:\r\n");
        printf("    ICache ECC: %s\r\n", ECC_IsICacheSupportECC() ? "yes" : "no");
        printf("    DCache ECC: %s\r\n", ECC_IsDCacheSupportECC() ? "yes" : "no");
        printf("       ILM ECC: %s\r\n", ECC_IsILMSupportECC() ? "yes" : "no");
        printf("       DLM ECC: %s\r\n", ECC_IsDLMSupportECC() ? "yes" : "no");
        printf("    CCache ECC: %s\r\n", cc_ecc_support ? "yes" : "no");

        /**
         * Register instruction access fault handler for the following situations:
         *  - Fetch instruction from ILM failed and caused by ECC error
         *  - Fetch instruction from ICache failed and caused by ECC error
         *  - Fetch instruction from CCache failed and caused by ECC error
         */
        Exception_Register_EXC(InsAccFault_EXCn, (unsigned long)inst_access_fault_handler);
        /**
         * Register load data fault handler for the following situations:
         *  - Load data from DLM failed and caused by ECC error
         *  - Load data from DCache failed and caused by ECC error
         *  - Load data from CCache failed and caused by ECC error
         */
        Exception_Register_EXC(LdFault_EXCn, (unsigned long)load_fault_handler);

        /* Check if the ECC Error Code injection mode from CPU CFG is valid */
        ECC_ERR_INJ_MODE probed_mode = ECC_IsXorErrorInjectMode() ?
            ECC_ERR_INJ_MODE_XOR : ECC_ERR_INJ_MODE_DIRECT_WRITE;

        /* Run ECC Error Code injection demo */
        if (inj_mode != probed_mode) {
            printf("[WARNING]: ECC Error Code injection mode mismatch!\r\n");
            printf("The configured ecc mode is : %s\r\n", inj_mode == ECC_ERR_INJ_MODE_NOT_EXIST ?
                   "Not exist" : (inj_mode == ECC_ERR_INJ_MODE_XOR ? "XOR" : "Direct Write"));
            printf("Force run ecc demo with probed ecc mode: %s\r\n",
                   probed_mode == ECC_ERR_INJ_MODE_XOR ? "XOR"
                                                       : "Direct Write");
            inj_mode = probed_mode;
        }
        res = run_ecc_err_inj_demo(cc_ecc_support);
    } else {
        printf("ECC feature is not supported!\r\n");
    }
    if (res == EXIT_SUCCESS) {
        printf("ECC demo passed.\r\n");
    } else {
        printf("ECC demo failed.\r\n");
    }
    return 0;
}
