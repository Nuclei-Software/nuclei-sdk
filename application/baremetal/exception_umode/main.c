// See LICENSE for license details.
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "nuclei_sdk_soc.h"

// 2048 is enough
#define SMODE_STACK_SIZE        2048
#define UMODE_STACK_SIZE        SMODE_STACK_SIZE

// Shared code region: Execute only on both S and U mode
#define S_U_EXECUTE_ONLY        SMPU_S | SMPU_W
// Shared code region: Execute only on U mode, read/execute on S mode
#define S_U_EXECUTE_S_READ      SMPU_S | SMPU_W | SMPU_X
// Shared data region: Read/write for both S and U mode
#define S_U_READ_WRITE          SMPU_W | SMPU_X

extern char ILM_MEMORY_BASE[], ILM_MEMORY_SIZE[], DLM_MEMORY_BASE[], DLM_MEMORY_SIZE[], DDR_MEMORY_BASE[], DDR_MEMORY_SIZE[], DDR_MEMORY_ROM_SIZE[], FLASH_MEMORY_BASE[],
            FLASH_MEMORY_SIZE[], SRAM_MEMORY_BASE[], SRAM_MEMORY_SIZE[], SRAM_MEMORY_ROM_SIZE[];

/* Create a stack for supervisor mode execution */
uint8_t smode_stack[SMODE_STACK_SIZE] __attribute__((aligned(16)));
uintptr_t smode_sp = (uintptr_t) (smode_stack + sizeof(smode_stack));

/* Create a stack for user mode execution */
uint8_t umode_stack[UMODE_STACK_SIZE] __attribute__((aligned(16)));
uintptr_t umode_sp = (uintptr_t) (umode_stack + sizeof(umode_stack));

