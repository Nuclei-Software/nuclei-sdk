// See LICENSE for license details.
#include <stdio.h>
#include <string.h>
#include "nuclei_sdk_soc.h"

#if !defined(__TEE_PRESENT) || (__TEE_PRESENT != 1)
/* __TEE_PRESENT should be defined in <Device>.h */
#warning "__TEE_PRESENT is not defined or equal to 1, please check!"
#endif

#if !defined(__SPMP_PRESENT) || (__SPMP_PRESENT != 1)
/* __SPMP_PRESENT should be defined in <Device>.h */
#warning "__SPMP_PRESENT is not defined or equal to 1, please check!"
#endif

/* different trigger condition */
#define INSTRUCTION_FETCH_PAGE_EXCEPTION     0
#define LOAD_PAGE_EXCEPTION                  1
#define STORE_PAGE_EXCEPTION                 2
#define EXECUTE_USERMODE_MEMORY_EXCEPTION    3
#define LOAD_USERMODE_MEMORY_EXCEPTION       4
#define STORE_USERMODE_MEMORY_EXCEPTION      5
#define RUN_WITH_NO_SPMP_CHECK               6

/* Just choose one from above defines to test sPMP */
#define TRIGGER_SPMP_VIOLATION_MODE          RUN_WITH_NO_SPMP_CHECK

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

