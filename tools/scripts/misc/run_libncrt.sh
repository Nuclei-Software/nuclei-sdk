#!/bin/env bash
CORE=${CORE:-n300}
BENCH=${BENCH:-dhrystone}
CYCM=${CORE}_best_config_cymodel_latest
MKOPTS="CORE=$CORE SIMU=xlspike SIMULATION=1 RISCV_TUNE= "

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)
COMMON_ENV=$(readlink -f $SCRIPTDIR/env.sh)

source $COMMON_ENV

OLDPATH=$PATH

function setup_gcc {
    local GCC_VER=${1:-latest}
    TOOL_VER=$GCC_VER source $DEVTOOL_ENV
}

function reset_gcc {
    export PATH=$OLDPATH
}

function build_app {
    local clib=$1

    local makeopts="STDCLIB=$clib $MKOPTS"

    runcmd="make $makeopts clean all"
    echo $runcmd
    if [ "x$DRYRUN" == "x0" ] ; then
        eval $runcmd
    fi
}

function run_app {
    local app=$1

    runcmd="$CYCM $app"
    echo $runcmd
    if [ "x$DRYRUN" == "x0" ] ; then
        eval $runcmd
    fi
}

function do_test {
    local app=${1:-dhrystone}
    local applogdir=$logdir/$app

    local appbuildlog=$applogdir/build.log
    local apprunlog=$applogdir/run.log
    mkdir -p $applogdir
    for cfg in newlib_small libncrt_small libncrt_balanced libncrt_fast
    do
        echo "Build $app for CORE=$CORE STDCLIB=$cfg"
        echo "==>Build for $cfg" >> $appbuildlog
        build_app $cfg >> $appbuildlog
        local bkapp=$applogdir/${app}_$cfg
        cp $app.elf $bkapp.elf
        echo "Run $app for CORE=$CORE STDCLIB=$cfg"
        local appsz=$(riscv-nuclei-elf-size $bkapp.elf | tail -1)
        echo "CSV, $appsz" >> $apprunlog
        run_app $app.elf >>  $apprunlog 2>&1
    done
    cat $apprunlog | grep CSV
}

function do_test_on_gcc {
    local GCC_VER=${1:-latest}
    local app=${2:-dhrystone}

    logdir=$LOGDIR/$GCC_VER
    buildtxt=$logdir/build.txt
    runlog=$logdir/run.log

    rm -rf $logdir
    mkdir -p $logdir

    setup_gcc $GCC_VER
    riscv-nuclei-elf-gcc -v > $buildtxt 2>&1

    do_test $app | tee $runlog

    reset_gcc
}

gen_logdir "libncrt_$BENCH"


function do_bench_on_gcc {
    local dhryappdir=$NSDK_ROOT/application/baremetal/benchmark/$BENCH
    pushd $dhryappdir
    local totallog=$LOGDIR/run.log
    for ver in 2021.11-eng1 2022.01 2022.04 2022.05-eng2
    do
        echo "Run on GCC $ver"
        do_test_on_gcc $ver $BENCH
    done | tee $totallog
    popd
    zip_logdir
}

do_bench_on_gcc
