#include <stdio.h>
#include <string.h>

#include "cpuinfo.h"
#include "nuclei_sdk_soc.h"

#define BUFSIZE 2048

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
#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
    // mirgb_info csr present for n100 with eclic, this csr will not be zero
    if (__RV_CSR_READ(CSR_MIRGB_INFO) != 0) {
        mcfg.b.iregion = 1;
        mcfg.b.eclic = 1;
    } else {
        cpuinfo.mcfg_exist = 0;
    }
#endif
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
        cpuinfo.iinfo = (IINFO_Type *)((unsigned long)iregion_base);
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
            cpuinfo.eclic = (ECLIC_Type *)((
                unsigned long)(iregion_base + CPUINFO_IRG_ECLIC_OFS));
        }
    }
    if (mcfg.b.ppi) {
        cpuinfo.mppicfginfo.d = (uint64_t)__RV_CSR_READ(CSR_MPPICFG_INFO);
    }
    if (mcfg.b.fio) {
        cpuinfo.mfiocfginfo.d = (uint64_t)__RV_CSR_READ(CSR_MFIOCFG_INFO);
    }

    IINFO_ISA_SUPPORT0_Type isa_support0;
    isa_support0.d = cpuinfo.iinfo->isa_support0;
    /* The init value of vlenb is 0, indicating that the vector extension is not supported */
    cpuinfo.vlenb = 0;
    /* misa.V only valid when the full vector extension is supported.
     * Judge vector extension existence from isa_support0.vector is more reliable */
    if (cpuinfo.misa.b.V || (isa_support0.b.exist && isa_support0.b.vector)) {
        /* Set mstatus.vs to enable vector extension, then read vlenb */
        __RV_CSR_SET(CSR_MSTATUS, MSTATUS_VS_INITIAL);
        cpuinfo.vlenb = __RV_CSR_READ(CSR_VLENB);
    }

    if (get_basic_cpuinfo(&cpuinfo, cpufeatbuf, BUFSIZE) > 0) {
        printf("%s\r\n", cpufeatbuf);
    }
    show_cpuinfo(&cpuinfo);

    return 0;
}
