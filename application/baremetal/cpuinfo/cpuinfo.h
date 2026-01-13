#ifndef __CPUINFO_H__
#define __CPUINFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* IREGION Offsets */
#define CPUINFO_IRG_IINFO_OFS (0x0)
#define CPUINFO_IRG_DEBUG_OFS (0x10000)
#define CPUINFO_IRG_ECLIC_OFS (0x20000)
#define CPUINFO_IRG_TIMER_OFS (0x30000)
#define CPUINFO_IRG_SMP_OFS (0x40000)
#define CPUINFO_IRG_IDU_OFS (0x50000)
#define CPUINFO_IRG_PL2_OFS (0x60000)
#define CPUINFO_IRG_DPREFETCH_OFS (0x70000)
#define CPUINFO_IRG_PLIC_OFS (0x4000000)

/* NOTE: The CSR register length various between 32-bit and 64-bit
 * but usually the effective data is in the lower 32 bits. So we
 * use 32-bit data type to represent most of these registers. */
typedef uint32_t u32_csr_t;
typedef uint64_t u64_csr_t;

/**
 * \brief  Union type to access MCFG_INFO CSR register.
 */
typedef union {
    struct {
        u32_csr_t tee:1;                         /*!< bit: 0 TEE present */
        u32_csr_t ecc:1;                         /*!< bit: 1 ECC present */
        u32_csr_t eclic:1;                       /*!< bit: 2 ECLIC present */
        u32_csr_t plic:1;                        /*!< bit: 3 PLIC present */
        u32_csr_t fio:1;                         /*!< bit: 4 FIO present */
        u32_csr_t ppi:1;                         /*!< bit: 5 PPI present */
        u32_csr_t nice:1;                        /*!< bit: 6 NICE present */
        u32_csr_t ilm:1;                         /*!< bit: 7 ILM present */
        u32_csr_t dlm:1;                         /*!< bit: 8 DLM present */
        u32_csr_t icache:1;                      /*!< bit: 9 ICache present */
        u32_csr_t dcache:1;                      /*!< bit: 10 DCache present */
        u32_csr_t smp:1;                         /*!< bit: 11 SMP present */
        u32_csr_t dsp_n1:1;                      /*!< bit: 12 DSP N1 present */
        u32_csr_t dsp_n2:1;                      /*!< bit: 13 DSP N2 present */
        u32_csr_t dsp_n3:1;                      /*!< bit: 14 DSP N3 present */
        u32_csr_t zc_xlcz:1;                     /*!< bit: 15 Zc or Zc and xlcz extension present */
        u32_csr_t iregion:1;                     /*!< bit: 16 IREGION present */
        u32_csr_t vpu_degree:2;                  /*!< bit: 17..18 Indicate the VPU degree of parallel */
        u32_csr_t sec_mode:1;                    /*!< bit: 19 Smwg extension present */
        u32_csr_t etrace:1;                      /*!< bit: 20 Etrace present */
        u32_csr_t safety_mecha:2;                /*!< bit: 21..22 Indicate Core's safety mechanism */
        u32_csr_t vnice:1;                       /*!< bit: 23 VNICE present */
        u32_csr_t xlcz:1;                        /*!< bit: 24 XLCZ extension present */
        u32_csr_t zilsd:1;                       /*!< bit: 25 Zilsd/Zclsd extension present */
        u32_csr_t sstc:1;                        /*!< bit: 26 SSTC extension present */
        u32_csr_t :5;                            /*!< bit: 27..31 Reserved */
    } b;                                         /*!< Structure used for bit access */
    u32_csr_t d;                                 /*!< Type      used for csr data access */
} U32_CSR_MCFG_INFO_Type;

/**
 * \brief  Union type to access MICFG_INFO CSR register.
 */
typedef union {
    struct {
        u32_csr_t set:4;                         /*!< bit: 0..3 I-Cache sets per way */
        u32_csr_t way:3;                         /*!< bit: 4..6 I-Cache way */
        u32_csr_t lsize:3;                       /*!< bit: 7..9 I-Cache line size */
        u32_csr_t ecc:1;                         /*!< bit: 10   I-Cache ECC support */
        u32_csr_t :5;                            /*!< bit: 11..15 Reserved */
        u32_csr_t lm_size:5;                     /*!< bit: 16..20 ILM size, need to be 2^n size */
        u32_csr_t lm_xonly:1;                    /*!< bit: 21 ILM Execute only permission or Reserved */
        u32_csr_t lm_ecc:1;                      /*!< bit: 22 ILM ECC support */
        u32_csr_t :9;                            /*!< bit: 23..31 Reserved */
    } b;                                         /*!< Structure used for bit access */
    u32_csr_t d;                                 /*!< Type      used for csr data access */
} U32_CSR_MICFG_INFO_Type;

