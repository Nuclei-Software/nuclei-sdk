// See LICENSE for license details.
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "nuclei_sdk_soc.h"

static void mmode_exception_handler(unsigned long mcause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;
    printf("\r\nmmode_exception_handler enters\r\n");
    switch (mcause & MCAUSE_CAUSE) {
        case IlleIns_EXCn:
            printf("Illegal instruction fault occurs, mcause: 0x%lx, mepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc, exc_frame->epc);
            break;
        case MmodeEcall_EXCn:
            printf("Environment call from M-mode, mcause: 0x%lx, mepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        default: break;
    }
#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(0);
#endif
    exc_frame->epc += 4; // The illegal instruction and ecall takes 4 bytes
}

static void trigger_illegal_inst(void)
{
    // The illegal instruction takes 4 bytes
    __ASM volatile(".word 0xffffffff");
}

static void trigger_ecall(void)
{
    // The ecall takes 4 bytes
    __ASM volatile("ecall");
}

int main(void)
{

    CSR_MCFGINFO_Type mcfg_info;

    mcfg_info.d = __RV_CSR_READ(CSR_MCFG_INFO);

#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT != 0)
    if (0 == mcfg_info.b.clic) {
        printf("You expect ECLIC present, but ECLIC is not present, will not run this example!\r\n");
        printf("You can rebuild and run this example with extra make option XLCFG_ECLIC=0 if ECLIC not present!\r\n");
        return 0;
    }
#endif

    /* register corresponding exception */
    Exception_Register_EXC(IlleIns_EXCn, (unsigned long)mmode_exception_handler);
    Exception_Register_EXC(MmodeEcall_EXCn, (unsigned long)mmode_exception_handler);

    trigger_illegal_inst();
    trigger_ecall();
    printf("\r\nM mode exception test finish and pass\r\n");
    return 0;
}

