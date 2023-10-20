#include <stdio.h>
#include "nuclei_sdk_soc.h"

#define KB                  (1024)
#define MB                  (KB * 1024)
#define GB                  (MB * 1024)
#define EXTENSION_NUM       (26)
#define POWER_FOR_TWO(n)    (1UL << (n))

void print_size(rv_csr_t bytes)
{
    if (bytes / GB) {
        printf(" %ld GB", bytes / GB);
    } else if (bytes / MB) {
        printf(" %ld MB", bytes / MB);
    } else if (bytes / KB) {
        printf(" %ld KB", bytes / KB);
    } else {
        printf(" %ld Byte", bytes);
    }
}

void show_cache_info(rv_csr_t set, rv_csr_t way, rv_csr_t lsize)
{
    print_size(set * way * lsize);
    printf("(set=%ld,", set);
    printf("way=%ld,", way);
    printf("lsize=%ld)\r\n", lsize);
}

void nuclei_cpuinfo(void)
{
    CSR_MCFGINFO_Type mcfg;
    CSR_MICFGINFO_Type micfg;
    CSR_MDCFGINFO_Type mdcfg;
    rv_csr_t iregion_base = 0;
    rv_csr_t csr_marchid = 0;
    rv_csr_t csr_mimpid = 0;
    rv_csr_t csr_misa = 0;
    rv_csr_t csr_mirgb = 0;
    rv_csr_t csr_mfiocfg = 0;
    rv_csr_t csr_mppicfg = 0;
    rv_csr_t csr_mtlbcfg = 0;

    printf("\r\n-----Nuclei RISC-V CPU Configuration Information-----\r\n");

    /* ID and version */
    csr_marchid = __RV_CSR_READ(CSR_MARCHID);
    csr_mimpid = __RV_CSR_READ(CSR_MIMPID);
    printf("         MARCHID: 0x%lx\r\n", csr_marchid);
    printf("          MIMPID: 0x%lx\r\n", csr_mimpid);

    /* ISA */
    csr_misa = __RV_CSR_READ(CSR_MISA);
    printf("             ISA:");
#if __RISCV_XLEN == 32
    printf(" RV32");
#else
    printf(" RV64");
#endif
    for (int i = 0; i < EXTENSION_NUM; i++) {
        if (csr_misa & BIT(i)) {
            if ('X' == ('A' + i)) {
                printf(" Zc Xxlcz");
            } else {
                printf(" %c", 'A' + i);
            }
        }
    }
    printf("\r\n");

    /* Support */
    mcfg = (CSR_MCFGINFO_Type)__RV_CSR_READ(CSR_MCFG_INFO);
    printf("            MCFG:");
    if (mcfg.b.tee) {
        printf(" TEE");
    }
    if (mcfg.b.ecc) {
        printf(" ECC");
    }
    if (mcfg.b.clic) {
        printf(" ECLIC");
    }
    if (mcfg.b.plic) {
        printf(" PLIC");
    }
    if (mcfg.b.fio) {
        printf(" FIO");
    }
    if (mcfg.b.ppi) {
        printf(" PPI");
    }
    if (mcfg.b.nice) {
        printf(" NICE");
    }
    if (mcfg.b.ilm) {
        printf(" ILM");
    }
    if (mcfg.b.dlm) {
        printf(" DLM");
    }
    if (mcfg.b.icache) {
        printf(" ICACHE");
    }
    if (mcfg.b.dcache) {
        printf(" DCACHE");
    }
    if (mcfg.d & BIT(11)) {
        printf(" SMP");
    }
    if (mcfg.d & BIT(12)) {
        printf(" DSP-N1");
    }
    if (mcfg.d & BIT(13)) {
        printf(" DSP-N2");
    }
    if (mcfg.d & BIT(14)) {
        printf(" DSP-N3");
    }
    if (mcfg.d & BIT(16)) {
        printf(" IREGION");
    }
    printf("\r\n");

    /* ILM */
    if (mcfg.b.ilm) {
        micfg = (CSR_MICFGINFO_Type)__RV_CSR_READ(CSR_MICFG_INFO);
        printf("             ILM:");
        print_size(POWER_FOR_TWO(micfg.b.lm_size - 1) * 256);
        if (micfg.b.lm_xonly) {
            printf(" execute-only");
        }
        if (micfg.b.lm_ecc) {
            printf(" has-ecc");
        }
        printf("\r\n");
    }

    /* DLM */
    if (mcfg.b.dlm) {
        mdcfg = (CSR_MDCFGINFO_Type)__RV_CSR_READ(CSR_MDCFG_INFO);
        printf("             DLM:");
        print_size(POWER_FOR_TWO(mdcfg.b.lm_size - 1) * 256);
        if (mdcfg.b.lm_ecc) {
            printf(" has-ecc");
        }
        printf("\r\n");
    }

    /* ICACHE */
    if (mcfg.b.icache) {
        micfg = (CSR_MICFGINFO_Type)__RV_CSR_READ(CSR_MICFG_INFO);
        printf("          ICACHE:");
        show_cache_info(POWER_FOR_TWO(micfg.b.set + 3), micfg.b.way + 1, POWER_FOR_TWO(micfg.b.lsize + 2));
    }

    /* DCACHE */
    if (mcfg.b.dcache) {
        mdcfg = (CSR_MDCFGINFO_Type)__RV_CSR_READ(CSR_MDCFG_INFO);
        printf("          DCACHE:");
        show_cache_info(POWER_FOR_TWO(mdcfg.b.set + 3), mdcfg.b.way + 1, POWER_FOR_TWO(mdcfg.b.lsize + 2));
    }

    /* IREGION */
    if (mcfg.d & BIT(16)) {
        rv_csr_t csr_mirgb = __RV_CSR_READ(CSR_MIRGB_INFO);
        printf("         IREGION:");
        iregion_base = csr_mirgb & (~0x3FF);
        printf(" %#lx", iregion_base);
        print_size(POWER_FOR_TWO(__RV_EXTRACT_FIELD(csr_mirgb, 0xF << 1) - 1) * KB);
        printf("\r\n");
        printf("                  Unit        Size        Address\r\n");
        printf("                  INFO        64KB        %#lx\r\n", iregion_base + IREGION_IINFO_OFS);
        printf("                  DEBUG       64KB        %#lx\r\n", iregion_base + IREGION_DEBUG_OFS);
        if (mcfg.b.clic) {
            printf("                  ECLIC       64KB        %#lx\r\n", iregion_base + IREGION_ECLIC_OFS);
        }
        printf("                  TIMER       64KB        %#lx\r\n", iregion_base + IREGION_TIMER_OFS);
        if (mcfg.d & BIT(11)) {
            printf("                  SMP         64KB        %#lx\r\n", iregion_base + IREGION_SMP_OFS);
        }
        rv_csr_t smp_cfg = *(rv_csr_t*)(iregion_base + 0x40004);
        if (mcfg.b.clic && (__RV_EXTRACT_FIELD(smp_cfg, 0x1F << 1) >= 2)) {
            printf("                  CIDU        64KB        %#lx\r\n", iregion_base + IREGION_IDU_OFS);
        }
        if (mcfg.b.plic) {
            printf("                  PLIC        64MB        %#lx\r\n", iregion_base + IREGION_PLIC_OFS);
        }
        /* SMP */
        if (mcfg.d & BIT(11)) {
            printf("         SMP_CFG:");
            printf(" CC_PRESENT=%ld", __RV_EXTRACT_FIELD(smp_cfg, 0x1));
            printf(" SMP_CORE_NUM=%ld", __RV_EXTRACT_FIELD(smp_cfg, 0x1F << 1));
            printf(" IOCP_NUM=%ld", __RV_EXTRACT_FIELD(smp_cfg, 0x3F << 7));
            printf(" PMON_NUM=%ld", __RV_EXTRACT_FIELD(smp_cfg, 0x3F << 13));
            printf("\r\n");
        }
        /* ECLIC */
        if (mcfg.b.clic) {
            printf("         ECLIC:");
            printf(" VERSION=0x%x", (unsigned int)ECLIC_GetInfoVer());
            printf(" NUM_INTERRUPT=%u", (unsigned int)ECLIC_GetInfoNum());
            printf(" CLICINTCTLBITS=%u", (unsigned int)ECLIC_GetInfoCtlbits());
            printf(" MTH=%u", (unsigned int)ECLIC_GetMth());
            printf(" NLBITS=%u", (unsigned int)ECLIC_GetCfgNlbits());
            printf("\r\n");
        }

        /* L2CACHE */
        if (smp_cfg & BIT(1)) {
            rv_csr_t cc_cfg = *(rv_csr_t*)(iregion_base + 0x40008);
            printf("         L2CACHE:");
            show_cache_info(POWER_FOR_TWO(__RV_EXTRACT_FIELD(smp_cfg, 0xF)), __RV_EXTRACT_FIELD(smp_cfg, 0x7 << 4) + 1,
                            POWER_FOR_TWO(__RV_EXTRACT_FIELD(smp_cfg, 0x7 << 7) + 2));
        }
        /* INFO */
        printf("     INFO-Detail:\r\n");
        rv_csr_t mpasize = *(rv_csr_t*)(iregion_base);
        printf("                  mpasize : %lu\r\n", mpasize);
        rv_csr_t cmo_info = *(rv_csr_t*)(iregion_base + 4);
        if (cmo_info & BIT(1)) {
            printf("                  cbozero : %luByte\r\n", (unsigned long)POWER_FOR_TWO(__RV_EXTRACT_FIELD(cmo_info, 0xF << 6) + 2));
            printf("                  cmo     : %luByte\r\n", (unsigned long)POWER_FOR_TWO(__RV_EXTRACT_FIELD(cmo_info, 0xF << 2) + 2));
            if (cmo_info & BIT(2)) {
                printf("                  has_prefecth\r\n");
            }
        }
        rv_csr_t mcppi_cfg_lo = *(rv_csr_t*)(iregion_base + 0x80);
        rv_csr_t mcppi_cfg_hi = *(rv_csr_t*)(iregion_base + 0x84);
        if (mcppi_cfg_lo & 0x1) {
#if __RISCV_XLEN == 32
            printf("                  cppi    : %#lx", mcppi_cfg_lo & (~0x3FF));
#else
            printf("                  cppi    : %#lx", (mcppi_cfg_hi << 32) | (mcppi_cfg_lo & (~0x3FF)));
#endif
            print_size(POWER_FOR_TWO(__RV_EXTRACT_FIELD(cmo_info, 0xF << 1) - 1) * KB);
            printf("\r\n");
        }
    }

    /* TLB */
    if (mcfg.b.plic) {
        csr_mtlbcfg = __RV_CSR_READ(CSR_MTLBCFG_INFO);
        if (csr_mtlbcfg != -1) {
            printf("            DTLB: %lu entry\r\n", __RV_EXTRACT_FIELD(csr_mtlbcfg, 0x7 << 19));
            printf("            ITLB: %lu entry\r\n", __RV_EXTRACT_FIELD(csr_mtlbcfg, 0x7 << 16));
            printf("            MTLB:");
            printf(" %lu entry", POWER_FOR_TWO(__RV_EXTRACT_FIELD(csr_mtlbcfg, 0xF) + 3) *
                                    (__RV_EXTRACT_FIELD(csr_mtlbcfg, 0x7 << 4) + 1) *
                                    (__RV_EXTRACT_FIELD(csr_mtlbcfg, 0x7 << 7) - 1));
            if (csr_mtlbcfg & BIT(10)) {
                printf(" has_ecc");
            }
            printf("\r\n");
        }
    }

    /* FIO */
    if (mcfg.b.fio) {
        csr_mfiocfg = __RV_CSR_READ(CSR_MFIOCFG_INFO);
        printf("             FIO:");
        printf(" %#lx", csr_mfiocfg & (~0x3FF));
        print_size(POWER_FOR_TWO(__RV_EXTRACT_FIELD(csr_mfiocfg, 0xF << 1) - 1) * KB);
        printf("\r\n");
    }

    /* PPI */
    if (mcfg.b.ppi) {
        csr_mppicfg = __RV_CSR_READ(CSR_MPPICFG_INFO);
        printf("             PPI:");
        printf(" %#lx", csr_mppicfg & (~0x3FF));
        print_size(POWER_FOR_TWO(__RV_EXTRACT_FIELD(csr_mppicfg, 0xF << 1) - 1) * KB);
        printf("\r\n");
    }

    printf("-----End of Nuclei CPU INFO-----\r\n");
}

int main(void)
{
    nuclei_cpuinfo();

    return 0;
}
