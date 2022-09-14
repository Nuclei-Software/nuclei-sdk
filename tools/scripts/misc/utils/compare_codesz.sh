#!/bin/env bash
GENROOT=${GENROOT:-cmpgenelfs}
CMODEL=${CMODEL:-medany}
LOCAL_BENCH_FLAGS=${BFLAGS:-"-Os -Wno-implicit"}

function compare_pulp() {
    PREFIX=${PREFIX-/Local/home/zhaofujin/pulp/pulp/pulp_gcc10/bin/riscv32-unknown-elf-}
    GENELFDIR=${GENROOT}/pulp
    mkdir -p ${GENELFDIR}
    appname=$(basename $(pwd))
    echo "Benchmark flags is $LOCAL_BENCH_FLAGS, Code Model is $CMODEL"
    echo "Toolchain is pulp, located in $PREFIX"
    for arch in rv32imc rv32imc_xpulpv3 rv32imc_xnucleiv1 rv32imac rv32imac_xnucleiv1
    do
        export BENCH_FLAGS=${LOCAL_BENCH_FLAGS}
        make COMPILE_PREFIX=${PREFIX} RISCV_ARCH=${arch} RISCV_ABI=ilp32 RISCV_CMODEL=${CMODEL} RISCV_TUNE= SILENT=1 clean all dasm 2>&1 | grep 'elf$\|Conf'
        [ -f ${appname}.elf ] && cp -f ${appname}.elf ${GENELFDIR}/${appname}_${arch}.elf
        [ -f ${appname}.elf ] && cp -f ${appname}.dasm ${GENELFDIR}/${appname}_${arch}.dasm
        [ -f ${appname}.elf ] && cp -f ${appname}.map ${GENELFDIR}/${appname}_${arch}.map
        make SILENT=1 clean > /dev/null 2>&1
        unset BENCH_FLAGS RISCV_CMODEL
    done
    unset PREFIX
}

function compare_plctzc() {
    PREFIX=${PREFIX-/Local/home/zhaofujin/pulp/gcc-zc/build/plct_gcczc3/bin/riscv32-unknown-elf-}
    GENELFDIR=${GENROOT}/plctzc
    mkdir -p ${GENELFDIR}
    appname=$(basename $(pwd))
    echo "Benchmark flags is $LOCAL_BENCH_FLAGS, Code Model is $CMODEL"
    echo "Toolchain is plct zc, located in $PREFIX"
    for arch in rv32imac rv32ima_zca_zcb_zcf_zcmp_zcmt rv32imaf_zca_zcb_zcf_zcmp_zcmt rv32imafd_zca_zcb_zcf_zcmp_zcmt
    do
        export BENCH_FLAGS=${LOCAL_BENCH_FLAGS}
        if [[ "$arch" =~ .*"imafd_".* ]] ; then
            ABI_OPTS="RISCV_ABI=ilp32d"
        elif [[ "$arch" =~ .*"imaf_".* ]] ; then
            ABI_OPTS="RISCV_ABI=ilp32f"
        else
            ABI_OPTS="RISCV_ABI=ilp32"
        fi
        make COMPILE_PREFIX=${PREFIX} RISCV_ARCH=${arch} ${ABI_OPTS} RISCV_CMODEL=${CMODEL} APP_COMMON_FLAGS="-misa-spec=2.2" RISCV_TUNE= SILENT=1 clean all dasm 2>&1 | grep 'elf$\|Conf'
        [ -f ${appname}.elf ] && cp -f ${appname}.elf ${GENELFDIR}/${appname}_${arch}.elf
        [ -f ${appname}.elf ] && cp -f ${appname}.dasm ${GENELFDIR}/${appname}_${arch}.dasm
        [ -f ${appname}.elf ] && cp -f ${appname}.map ${GENELFDIR}/${appname}_${arch}.map
        make SILENT=1 clean > /dev/null 2>&1
        unset BENCH_FLAGS RISCV_CMODEL ABI_OPTS
    done
    unset PREFIX
}


function compare_toolchain {
    TOOLCHAIN=${1:-terapines}
    GENELFDIR=${GENROOT}/${TOOLCHAIN}
    mkdir -p ${GENELFDIR}
    appname=$(basename $(pwd))
    echo "Toolchain is using $TOOLCHAIN"
    echo "Benchmark flags is $LOCAL_BENCH_FLAGS, Code Model is $CMODEL"
    for core in n300 n300f n300fd
    do
        export BENCH_FLAGS=${LOCAL_BENCH_FLAGS}
        export RISCV_CMODEL=${CMODEL}
        make CORE=${core} TOOLCHAIN=${TOOLCHAIN} SILENT=1 clean all dasm 2>&1 | grep 'elf$\|Conf'
        make CORE=${core} TOOLCHAIN=${TOOLCHAIN} SILENT=1 showflags 2>&1 | grep 'CFLAGS'
        [ -f ${appname}.elf ] && cp -f ${appname}.elf ${GENELFDIR}/${appname}_${core}.elf
        [ -f ${appname}.elf ] && cp -f ${appname}.dasm ${GENELFDIR}/${appname}_${core}.dasm
        [ -f ${appname}.elf ] && cp -f ${appname}.map ${GENELFDIR}/${appname}_${core}.map
        make CORE=${core} TOOLCHAIN=${TOOLCHAIN} SILENT=1 clean > /dev/null 2>&1
        unset BENCH_FLAGS RISCV_CMODEL
    done
}

mkdir -p ${GENROOT}
compare_pulp | tee ${GENROOT}/build.log
compare_plctzc | tee -a ${GENROOT}/build.log
compare_toolchain terapines | tee -a ${GENROOT}/build.log
compare_toolchain nuclei_gnu | tee -a ${GENROOT}/build.log
