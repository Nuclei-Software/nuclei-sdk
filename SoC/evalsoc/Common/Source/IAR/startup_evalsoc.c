#include <stdlib.h>
#include "evalsoc.h"

extern __weak void eclic_msip_handler();
extern __weak void eclic_mtip_handler();
extern void default_intexc_handler();

typedef void(*__fp)();

/* If rodata is placed in real readonly section,
 * if you want to register vector interrupt with new entry,
 * you need to place it in ram
 */
#pragma data_alignment = 256
static const __fp vector_base[] = {
    0,
    0,
    0,
    eclic_msip_handler,
    0,
    0,
    0,
    eclic_mtip_handler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
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

    /* No need to call it again, since it is initialized */
    return 0;
}
