#!/bin/env bash

# how to use
# cd <Nuclei SDK>/application/baremetal/benchmark/coremark
# /path/to/build_xlcz.sh [CORE] [BASEEXT] [TOLOC]

# CORE used to run on
CORE=${1:-n300f}
# BASE Extension on this CORE
BASEEXT=${2:-_zba_zbb_zbc_zbs}
# Copy to location
TOLOC=${3:-${HOME}/benchelfs/xlcz}

DATE=$(date +%Y%m%d)

CASE=$(basename $(pwd))

# for whetstone need to pass TIMEOUT=20s /path/to/build_xlcz.sh
if [ "x$CASE" = "xwhetstone" ] ; then
    TIMEOUT=${TIMEOUT:-20s}
else
    TIMEOUT=${TIMEOUT:-3s}
fi

mkdir -p $TOLOC

echo "INFO: Benchmark $CASE run on $CORE with base extension $BASEEXT, generated elfs will be copy to $TOLOC, TIMEOUT $TIMEOUT"

ZCF="_zcf"
if [[ "$CORE" == *"x"* ]] ; then
    ZCF=""
fi

echo "text data bss dec filename cminscnt xlczinscnt valonqemu valonrtl"
for ext in "" _xxlczbitop _xxlcz_nobitop _xxlcz _zca_zcb${ZCF}_zcmp_zcmt _zca_zcb${ZCF}_zcmp_zcmt_xxlcz ; do
    archext=${BASEEXT}$ext
    runarchext=$archext
    toelf=${TOLOC}/${CASE}_${CORE}${archext}_${DATE}_sim.elf
    makeopts=""
    if [[ "$ext" == *"_nobitop"* ]] ; then
        makeopts="APP_COMMON_FLAGS=\"-DNO_EXTRACTU\""
        archext=${archext/_nobitop/}
    fi
    if [[ "$ext" == *"bitop"* ]] ; then
        runarchext=${archext/bitop/}
    fi

    #echo "Build for ${CORE}${archext} -> $toelf"
    outstr=$(make CORE=$CORE ARCH_EXT=$archext $makeopts SIMULATION=1 SILENT=1 clean all | tail -n1)
    read -a parts <<< "$outstr"
    szstr="${parts[@]:0:4}"
    outstr=$(timeout --foreground ${TIMEOUT} make CORE=$CORE ARCH_EXT=$runarchext SIMULATION=1 SILENT=1 run_qemu 2>&1 | grep "CSV" | tail -n1)
    read -a parts <<< "$outstr"
    benchval=${parts[-1]}
    command cp -f ${CASE}.elf $toelf
    xlczinscnt=$(riscv64-unknown-elf-objdump -d -M no-aliases $toelf | grep -e "xl\." | wc -l)
    cminscnt=$(riscv64-unknown-elf-objdump -d -M no-aliases $toelf | grep -e "[c|cm]\." | wc -l)
    echo "$szstr $toelf $cminscnt $xlczinscnt $benchval"
done