/**
 * \brief  Union type to access MDCFG_INFO CSR register.
 */
typedef union {
    struct {
        u32_csr_t set:4;                         /*!< bit: 0..3 D-Cache sets per way */
        u32_csr_t way:3;                         /*!< bit: 4..6 D-Cache way */
        u32_csr_t lsize:3;                       /*!< bit: 7..9 D-Cache line size */
        u32_csr_t ecc:1;                         /*!< bit: 10   D-Cache ECC support */
        u32_csr_t :5;                            /*!< bit: 11..15 Reserved */
        u32_csr_t lm_size:5;                     /*!< bit: 16..20 DLM size, need to be 2^n size */
        u32_csr_t lm_ecc:1;                      /*!< bit: 21 DLM ECC present */
        u32_csr_t :10;                           /*!< bit: 22..31 Reserved */
    } b;                                         /*!< Structure used for bit access */
    u32_csr_t d;                                 /*!< Type      used for csr data access */
} U32_CSR_MDCFG_INFO_Type;

/**
 * \brief  Union type to access MTLBCFG_INFO CSR register.
 */
typedef union {
    struct {
        u32_csr_t set:4;                         /*!< bit: 0..3  Main TLB entry per way */
        u32_csr_t way:3;                         /*!< bit: 4..6  Main TLB ways */
        u32_csr_t lsize:3;                       /*!< bit: 7..9  Main TLB line size or Reserved */
        u32_csr_t ecc:1;                         /*!< bit: 10  Main TLB supports ECC or not */
        u32_csr_t napot:1;                       /*!< bit: 11  TLB supports Svnapot or not */
        u32_csr_t :4;                            /*!< bit: 12..15  Reserved */
        u32_csr_t i_size:3;                      /*!< bit: 16..18  ITLB size */
        u32_csr_t d_size:3;                      /*!< bit: 19..21  DTLB size */
        u32_csr_t :9;                            /*!< bit: 22..30  Reserved */
        u32_csr_t mapping:1;                     /*!< bit: 31  TLB mapping type */
    } b;                                         /*!< Structure used for bit access */
    struct {
        u32_csr_t set:4;                         /*!< bit: 0..3  Main TLB entry per way */
        u32_csr_t way:3;                         /*!< bit: 4..6  Main TLB ways */
        u32_csr_t lsize:3;                       /*!< bit: 7..9  Main TLB line size or Reserved */
        u32_csr_t ecc:1;                         /*!< bit: 10  Main TLB supports ECC or not */
        u32_csr_t napot:1;                       /*!< bit: 11  TLB supports Svnapot or not */
        u32_csr_t i_size:7;                      /*!< bit: 12..18  ITLB size */
        u32_csr_t d_size:8;                      /*!< bit: 19..26  DTLB size */
        u32_csr_t :4;                            /*!< bit: 27..30  Reserved */
        u32_csr_t mapping:1;                     /*!< bit: 31  mapping type */
    } nb;                                        /*!< Structure used for bit access */
    u32_csr_t d;                                 /*!< Type      used for csr data access */
} U32_CSR_MTLBCFG_INFO_Type;

/**
 * \brief  Union type to access MARCHID CSR register.
 */
typedef union {
    struct {
        u32_csr_t last_name:4;                   /*!< bit: 0..3   the last series name */
        u32_csr_t third_name:4;                  /*!< bit: 4..7   the third series name */
        u32_csr_t second_name:4;                 /*!< bit: 8..11  the second series name */
        u32_csr_t first_name:4;                  /*!< bit: 12..15 the first series name */
        u32_csr_t :16;                           /*!< bit: 16..31 Reserved */
    } b;                                         /*!< Structure used for bit access */
    u32_csr_t d;                                 /*!< Type      used for csr data access */
} U32_CSR_MARCHID_Type;

/**
 * \brief  Union type to access MIMPID CSR register.
 */
typedef union {
    struct {
        u32_csr_t last_vernum:8;                 /*!< bit: 0..7   the last version number */
        u32_csr_t mid_vernum:8;                  /*!< bit: 8..15  the middle version number */
        u32_csr_t first_vernum:8;                /*!< bit: 16..23 the first version number */
        u32_csr_t :8;                            /*!< bit: 24..31 Reserved */
    } b;                                         /*!< Structure used for bit access */
    u32_csr_t d;                                 /*!< Type      used for csr data access */
} U32_CSR_MIMPID_Type;

/**
 * \brief  Union type to access low 26 bits of MISA CSR register.
 *
 */
