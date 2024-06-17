// See LICENSE for license details.
#include <stdio.h>
#include <string.h>
#include "nuclei_sdk_soc.h"

#if !defined(__TEE_PRESENT) || (__TEE_PRESENT != 1)
/* __TEE_PRESENT should be defined in <Device>.h */
#warning "__TEE_PRESENT is not defined or equal to 1, please check!"
#endif

#if !defined(__SMPU_PRESENT) || (__SMPU_PRESENT != 1)
/* __SMPU_PRESENT should be defined in <Device>.h */
#warning "__SMPU_PRESENT is not defined or equal to 1, please check!"
#endif


/* SMPU reuses exception codes of page fault for SMPU fault */

#define SMPU_InsFault_EXCn        InsPageFault_EXCn
#define SMPU_LdFault_EXCn         LdPageFault_EXCn
#define SMPU_StFault_EXC          StPageFault_EXCn

/* different trigger condition */
#define INSTRUCTION_SMPU_EXCEPTION                  0
#define LOAD_SMPU_EXCEPTION                         1
#define STORE_SMPU_EXCEPTION                        2
#define EXECUTE_SHARED_DATA_REGION_EXCEPTION        3
#define WRITE_READONLY_SHARED_DATA_EXCEPTION        4
// No exception below
#define SHARE_CODE_DATA_REGION                      5
#define RUN_WITH_ENTRY_INACTIVE                     6

/* Just choose one from above defines to test smpu */
#define TRIGGER_SMPU_VIOLATION_MODE                 RUN_WITH_ENTRY_INACTIVE

// 2048 is enough
#define SMODE_STACK_SIZE      2048
/* Create a stack for supervisor mode execution */
uint8_t smode_stack[SMODE_STACK_SIZE] __attribute__((aligned(16)));
uintptr_t smode_sp = (uintptr_t) (smode_stack + sizeof(smode_stack));

volatile uint8_t protected_data[0x2000] __attribute__((aligned(0x2000))) =    \
    {0xaa, 0x1, 0x02, 0x03, 0x04, 0x05, 0x06, 0xaa};

#ifndef __ICCRISCV__
#define __PMP_PROTECT   __attribute__((section (".text"), aligned(0x2000)))
#else
/* IAR CC currently don't support align function in section,
 * so we provide a workaround using a customized iar_evalsoc_ilm.icf in this demo
 * we define a block called PMPFUNC alignment set to 0x2000 */
#define __PMP_PROTECT   __attribute__((section (".text.pmpfunc")))
#endif

