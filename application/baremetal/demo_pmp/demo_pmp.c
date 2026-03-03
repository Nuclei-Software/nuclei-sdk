// See LICENSE for license details.
#include <stdio.h>
#include <string.h>
#include "nuclei_sdk_soc.h"

#if !defined(__PMP_PRESENT) || (__PMP_PRESENT == 0)
/* __PMP_PRESENT should be defined in <Device>.h */
#warning "__PMP_PRESENT is not defined or equal to 0, please check!"
#warning "This example require CPU PMP feature!"
#endif

// NOTE: This example require exception feature
// But for different interrupt type such as eclic or plic, interrupt
// and exception code is different, you need to pass correct interrupt type
// If you dont have ECLIC present, you **MUST** pass XLCFG_ECLIC=0 to disable
// ECLIC pushmcause/pushmepc/pushmsubm code used in intexc_evalsoc.S code

/* different trigger condition */
#define INSTRUCTION_FETCH_EXCEPTION    0
#define LOAD_EXCEPTION                 1
#define STORE_EXCEPTION                2
#define RUN_WITH_NO_PMP_CHECK          3

// 2048 is enough
#define SMODE_STACK_SIZE        2048
#define UMODE_STACK_SIZE        SMODE_STACK_SIZE

typedef void(*__funcpt)(void);

#ifndef __ICCRISCV__
#define __PMP_PROTECT   __attribute__((section (".text"), aligned(0x2000)))
#else
/* IAR CC currently don't support align function in section,
 * so we provide a workaround using a customized iar_evalsoc_ilm.icf in this demo
 * we define a block called PMPFUNC alignment set to 0x2000 */
#define __PMP_PROTECT   __attribute__((section (".text.pmpfunc")))
#endif

volatile uint8_t protected_data[0x2000] __attribute__((aligned(0x2000))) = \
{0xaa, 0x1, 0x02, 0x03, 0x04, 0x05, 0x06, 0xaa};

/* Create a stack for supervisor mode execution */
uint8_t smode_stack[SMODE_STACK_SIZE] __attribute__((aligned(16)));
uintptr_t smode_sp = (uintptr_t) (smode_stack + sizeof(smode_stack));

/* Create a stack for user mode execution */
uint8_t umode_stack[UMODE_STACK_SIZE] __attribute__((aligned(16)));
uintptr_t umode_sp = (uintptr_t) (umode_stack + sizeof(umode_stack));

/* Configuration of execution region*/
pmp_config pmp_config_x = {
    /*
     * Locked PMP entries remain locked until the hart is reset,
     * the L bit also indicates whether the R/W/X permissions are enforced on M-mode accesses
     */
    .protection = PMP_R | PMP_W | PMP_X,
    /* Initial protected excutable address range is 2^14 = 16K bytes*/
    .order = 14, // 2^14 = 0x4000 = 16KBytes
    /* Initial base address is 0 */
    .base_addr = 0,
};

/* configuration of read/write data region*/
pmp_config pmp_config_rw = {
    .protection = PMP_R | PMP_W,
    /* Initial protected readable/writable address range is 2^13 = 8K bytes */
    .order = 13, // 2^13 = 0x2000 = 8KBytes
    .base_addr = 0,
};

/* configuration of read/write data region that not protected */
pmp_config pmp_config_rw_unprotected = {
    .protection = PMP_R | PMP_W | PMP_X,
    .order = __RISCV_XLEN,
    .base_addr = (unsigned long)protected_data + 0x2000,
};

__STATIC_FORCEINLINE void retrieve_full_permission(void)
{
    /* retrieve full permission to S mode*/
    pmp_config_x.protection = PMP_R | PMP_W | PMP_X;
    pmp_config_x.order = __RISCV_XLEN;
    pmp_config_x.base_addr = 0;

    pmp_config_rw.protection = PMP_R | PMP_W;
    pmp_config_rw.order = 13;
    pmp_config_rw.base_addr = 0;

    __set_PMPENTRYx(0, &pmp_config_x);
    __set_PMPENTRYx(1, &pmp_config_rw);
}