typedef union {
    struct {
        u32_csr_t A:1;                           /*!< bit: 0  Atomic extension */
        u32_csr_t B:1;                           /*!< bit: 1  B extension */
        u32_csr_t C:1;                           /*!< bit: 2  Compressed exteionsion */
        u32_csr_t D:1;                           /*!< bit: 3  Double-precision floating-point extension */
        u32_csr_t E:1;                           /*!< bit: 4  RV32E/64E base ISA */
        u32_csr_t F:1;                           /*!< bit: 5  Single-precision floating-point extension */
        u32_csr_t G:1;                           /*!< bit: 6  Reserved */
        u32_csr_t H:1;                           /*!< bit: 7  Hypervisor extension */
        u32_csr_t I:1;                           /*!< bit: 8  RV32I/64I/128I base ISA */
        u32_csr_t J:1;                           /*!< bit: 9  Reserved */
        u32_csr_t K:1;                           /*!< bit: 10 Reserved */
        u32_csr_t L:1;                           /*!< bit: 11 Reserved */
        u32_csr_t M:1;                           /*!< bit: 12 Integer Multiply/Divide extension */
        u32_csr_t N:1;                           /*!< bit: 13 Tentatively reserved for User-Level Interrupts extension */
        u32_csr_t O:1;                           /*!< bit: 14 Reserved */
        u32_csr_t P:1;                           /*!< bit: 15 Tentatively reserved for Packed-SIMD extension */
        u32_csr_t Q:1;                           /*!< bit: 16 Quad-precision floating-point extension */
        u32_csr_t R:1;                           /*!< bit: 17 Reserved */
        u32_csr_t S:1;                           /*!< bit: 18 Supervisor mode implemented */
        u32_csr_t T:1;                           /*!< bit: 19 Reserved */
        u32_csr_t U:1;                           /*!< bit: 20 User mode implemented */
        u32_csr_t V:1;                           /*!< bit: 21 Vector extension */
        u32_csr_t W:1;                           /*!< bit: 22 Reserved */
        u32_csr_t X:1;                           /*!< bit: 23 Non-standard extension present */
        u32_csr_t Y:1;                           /*!< bit: 24 Reserved */
        u32_csr_t Z:1;                           /*!< bit: 25 Reserved */
        u32_csr_t :6;                            /*!< bit: 26..31 Reserved */
    } b;                                         /*!< Structure used for bit access */
    u32_csr_t d;                                 /*!< Type      used for csr data access */
} U32_CSR_MISA_Type;

/**
 * \brief  Union type to access MIRGB_INFO CSR register.
 */
typedef union {
    struct {
        u64_csr_t :1;                            /*!< bit: 0 Reserved */
        u64_csr_t iregion_size:5;                /*!< bit: 1..5 IREGION size */
        u64_csr_t :4;                            /*!< bit: 6..9 Reserved */
        u64_csr_t iregion_base:54;               /*!< bit: 10..63 IREGION base */
    } b;                                         /*!< Structure used for bit access */
    u64_csr_t d;                                 /*!< Type      used for csr data access */
} U64_CSR_MIRGB_INFO_Type;

/**
 * \brief  Union type to access MPPICFG_INFO CSR register.
 */
typedef union {
    struct {
        u64_csr_t :1;                            /*!< bit: 0 Reserved */
        u64_csr_t ppi_size:5;                    /*!< bit: 1..5 PPI size */
        u64_csr_t :3;                            /*!< bit: 6..8 Reserved */
        u64_csr_t :1;                            /*!< bit: 9 PPI Enable */
        u64_csr_t ppi_base:54;                   /*!< bit: 10..63 PPI base */
    } b;                                         /*!< Structure used for bit access */
    u64_csr_t d;                                 /*!< Type      used for csr data access */
} U64_CSR_MPPICFG_INFO_Type;

/**
 * \brief  Union type to access MFIOCFG_INFO CSR register.
 */
typedef union {
    struct {
        u64_csr_t :1;                            /*!< bit: 0 Reserved */
        u64_csr_t fio_size:5;                    /*!< bit: 1..5 FIO size */
        u64_csr_t :4;                            /*!< bit: 6..9 Reserved */
        u64_csr_t fio_base:54;                   /*!< bit: 10..63 FIO base */
    } b;                                         /*!< Structure used for bit access */
    u64_csr_t d;                                 /*!< Type      used for csr data access */
} U64_CSR_MFIOCFG_INFO_Type;

