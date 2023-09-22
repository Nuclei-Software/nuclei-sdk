#include <stdlib.h>
#include <string.h>
#include "evalsoc.h"

extern __weak void eclic_msip_handler(void);
extern __weak void eclic_mtip_handler(void);
extern void default_intexc_handler(void);

/*
 * Uncomment it if your vector table is placed in readonly section,
 * and you have defined .mintvec_rw section in a writable section in iar
 * linker icf file
 * In this implementation, when use with iar_evalsoc_flashxip.icf, you can
 * try this FLASH_RAM_VECTOR
 */
// #define FLASH_RAM_VECTOR        1

typedef void(*__fp)(void);

/* If .mintvec section is placed in real readonly section,
 * if you want to register vector interrupt with new entry,
 * you need to place it in writable section or create a ram vector
 * after bootup.
 * This alignment is set to 256 byte for up to 64 interrupts,
 * If you have more interrupts, you need to adjust the alignment
 * to other value, for details, please check mtvt csr documented
 * in Nuclei RISC-V ISA Spec
 */
#pragma data_alignment = 256
static const __fp vector_base[SOC_INT_MAX] __attribute__((section (".mintvec"))) = {
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    eclic_msip_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    eclic_mtip_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler,
    default_intexc_handler
};

#if defined(FLASH_RAM_VECTOR)
#pragma data_alignment = 256
static __fp vector_base_ram[SOC_INT_MAX] __attribute__((section (".mintvec_rw")));

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
static unsigned long vector_base_s_ram[SOC_INT_MAX] __attribute__((section (".sintvec_rw")));
extern const unsigned long vector_table_s[SOC_INT_MAX];
#endif

static void prepare_ram_vector(void)
{
    memcpy((void *)vector_base_ram, (const void *)vector_base, (size_t)(sizeof(__fp) * SOC_INT_MAX));
    __RV_CSR_WRITE(CSR_MTVT, (unsigned long)(&vector_base_ram));
#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
    memcpy((void *)vector_base_s_ram, (const void *)vector_table_s, (size_t)(sizeof(unsigned long) * SOC_INT_MAX));
    __RV_CSR_WRITE(CSR_STVT, (unsigned long)(&vector_base_s_ram));
#endif
}
#endif

extern void exc_entry(void);
extern void irq_entry(void);
extern void _premain_init(void);
extern void early_exc_entry(void);

#ifndef IAR_DATA_INIT
#define IAR_DATA_INIT _GLUE(__iar_data_init, _DLIB_ELF_INIT_INTERFACE_VERSION)
#endif

extern void IAR_DATA_INIT(void);

int __low_level_init(void)
{
    __disable_interrupt();

    /* Set the the NMI base to share with mtvec by setting CSR_MMISC_CTL */
    __RV_CSR_SET(CSR_MMISC_CTL, MMISC_CTL_NMI_CAUSE_FFF);

#if defined(__riscv_zcmp) || defined(__riscv_zcmt)
   /* Enable Zc feature when compiled zcmp & zcmt */
    __RV_CSR_SET(CSR_MMISC_CTL, MMISC_CTL_ZC);
#endif

    /* Intialize ECLIC vector interrupt base address mtvt to vector_base */
    __RV_CSR_WRITE(CSR_MTVT, (unsigned long)(&vector_base));
    /*
     * Set ECLIC non-vector entry to be controlled by mtvt2 CSR register.
     * Intialize ECLIC non-vector interrupt base address mtvt2 to irq_entry.
     */
    __RV_CSR_WRITE(CSR_MTVT2, 0x1|(unsigned long)(&irq_entry));

    /* Set Exception Entry MTVEC to early_exc_entry
     * Due to settings above, Exception and NMI
     * will share common entry.
     * This early_exc_entry is only used during early
     * boot stage before main */
    __RV_CSR_WRITE(CSR_MTVEC, ((unsigned long)&exc_entry));
    __RV_CSR_CLEAR(CSR_MTVEC, 0x3f);
    __RV_CSR_SET(CSR_MTVEC, 0x3);

    /* Enable FPU and Vector Unit if f/d/v exist in march */
#if defined(__riscv_flen) && __riscv_flen > 0
    /* Enable FPU, and set state to initial */
    __RV_CSR_CLEAR(CSR_MSTATUS, MSTATUS_FS);
    __RV_CSR_SET(CSR_MSTATUS, MSTATUS_FS_INITIAL);
#endif

#if defined(__riscv_vector)
    /* Enable Vector, and set state to initial */
    __RV_CSR_CLEAR(CSR_MSTATUS, MSTATUS_VS);
    __RV_CSR_SET(CSR_MSTATUS, MSTATUS_VS_INITIAL);
#endif

    /* Enable mcycle and minstret counter */
    __RV_CSR_CLEAR(CSR_MCOUNTINHIBIT, 0x5);

    /* Call IAR Internal data initial function */
    IAR_DATA_INIT();

    /*
     * You can place it before calling IAR_DATA_INIT
     * if in this SystemInit function use no global variable.
     * Call vendor defined SystemInit to
     * initialize the micro-controller system
     * SystemInit will just be called by boot cpu
     */
    SystemInit();

    /* Get CPU frequency and initialize uart for print */
    _premain_init();

    /* Prepare ram vector table for initial vector table located in readonly section case */
#if defined(FLASH_RAM_VECTOR)
    prepare_ram_vector();
#endif

#ifdef RTOS_RTTHREAD
    /* Directly jump to rtthread startup process, no longer return */
    extern int rtthread_startup(void);
    rtthread_startup();
#endif

    /* No need to call it again, since it is initialized */
    return 0;
}
