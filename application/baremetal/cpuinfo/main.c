#include <stdio.h>
#include <string.h>

#include "cpuinfo.h"
#include "nuclei_sdk_soc.h"

#define BUFSIZE 2048

volatile int ille_ins_flag = 0;
void illegal_instruction_handler(unsigned long mcause, unsigned long sp) {
    ille_ins_flag = 1;
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;
    // read csr is a 4 bytes instruction
    // so we just add 4 to mepc to return to the next instruction
    exc_frame->epc += 4;
}

int main(void)
{
    static char cpufeatbuf[BUFSIZE];
#ifdef CFG_CPU_NAME
    printf("CPU NAME: %s\n", CFG_CPU_NAME);
#endif
#ifdef CFG_CPU_VERSION
    printf("CPU VERSION: %s\n", CFG_CPU_VERSION);
#endif
#ifdef CPU_ISA
    printf("CPU ISA: %s\n", CPU_ISA);
#endif

    CPU_INFO_Group cpuinfo;
    memset(&cpuinfo, 0, sizeof(cpuinfo)); // clear the struct

    cpuinfo.marchid.d = (uint32_t)__RV_CSR_READ(CSR_MARCHID);
    cpuinfo.mhartid = (uint32_t)__RV_CSR_READ(CSR_MHARTID);
    cpuinfo.mimpid.d = (uint32_t)__RV_CSR_READ(CSR_MIMPID);
    cpuinfo.misa.d = (uint32_t)__RV_CSR_READ(CSR_MISA);
    U32_CSR_MCFG_INFO_Type mcfg;
    if (cpuinfo.marchid.d == 0x80000022U && cpuinfo.mimpid.d == 0x100U) {
        cpuinfo.mcfg_exist = 0;
        mcfg.d = 0;
    } else {
        cpuinfo.mcfg_exist = 1;
        mcfg.d = (uint32_t)__RV_CSR_READ(CSR_MCFG_INFO);
    }

    // NOTE: workaround for n100, since the CSR mcfg_info not present in n100,
    // but eclic and iregion present
#if defined(CPU_SERIES) && CPU_SERIES == 100
    mcfg.d = 0;
    // mirgb_info csr present for n100 with eclic, this csr will not be zero
    if (__RV_CSR_READ(CSR_MIRGB_INFO) != 0) {
        mcfg.b.iregion = 1;
        mcfg.b.eclic = 1;
    } else {
        cpuinfo.mcfg_exist = 0;
    }
#endif

    cpuinfo.mcfginfo = mcfg;
    if (__RISCV_XLEN == 32) {
        cpuinfo.xlen = 32;
        if (mcfg.b.plic) {
            cpuinfo.mtlbcfginfo.d = (uint32_t)__RV_CSR_READ(CSR_MTLBCFG_INFO);
        }
    } else {
        cpuinfo.xlen = 64;
        /**
         * mtlbcfginfo has a `mapping` field at the highest bit.
         * For RV64, move the bit 63 to bit 31 to use the common
         * struct as RV32.
         */
        if (mcfg.b.plic) {
            uint64_t mtlbcfginfo = __RV_CSR_READ(CSR_MTLBCFG_INFO);
            cpuinfo.mtlbcfginfo.d =
                (uint32_t)mtlbcfginfo | (uint32_t)((mtlbcfginfo >> 63) << 31);
        }
    }

    if (mcfg.b.icache || mcfg.b.ilm) {
        cpuinfo.micfginfo.d = (uint32_t)__RV_CSR_READ(CSR_MICFG_INFO);
    }
    if (mcfg.b.dcache || mcfg.b.dlm) {
        cpuinfo.mdcfginfo.d = (uint32_t)__RV_CSR_READ(CSR_MDCFG_INFO);
    }
    if (mcfg.b.iregion) {
        cpuinfo.mirgbinfo.d = (uint64_t)__RV_CSR_READ(CSR_MIRGB_INFO);
        uint64_t iregion_base = cpuinfo.mirgbinfo.d & (~0x3FFULL);
        cpuinfo.iinfo = (CIF_IINFO_Type *)((unsigned long)iregion_base);
        cpuinfo.iregion_base = iregion_base;
        if (mcfg.b.smp) {
            cpuinfo.smpcfg.d = *(uint32_t *)((
                unsigned long)(iregion_base + CPUINFO_IRG_SMP_OFS + 0x4));
        }
        if (cpuinfo.smpcfg.b.cc) {
            cpuinfo.cccfg.d = *(uint32_t *)((
                unsigned long)(iregion_base + CPUINFO_IRG_SMP_OFS + 0x8));
        }
        if (mcfg.b.eclic) {
            cpuinfo.eclic = (CIF_ECLIC_Type *)((
                unsigned long)(iregion_base + CPUINFO_IRG_ECLIC_OFS));
        }
    }
    if (mcfg.b.ppi) {
        cpuinfo.mppicfginfo.d = (uint64_t)__RV_CSR_READ(CSR_MPPICFG_INFO);
    }
    if (mcfg.b.fio) {
        cpuinfo.mfiocfginfo.d = (uint64_t)__RV_CSR_READ(CSR_MFIOCFG_INFO);
    }

    CIF_IINFO_ISA_SUPPORT0_Type isa_support0;
#if defined(CPU_SERIES) && CPU_SERIES == 100
    isa_support0.d = 0;
#else
    isa_support0.d = cpuinfo.iinfo->isa_support0;
#endif
    /* The init value of vlenb is 0, indicating that the vector extension is not supported */
    cpuinfo.vlenb = 0;
    /* misa.V only valid when the full vector extension is supported.
     * Judge vector extension existence from isa_support0.vector is more reliable */
    if (cpuinfo.misa.b.V || (isa_support0.b.exist && isa_support0.b.vector)) {
        /* Set mstatus.vs to enable vector extension, then read vlenb */
        __RV_CSR_SET(CSR_MSTATUS, MSTATUS_VS_INITIAL);
        cpuinfo.vlenb = __RV_CSR_READ(CSR_VLENB);
    }

    /* Get miscellaneous information */
#if defined(CPU_SERIES) && CPU_SERIES == 100
    cpuinfo.misc.b.pma_macro = 0;
    cpuinfo.misc.b.misaligned_access = 1;
    cpuinfo.misc.b.cidu_exist = 0;
#else
    /* Check PMA_MACRO by reading `mmacro_dev_en`, if no exception then PMA_MACRO is supported */
    unsigned long old_handler = Exception_Get_EXC(IlleIns_EXCn);
    ille_ins_flag = 0;
    Exception_Register_EXC(IlleIns_EXCn, (unsigned long)illegal_instruction_handler);
    rv_csr_t mmacro_dev_en = __RV_CSR_READ(CSR_MMACRO_DEV_EN);
    /* Restore the default exception handler */
    Exception_Register_EXC(IlleIns_EXCn, old_handler);
    cpuinfo.misc.b.pma_macro = !ille_ins_flag;

    /* Check MISALIGNED_ACCESS by reading mmisc_ctl register */
    CSR_MMISC_CTL_Type mmisc_ctl;
    mmisc_ctl.d = (uint32_t)__RV_CSR_READ(CSR_MMISC_CTL);
    cpuinfo.misc.b.misaligned_access = mmisc_ctl.b.misalign;

    /* Check CIDU by CIDU interrupt number */
    if (mcfg.b.iregion) {
        unsigned long cidu_addr = (unsigned long)cpuinfo.iregion_base + CPUINFO_IRG_IDU_OFS;
        const uint32_t *cidu_int_num_addr = (const uint32_t *)(cidu_addr + CIF_CIDU_INT_NUM_OFS);
        cpuinfo.misc.b.cidu_exist = !!(*cidu_int_num_addr);
    }
#endif

    if (get_basic_cpuinfo(&cpuinfo, cpufeatbuf, BUFSIZE) > 0) {
        printf("%s\r\n", cpufeatbuf);
    }
    show_cpuinfo(&cpuinfo);

    return 0;
}
