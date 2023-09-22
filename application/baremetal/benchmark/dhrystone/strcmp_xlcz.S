// TODO: clang not yet provide any xxlcz instruction support
#if defined(__riscv_xxlcz) && !defined(__clang__)
.global strcmp_xlcz
strcmp_xlcz:
    xl.addrchk a0, a1, unalgns
loop:
    xl.lw a2, 4(a0)
    xl.lw a3, 4(a1)

    xl.lw a4, 4(a0)
    xl.lw a5, 4(a1)

    xl.lw a6, 4(a0)
    xl.lw a7, 4(a1)

    xl.lw t0, 4(a0)
    xl.lw t1, 4(a1)

    xl.bezm a2, a3, Lcomp1
    xl.bezm a4, a5, Lcomp2
    xl.bezm a6, a7, Lcomp3
    xl.bezm t0, t1, Lcomp4

    j loop

Lcomp1:
    xl.nzmsk a5, a2
    j ffnz

Lcomp2:
    xl.nzmsk a5, a4
    j ffnz

Lcomp3:
    xl.nzmsk a5, a6
    j ffnz

Lcomp4:
    xl.nzmsk a5, t0

ffnz:
    and a5, a5, t6
    xl.ffnz a0, a5
    ret

unalgns:
    xl.lbu a2, 1(a0)
    xl.lbu a3, 1(a1)
    bne a2, a3, get_res;
    bnez a2, unalgns

get_res:
    sub a0, a2, a3
    ret

#endif