void print_sp_judge_privilege_mode(void)
{
    uintptr_t sp;

    __asm__ volatile("add  %0, x0, sp" :"=r"(sp));

    printf("Current sp is 0x%lx, ", (unsigned long)sp);

    if ( ((uint8_t *)sp <= &(smode_stack[SMODE_STACK_SIZE - 1])) && ((uint8_t *)sp >= &(smode_stack[0])) ) {
        printf("so it is in Supervisor Mode!\r\n");
    } else if ( ((uint8_t *)sp <= &(umode_stack[UMODE_STACK_SIZE - 1])) && ((uint8_t *)sp >= &(umode_stack[0])) ) {
        printf("so it is in User Mode!\r\n");
    } else {
        printf("so it is in Machine Mode!\r\n");
    }
}

void __PMP_PROTECT protected_execute(void)
{
    printf("Attempting to read protected_data[0]\r\n");
    printf("protected_data[0]: 0x%0X succeed\r\n", protected_data[0]);

    printf("Attempting to write protected_data[0]\r\n");
    protected_data[0] = 0xFF;

    printf("Won't run here if violates L R\\W\\X permission check!\r\n");
}

/* In case compiler use inline optimization of protected_execute */
__funcpt fncptr = ((__funcpt)protected_execute);

static void set_pmp_entry(void)
{
    pmp_config_x.base_addr = (unsigned long)protected_execute;
    pmp_config_rw.base_addr = (unsigned long)protected_data;

    __set_PMPENTRYx(0, &pmp_config_x);
    /* Verify the configuration takes effect */
    memset(&pmp_config_x, 0, sizeof(pmp_config));
    __get_PMPENTRYx(0, &pmp_config_x);
    printf("Get pmp entry: index %d, prot_out: 0x%x, addr_out: 0x%lx, order_out: %lu\r\n", \
        0, pmp_config_x.protection, pmp_config_x.base_addr, pmp_config_x.order);

    __set_PMPENTRYx(1, &pmp_config_rw);
    /* Verify the configuration takes effect */
    memset(&pmp_config_rw, 0, sizeof(pmp_config));
    __get_PMPENTRYx(1, &pmp_config_rw);
    printf("Get pmp entry: index %d, prot_out: 0x%x, addr_out: 0x%lx, order_out: %lu\r\n", \
        1, pmp_config_rw.protection, pmp_config_rw.base_addr, pmp_config_rw.order);
}

static void trigger_pmp_exception(unsigned long exec_type)
{
    print_sp_judge_privilege_mode();
    __ASM volatile(
        "addi a0, %0, 0\n\t"
        "ecall"
        : /* no outputs */
        : "r"(exec_type)
        : "a0"
    );
    fncptr();
    while(1);
}

__STATIC_FORCEINLINE void trigger_instruction_exception(void)
{
    trigger_pmp_exception(INSTRUCTION_FETCH_EXCEPTION);
}

__STATIC_FORCEINLINE void trigger_load_exception(void)
{
    trigger_pmp_exception(LOAD_EXCEPTION);
}

__STATIC_FORCEINLINE void trigger_store_exception(void)
{
    trigger_pmp_exception(STORE_EXCEPTION);
}

__STATIC_FORCEINLINE void trigger_no_pmp_check(void)
{
    trigger_pmp_exception(RUN_WITH_NO_PMP_CHECK);
}