/* IREGION INFO Memory-Mapped Register Type*/
typedef struct {
    uint32_t mpasize;                            /*!< offset 0x0000 */
    uint32_t cmo_info;                           /*!< offset 0x0004 */
    uint32_t sec_base_addr_lo;                   /*!< offset 0x0008 */
    uint32_t sec_base_addr_hi;                   /*!< offset 0x000C */
    uint32_t sec_cfg_info;                       /*!< offset 0x0010 */
    uint32_t reserved0[4];                       /*!< offset 0x0014 */
    uint32_t mvlm_cfg_lo;                        /*!< offset 0x0024 */
    uint32_t mvlm_cfg_hi;                        /*!< offset 0x0028 */
    uint32_t flash_base_addr_lo;                 /*!< offset 0x002C */
    uint32_t flash_base_addr_hi;                 /*!< offset 0x0030 */
    uint32_t reserved1[7];                       /*!< offset 0x0034 */
    uint32_t vpu_cfg_info;                       /*!< offset 0x0050 */
    uint32_t mem_region0_cfg_lo;                 /*!< offset 0x0054 */
    uint32_t mem_region0_cfg_hi;                 /*!< offset 0x0058 */
    uint32_t mem_region1_cfg_lo;                 /*!< offset 0x005c */
    uint32_t mem_region1_cfg_hi;                 /*!< offset 0x0060 */
    uint32_t reserved2[3];                       /*!< offset 0x0064 */
    uint32_t isa_support0;                       /*!< offset 0x0070 */
    uint32_t isa_support1;                       /*!< offset 0x0074 */
    uint32_t reserved3[2];                       /*!< offset 0x0078 */
    uint32_t mcppi_cfg_lo;                       /*!< offset 0x0080 */
    uint32_t mcppi_cfg_hi;                       /*!< offset 0x0084 */
    uint32_t reserved4[2];                       /*!< offset 0x0088 */
    uint32_t performance_cfg0;                   /*!< offset 0x0090 */
    uint32_t performance_cfg1;                   /*!< offset 0x0094 */
    uint32_t reserved5[26];                      /*!< offset 0x0098 */
    uint32_t pfl1dctrl1;                         /*!< offset 0x0100 */
    uint32_t pfl1dctrl2;                         /*!< offset 0x0104 */
    uint32_t mergel1dctrl;                       /*!< offset 0x0108 */
    uint32_t reserved6[2];                       /*!< offset 0x010C */
    uint32_t access_ctrl;                        /*!< offset 0x0114 */
    uint32_t reserved7[2];                       /*!< offset 0x0118 */
    uint32_t pfl1dctrl3;                         /*!< offset 0x0120 */
    uint32_t pfl1dctrl4;                         /*!< offset 0x0124 */
    uint32_t pfl1info;                           /*!< offset 0x0128 */
    uint32_t reserved8[27];                      /*!< offset 0x012C */
    uint32_t crc_rf0;                            /*!< offset 0x0198 */
    uint32_t crc_rf1;                            /*!< offset 0x019C */
    uint32_t crc_fp0;                            /*!< offset 0x01A0 */
} CIF_IINFO_Type;

/**
 * \brief  Union type to access SMP_CFG register.
 */
typedef union {
    struct {
        u32_csr_t cc:1;                          /*!< bit: 0 Cluster Cache present */
        u32_csr_t smp_core_num:6;                /*!< bit: 1..6 smp core number */
        u32_csr_t iocp_num:6;                    /*!< bit: 7..12 IO Coherency port number */
        u32_csr_t pmon_num:6;                    /*!< bit: 13..18 performance monitor number */
        u32_csr_t :13;                           /*!< bit: 19..31 Reserved 0 */
    } b;                                         /*!< Structure used for bit access */
    u32_csr_t d;                                 /*!< Type      used for register data access */
} U32_SMP_CFG_Type;

/**
 * \brief  Union type to access CC_CFG register.
 */
typedef union {
    struct {
        u32_csr_t set:4;                         /*!< bit: 0..3 Cluster cache set number */
        u32_csr_t way:4;                         /*!< bit: 4..7 Cluster cache way number */
        u32_csr_t lsize:3;                       /*!< bit: 8..10 Cluster cache line size */
        u32_csr_t ecc:1;                         /*!< bit: 11   Cluster cache ECC support */
        u32_csr_t tcycle:3;                      /*!< bit: 12..14 Tag ram access cycle */
        u32_csr_t dcycle:3;                      /*!< bit: 15..17 Data ram access cycle */
        u32_csr_t :14;                           /*!< bit: 18..31 Reserved */
    } b;                                         /*!< Structure used for bit access */
    u32_csr_t d;                                 /*!< Type      used for register data access */
} U32_CC_CFG_Type;

/**
 * \brief  Union type to access ECLIC_INFO register.
 */