static void smode_exception_handler(unsigned long scause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;
    printf("\r\nsmode_exception_handler enters\r\n");

    switch (scause & SCAUSE_CAUSE) {
        case IlleIns_EXCn:
            printf("Illegal instruction fault occurs, scause: 0x%lx, sepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        case UmodeEcall_EXCn:
            printf("Environment call from U-mode, scause: 0x%lx, sepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        default: break;
    }
    exc_frame->epc += 4; // The illegal instruction and ecall takes 4 bytes
}

static void mmode_exception_handler(unsigned long mcause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;
    printf("\r\nmmode_exception_handler enters\r\n");

    switch (mcause & MCAUSE_CAUSE) {
        case IlleIns_EXCn:
            printf("Illegal instruction fault occurs, mcause: 0x%lx, mepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        case UmodeEcall_EXCn:
            printf("Environment call from U-mode, mcause: 0x%lx, mepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
            /* Corresponding exceptions occurs in S/U-mode will be delegated to S-mode if delegating enabled */
            __set_medeleg(ILLEGAL_INSTRUCTION | USER_ECALL);
#endif
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
    // The ecall takes 4 bytes
    __ASM volatile("ecall");
}

void print_sp_judge_privilege_mode(void)
{
    uintptr_t sp;

    __asm__ volatile("add  %0, x0, sp" :"=r"(sp));

    printf("Current sp is 0x%lx, ", (unsigned long)sp);

    if ( ((uint8_t *)sp <= &(smode_stack[SMODE_STACK_SIZE - 1])) && ((uint8_t *)sp >= &(smode_stack[0])) ) {
        printf("so it is in Supervisor Mode!\r\n");
    } 
    else if ( ((uint8_t *)sp <= &(umode_stack[UMODE_STACK_SIZE - 1])) && ((uint8_t *)sp >= &(umode_stack[0])) ) {
        printf("so it is in User Mode!\r\n");
    }
    else {
        printf("so it is in Machine Mode!\r\n");
    }
}

static void user_mode_entry_point(void)
{
    CSR_MSTATUS_Type mstatus;
    mstatus.d = __RV_CSR_READ(CSR_MSTATUS);
    if (PRV_S == mstatus.b.mpp) {
        printf("\r\nFrom S mode switch into U mode\r\n");
    } else if (PRV_M == mstatus.b.mpp) {
        printf("\r\nFrom M mode switch into U mode\r\n");
    }

    print_sp_judge_privilege_mode();

    printf("\r\n1. The first time, umode's illegal instruction and ecall will be handled in mmode");
    trigger_illegal_inst();

    printf("\r\n2. Ecall to mmode");
    trigger_ecall();

#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
    printf("\r\n3. Then umode's illegal instruction and ecall will be delegated to smode's handler");
    trigger_illegal_inst();
    trigger_ecall();
#endif

    printf("\r\nBack to U mode! U mode exception test finish and pass!\r\n");
#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(0);
#endif
    while (1);
}

#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
// it's not reliable to use sp range to check privilege mode in exception/interrupt handler

static void supervisor_mode_entry_point(void)
{
    printf("[IN S-MODE ENTRY POINT] Hello Supervisor Mode!!!\r\n");

    print_sp_judge_privilege_mode();

    /* Drop to U mode from S mode */
    __s_switch_mode(PRV_U, umode_sp, user_mode_entry_point);

#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(0);
#endif
    while (1);
}
#endif

int main(void)
{
#if defined(__PMP_PRESENT) && (__PMP_PRESENT == 1)
    CSR_MCFGINFO_Type mcfg_info;
    CSR_MISA_Type misa;

    mcfg_info.d = __RV_CSR_READ(CSR_MCFG_INFO);
    misa.d = __RV_CSR_READ(CSR_MISA);

#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT != 0)
    if (0 == mcfg_info.b.clic) {
        printf("You expect ECLIC present, but ECLIC is not present, will not run this example!\r\n");
        printf("You can rebuild and run this example with extra make option XLCFG_ECLIC=0 if ECLIC not present!\r\n");
        return 0;
    }
#endif

#if defined(__SMPU_PRESENT) && (__SMPU_PRESENT == 1)
#if defined(CPU_SERIES) && CPU_SERIES == 100
#else
    smpu_config smpu_config_rw1, smpu_config_x1, smpu_config_x2, smpu_config_rw2;
    if (1 == mcfg_info.b.tee) {
        printf("Configure SMPU due to TEE Present, and it depends on Nuclei evalsoc's linker memory map!\r\n");

        // For simplicity to show U mode's exception and delegation to S mode,
        // this demo grant S and U the same permissions with memory range as big as possible, and can't be used as a practical reference

        /* Configuration of S and U shared r/w region:0x0 ~ 0x1FFF FFFF, containing peripheral device range like uart*/
        smpu_config_rw1.protection = S_U_READ_WRITE | SMPU_A_NAPOT;
        smpu_config_rw1.order = 29;
        smpu_config_rw1.base_addr = 0;

        if (DOWNLOAD_MODE == DOWNLOAD_MODE_FLASH || DOWNLOAD_MODE == DOWNLOAD_MODE_ILM) {
            /* Configuration of S and U shared code region: 0x2000 0000 ~ 0x207F FFFF, containing flash */
            smpu_config_x1.protection = S_U_EXECUTE_S_READ | SMPU_A_NAPOT;
            smpu_config_x1.order = __CTZ((unsigned long)FLASH_MEMORY_SIZE);
            smpu_config_x1.base_addr = (unsigned long)FLASH_MEMORY_BASE;

            /* Configuration of S and U shared code region: 0x8000 0000 ~ 0x8FFF FFFF, containing ilm */
            smpu_config_x2.protection = S_U_EXECUTE_ONLY | SMPU_A_NAPOT;
            smpu_config_x2.order = __CTZ((unsigned long)ILM_MEMORY_SIZE);
            smpu_config_x2.base_addr = (unsigned long)ILM_MEMORY_BASE;

            /* Configuration of S and U shared r/w region:0x9000 0000 ~ 0x9FFF FFFF, containing dlm */
            smpu_config_rw2.protection = S_U_READ_WRITE | SMPU_A_NAPOT;
            smpu_config_rw2.order = __CTZ((unsigned long)DLM_MEMORY_SIZE);
            smpu_config_rw2.base_addr = (unsigned long)DLM_MEMORY_BASE;
        } else if (DOWNLOAD_MODE == DOWNLOAD_MODE_SRAM) {
            /* Configuration of S and U shared code region: 0xA000 0000 ~ 0x7FF FFFF, containing sram */
            smpu_config_x2.protection = S_U_EXECUTE_ONLY | SMPU_A_NAPOT;
            smpu_config_x2.order = __CTZ((unsigned long)SRAM_MEMORY_ROM_SIZE);
            smpu_config_x2.base_addr = (unsigned long)SRAM_MEMORY_BASE;

            /* Configuration of S and U shared data region: 0xA800 0000 ~ 0xAFFF FFFF, containing ddr */
            smpu_config_rw2.protection = S_U_READ_WRITE | SMPU_A_NAPOT;
            smpu_config_rw2.order = __CTZ((unsigned long)SRAM_MEMORY_SIZE - (unsigned long)SRAM_MEMORY_ROM_SIZE);
            smpu_config_rw2.base_addr = (unsigned long)SRAM_MEMORY_BASE + (unsigned long)SRAM_MEMORY_ROM_SIZE;
        } else if (DOWNLOAD_MODE == DOWNLOAD_MODE_DDR) {
            /* Configuration of S and U shared code region: 0xA000 0000 ~ 0x7FF FFFF, containing ddr */
            smpu_config_x2.protection = S_U_EXECUTE_ONLY | SMPU_A_NAPOT;
            smpu_config_x2.order = __CTZ((unsigned long)DDR_MEMORY_ROM_SIZE);
            smpu_config_x2.base_addr = (unsigned long)DDR_MEMORY_BASE;

            /* Configuration of S and U shared data region: 0xA800 0000 ~ 0xAFFF FFFF, containing ddr */
            smpu_config_rw2.protection = S_U_READ_WRITE | SMPU_A_NAPOT;
            smpu_config_rw2.order = __CTZ((unsigned long)DDR_MEMORY_SIZE - (unsigned long)DDR_MEMORY_ROM_SIZE);
            smpu_config_rw2.base_addr = (unsigned long)DDR_MEMORY_BASE + (unsigned long)DDR_MEMORY_ROM_SIZE;

        } else {
            printf("Unsupported flashxip download mode, will not run this example!\r\n");
            return 0;
        }
        // enable smpu entries
        __set_SMPUENTRYx(0, &smpu_config_rw1);
        __set_SMPUENTRYx(1, &smpu_config_x1);
        __set_SMPUENTRYx(2, &smpu_config_x2);
        __set_SMPUENTRYx(3, &smpu_config_rw2);
        __set_SMPUSWITCHx(0xF);
    } else {
        printf("You expect SMPU present, but SMPU is not present, will not run this example!\r\n");
        printf("You can rebuild and run this example with extra make option XLCFG_SMPU=0 if SMPU not present!\r\n");
        return 0;
    }
#endif
#else
    if (1 == mcfg_info.b.tee) {
        printf("You expect SMPU not present, but SMPU is present, will not run this example!\r\n");
        printf("You can rebuild and run this example with extra make option XLCFG_SMPU=1 if SMPU is present!\r\n");
        return 0;
    }
#endif

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

    Exception_Register_EXC(IlleIns_EXCn, (unsigned long)mmode_exception_handler);
    Exception_Register_EXC(UmodeEcall_EXCn, (unsigned long)mmode_exception_handler);
#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
    if (0 == misa.b.s) {
        printf("You expect SMODE present, but SMODE is not present, will not run this example!\r\n");
        printf("You can rebuild and run this example with extra make option XLCFG_SMODE=0 if SMODE not present!\r\n");
        return 0;
    }
    Exception_Register_EXC_S(IlleIns_EXCn, (unsigned long)smode_exception_handler);
    Exception_Register_EXC_S(UmodeEcall_EXCn, (unsigned long)smode_exception_handler);
    printf("Drop to S-Mode now\n\r");
    /* Drop to S mode */
    __switch_mode(PRV_S, smode_sp, supervisor_mode_entry_point);
#else
    printf("Drop to U-Mode now\n\r");
    /* Drop to U mode */
    __switch_mode(PRV_U, umode_sp, user_mode_entry_point);
#endif

#else
    printf("__PMP_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}