static void pmp_violation_fault_handler(unsigned long mcause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;
    CSR_MSTATUS_Type mstatus;
    mstatus.d = __RV_CSR_READ(CSR_MSTATUS);
    if (PRV_S == mstatus.b.mpp) {
        printf("\r\nFrom S mode traps into M mode handler\r\n");
    } else if (PRV_U == mstatus.b.mpp) {
        printf("\r\nFrom U mode traps into M mode handler\r\n");
    }
    switch (mcause & MCAUSE_CAUSE) {
        case InsAccFault_EXCn:
            printf("Instruction access fault occurs, mcause: 0x%lx, mepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            retrieve_full_permission();
            if (PRV_S == mstatus.b.mpp) {
                __switch_mode(PRV_S, smode_sp, trigger_load_exception);
            } else if (PRV_U == mstatus.b.mpp) {
                __switch_mode(PRV_U, umode_sp, trigger_load_exception);
            }
            break;
        case LdFault_EXCn:
            printf("Load access fault occurs, mcause: 0x%lx, mepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            retrieve_full_permission();
            if (PRV_S == mstatus.b.mpp) {
                __switch_mode(PRV_S, smode_sp, trigger_store_exception);
            } else if (PRV_U == mstatus.b.mpp) {
                __switch_mode(PRV_U, umode_sp, trigger_store_exception);
            }
            break;
        case StAccessFault_EXCn:
            printf("Store/AMO access fault occurs, mcause: 0x%lx, mepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            retrieve_full_permission();
            if (PRV_S == mstatus.b.mpp) {
                __switch_mode(PRV_S, smode_sp, trigger_no_pmp_check);
            } else if (PRV_U == mstatus.b.mpp) {
                __switch_mode(PRV_U, umode_sp, trigger_no_pmp_check);
            }
            break;
        case SmodeEcall_EXCn:
        case UmodeEcall_EXCn:
            printf("Environment call occurs, mcause: 0x%lx, mepc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);
            pmp_config_x.order = 14;
            // configure pmp entry for different trigger condition
            if (INSTRUCTION_FETCH_EXCEPTION == exc_frame->a0) {
                /* 1. trigger instruction fetch exception */
                /* Remove X permission of protected_execute region */
                pmp_config_x.protection = PMP_R | PMP_W;
                pmp_config_rw.protection = PMP_R | PMP_W;
                set_pmp_entry();
            } else if (LOAD_EXCEPTION == exc_frame->a0) {
                /* 2. trigger load exception */
                /* retrieve X permission of protected_execute region */
                pmp_config_x.protection = PMP_R | PMP_W | PMP_X;
                /* Remove R permission of protected_data region */
                pmp_config_rw.protection = 0;
                set_pmp_entry();
            } else if (STORE_EXCEPTION == exc_frame->a0) {
                /* 3. trigger store exception */
                /* retrieve X permission of protected_execute region */
                pmp_config_x.protection = PMP_R | PMP_W | PMP_X;
                /* Remove W permission of protected_data region */
                pmp_config_rw.protection = PMP_R;
                set_pmp_entry();
            } else if (RUN_WITH_NO_PMP_CHECK == exc_frame->a0) {
                /* 4. trigger no pmp check */
                /* any S-mode access will succeed */
                pmp_config_x.protection = PMP_R | PMP_W | PMP_X;
                pmp_config_rw.protection = PMP_R | PMP_W ;
                pmp_config_x.order = __RISCV_XLEN;
                pmp_config_rw.order = __RISCV_XLEN;
                set_pmp_entry();
            }
            exc_frame->epc += 4 ; // ecall reserves 4 bytes, add 4 to mepc to next instruction
            break;
        default: break;
    }
#ifdef CFG_SIMULATION
    // directly exit if in nuclei internally simulation
    SIMULATION_EXIT(0);
#endif
}


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

    // initial pmp entries
    __set_PMPENTRYx(0, &pmp_config_x);
    __set_PMPENTRYx(1, &pmp_config_rw);
    __set_PMPENTRYx(2, &pmp_config_rw_unprotected);

    printf("------PMP demo with trigger common condition------\r\n");

    /* register corresponding exception */
    Exception_Register_EXC(InsAccFault_EXCn, (unsigned long)pmp_violation_fault_handler);
    Exception_Register_EXC(LdFault_EXCn, (unsigned long)pmp_violation_fault_handler);
    Exception_Register_EXC(StAccessFault_EXCn, (unsigned long)pmp_violation_fault_handler);

#if defined(__SMODE_PRESENT) && (__SMODE_PRESENT == 1)
    if (0 == misa.b.s) {
        printf("You expect SMODE present, but SMODE is not present, will not run this example!\r\n");
        printf("You can rebuild and run this example with extra make option XLCFG_SMODE=0 if SMODE not present!\r\n");
        return 0;
    }
    Exception_Register_EXC(SmodeEcall_EXCn, (unsigned long)pmp_violation_fault_handler);
    printf("Drop to S-Mode now\n\r");
    /* Drop to S mode */
    __switch_mode(PRV_S, smode_sp, trigger_instruction_exception);
#else
    if (1 == mcfg_info.b.tee) {
        printf("TEE/SMPU is present, you must pass XLCFG_SMODE=1, or else configure the SMPU entries(see demo exception_umode), will not run this example!\r\n");
        return 0;
    }
    Exception_Register_EXC(UmodeEcall_EXCn, (unsigned long)pmp_violation_fault_handler);
    printf("Drop to U-Mode now\n\r");
    /* Drop to S mode */
    __switch_mode(PRV_U, umode_sp, trigger_instruction_exception);
#endif

#else
    printf("[ERROR]__PMP_PRESENT must be defined as 1 in <Device>.h!\r\n");
#endif
    return 0;
}