typedef union {
    struct {
        u32_csr_t num_interrupt:13;              /*!< bit: 0..12 interrupt source number */
        u32_csr_t version:8;                     /*!< bit: 13..20 version number */
        u32_csr_t clicintctlbits:4;              /*!< bit: 21..24 clicintctl register bit-width */
        u32_csr_t :7;                            /*!< bit: 25..31 Reserved 0 */
    } b;                                         /*!< Structure used for bit access */
    u32_csr_t d;                                 /*!< Type      used for register data access */
} U32_ECLIC_INFO_Type;

/**
 * \brief Access to the structure of ECLIC Memory Map, which is compatible with TEE.
 */
typedef struct {
    uint8_t cfg;                                 /*!< Offset: 0x000 (R/W)  CLIC configuration register */
    uint8_t reserved0[3];
    U32_ECLIC_INFO_Type info;                    /*!< Offset: 0x004 (R/ )  CLIC information register */
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t mth;                                 /*!< Offset: 0x00B(R/W)  CLIC machine mode interrupt-level threshold */
} CIF_ECLIC_Type;

/**
 * \brief  CPU INFO Structure
 */
typedef struct {
    U32_CSR_MARCHID_Type marchid;
    uint32_t mhartid;
    U32_CSR_MIMPID_Type mimpid;
    U32_CSR_MISA_Type misa;
    uint32_t mcfg_exist;
    U32_CSR_MCFG_INFO_Type mcfginfo;
    U32_CSR_MICFG_INFO_Type micfginfo;
    U32_CSR_MDCFG_INFO_Type mdcfginfo;
    U32_CSR_MTLBCFG_INFO_Type mtlbcfginfo;
    U64_CSR_MIRGB_INFO_Type mirgbinfo;
    uint64_t iregion_base;
    U64_CSR_MPPICFG_INFO_Type mppicfginfo;
    U64_CSR_MFIOCFG_INFO_Type mfiocfginfo;
    U32_SMP_CFG_Type smpcfg;
    U32_CC_CFG_Type cccfg;
    uint32_t xlen;
    uint32_t vlenb;
    CIF_IINFO_Type *iinfo;                           /*!< IREGION INFO memory pointer */
    CIF_ECLIC_Type *eclic;                           /*!< ECLIC memory pointer */
} CPU_INFO_Group;

/**
 * \brief  Union type to access MCMO_INFO register.
 */
