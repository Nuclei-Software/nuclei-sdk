#!/bin/env bash

simulation=${simulation:-1}
silent=${silent:-1}
parallel=${parallel:-"-j"}
gen_elf_loc=${gen_elf_loc:-"gen"}
DRYRUN=${DRYRUN:-0}
TOOL_VER=${TOOL_VER:-2022.01}

NSDK_ROOT=${NSDK_ROOT:-}

DEVTOOL_ENV=${DEVTOOL_ENV:-/home/share/devtools/env.sh}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
if [ "x$NSDK_ROOT" == "x"  ] ; then
    NSDK_ROOT=$(readlink -f $SCRIPTDIR/../../..)
fi

function env_setup {
    if [ -f $DEVTOOL_ENV  ] ; then
        TOOL_VER=$TOOL_VER source $DEVTOOL_ENV
    else
        pushd $NSDK_ROOT
        source setup.sh
        popd
    fi
}

function do_bench {
    bench=$1
    core=$2
    series=$3
    stdclib=$4
    bench_rstloc=$5

    if [[ "x$bench_rstloc" != "x" ]] ; then
        bench_rstloc=$(readlink -f $bench_rstloc)
    fi
    pushd $bench

    export CORE=$core CPU_SERIES=$series STDCLIB=$stdclib SIMULATION=$simulation SILENT=$silent
    echo "Build $bench for CORE=$core CPU_SERIES=$series STDCLIB=$stdclib SIMULATION=$simulation SILENT=$silent"
    if [ "x$DRYRUN" == "x0" ] ; then
        make clean
        make $parallel info dasm > $bench_rstloc/build.log
        if [ $? == 0 ] && [[ "x$bench_rstloc" != "x" ]] ; then
            cp -f $bench.verilog $bench_rstloc
            cp -f $bench.elf $bench_rstloc
        fi
    fi
    unset CORE CPU_SERIES STDCLIB SIMULATION SILENT
    popd
}

function do_all_benches {
    for core_series in n205-200 n305-300 n300fd-300 n600-600 n600fd-600 n900-900 n900fd-900 nx600-600 nx600fd-600 nx900-900 nx900fd-900
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
                    bench_rstloc=$gen_elf_loc/$bench/${core}_${stdclib}
                    mkdir -p $bench_rstloc
                    pushd application/baremetal/benchmark > /dev/null
                    do_bench $bench $core $series $stdclib $bench_rstloc
                    popd > /dev/null
                fi
            done
        done
    done
}

function collect_buildinfo {
    benchbuild=$1
    date -u +"Build time: %Y-%m-%d, %H:%M:%S" > $benchbuild
    echo "Nuclei SDK Version: on $(git rev-parse --abbrev-ref HEAD) branch, $(git describe --always --abbrev=10 --dirty --tags 2>&1)" >> $benchbuild
    echo -n "GCC Version: " >> $benchbuild
    riscv-nuclei-elf-gcc -v >> $benchbuild 2>&1
}

gen_elf_loc=$(readlink -f $gen_elf_loc)
benchzip=genbench_$(date -u +"%Y%m%d-%H%M%S").zip
benchbuild=$gen_elf_loc/build.txt
benchlog=$gen_elf_loc/build.log

echo "Remove previous build ${gen_elf_loc}"
rm -rf $gen_elf_loc
mkdir $gen_elf_loc

echo "Nuclei SDK location: $NSDK_ROOT"

pushd $NSDK_ROOT

env_setup

echo "Do all benchmarks and generate bench to folder ${gen_elf_loc}"
do_all_benches | tee ${benchlog}

echo "Collect bench build information"
collect_buildinfo $benchbuild
popd

echo "collect all generated elf and verilog files to $benchzip"
zip -q -r $benchzip $gen_elf_loc
echo "Copy to your local machine using command below"
echo "scp $(whoami)@$(hostname):$(pwd)/$benchzip ."

