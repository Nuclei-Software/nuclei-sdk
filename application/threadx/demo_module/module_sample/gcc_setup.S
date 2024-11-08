
    .text
    .align 4

    .global _gcc_setup
_gcc_setup:

    ret

    /* Setup attibutes of heap section so it doesn't take up room in the elf file */
    .section .heap, "wa", %nobits