static void smpu_violation_fault_handler(unsigned long scause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;

    switch (scause & SCAUSE_CAUSE) {
        case SMPU_InsFault_EXCn:
            printf("Instruction SMPU fault occurs, cause: 0x%lx, epc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        case SMPU_LdFault_EXCn:
            printf("Load SMPU fault occurs, cause: 0x%lx, epc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        case SMPU_StFault_EXC:
            printf("Store/AMO SMPU fault occurs, cause: 0x%lx, epc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        default: break;
    }
#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(0);
#endif
    while(1);
}

static void __PMP_PROTECT protected_execute(void)
{
    printf("----protected_execute succeed!----\r\n");
}

typedef void(*__funcpt)(void);

static void supervisor_mode_entry_point(void)
{
    /* In case compiler use inline optimization of protected_execute */
    __funcpt fncptr = ((__funcpt)protected_execute);

    printf("Attempting to fetch instruction from protected address 0x%p\r\n", fncptr);
    /*
     * If TRIGGER_SMPU_INSTRUCTION_EXCEPTION, it will trigger instruction page fault,
     * see scause sdcause mepc for verification
     */
    fncptr();


    printf("Attempting to read protected_data[0] at %p\r\n", &protected_data[0]);
    printf("protected_data[0]: 0x%0X succeed\r\n", protected_data[0]);

    printf("Attempting to write protected_data[0] at %p\r\n", &protected_data[0]);
    protected_data[0] = 0xFF;

    printf("Won't run here if violates rules check!\r\n");
    while(1);
}

int main(void)
{
#if defined(__SMPU_PRESENT) && (__SMPU_PRESENT == 1)
    /* The SMPU is checked before the PMA checks and PMP checks */
    pmp_config pmp_cfg = {
        /* M mode grants S and U mode with full permission of the whole address range */
        .protection = PMP_L | PMP_R | PMP_W | PMP_X,
        /* Memory region range 2^__RISCV_XLEN bytes */
        .order = __RISCV_XLEN,
        /* Initial base address is 0 */
        .base_addr = 0,
    };

    /* Configuration of execution region*/
    smpu_config smpu_config_x = {
        /* Locking bit is set, means you can't modify corresponding smpu csrs in S-mode */
        .protection = SMPU_R | SMPU_W | SMPU_X,
        /* Initial protected excutable address range is 2^12 = 4K bytes */
        .order = 12,
        /* Initial base address is 0, change it to your memory assignment */
        .base_addr = 0,
    };

    /* configuration of read/write region*/
    smpu_config smpu_config_rw = {
        /* Locking bit is set, means you can't modify smpu csrs in S-mode */
        .protection = SMPU_S | SMPU_R | SMPU_W,
        /* Initial protected readable/writable address range is 2^12 = 4K bytes */
        .order = 12,
        /* Initial base address is 0, change it to your memory assignment */
        .base_addr = 0,
    };

    printf("------smpu demo with trigger condition %d------\r\n", TRIGGER_SMPU_VIOLATION_MODE);

    __set_PMPENTRYx(0, &pmp_cfg);
    /* Verify the configuration takes effect */
    memset(&pmp_cfg, 0, sizeof(pmp_cfg));
    __get_PMPENTRYx(0, &pmp_cfg);
    printf("Get pmp entry: index %d, prot_out: 0x%x, addr_out: 0x%lx, order_out: %lu\r\n", \
        0, pmp_cfg.protection, pmp_cfg.base_addr, pmp_cfg.order);
#endif

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
    /* Enable corresponpding entrie 0 and 1*/
    __set_SMPUSWITCHx(0x3);
    /* Corresponding exceptions occurs in S/U-mode will be delegated to S-mode */
    __set_medeleg(FETCH_PAGE_FAULT | LOAD_PAGE_FAULT | STORE_PAGE_FAULT);

    /* register corresponding exception */
    Exception_Register_EXC_S(SMPU_InsFault_EXCn, (unsigned long)smpu_violation_fault_handler);
    Exception_Register_EXC_S(SMPU_LdFault_EXCn, (unsigned long)smpu_violation_fault_handler);
    Exception_Register_EXC_S(SMPU_StFault_EXC, (unsigned long)smpu_violation_fault_handler);

    /* Must align by 2^order */
    smpu_config_x.base_addr = (unsigned long)protected_execute;
    smpu_config_rw.base_addr = (unsigned long)protected_data;

#if (INSTRUCTION_SMPU_EXCEPTION == TRIGGER_SMPU_VIOLATION_MODE)
    /* Read/Write region */
    smpu_config_x.protection = SMPU_S | SMPU_R | SMPU_W;

    /* Read/Write region */
    smpu_config_rw.protection = SMPU_S | SMPU_R | SMPU_W;
#elif (LOAD_SMPU_EXCEPTION == TRIGGER_SMPU_VIOLATION_MODE)
    /* Execute-only region */
    smpu_config_x.protection = SMPU_S | SMPU_X;

    /* Execute-only region */
    smpu_config_rw.protection = SMPU_S | SMPU_X;
#elif (STORE_SMPU_EXCEPTION == TRIGGER_SMPU_VIOLATION_MODE)
    /* Execute-only region */
    smpu_config_x.protection = SMPU_S | SMPU_X;

    /* Read-only region */
    smpu_config_rw.protection = SMPU_S | SMPU_R ;
#elif (EXECUTE_SHARED_DATA_REGION_EXCEPTION == TRIGGER_SMPU_VIOLATION_MODE)
    /* Execute on shared data region*/
    smpu_config_x.protection = SMPU_W | SMPU_X;
    /* Shared data region: Read/write for both S and U mode */
    smpu_config_rw.protection = SMPU_W | SMPU_X ;
#elif (WRITE_READONLY_SHARED_DATA_EXCEPTION == TRIGGER_SMPU_VIOLATION_MODE)
    /* Shared code region: Execute only on both S and U mode */
    smpu_config_x.protection = SMPU_S | SMPU_W;
    /* Shared data region: Read only on both S and U mode */
    smpu_config_rw.protection = SMPU_S | SMPU_R | SMPU_W | SMPU_X ;
#elif (SHARE_CODE_DATA_REGION == TRIGGER_SMPU_VIOLATION_MODE)
    /* Shared code region: Execute only on both S and U mode */
    smpu_config_x.protection = SMPU_S | SMPU_W;
    /* Shared data region: Read/write for both S and U mode */
    smpu_config_rw.protection = SMPU_W | SMPU_X ;
#elif (RUN_WITH_ENTRY_INACTIVE == TRIGGER_SMPU_VIOLATION_MODE)
    /* Inaccessible region */
    smpu_config_x.protection = 0;
    smpu_config_rw.protection = 0;

    /* But disable corresponpding entries*/
    __set_SMPUSWITCHx(0x0);
#endif

    __set_SMPUENTRYx(0, &smpu_config_x);
    /* Verify the configuration takes effect */
    memset(&smpu_config_x, 0, sizeof(smpu_config));
    __get_SMPUENTRYx(0, &smpu_config_x);
    printf("Get smpu entry: index %d, prot_out: 0x%x, addr_out: 0x%x, order_out: %d\r\n", \
        0, smpu_config_x.protection, smpu_config_x.base_addr, smpu_config_x.order);

    __set_SMPUENTRYx(1, &smpu_config_rw);
    /* Verify the configuration takes effect */
    memset(&smpu_config_rw, 0, sizeof(smpu_config));
    __get_SMPUENTRYx(1, &smpu_config_rw);
    printf("Get smpu entry: index %d, prot_out: 0x%x, addr_out: 0x%x, order_out: %d\r\n", \
        1, smpu_config_rw.protection, smpu_config_rw.base_addr, smpu_config_rw.order);

    /* Drop to S mode */
    __switch_mode(PRV_S, smode_sp, supervisor_mode_entry_point);
    while(1);
#else
    printf("[ERROR]__TEE_PRESENT & __SMPU_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif/* defined(__TEE_PRESENT) && (__TEE_PRESENT == 1) */

    return 0;
}
