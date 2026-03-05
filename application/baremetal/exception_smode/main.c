// See LICENSE for license details.
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "nuclei_sdk_soc.h"

// 2048 is enough
#define SMODE_STACK_SIZE        2048

/* Create a stack for supervisor mode execution */
uint8_t smode_stack[SMODE_STACK_SIZE] __attribute__((aligned(16)));
uintptr_t smode_sp = (uintptr_t) (smode_stack + sizeof(smode_stack));

static void mmode_exception_handler(unsigned long mcause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;
    printf("\r\nmmode_exception_handler enters\r\n");

    switch (mcause & MCAUSE_CAUSE) {
        case IlleIns_EXCn:
            printf("Illegal instruction fault occurs, mcause: 0x%lx, mepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        case SmodeEcall_EXCn:
            printf("Environment call from S-mode, mcause: 0x%lx, mepc: 0x%lx, disable the medeleg\r\n", exc_frame->cause, exc_frame->epc);
            __set_medeleg(0);
            break;
        default: break;
    }
    exc_frame->epc += 4; // The illegal instruction and ecall takes 4 bytes
}

static void smode_exception_handler(unsigned long scause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;
    printf("\r\nsmode_exception_handler enters\r\n");
    switch (scause & SCAUSE_CAUSE) {
        case IlleIns_EXCn:
            printf("Illegal instruction fault occurs, scause: 0x%lx, sepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        default: break;
    }
    exc_frame->epc += 4; // The illegal instruction and ecall takes 4 bytes
}

static void trigger_illegal_inst(void)
{
    // The illegal instruction takes 4 bytes
    __ASM volatile(".word 0xffffffff");
}

static void trigger_ecall(void)
{
    // The ecall instruction takes 4 bytes
    __ASM volatile("ecall");
}

#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
// it's not reliable to use sp range to check privilege mode in exception/interrupt handler
void print_sp_judge_privilege_mode(void)
{
    uintptr_t sp;

    __asm__ volatile("add  %0, x0, sp" :"=r"(sp));

    printf("Current sp is 0x%lx, ", (unsigned long)sp);

    if ( ((uint8_t *)sp <= &(smode_stack[SMODE_STACK_SIZE - 1])) && ((uint8_t *)sp >= &(smode_stack[0])) ) {
        printf("so it is in Supervisor Mode!\r\n");
    }
    else {
        printf("so it is in Machine Mode!\r\n");
    }
}

static void supervisor_mode_entry_point(void)
{
    printf("[IN S-MODE ENTRY POINT] Hello Supervisor Mode!!!\r\n");

    print_sp_judge_privilege_mode();

    printf("\r\n1. The first time, smode's illegal instruction will be delegated to smode's handler");
    trigger_illegal_inst();

    printf("\r\n2. Ecall to mmode to disable the medeleg");
    trigger_ecall();

    printf("\r\n3. Then smode's illegal instruction will be handled in mmode");
    trigger_illegal_inst();

    printf("\r\nBack to S mode! S mode exception test finish and pass!\r\n");

#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(0);
#endif
    while(1);
}
#endif


int main(void)
{

#if defined(__PMP_PRESENT) && (__PMP_PRESENT == 1) && defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
    CSR_MCFGINFO_Type mcfg_info;
    CSR_MISA_Type misa;

    // Generally, Nuclei CPU's pmp/smpu granularity is 2^12 = 4KBytes(order should at least be 12), you should refer to the databook.
    // set pmp, S mode can access all address range
    pmp_config pmp_cfg = {
        /* M mode grants S and U mode with full permission of the whole address range */
        .protection = PMP_L | PMP_R | PMP_W | PMP_X,
        /* Memory region range 2^__RISCV_XLEN bytes */
        .order = __RISCV_XLEN,
        /* Initial base address is 0 */
        .base_addr = 0,
    };
    __set_PMPENTRYx(0, &pmp_cfg);

    mcfg_info.d = __RV_CSR_READ(CSR_MCFG_INFO);
    misa.d = __RV_CSR_READ(CSR_MISA);
    if (0 == misa.b.s) {
        printf("INFO: S-Mode is required, will not run this example!\r\n");
        return 0;
    }
#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT != 0)
    if (0 == mcfg_info.b.clic) {
        printf("You expect ECLIC present, but ECLIC is not present, will not run this example!\r\n");
        printf("You can rebuild and run this example with extra make option XLCFG_ECLIC=0 if ECLIC not present!\r\n");
        return 0;
    }
#endif
#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
    if (0 == misa.b.s) {
        printf("You expect SMODE present, but SMODE is not present, will not run this example!\r\n");
        printf("You can rebuild and run this example with extra make option XLCFG_SMODE=0 if SMODE not present!\r\n");
        return 0;
    }
#endif

    /* register corresponding exception */
    Exception_Register_EXC(IlleIns_EXCn, (unsigned long)mmode_exception_handler);
    Exception_Register_EXC(SmodeEcall_EXCn, (unsigned long)mmode_exception_handler);

    /* Corresponding exceptions occurs in S/U-mode will be delegated to S-mode */
    __set_medeleg(ILLEGAL_INSTRUCTION);
    Exception_Register_EXC_S(IlleIns_EXCn, (unsigned long)smode_exception_handler);

    printf("Drop to S-Mode now\n\r");
    /* Drop to S mode */
    __switch_mode(PRV_S, smode_sp, supervisor_mode_entry_point);

#else
    printf("[ERROR]__SMODE_PRESENT & __PMP_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}