static void spmp_violation_fault_handler(unsigned long scause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;

    switch (scause & SCAUSE_CAUSE) {
        case InsPageFault_EXCn:
            printf("Instruction page fault occurs, cause: 0x%lx, epc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        case LdPageFault_EXCn:
            printf("Load page fault occurs, cause: 0x%lx, epc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            break;
        case StPageFault_EXCn:
            printf("Store/AMO page fault occurs, cause: 0x%lx, epc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
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
     * If TRIGGER_SPMP_INSTRUCTION_EXCEPTION, it will trigger instruction page fault,
     * see scause sdcause mepc for verification
     */
    fncptr();

#if (STORE_USERMODE_MEMORY_EXCEPTION != TRIGGER_SPMP_VIOLATION_MODE)
    printf("Attempting to read protected_data[0] at %p\r\n", &protected_data[0]);
    printf("protected_data[0]: 0x%0X succeed\r\n", protected_data[0]);
#endif

#if (LOAD_USERMODE_MEMORY_EXCEPTION != TRIGGER_SPMP_VIOLATION_MODE)
    printf("Attempting to write protected_data[0] at %p\r\n", &protected_data[0]);
    protected_data[0] = 0xFF;
#endif
    printf("Won't run here if violates L U\\R\\W\\X permission check!\r\n");
#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(0);
#endif
    while(1);
}

int main(void)
{
#if defined(__SPMP_PRESENT) && (__SPMP_PRESENT == 1)
    /* The sPMP values are checked after the physical address to be accessed pass PMP checks */
    pmp_config pmp_cfg = {
        /* M mode grants S and U mode with full permission of the whole address range */
        .protection = PMP_L | PMP_R | PMP_W | PMP_X,
        /* Memory region range 2^__RISCV_XLEN bytes */
        .order = __RISCV_XLEN,
        /* Initial base address is 0 */
        .base_addr = 0,
    };

    /* Configuration of execution region*/
    spmp_config spmp_config_x = {
        /* Locking bit is set, means you can't modify corresponding spmp csrs in S-mode */
        .protection = SPMP_L | SPMP_R | SPMP_W | SPMP_X,
        /* Initial protected excutable address range is 2^12 = 4K bytes */
        .order = 12,
        /* Initial base address is 0, change it to your memory assignment */
        .base_addr = 0,
    };

    /* configuration of read/write region*/
    spmp_config spmp_config_rw = {
        /* Locking bit is set, means you can't modify spmp csrs in S-mode */
        .protection = SPMP_L | SPMP_R | SPMP_W,
        /* Initial protected readable/writable address range is 2^12 = 4K bytes */
        .order = 12,
        /* Initial base address is 0, change it to your memory assignment */
        .base_addr = 0,
    };

    printf("------sPMP demo with trigger condition %d------\r\n", TRIGGER_SPMP_VIOLATION_MODE);

    __set_PMPENTRYx(0, &pmp_cfg);
    /* Verify the configuration takes effect */
    memset(&pmp_cfg, 0, sizeof(pmp_cfg));
    __get_PMPENTRYx(0, &pmp_cfg);
    printf("Get pmp entry: index %d, prot_out: 0x%x, addr_out: 0x%lx, order_out: %lu\r\n", \
        0, pmp_cfg.protection, pmp_cfg.base_addr, pmp_cfg.order);
#endif

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
    /* Corresponding exceptions occurs in S/U-mode will be delegated to S-mode */
    __set_medeleg(FETCH_PAGE_FAULT | LOAD_PAGE_FAULT | STORE_PAGE_FAULT);

    /* register corresponding exception */
    Exception_Register_EXC_S(InsPageFault_EXCn, (unsigned long)spmp_violation_fault_handler);
    Exception_Register_EXC_S(LdPageFault_EXCn, (unsigned long)spmp_violation_fault_handler);
    Exception_Register_EXC_S(StPageFault_EXCn, (unsigned long)spmp_violation_fault_handler);

    /* Must align by 2^order */
    spmp_config_x.base_addr = (unsigned long)protected_execute;
    spmp_config_rw.base_addr = (unsigned long)protected_data;

#if (INSTRUCTION_FETCH_PAGE_EXCEPTION == TRIGGER_SPMP_VIOLATION_MODE)
    /* Remove X permission of protected_execute region */
    spmp_config_x.protection = SPMP_L | SPMP_R | SPMP_W;

    spmp_config_rw.protection = SPMP_L | SPMP_R | SPMP_W;
#elif (LOAD_PAGE_EXCEPTION == TRIGGER_SPMP_VIOLATION_MODE)
    /* Retrive X permission of protected_execute region */
    spmp_config_x.protection = SPMP_L | SPMP_R | SPMP_W | SPMP_X;

    /* Remove R permission of protected_data region */
    spmp_config_rw.protection = SPMP_L | SPMP_W;
#elif (STORE_PAGE_EXCEPTION == TRIGGER_SPMP_VIOLATION_MODE)
    /* Retrive X permission of protected_execute region */
    spmp_config_x.protection = SPMP_L | SPMP_R | SPMP_W | SPMP_X;

    /* Remove W permission of protected_data region */
    spmp_config_rw.protection = SPMP_L | SPMP_R ;
#elif (EXECUTE_USERMODE_MEMORY_EXCEPTION == TRIGGER_SPMP_VIOLATION_MODE)
    /* add U permission of exclusive protected_execute region for usermode */
    spmp_config_x.protection = SPMP_L | SPMP_U | SPMP_R | SPMP_W | SPMP_X;
#elif (LOAD_USERMODE_MEMORY_EXCEPTION == TRIGGER_SPMP_VIOLATION_MODE)
    /* remove U permission of exclusive protected_execute region for usermode */
    spmp_config_x.protection = SPMP_L | SPMP_R | SPMP_W | SPMP_X;

    /* add U permission of exclusive protected_data region for usermode */
    spmp_config_rw.protection = SPMP_L | SPMP_U | SPMP_R ;
#elif (STORE_USERMODE_MEMORY_EXCEPTION == TRIGGER_SPMP_VIOLATION_MODE)
    /* remove U permission of exclusive protected_execute region for usermode */
    spmp_config_x.protection = SPMP_L | SPMP_R | SPMP_W | SPMP_X;

    /* add U permission of exclusive protected_data region for usermode */
    spmp_config_rw.protection = SPMP_L | SPMP_U | SPMP_W ;
#elif (RUN_WITH_NO_SPMP_CHECK == TRIGGER_SPMP_VIOLATION_MODE)
    /* Unset Locking bit, any S-mode access matching the sPMP entry will succeed */
    spmp_config_x.protection = SPMP_R | SPMP_W | SPMP_X;

    spmp_config_rw.protection = SPMP_R | SPMP_W ;
#endif

    __set_sPMPENTRYx(0, &spmp_config_x);
    /* Verify the configuration takes effect */
    memset(&spmp_config_x, 0, sizeof(spmp_config));
    __get_sPMPENTRYx(0, &spmp_config_x);
    printf("Get spmp entry: index %d, prot_out: 0x%x, addr_out: 0x%x, order_out: %d\r\n", \
        0, spmp_config_x.protection, spmp_config_x.base_addr, spmp_config_x.order);

    __set_sPMPENTRYx(1, &spmp_config_rw);
    /* Verify the configuration takes effect */
    memset(&spmp_config_rw, 0, sizeof(spmp_config));
    __get_sPMPENTRYx(1, &spmp_config_rw);
    printf("Get spmp entry: index %d, prot_out: 0x%x, addr_out: 0x%x, order_out: %d\r\n", \
        1, spmp_config_rw.protection, spmp_config_rw.base_addr, spmp_config_rw.order);
    /* Drop to S mode */
    __switch_mode(PRV_S, smode_sp, supervisor_mode_entry_point);
    while(1);
#else
    printf("[ERROR]__TEE_PRESENT & __SPMP_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif/* defined(__TEE_PRESENT) && (__TEE_PRESENT == 1) */

    return 0;
}
