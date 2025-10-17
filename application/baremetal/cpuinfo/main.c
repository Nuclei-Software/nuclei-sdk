#include <stdio.h>
#include <string.h>

#include "cpuinfo.h"
#include "nuclei_sdk_soc.h"

int main(void)
{
#ifdef CFG_CPU_NAME
    printf("CPU NAME: %s\n", CFG_CPU_NAME);
#endif
#ifdef CFG_CPU_VERSION
    printf("CPU VERSION: %s\n", CFG_CPU_VERSION);
#endif
#ifdef CPU_ISA
    printf("CPU ISA: %s\n", CPU_ISA);
#endif

    CIF_XLEN_Type xlen;
    CPU_INFO_Group cpuinfo;
    memset(&cpuinfo, 0, sizeof(cpuinfo)); // clear the struct

    cpuinfo.marchid.d = (uint32_t)__RV_CSR_READ(CSR_MARCHID);
    cpuinfo.mhartid = (uint32_t)__RV_CSR_READ(CSR_MHARTID);
    cpuinfo.mimpid.d = (uint32_t)__RV_CSR_READ(CSR_MIMPID);
    cpuinfo.misa.d = (uint32_t)__RV_CSR_READ(CSR_MISA);
    U32_CSR_MCFG_INFO_Type mcfg;
    mcfg.d = (uint32_t)__RV_CSR_READ(CSR_MCFG_INFO);
    cpuinfo.mcfg_exist = 1;
    cpuinfo.mcfginfo = mcfg;

    /**
     * mtlbcfginfo has a `mapping` field at the highest bit.
     * For RV64, move the bit 63 to bit 31 to use the common
     * struct as RV32.
     */
    if (__RISCV_XLEN == 32) {
        xlen = CIF_XLEN_32;
        if (mcfg.b.plic) {
            cpuinfo.mtlbcfginfo.d = (uint32_t)__RV_CSR_READ(CSR_MTLBCFG_INFO);
        }
    } else {
        xlen = CIF_XLEN_64;
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
        unsigned long iregion_base = cpuinfo.mirgbinfo.d & (~0x3FF);
        cpuinfo.iinfo = (IINFO_Type *)iregion_base;
        if (mcfg.b.smp) {
            cpuinfo.smpcfg.d =
                *(uint32_t *)(iregion_base + CPUINFO_IRG_SMP_OFS + 0x4);
        }
        if (cpuinfo.smpcfg.b.cc) {
            cpuinfo.cccfg.d =
                *(uint32_t *)(iregion_base + CPUINFO_IRG_SMP_OFS + 0x8);
        }
        if (mcfg.b.eclic) {
            cpuinfo.eclic = (ECLIC_Type *)(iregion_base + CPUINFO_IRG_ECLIC_OFS);
        }
    }
    if (mcfg.b.ppi) {
        cpuinfo.mppicfginfo.d = (uint64_t)__RV_CSR_READ(CSR_MPPICFG_INFO);
    }
    if (mcfg.b.fio) {
        cpuinfo.mfiocfginfo.d = (uint64_t)__RV_CSR_READ(CSR_MFIOCFG_INFO);
    }

    show_cpuinfo(xlen, &cpuinfo);

    return 0;
}
