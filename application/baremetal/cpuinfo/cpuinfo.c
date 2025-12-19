#include "cpuinfo.h"
#include "cpuinfo_cfg.h"

#include <string.h>

#define BIT(ofs) (0x1U << (ofs))
#define EXTENSION_NUM (26)
#define POW2(n) (1U << (n))
#define LINESZ(n) ((n) > 0U ? POW2((n) - 1) : 0)

/* Check register field with default field name print */
#define CHECK_FIELD_DFT(reg, field)                                            \
    if (reg.b.field) {                                                         \
        CIF_PRINTF(" %s", #field);                                             \
    }
/* Check register field with string specified */
#define CHECK_FIELD(reg, field, str)                                           \
    if (reg.b.field) {                                                         \
        CIF_PRINTF(" %s", str);                                                \
    }
/* Show register value */
#define SHOW_VALUE(reg, field)                                                 \
    CIF_PRINTF("                      %s=%u\r\n", #field, reg.b.field);

#define STRCAT_BUF(buf, fmt, ...)                                              \
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), fmt, ##__VA_ARGS__)

#define CHECK_STRCAT_BUF(reg, field, buf, fmt, ...)                            \
    do {                                                                       \
        if (reg.b.field) {                                                     \
            STRCAT_BUF(buf, fmt, ##__VA_ARGS__);                               \
        }                                                                      \
    } while (0)

#define BASIC_CPUINFO_FMT "Nuclei CPU Detected: mhartid-0x%x marchid-0x%04x v%d.%d.%d, ISA: RV%d%s"

/** `BUF_SIZE` is the size of string buffer in `get_basic_cpuinfo`
 */
#ifndef BUF_SIZE
#define BUF_SIZE (1024)
#endif

static void show_isa(uint32_t xlen, U32_CSR_MISA_Type misa,
                     U32_CSR_MCFG_INFO_Type mcfg);
static void show_mcfg(const CPU_INFO_Group *cpuinfo);
static void show_micfg_mdcfg(U32_CSR_MCFG_INFO_Type mcfg,
                             U32_CSR_MICFG_INFO_Type micfg,
                             U32_CSR_MDCFG_INFO_Type mdcfg);
static void show_mtlbcfg(U32_CSR_MCFG_INFO_Type mcfg,
                         U32_CSR_MTLBCFG_INFO_Type mtlbcfg);
static void show_iregion(const CPU_INFO_Group *cpuinfo);
static void show_mfiocfg(U32_CSR_MCFG_INFO_Type mcfg,
                         U64_CSR_MFIOCFG_INFO_Type mfiocfg);
static void show_mppicfg(U32_CSR_MCFG_INFO_Type mcfg,
                         U64_CSR_MPPICFG_INFO_Type mppicfg);

static void show_prefetch_cfg(IINFO_Type *iinfo);
static void show_isa_support(IINFO_Type *iinfo);
static void show_mvlm_cfg(IINFO_Type *iinfo);
static void show_flash_bus(IINFO_Type *iinfo);
static void show_mem_region_cfg(IINFO_Type *iinfo);
static void show_mcppi_cfg(IINFO_Type *iinfo);
static void show_cmo(IINFO_Type *iinfo);
static void show_performance_cfg(IINFO_Type *iinfo);
static void show_misc_cfg(IINFO_Type *iinfo);

/**
 * Convert to human readable size with option
 * \param size: size in bytes
 * \param lite: !=0 for lite version
 */
static char *cvt_size_opt(uint32_t size, int lite);
#define cvt_size(size) cvt_size_opt(size, 0)
static void show_cache_info(uint32_t set, uint32_t way, uint32_t lsize,
                            uint32_t ecc);

void show_cpuinfo(const CPU_INFO_Group *cpuinfo)
{
    if (cpuinfo == NULL) {
        return;
    }
    if (cpuinfo->misa.d == 0) {
        CIF_PRINTF("\r\n-----Invalid RISC-V MISA CSR found, no CPU Information could be dumped-----\r\n");
        return;
    }
    CIF_PRINTF("\r\n-----Nuclei RISC-V CPU Configuration Information-----\r\n");

    /* ID and version */
    CIF_PRINTF("         MARCHID: 0x%04x\r\n", cpuinfo->marchid.d);
    CIF_PRINTF("         MHARTID: 0x%x\r\n", cpuinfo->mhartid);
    CIF_PRINTF("          MIMPID: 0x%06x\r\n", cpuinfo->mimpid.d);

    /* ISA */
    show_isa(cpuinfo->xlen, cpuinfo->misa, cpuinfo->mcfginfo);
    /* Support */
    show_mcfg(cpuinfo);
    /* ILM, DLM, I/D Cache */
    show_micfg_mdcfg(cpuinfo->mcfginfo, cpuinfo->micfginfo, cpuinfo->mdcfginfo);
    /* TLB */
    show_mtlbcfg(cpuinfo->mcfginfo, cpuinfo->mtlbcfginfo);
    /* FIO */
    show_mfiocfg(cpuinfo->mcfginfo, cpuinfo->mfiocfginfo);
    /* PPI */
    show_mppicfg(cpuinfo->mcfginfo, cpuinfo->mppicfginfo);
    /* IREGION */
    show_iregion(cpuinfo);

    CIF_PRINTF("-----End of Nuclei CPU INFO-----\r\n");
}

int get_basic_cpuinfo(const CPU_INFO_Group *cpuinfo, char *str, unsigned long len)
{
    if (str == NULL || cpuinfo == NULL) {
        return -1;
    }
    if (cpuinfo->misa.d == 0) {
        return -1;
    }

    static char buf[BUF_SIZE] = {0}; // features string buffer
    buf[0] = '\0'; // clear the buffer each time call this function
    char isa[EXTENSION_NUM + 1];

    /* construct ISA string */
    int pos = 0;
    for (int i = 0; i < EXTENSION_NUM; ++i) {
        if (cpuinfo->misa.d & BIT(i)) {
            isa[pos++] = 'A' + i;
        }
    }
    isa[pos] = '\0';

    if (!cpuinfo->mcfg_exist) {
        goto simple;
    }

    /* construct features string */
    U32_CSR_MCFG_INFO_Type mcfg = cpuinfo->mcfginfo;
    CHECK_STRCAT_BUF(mcfg, plic, buf, "MMU, PLIC, ");
    CHECK_STRCAT_BUF(mcfg, eclic, buf, "ECLIC, ");
    CHECK_STRCAT_BUF(mcfg, fio, buf, "FIO, ");
    CHECK_STRCAT_BUF(mcfg, ppi, buf, "PPI, ");
    CHECK_STRCAT_BUF(mcfg, nice, buf, "NICE, ");
    CHECK_STRCAT_BUF(mcfg, vnice, buf, "VNICE, ");
    CHECK_STRCAT_BUF(mcfg, etrace, buf, "ETRACE, ");
    CHECK_STRCAT_BUF(mcfg, ecc, buf, "ECC, ");
    CHECK_STRCAT_BUF(mcfg, tee, buf, "TEE, ");
    CHECK_STRCAT_BUF(mcfg, sec_mode, buf, "SMWG, ");

    IINFO_ISA_SUPPORT0_Type isa_support0;
    isa_support0.d = cpuinfo->iinfo->isa_support0;
    CHECK_STRCAT_BUF(isa_support0, svpbmt, buf, "Svpbmt, ");

    IINFO_MCMO_INFO_Type cmo;
    cmo.d = cpuinfo->iinfo->cmo_info;
    CHECK_STRCAT_BUF(cmo, cmo_cfg, buf, "CMO, ");

    if (mcfg.b.smp) {
        STRCAT_BUF(buf, "SMPx%d, ", cpuinfo->smpcfg.b.smp_core_num + 1);
    }

    /* show local memory and cache info */
    U32_CSR_MICFG_INFO_Type micfg = cpuinfo->micfginfo;
    U32_CSR_MDCFG_INFO_Type mdcfg = cpuinfo->mdcfginfo;
    CHECK_STRCAT_BUF(mcfg, ilm, buf, "ILM-%s, ",
                     cvt_size_opt(POW2(micfg.b.lm_size + 7), 1));
    CHECK_STRCAT_BUF(mcfg, dlm, buf, "DLM-%s, ",
                     cvt_size_opt(POW2(mdcfg.b.lm_size + 7), 1));
    CHECK_STRCAT_BUF(
        mcfg, icache, buf, "IC-%s, ",
        cvt_size_opt(POW2(micfg.b.set + 3) * POW2(micfg.b.lsize + 2) *
                         (micfg.b.way + 1),
                     1));
    CHECK_STRCAT_BUF(
        mcfg, dcache, buf, "DC-%s, ",
        cvt_size_opt(POW2(mdcfg.b.set + 3) * POW2(mdcfg.b.lsize + 2) *
                         (mdcfg.b.way + 1),
                     1));

    /* remove the comma at the end */
    if (strlen(buf) > 0 && buf[strlen(buf) - 2] == ',') {
        buf[strlen(buf) - 2] = '\0';
    }
    /* Feature name must have at least 2 chars */
    if (strlen(buf) < 2) {
        goto simple;
    }

    return snprintf(str, len, BASIC_CPUINFO_FMT ", Feature: %s", cpuinfo->mhartid,
                    cpuinfo->marchid.d, cpuinfo->mimpid.b.first_vernum,
                    cpuinfo->mimpid.b.mid_vernum, cpuinfo->mimpid.b.last_vernum,
                    cpuinfo->xlen, isa, buf);
simple:
    return snprintf(str, len, BASIC_CPUINFO_FMT, cpuinfo->mhartid, cpuinfo->marchid.d,
                    cpuinfo->mimpid.b.first_vernum, cpuinfo->mimpid.b.mid_vernum,
                    cpuinfo->mimpid.b.last_vernum, cpuinfo->xlen, isa);
}

static void show_isa(uint32_t xlen, U32_CSR_MISA_Type misa,
                     U32_CSR_MCFG_INFO_Type mcfg)
{
    CIF_PRINTF("             ISA:");
    CIF_PRINTF(" RV%d", xlen);
    for (int i = 0; i < EXTENSION_NUM; i++) {
        if (misa.d & BIT(i)) {
            if ('X' == ('A' + i)) {
                CIF_PRINTF(" NICE");
            } else {
                CIF_PRINTF(" %c", 'A' + i);
            }
        }
    }
    CHECK_FIELD(mcfg, dsp_n1, "Xxldspn1x");
    CHECK_FIELD(mcfg, dsp_n2, "Xxldspn2x");
    CHECK_FIELD(mcfg, dsp_n3, "Xxldspn3x");
    if (mcfg.b.zc_xlcz) {
        if (mcfg.b.xlcz == 0) {
            CIF_PRINTF(" Zc Xxlcz");
        } else {
            CIF_PRINTF(" Zc");
        }
    }
    CHECK_FIELD(mcfg, zilsd, "Zilsd");
    CIF_PRINTF("\r\n");
}

static void show_mcfg(const CPU_INFO_Group *cpuinfo)
{
    if (!cpuinfo->mcfg_exist) {
        return;
    }

    U32_CSR_MCFG_INFO_Type mcfg = cpuinfo->mcfginfo;
    CIF_PRINTF("            MCFG:");
    CHECK_FIELD(mcfg, tee, "TEE")
    CHECK_FIELD(mcfg, ecc, "ECC")
    CHECK_FIELD(mcfg, eclic, "ECLIC")
    CHECK_FIELD(mcfg, plic, "PLIC")
    CHECK_FIELD(mcfg, fio, "FIO")
    CHECK_FIELD(mcfg, ppi, "PPI")
    CHECK_FIELD(mcfg, nice, "NICE")
    CHECK_FIELD(mcfg, ilm, "ILM")
    CHECK_FIELD(mcfg, dlm, "DLM")
    CHECK_FIELD(mcfg, icache, "ICACHE")
    CHECK_FIELD(mcfg, dcache, "DCACHE")
    CHECK_FIELD(mcfg, smp, "SMP")
    CHECK_FIELD(mcfg, iregion, "IREGION")
    CHECK_FIELD(mcfg, sec_mode, "SMWG")
    CHECK_FIELD(mcfg, etrace, "ETRACE")
    CHECK_FIELD(mcfg, vnice, "VNICE")
    CHECK_FIELD(mcfg, sstc, "SSTC")
    switch (mcfg.b.safety_mecha) {
        case 0b00:
            CIF_PRINTF(" No-Safety-Mechanism");
            break;
        case 0b01:
            CIF_PRINTF(" Lockstep");
            break;
        case 0b10:
            CIF_PRINTF(" Lockstep+SplitMode");
            break;
        case 0b11:
            CIF_PRINTF(" ASIL-B");
            break;
        default:
            break;
    }
    if (cpuinfo->misa.b.V) {
        switch (mcfg.b.vpu_degree) {
            case 0b00:
                CIF_PRINTF(" DLEN=VLEN/2");
                break;
            case 0b01:
                CIF_PRINTF(" DLEN=VLEN");
                break;
            default:
                break;
        }
    }
    CIF_PRINTF("\r\n");
}

static void show_micfg_mdcfg(U32_CSR_MCFG_INFO_Type mcfg,
                             U32_CSR_MICFG_INFO_Type micfg,
                             U32_CSR_MDCFG_INFO_Type mdcfg)
{
    /* ILM */
    if (mcfg.b.ilm) {
        CIF_PRINTF("             ILM:");
        CIF_PRINTF(" %s", cvt_size(POW2(micfg.b.lm_size + 7)));
        CHECK_FIELD(micfg, lm_xonly, "execute-only");
        CHECK_FIELD(micfg, lm_ecc, "has-ecc");
        CIF_PRINTF("\r\n");
    }
    /* DLM */
    if (mcfg.b.dlm) {
        CIF_PRINTF("             DLM:");
        CIF_PRINTF(" %s", cvt_size(POW2(mdcfg.b.lm_size + 7)));
        CHECK_FIELD(mdcfg, lm_ecc, "has-ecc");
        CIF_PRINTF("\r\n");
    }
    /* ICACHE */
    if (mcfg.b.icache) {
        CIF_PRINTF("          ICACHE:");
        show_cache_info(POW2(micfg.b.set + 3), micfg.b.way + 1,
                        POW2(micfg.b.lsize + 2), mcfg.b.ecc);
    }
    /* DCACHE */
    if (mcfg.b.dcache) {
        CIF_PRINTF("          DCACHE:");
        show_cache_info(POW2(mdcfg.b.set + 3), mdcfg.b.way + 1,
                        POW2(mdcfg.b.lsize + 2), mcfg.b.ecc);
    }
}

static void show_mtlbcfg(U32_CSR_MCFG_INFO_Type mcfg,
                         U32_CSR_MTLBCFG_INFO_Type mtlbcfg)
{
    /* TLB only present with MMU, when PLIC present MMU will present */
    if (mcfg.b.plic) {
        if (mtlbcfg.nb.mapping == 1) {
            CIF_PRINTF("             TLB:");
            CIF_PRINTF(" MainTLB(entry=%u,way=%u,ecc=%u)",
                       POW2(mtlbcfg.nb.set + 3), mtlbcfg.nb.way + 1,
                       mtlbcfg.nb.ecc);
            CIF_PRINTF(" ITLB(entry=%u) DTLB(entry=%u)\r\n",
                       mtlbcfg.nb.i_size + 1, mtlbcfg.nb.d_size + 1);
        } else {
            CIF_PRINTF("             TLB:");
            CIF_PRINTF(" MainTLB(entry=%u,way=%u,ecc=%u)",
                       POW2(mtlbcfg.b.set + 3), mtlbcfg.b.way + 1,
                       mtlbcfg.b.ecc);
            CIF_PRINTF(" ITLB(entry=%u) DTLB(entry=%u)\r\n",
                       LINESZ(mtlbcfg.b.i_size), LINESZ(mtlbcfg.b.d_size));
        }
    }
}

static void show_iregion(const CPU_INFO_Group *cpuinfo)
{
    U32_CSR_MCFG_INFO_Type mcfg = cpuinfo->mcfginfo;
    if (!mcfg.b.iregion) {
        return;
    }

    CIF_PRINTF("         IREGION:");
    U64_CSR_MIRGB_INFO_Type mirgb = cpuinfo->mirgbinfo;
    uint64_t iregion_base = cpuinfo->iregion_base;
    CIF_PRINTF(" %#" CIF_PRIxADDR "", (addr_t)iregion_base);
    CIF_PRINTF(" %s\r\n", cvt_size(POW2(mirgb.b.iregion_size + 9)));
    CIF_PRINTF("                  Unit        Size        Address\r\n");
    CIF_PRINTF("                  IINFO       64KB        0x%" CIF_PRIxADDR "\r\n",
               (addr_t)iregion_base + CPUINFO_IRG_IINFO_OFS);
    CIF_PRINTF("                  DEBUG       64KB        %#" CIF_PRIxADDR "\r\n",
               (addr_t)iregion_base + CPUINFO_IRG_DEBUG_OFS);
    if (mcfg.b.eclic) {
        CIF_PRINTF("                  ECLIC       64KB        %#" CIF_PRIxADDR "\r\n",
                   (addr_t)iregion_base + CPUINFO_IRG_ECLIC_OFS);
    }
    CIF_PRINTF("                  TIMER       64KB        %#" CIF_PRIxADDR "\r\n",
               (addr_t)iregion_base + CPUINFO_IRG_TIMER_OFS);
    if (mcfg.b.smp) {
        CIF_PRINTF("                  SMP         64KB        %#" CIF_PRIxADDR "\r\n",
                   (addr_t)iregion_base + CPUINFO_IRG_SMP_OFS);
    }
    U32_SMP_CFG_Type smp_cfg = cpuinfo->smpcfg;
    /* If has eclic and has equal or more than 1 core, CIDU will present
     * The actual core number is `smp_core_num + 1` */
    if (mcfg.b.eclic && (smp_cfg.b.smp_core_num >= 1)) {
        CIF_PRINTF("                  CIDU        64KB        %#" CIF_PRIxADDR "\r\n",
                   (addr_t)iregion_base + CPUINFO_IRG_IDU_OFS);
    }
    if (mcfg.b.plic) {
        CIF_PRINTF("                  PLIC        64MB        %#" CIF_PRIxADDR "\r\n",
                   (addr_t)iregion_base + CPUINFO_IRG_PLIC_OFS);
    }
    /* SMP */
    if (mcfg.b.smp) {
        CIF_PRINTF("         SMP_CFG:");
        CIF_PRINTF(" CC_PRESENT=%d", smp_cfg.b.cc);
        CIF_PRINTF(" SMP_NUM=%d", smp_cfg.b.smp_core_num + 1);
        CIF_PRINTF(" IOCP_NUM=%d", smp_cfg.b.iocp_num);
        CIF_PRINTF(" PMON_NUM=%d", smp_cfg.b.pmon_num);
        CIF_PRINTF("\r\n");
    }
    /* ECLIC */
    if (mcfg.b.eclic) {
        ECLIC_Type *eclic = cpuinfo->eclic;
        U32_ECLIC_INFO_Type eclic_info = eclic->info;
        CIF_PRINTF("           ECLIC:");
        CIF_PRINTF(" VERSION=0x%x", eclic_info.b.version);
        CIF_PRINTF(" NUM_INTERRUPT=%u", eclic_info.b.num_interrupt);
        CIF_PRINTF(" CLICINTCTLBITS=%u", eclic_info.b.clicintctlbits);
        CIF_PRINTF(" MTH=%u", eclic->mth);
        CIF_PRINTF(" NLBITS=%u", (eclic->cfg & 0x1E) >> 1);
        CIF_PRINTF("\r\n");
    }
    /* L2CACHE */
    if (smp_cfg.b.cc) {
        U32_CC_CFG_Type cc_cfg = cpuinfo->cccfg;
        CIF_PRINTF("         L2CACHE:");
        show_cache_info(POW2(cc_cfg.b.set), cc_cfg.b.way + 1,
                        POW2(cc_cfg.b.lsize + 2), cc_cfg.b.ecc);
    }

    // NOTE: mpasize always have, and is PA_SIZE of RTL configuration
    // if it is 0, it means it dont have IINFO region
    if (cpuinfo->iinfo->mpasize == 0) {
        return;
    }
    /* IREGION INFO */
    CIF_PRINTF("     INFO-Detail:\r\n");
    /* MPASIZE */
    uint32_t mpasize = cpuinfo->iinfo->mpasize;
    CIF_PRINTF("                  mpasize : %u\r\n", mpasize);
    /* prefetch related registers */
    show_prefetch_cfg(cpuinfo->iinfo);
    /* ISA_SUPPORT VPU_CFG_INFO */
    show_isa_support(cpuinfo->iinfo);
    /* MVLM_CFG */
    show_mvlm_cfg(cpuinfo->iinfo);
    /* FLASH_BASE_ADDR */
    show_flash_bus(cpuinfo->iinfo);
    /* MEM_REGION_CFG */
    show_mem_region_cfg(cpuinfo->iinfo);
    /* MCPPI_CFG */
    show_mcppi_cfg(cpuinfo->iinfo);
    /* CMO_INFO */
    show_cmo(cpuinfo->iinfo);
    /* PERFORMANCE_CFG */
    show_performance_cfg(cpuinfo->iinfo);
    /* MERGEL1DCTRL and ACCESS_CTRL */
    show_misc_cfg(cpuinfo->iinfo);
}

static void show_mfiocfg(U32_CSR_MCFG_INFO_Type mcfg,
                         U64_CSR_MFIOCFG_INFO_Type mfiocfg)
{
    if (mcfg.b.fio) {
        CIF_PRINTF("             FIO:");
        CIF_PRINTF(" %#" CIF_PRIxADDR "", (addr_t)((uint64_t)mfiocfg.d & (~0x3FFULL)));
        CIF_PRINTF(" %s\r\n", cvt_size(POW2(mfiocfg.b.fio_size + 9)));
    }
}

static void show_mppicfg(U32_CSR_MCFG_INFO_Type mcfg,
                         U64_CSR_MPPICFG_INFO_Type mppicfg)
{
    if (mcfg.b.ppi) {
        CIF_PRINTF("             PPI:");
        CIF_PRINTF(" %#" CIF_PRIxADDR "", (addr_t)((uint64_t)mppicfg.d & (~0x3FFULL)));
        CIF_PRINTF(" %s\r\n", cvt_size(POW2(mppicfg.b.ppi_size + 9)));
    }
}

static void show_prefetch_cfg(IINFO_Type *iinfo)
{
    IINFO_PFL1INFO_Type pfl1info;
    pfl1info.d = iinfo->pfl1info;
    if (pfl1info.b.pf_cfg) {
        CIF_PRINTF("                  prefetch: present\r\n");
        const char *pf_cfg[] = {"none", "normal", "high performance"};
        if (pfl1info.b.pf_cfg < 3) {
            CIF_PRINTF("                      prefetch_mode: %s\r\n",
                       pf_cfg[pfl1info.b.pf_cfg]);
        }
        if (iinfo->pfl1dctrl4 & BIT(0)) {
            CIF_PRINTF("                      status: enable\r\n");
        } else {
            CIF_PRINTF("                      status: disable\r\n");
        }
        if (pfl1info.b.pf_ver) {
            CIF_PRINTF("                      version=%u\r\n",
                       pfl1info.b.pf_ver);
        }

        /* prefetch config */
        IINFO_PFL1DCTRL1_Type pfl1dctrl1;
        IINFO_PFL1DCTRL2_Type pfl1dctrl2;
        IINFO_PFL1DCTRL3_Type pfl1dctrl3;
        pfl1dctrl1.d = iinfo->pfl1dctrl1;
        pfl1dctrl2.d = iinfo->pfl1dctrl2;
        pfl1dctrl3.d = iinfo->pfl1dctrl3;
        SHOW_VALUE(pfl1info, l2_pf_lbuf_num);
        SHOW_VALUE(pfl1info, l2_pf_dbuf_num);
        SHOW_VALUE(pfl1dctrl1, l1d_ena);
        SHOW_VALUE(pfl1dctrl1, cc_ena);
        SHOW_VALUE(pfl1dctrl1, scalar_ena);
        SHOW_VALUE(pfl1dctrl1, vector_ena);
        SHOW_VALUE(pfl1dctrl1, write_pref_ena);
        SHOW_VALUE(pfl1dctrl1, cross_page_pref_ena);
        SHOW_VALUE(pfl1dctrl1, pref_conflict_stop_th);
        SHOW_VALUE(pfl1dctrl1, pref_conflict_decr_sel);
        SHOW_VALUE(pfl1dctrl2, degree_incr_th);
        SHOW_VALUE(pfl1dctrl2, degree_decr_th);
        SHOW_VALUE(pfl1dctrl2, next_line_ena_th);
        SHOW_VALUE(pfl1dctrl2, write_noalloc_l1_th);
        SHOW_VALUE(pfl1dctrl2, write_noalloc_l2_th);
        SHOW_VALUE(pfl1dctrl3, max_stream_l1_degree);
        SHOW_VALUE(pfl1dctrl3, max_stream_l2_degree);
        SHOW_VALUE(pfl1dctrl3, max_stride_cplx_l1_degree);
        SHOW_VALUE(pfl1dctrl3, max_stride_cplx_l2_degree);
    } else {
        CIF_PRINTF("                  prefetch: absent\r\n");
    }
}

static void show_isa_support(IINFO_Type *iinfo)
{
    IINFO_ISA_SUPPORT0_Type isa_support0;
    IINFO_ISA_SUPPORT1_Type isa_support1;
    isa_support0.d = iinfo->isa_support0;
    isa_support1.d = iinfo->isa_support1;

    if (isa_support0.b.exist || isa_support1.b.exist) {
        CIF_PRINTF("                  isa supported: present\r\n");
        CIF_PRINTF("                      extension_list:");
        if (isa_support0.b.exist) {
            CHECK_FIELD_DFT(isa_support0, vector);
            CHECK_FIELD_DFT(isa_support0, smepmp);
            CHECK_FIELD_DFT(isa_support0, sscofpmf);
            CHECK_FIELD_DFT(isa_support0, zfh);
            CHECK_FIELD_DFT(isa_support0, zfhmin);
            CHECK_FIELD_DFT(isa_support0, zfa);
            CHECK_FIELD_DFT(isa_support0, svnapot);
            CHECK_FIELD_DFT(isa_support0, svpbmt);
            CHECK_FIELD_DFT(isa_support0, svinval);
            CHECK_FIELD_DFT(isa_support0, bf16);
            CHECK_FIELD_DFT(isa_support0, zimop);
            CHECK_FIELD_DFT(isa_support0, zcmop);
            CHECK_FIELD_DFT(isa_support0, zicond);
            CHECK_FIELD_DFT(isa_support0, zihintntl);
            CHECK_FIELD_DFT(isa_support0, zihintpause);
            CHECK_FIELD_DFT(isa_support0, smrnmi);
            CHECK_FIELD_DFT(isa_support0, zihpm);
            CHECK_FIELD_DFT(isa_support0, smcntrpmf);
            CHECK_FIELD_DFT(isa_support0, zicntr);
            CHECK_FIELD_DFT(isa_support0, zawrs);
        }
        if (isa_support1.b.exist) {
            CHECK_FIELD_DFT(isa_support1, ssqosid);
            CHECK_FIELD_DFT(isa_support1, zicflip);
            CHECK_FIELD_DFT(isa_support1, zicfiss);
            CHECK_FIELD_DFT(isa_support1, smctr);
            CHECK_FIELD_DFT(isa_support1, zacas);
            CHECK_FIELD_DFT(isa_support1, zabha);
            CHECK_FIELD_DFT(isa_support1, smdbltrp);
            CHECK_FIELD_DFT(isa_support1, ssdbltrp);
            CHECK_FIELD_DFT(isa_support1, smcdeleg);
            CHECK_FIELD_DFT(isa_support1, smmpm);
            CHECK_FIELD_DFT(isa_support1, smnpm);
            CHECK_FIELD_DFT(isa_support1, ssnpm);
            CHECK_FIELD_DFT(isa_support1, smstateen);
            CHECK_FIELD_DFT(isa_support1, sstateen);
            CHECK_FIELD_DFT(isa_support1, smcsrind);
            CHECK_FIELD_DFT(isa_support1, sscsrind);
            CHECK_FIELD_DFT(isa_support1, svadu);
        }
        CIF_PRINTF("\r\n");
    } else {
        CIF_PRINTF("                  isa supported: absent\r\n");
    }

    /* VPU related extensions */
    if (isa_support0.b.exist && isa_support0.b.vector) {
        CIF_PRINTF("                  vpu: present\r\n");
        CIF_PRINTF("                      vpu_extension_list:");
        CHECK_FIELD_DFT(isa_support0, vector_b);
        CHECK_FIELD_DFT(isa_support0, vector_k);
        CHECK_FIELD_DFT(isa_support0, zve32x);
        CHECK_FIELD_DFT(isa_support0, zve32f);
        CHECK_FIELD_DFT(isa_support0, zve64x);
        CHECK_FIELD_DFT(isa_support0, zve64f);
        CHECK_FIELD_DFT(isa_support0, zve64d);
        CHECK_FIELD_DFT(isa_support0, zvfh);
        CHECK_FIELD_DFT(isa_support0, zvfhmin);
        CIF_PRINTF("\r\n");

        uint32_t vpu_cfg_info = iinfo->vpu_cfg_info;
        CIF_PRINTF("                      noseg_noshuf=%u\r\n",
                   (unsigned int)(vpu_cfg_info & BIT(0)));
        CIF_PRINTF("                      elen64=%u\r\n",
                   (unsigned int)(vpu_cfg_info & BIT(1)) >> 1);
    } else {
        CIF_PRINTF("                  vpu: absent\r\n");
    }
}

static void show_mvlm_cfg(IINFO_Type *iinfo)
{
    IINFO_MVLM_CFG_LO_Type mvlm_cfg_lo;
    mvlm_cfg_lo.d = iinfo->mvlm_cfg_lo;
    if (mvlm_cfg_lo.b.vlm) {
        CIF_PRINTF("                  vlm: present\r\n");
        IINFO_MVLM_CFG_HI_Type mvlm_cfg_hi = iinfo->mvlm_cfg_hi;
        uint64_t vlm_base =
            (uint64_t)mvlm_cfg_hi << 32 | (mvlm_cfg_lo.d & (~0x3FFULL));
        CIF_PRINTF("                      base=0x%" CIF_PRIxADDR "\r\n", (addr_t)vlm_base);
        CIF_PRINTF("                      size=%s\r\n",
                   cvt_size(POW2(mvlm_cfg_lo.b.vlm_size + 9)));
    } else {
        CIF_PRINTF("                  vlm: absent\r\n");
    }
}

static void show_flash_bus(IINFO_Type *iinfo)
{
    IINFO_FLASH_BASE_ADDR_LO_Type addr_lo;
    addr_lo.d = iinfo->flash_base_addr_lo;
    if (addr_lo.b.flash) {
        CIF_PRINTF("                  flash bus: present\r\n");
        IINFO_FLASH_BASE_ADDR_HI_Type addr_hi = iinfo->flash_base_addr_hi;
        uint64_t flash_base =
            (uint64_t)addr_hi << 32 | (addr_lo.d & (~0x3FFULL));
        CIF_PRINTF("                      base=0x%" CIF_PRIxADDR "\r\n", (addr_t)flash_base);
        CIF_PRINTF("                      size=%s\r\n",
                   cvt_size(addr_lo.b.flash_size + 9));
    } else {
        CIF_PRINTF("                  flash bus: absent\r\n");
    }
}

static void show_mem_region_cfg(IINFO_Type *iinfo)
{
    IINFO_MEM_REGION_CFG_LO_Type region_lo;
    uint64_t region_base;
    region_lo.d = iinfo->mem_region0_cfg_lo;
    if (region_lo.b.exist) {
        CIF_PRINTF("                  mem_region0: present\r\n");
        IINFO_MEM_REGION_CFG_HI_Type region_hi = iinfo->mem_region0_cfg_hi;
        region_base = (uint64_t)region_hi << 32 | (region_lo.d & (~0x3FFULL));
        if (region_lo.b.mem_region_ena) {
            CIF_PRINTF("                      status: enable\r\n");
        } else {
            CIF_PRINTF("                      status: disable\r\n");
        }
        CIF_PRINTF("                      base=0x%" CIF_PRIxADDR "\r\n", (addr_t)region_base);
        CIF_PRINTF("                      size=%s\r\n",
                   cvt_size(region_lo.b.mem_region_size + 9));
    } else {
        CIF_PRINTF("                  mem_region0: absent\r\n");
    }
    region_lo.d = iinfo->mem_region1_cfg_lo;
    if (region_lo.b.exist) {
        CIF_PRINTF("                  mem_region1: present\r\n");
        IINFO_MEM_REGION_CFG_HI_Type region_hi = iinfo->mem_region1_cfg_hi;
        region_base = (uint64_t)region_hi << 32 | (region_lo.d & (~0x3FFULL));
        if (region_lo.b.mem_region_ena) {
            CIF_PRINTF("                      status: enable\r\n");
        } else {
            CIF_PRINTF("                      status: disable\r\n");
        }
        CIF_PRINTF("                      base=0x%" CIF_PRIxADDR "\r\n", (addr_t)region_base);
        CIF_PRINTF("                      size=%s\r\n",
                   cvt_size(region_lo.b.mem_region_size + 9));
    } else {
        CIF_PRINTF("                  mem_region1: absent\r\n");
    }
}

static void show_mcppi_cfg(IINFO_Type *iinfo)
{
    IINFO_MCPPI_CFG_LO_Type mcppi_lo;
    mcppi_lo.d = iinfo->mcppi_cfg_lo;
    if (mcppi_lo.b.exist) {
        CIF_PRINTF("                  cppi: present\r\n");
        IINFO_MCPPI_CFG_HI_Type mcppi_hi = iinfo->mcppi_cfg_hi;
        uint64_t mcppi_base =
            (uint64_t)mcppi_hi << 32 | (mcppi_lo.d & (~0x3FFULL));
        if (mcppi_lo.b.cppi_ena) {
            CIF_PRINTF("                      status: enable\r\n");
        } else {
            CIF_PRINTF("                      status: disable\r\n");
        }
        CIF_PRINTF("                      base=0x%" CIF_PRIxADDR "\r\n", (addr_t)mcppi_base);
        CIF_PRINTF("                      size=%s\r\n",
                   cvt_size(mcppi_lo.b.cppi_size + 9));
    } else {
        CIF_PRINTF("                  cppi: absent\r\n");
    }
}

static void show_cmo(IINFO_Type *iinfo)
{
    IINFO_MCMO_INFO_Type cmo_info;
    cmo_info.d = iinfo->cmo_info;
    if (cmo_info.b.cmo_cfg) {
        CIF_PRINTF("                  cmo: present\r\n");
        CIF_PRINTF("                      cbozero_size: %u Bytes\r\n",
                   POW2(cmo_info.b.cbozero_size + 2));
        CIF_PRINTF("                      cmo_size: %u Bytes\r\n",
                   POW2(cmo_info.b.cmo_size + 2));
        CIF_PRINTF("                      cmo_prefetch=%u\r\n",
                   cmo_info.b.cmo_pft);
    } else {
        CIF_PRINTF("                  cmo: absent\r\n");
    }
}

static void show_performance_cfg(IINFO_Type *iinfo)
{
    IINFO_PERFORMANCE_CFG0_Type performance_cfg0;
    IINFO_PERFORMANCE_CFG1_Type performance_cfg1;
    performance_cfg0.d = iinfo->performance_cfg0;
    performance_cfg1.d = iinfo->performance_cfg1;

    if (performance_cfg0.b.exist || performance_cfg1.b.exist) {
        CIF_PRINTF("                  hw performance: present\r\n");
        if (performance_cfg0.b.exist) {
            const char *bus[] = {"ICB", "AXI", "AHBL"};
            if (performance_cfg0.b.bus_type < 3) {
                CIF_PRINTF("                      bus: %s\r\n",
                           bus[performance_cfg0.b.bus_type]);
            }
            SHOW_VALUE(performance_cfg0, fpu_cycle);
            SHOW_VALUE(performance_cfg0, high_div);
            SHOW_VALUE(performance_cfg0, dcache_2stage);
            SHOW_VALUE(performance_cfg0, delay_branch_flush);
            SHOW_VALUE(performance_cfg0, dual_issue);
            SHOW_VALUE(performance_cfg0, cross_4k);
            SHOW_VALUE(performance_cfg0, dlm_2stage);
            SHOW_VALUE(performance_cfg0, lsu_cut_fwd);
            SHOW_VALUE(performance_cfg0, dsp_cycle);
            SHOW_VALUE(performance_cfg0, ifu_cut_timing);
            SHOW_VALUE(performance_cfg0, mem_cut_timing);
            SHOW_VALUE(performance_cfg0, dcache_prefetch);
            SHOW_VALUE(performance_cfg0, dcache_lbuf_num);
            SHOW_VALUE(performance_cfg0, mul_cyc);
        }
        if (performance_cfg1.b.exist) {
            SHOW_VALUE(performance_cfg1, vfpu_cyc);
            SHOW_VALUE(performance_cfg1, bht_entry_width);
            SHOW_VALUE(performance_cfg1, high_performance);
            SHOW_VALUE(performance_cfg1, agu_quick_forward);
            SHOW_VALUE(performance_cfg1, cau_fwd);
            SHOW_VALUE(performance_cfg1, hpm_ver);
        }
    } else {
        CIF_PRINTF("                  hw performance: absent\r\n");
    }
}

static void show_misc_cfg(IINFO_Type *iinfo)
{
    IINFO_MERGEL1DCTRL_Type mergel1dctrl;
    IINFO_ACCESS_CTRL_Type access_ctrl;
    mergel1dctrl.d = iinfo->mergel1dctrl;
    access_ctrl.d = iinfo->access_ctrl;
    CIF_PRINTF("                  misc: \r\n");
    SHOW_VALUE(mergel1dctrl, ws_tmout_max);
    SHOW_VALUE(mergel1dctrl, nc_tmout_max);
    SHOW_VALUE(mergel1dctrl, dev_store_early_ret);
    SHOW_VALUE(access_ctrl, pf_access);
    SHOW_VALUE(access_ctrl, cache_csr_access);
    SHOW_VALUE(access_ctrl, pma_csr_access);
}

static char *cvt_size_opt(uint32_t size, int lite)
{
    static char buf[32];
    char units[] = {'B', 'K', 'M', 'G'};
    int i = 0;
    while (size >= 1024 && i < 3) {
        size >>= 10;
        i++;
    }
    if (lite) {
        sprintf(buf, "%u%c", size, units[i]);
    } else {
        sprintf(buf, "%u %c%s", size, units[i], i > 0 ? "B" : "");
    }
    return buf;
}

static void show_cache_info(uint32_t set, uint32_t way, uint32_t lsize,
                            uint32_t ecc)
{
    CIF_PRINTF(" %s", cvt_size(set * way * lsize));
    CIF_PRINTF("(set=%d,", set);
    CIF_PRINTF("way=%d,", way);
    CIF_PRINTF("lsize=%d,", lsize);
    CIF_PRINTF("ecc=%d)\r\n", !!ecc);
}