typedef union {
    struct {
        uint32_t cmo_cfg:1;                      /*!< bit: 0 CMO exist */
        uint32_t cmo_pft:1;                      /*!< bit: 1 CMO has prefetch */
        uint32_t cmo_size:4;                     /*!< bit: 2..5 cache block size */
        uint32_t cbozero_size:4;                 /*!< bit: 6..9 cache block size of cbo.zero */
        uint32_t :22;                            /*!< bit: 10..31 Reserved 0 */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_MCMO_INFO_Type;

/**
 * \brief  Union type to access MVLM_CFG_LO register.
 */
typedef union {
    struct {
        uint32_t vlm:1;                          /*!< bit: 0 whether VLM configuration exist */
        uint32_t vlm_size:5;                     /*!< bit: 1..5 VLM size */
        uint32_t :4;                             /*!< bit: 6..9 reserved */
        uint32_t vlm_base_lo:22;                 /*!< bit: 10..31 VLM base address */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_MVLM_CFG_LO_Type;

typedef uint32_t CIF_IINFO_MVLM_CFG_HI_Type;

/**
 * \brief  Union type to access ISA_SUPPORT0 register.
 */
typedef union {
    struct {
        uint32_t exist:1;                        /*!< bit: 0 whether this register is valid */
        uint32_t vector:1;                       /*!< bit: 1 support Vector extension */
        uint32_t vector_b:1;                     /*!< bit: 2 support Vector-B extension */
        uint32_t vector_k:1;                     /*!< bit: 3 support Vector-K extension */
        uint32_t smepmp:1;                       /*!< bit: 4 support Smepmp extension */
        uint32_t sscofpmf:1;                     /*!< bit: 5 support Sscofpmf extension */
        uint32_t zfh:1;                          /*!< bit: 6 support Zfh extension */
        uint32_t zfhmin:1;                       /*!< bit: 7 support Zfhmin extension */
        uint32_t zfa:1;                          /*!< bit: 8 support Zfa extension */
        uint32_t svnapot:1;                      /*!< bit: 9 support Svnapot extension */
        uint32_t svpbmt:1;                       /*!< bit: 10 support Svpbmt extension */
        uint32_t svinval:1;                      /*!< bit: 11 support Svinval extension */
        uint32_t bf16:1;                         /*!< bit: 12 support Bf16 extension */
        uint32_t zve32x:1;                       /*!< bit: 13 support Zve32x extension */
        uint32_t zve32f:1;                       /*!< bit: 14 support Zve32f extension */
        uint32_t zve64x:1;                       /*!< bit: 15 support Zve64x extension */
        uint32_t zve64f:1;                       /*!< bit: 16 support Zve64f extension */
        uint32_t zve64d:1;                       /*!< bit: 17 support Zve64d extension */
        uint32_t zimop:1;                        /*!< bit: 18 support Zimop extension */
        uint32_t zcmop:1;                        /*!< bit: 19 support Zomop extension */
        uint32_t zicond:1;                       /*!< bit: 20 support Zicond extension */
        uint32_t zihintntl:1;                    /*!< bit: 21 support Zihintntl extension */
        uint32_t zihintpause:1;                  /*!< bit: 22 support Zihintpause extension */
        uint32_t zvfh:1;                         /*!< bit: 23 support Zvfh extension */
        uint32_t zvfhmin:1;                      /*!< bit: 24 support Zvfhmin extension */
        uint32_t smrnmi:1;                       /*!< bit: 25 support Smrnmi extension */
        uint32_t zihpm:1;                        /*!< bit: 26 support Zihpm extension */
        uint32_t smcntrpmf:1;                    /*!< bit: 27 support Smcntrpmf extension */
        uint32_t zicntr:1;                       /*!< bit: 28 support Zicntr extension */
        uint32_t zawrs:1;                        /*!< bit: 29 support Zawrs extension */
        uint32_t :2;                             /*!< bit: 30..31 reserved */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_ISA_SUPPORT0_Type;

/**
 * \brief  Union type to access ISA_SUPPORT1 register.
 */
typedef union {
    struct {
        uint32_t exist:1;                        /*!< bit: 0 whether this register is valid */
        uint32_t ssqosid:1;                      /*!< bit: 1 support Ssqosid extension */
        uint32_t zicflip:1;                      /*!< bit: 2 support zicflip extension */
        uint32_t zicfiss:1;                      /*!< bit: 3 support zicfiss extension */
        uint32_t smctr:1;                        /*!< bit: 4 support Smctr extension */
        uint32_t zacas:1;                        /*!< bit: 5 support Zacas extension */
        uint32_t zabha:1;                        /*!< bit: 6 support Zabha extension */
        uint32_t smdbltrp:1;                     /*!< bit: 7 support Smdbltrp extension */
        uint32_t ssdbltrp:1;                     /*!< bit: 8 support Ssdbltrp extension */
        uint32_t smcdeleg:1;                     /*!< bit: 9 support Smcdeleg extension */
        uint32_t smmpm:1;                        /*!< bit: 10 support Smmpm extension */
        uint32_t smnpm:1;                        /*!< bit: 11 support Smnpm extension */
        uint32_t ssnpm:1;                        /*!< bit: 12 support Smnpm extension */
        uint32_t smstateen:1;                    /*!< bit: 13 support Smstateen extension */
        uint32_t sstateen:1;                     /*!< bit: 14 support Sstateen extension */
        uint32_t smcsrind:1;                     /*!< bit: 15 support Smcsrind extension */
        uint32_t sscsrind:1;                     /*!< bit: 16 support Sscsrind extension */
        uint32_t svadu:1;                        /*!< bit: 17 support Svadu extension */
        uint32_t :14;                            /*!< bit: 18..31 reserved */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_ISA_SUPPORT1_Type;

/**
 * \brief  Union type to access PERFORMANCE_CFG0 register.
 */
typedef union {
    struct {
        uint32_t exist:1;                        /*!< bit: 0 whether this register is valid */
        uint32_t fpu_cycle:5;                    /*!< bit: 1..5 fpu cycle count */
        uint32_t high_div:1;                     /*!< bit: 6 high performance divider */
        uint32_t dcache_2stage:1;                /*!< bit: 7 dcache 2 stage */
        uint32_t delay_branch_flush:1;           /*!< bit: 8 delay branch flash */
        uint32_t bus_type:3;                     /*!< bit: 9..11 memory bus protocol type */
        uint32_t dual_issue:1;                   /*!< bit: 12 dual issue */
        uint32_t cross_4k:1;                     /*!< bit: 13 cross 4k */
        uint32_t dlm_2stage:1;                   /*!< bit: 14 DLM 2 stage */
        uint32_t lsu_cut_fwd:1;                  /*!< bit: 15 LSU cut forwarding */
        uint32_t dsp_cycle:4;                    /*!< bit: 16..19 DSP cycle */
        uint32_t ifu_cut_timing:1;               /*!< bit: 20 IFU cut timing */
        uint32_t mem_cut_timing:1;               /*!< bit: 21 MEM cut timing */
        uint32_t dcache_prefetch:1;              /*!< bit: 22 dcache prefetch */
        uint32_t dcache_lbuf_num:5;              /*!< bit: 23..27 dcache line buffer number */
        uint32_t mul_cyc:3;                      /*!< bit: 28..30 multiplier cycle */
        uint32_t :1;                             /*!< bit: 31 reserved */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_PERFORMANCE_CFG0_Type;

/**
 * \brief  Union type to access PERFORMANCE_CFG1 register.
 */
typedef union {
    struct {
        uint32_t exist:1;                        /*!< bit: 0 whether this register is valid */
        uint32_t vfpu_cyc:5;                     /*!< bit: 1..5 Vector FPU cycle */
        uint32_t bht_entry_width:5;              /*!< bit: 6..10 BHT entry width */
        uint32_t high_performance:1;             /*!< bit: 11 high performance */
        uint32_t agu_quick_forward:1;            /*!< bit: 12 agu quick forward */
        uint32_t cau_fwd:1;                      /*!< bit: 13 crypto instruction forward */
        uint32_t hpm_ver:2;                      /*!< bit: 14..15 HPM version */
        uint32_t :16;                            /*!< bit: 16..31 reserved */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_PERFORMANCE_CFG1_Type;

/**
 * \brief  Union type to access PFL1INFO register.
 */
typedef union {
    struct {
        uint32_t pf_cfg:8;                       /*!< bit: 0..7 prefetch configuration */
        uint32_t l2_pf_lbuf_num:8;               /*!< bit: 8..15 L2 prefetch req address buf number */
        uint32_t l2_pf_dbuf_num:8;               /*!< bit: 16..23 L2 prefetch data buf number */
        uint32_t pf_ver:8;                       /*!< bit: 24..31 prefetch version */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_PFL1INFO_Type;

/**
 * \brief  Union type to access PFL1DCTRL1 register.
 */
typedef union {
    struct {
        uint32_t l1d_ena:1;                      /*!< bit: 0 L1 dcache prefetch enable */
        uint32_t cc_ena:1;                       /*!< bit: 1 cluster cache prefetch enable */
        uint32_t scalar_ena:1;                   /*!< bit: 2 scalar pipeline prefetch enable */
        uint32_t vector_ena:1;                   /*!< bit: 3 vector pipeline prefetch enable */
        uint32_t write_pref_ena:1;               /*!< bit: 4 store prefetch enable */
        uint32_t cross_page_pref_ena:1;          /*!< bit: 5 prefetch cross page enable */
        uint32_t :2;                             /*!< bit: 6..7 reserved */
        uint32_t pref_conflict_stop_th:4;        /*!< bit: 8..11 prefetch inc counter stop threshold */
        uint32_t pref_conflict_decr_sel:3;       /*!< bit: 12..14 prefetch num to dec counter */
        uint32_t :17;                            /*!< bit: 15..31 reserved */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_PFL1DCTRL1_Type;

/**
 * \brief  Union type to access PFL1DCTRL2 register.
 */
typedef union {
    struct {
        uint32_t degree_incr_th:6;               /*!< bit: 0..5 prefetch degree threshold of incr */
        uint32_t degree_decr_th:6;               /*!< bit: 6..11 prefetch degree threshold of decr */
        uint32_t next_line_ena_th:4;             /*!< bit: 12..15 next line enable threshold */
        uint32_t write_noalloc_l1_th:2;          /*!< bit: 16..17 see ISA doc */
        uint32_t write_noalloc_l2_th:2;          /*!< bit: 18..19 see ISA doc */
        uint32_t :12;                            /*!< bit: 20..31 reserved */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_PFL1DCTRL2_Type;

/**
 * \brief  Union type to access PFL1DCTRL3 register.
 */
typedef union {
    struct {
        uint32_t max_stream_l1_degree:4;         /*!< bit: 0..3 mat stream prefetch L1 degree */
        uint32_t :1;                             /*!< bit: 4 reserved */
        uint32_t max_stream_l2_degree:7;         /*!< bit: 5..11 next line enable threshold */
        uint32_t :4;                             /*!< bit: 12..15 reserved */
        uint32_t max_stride_cplx_l1_degree:4;    /*!< bit: 16..19 the max stride/cplx pref L1 degree */
        uint32_t :1;                             /*!< bit: 20 reserved */
        uint32_t max_stride_cplx_l2_degree:7;    /*!< bit: 21..27 the max stride/cplx pref L2 degree */
        uint32_t :4;                             /*!< bit: 28..31 reserved */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_PFL1DCTRL3_Type;

/**
 * \brief  Union type to access MERGEL1DCTRL register.
 */
typedef union {
    struct {
        uint32_t ws_tmout_max:12;                /*!< bit: 0..11 write streaming tmout cnt max value */
        uint32_t :4;                             /*!< bit: 12..15 reserved */
        uint32_t nc_tmout_max:8;                 /*!< bit: 16..23 non-cacheable tmout cnt max value */
        uint32_t dev_store_early_ret: 1;         /*!< bit: 24 whether device region store is blocking */
        uint32_t :7;                             /*!< bit: 25..31 reserved */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_MERGEL1DCTRL_Type;

/**
 * \brief  Union type to access ACCESS_CTRL register.
 */
typedef union {
    struct {
        uint32_t :1;                             /*!< bit: 0 reversed */
        uint32_t pf_access: 1;                   /*!< bit: 1 s-mode register access, 0 is enable */
        uint32_t cache_csr_access: 1;            /*!< bit: 2 s-mode cache csr access */
        uint32_t pma_csr_access: 1;              /*!< bit: 3 s-mode pma csr access */
        uint32_t :28;                            /*!< bit: 4..31 reserved */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_ACCESS_CTRL_Type;

/**
 * \brief  Union type to access FLASH_BASE_ADDR_LO register.
 */
typedef union {
    struct {
        uint32_t flash:1;                        /*!< bit: 0 whether flash bus configuration exist */
        uint32_t flash_size:5;                   /*!< bit: 1..5 flash size */
        uint32_t :4;                             /*!< bit: 6..9 reserved */
        uint32_t flash_base_lo:22;               /*!< bit: 10..31 flash base address low */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_FLASH_BASE_ADDR_LO_Type;

typedef uint32_t CIF_IINFO_FLASH_BASE_ADDR_HI_Type;

/**
 * \brief  Union type to access MEM_REGION_CFG_LO register.
 */
typedef union {
    struct {
        uint32_t exist:1;                        /*!< bit: 0 whether mem_region_cfg exist */
        uint32_t mem_region_size:5;              /*!< bit: 1..5 mem region size */
        uint32_t :3;                             /*!< bit: 6..8 reserved */
        uint32_t mem_region_ena: 1;              /*!< bit: 9 enable mem region or not */
        uint32_t mem_region_base_lo:22;          /*!< bit: 10..31 mem region base address low */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_MEM_REGION_CFG_LO_Type;

typedef uint32_t CIF_IINFO_MEM_REGION_CFG_HI_Type;

/**
 * \brief  Union type to access MCPPI_CFG_LO register.
 */
typedef union {
    struct {
        uint32_t exist:1;                        /*!< bit: 0 whether cppi_cfg exist */
        uint32_t cppi_size:5;                    /*!< bit: 1..5 cppi size */
        uint32_t :3;                             /*!< bit: 6..8 reserved */
        uint32_t cppi_ena: 1;                    /*!< bit: 9 enable cppi or not */
        uint32_t cppi_base_lo:22;                /*!< bit: 10..31 cppi base address low */
    } b;                                         /*!< Structure used for bit access */
    uint32_t d;                                  /*!< Type      used for register data access */
} CIF_IINFO_MCPPI_CFG_LO_Type;

typedef uint32_t CIF_IINFO_MCPPI_CFG_HI_Type;

/**
 * \brief  Nuclei CPU INFO Structure For OpenOCD Usage
 */
typedef struct {
    CPU_INFO_Group cpuinfo;
    CIF_IINFO_Type iinfo;
    CIF_ECLIC_Type eclic;
} NUCLEI_CPUINFO;

/**
 * \brief Show full CPU information about ISA, ILD/DLM, I/D cache, etc.
 * \param cpuinfo: pointer to CPU_INFO_Group
 */
void show_cpuinfo(const CPU_INFO_Group *cpuinfo);

/**
 * \brief Get basic CPU information in a single line.
 *        This function is **not reentrant** because it uses
 *        a shared static buffer.
 * \param cpuinfo: pointer to CPU_INFO_Group
 * \param str: pointer to string buffer
 * \param len: length of string buffer
 * \return actual length of result string or '-1' for `str == NULL`,
 *         it is same as the return value of `snprintf`
 */
int get_basic_cpuinfo(const CPU_INFO_Group *cpuinfo, char *str, unsigned long len);

#ifdef __cplusplus
}
#endif
#endif /* #ifndef __IREGION_INFO_H__ */
