#!/bin/env bash

simulation=${simulation:-1}
simu=${simu:-xlspike}
silent=${silent:-1}
parallel=${parallel:-"-j"}


SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)
COMMON_ENV=$(readlink -f $SCRIPTDIR/env.sh)

source $COMMON_ENV

gen_logdir benchmark
describe_env

function do_bench {
    local bench=$1
    local core=$2
    local series=$3
    local stdclib=$4
    local bench_rstloc=$5

    if [[ "x$bench_rstloc" != "x" ]] ; then
        bench_rstloc=$(readlink -f $bench_rstloc)
    fi
    pushd $bench

    export CORE=$core CPU_SERIES=$series STDCLIB=$stdclib SIMULATION=$simulation SILENT=$silent SIMU=$simu
    echo "Build $bench for CORE=$core CPU_SERIES=$series STDCLIB=$stdclib SIMULATION=$simulation SIMU=$simu SILENT=$silent"
    if [ "x$DRYRUN" == "x0" ] ; then
        make clean
        echo "record build log in $bench_rstloc/build.log"
        make $parallel info showflags dasm > $bench_rstloc/build.log 2>&1
        if [ $? == 0 ] && [[ "x$bench_rstloc" != "x" ]] ; then
            cp -f $bench.verilog $bench_rstloc
            cp -f $bench.dump $bench_rstloc
            cp -f $bench.elf $bench_rstloc
        fi
    fi
    unset CORE CPU_SERIES STDCLIB SIMULATION SILENT SIMU
    popd
}

function do_all_benches {
    local genloc=${1:-$gen_elf_loc}
    for core_series in n205-200 n305-300 n300fd-300 n600-600 n600fd-600 n900-900 n900fd-900 nx600-600 nx600fd-600 nx900-900 nx900fd-900 ux1000fd-1000
    do
        for stdclib in newlib_small libncrt_small
        do
            for bench in coremark whetstone dhrystone
            do
                tmparr=(${core_series//-/ })
                core=${tmparr[0]} series=${tmparr[1]}
                if [[ "$core" == *"x"* ]] && [[ "$stdclib" == "libncrt"* ]] ; then
                    echo "Ignore build for $bench: CORE=$core CPU_SERIES=$series STDCLIB=$stdclib"
                else
                    echo "Do build for $bench: CORE=$core CPU_SERIES=$series STDCLIB=$stdclib"
                    bench_rstloc=$genloc/$bench/${core}_${stdclib}
                    mkdir -p $bench_rstloc
                    pushd application/baremetal/benchmark
                    if [[ "$bench" == "dhrystone" ]] ; then
                        for dhry_mode in ground inline best
                        do
                            export DHRY_MODE=$dhry_mode
                            echo "Do build for dhrystone DHRY_MODE=$dhry_mode"
                            dhry_bench_rstloc=${bench_rstloc}/${dhry_mode}
                            mkdir -p $dhry_bench_rstloc
                            do_bench $bench $core $series $stdclib $dhry_bench_rstloc
                            unset DHRY_MODE
                        done
                    else
                        do_bench $bench $core $series $stdclib $bench_rstloc
                    fi
                    popd
                fi
            done
        done
    done
}

function dobench_for_runmode {
    local genloc=${1:-$gen_elf_loc}
    for runmode in "" lm icdlm dcilm cache bus clm
    do
        echo "Generate RUNMODE=$runmode"
        export RUNMODE=$runmode
        if [ "x$RUNMODE" == "x" ] ; then
            runmode=default
        fi
        # for runmode=cache, need to run on sram area
        if [ "x$RUNMODE" == "xcache" ] ; then
            export DOWNLOAD=sram
        fi
        do_all_benches $genloc/$runmode
        unset RUNMODE DOWNLOAD
    done
}

function collect_buildinfo {
    local benchbuild=$1
    date -u +"Build time: %Y-%m-%d, %H:%M:%S" >> $benchbuild
    echo "Nuclei SDK Version: on $(git rev-parse --abbrev-ref HEAD) branch, $(git describe --always --abbrev=10 --dirty --tags 2>&1)" >> $benchbuild
    echo -n "GCC Version: " >> $benchbuild
    riscv-nuclei-elf-gcc -v >> $benchbuild 2>&1
    riscv64-unknown-elf-gcc -v >> $benchbuild 2>&1
}

gen_elf_loc=${LOGDIR}

gen_elf_loc=$(readlink -f $gen_elf_loc)
benchbuild=$gen_elf_loc/build.txt
benchlog=$gen_elf_loc/build.log

pushd $NSDK_ROOT

#echo "Do all benchmarks and generate bench to folder ${gen_elf_loc}"
#do_all_benches | tee ${benchlog}
echo "Do all benchmarks and generate bench for various RUNMODE to folder ${gen_elf_loc}"
dobench_for_runmode | tee ${benchlog}

echo "Collect bench build information"
collect_buildinfo $benchbuild
popd

echo "collect all generated elf and verilog files to $LOGZIP"
zip_logdir

