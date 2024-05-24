#!/bin/env bash

## Sample runs:
### Build for fpga: SIMU_OPTS= LOGROOT=gen/fpga RUNTARGET= ./run.sh
### Build for cycle model or rtl sim: LOGROOT=gen/rtl RUNTARGET= ./run.sh

SIMU_OPTS=${SIMU_OPTS-"SIMULATION=1 SIMU=xlspike"}
RUNTARGET=${RUNTARGET-xlspike}
DATALOC=${DATALOC:-dlm}
SOC=${SOC:-evalsoc}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)
COMMON_ENV=$(readlink -f $SCRIPTDIR/../env.sh)
DOBENCH_CONFLOC=$SCRIPTDIR

source $COMMON_ENV

MAKEOPTS="SOC=$SOC"

LINKSCRIPT="SoC/${SOC}/Board/nuclei_fpga_eval/Source/GCC/gcc_${SOC}_ilm.ld"

function genreport {
    local logdir=${1:-$LOGDIR}
    REPORT_CMD="python3 $NSDK_REPORT_PY --logdir $logdir --split"
    if [ "x$RUNTARGET" != "x" ] ; then
        REPORT_CMD="$REPORT_CMD --run"
    fi
    echo $REPORT_CMD
    if [[ $DRYRUN == 0 ]] ; then
        eval $REPORT_CMD
    fi
}

function runbench {
    local appcfg=$1
    local hwcfg=$2
    local logdir=$3
    local mkopts=${@:4}

    RUN_OPTS=""
    if [ "x$RUNTARGET" != "x" ] ; then
        RUN_OPTS="--run --run_target $RUNTARGET"
    fi
    local BENCH_CMD="python3 $NSDK_BENCH_PY --appcfg $appcfg --hwcfg $hwcfg --parallel=-j --logdir $logdir --make_options \"$MAKEOPTS $SIMU_OPTS $mkopts\" $RUN_OPTS"
    echo $BENCH_CMD
    if [[ $DRYRUN == 0 ]] ; then
        eval $BENCH_CMD
    fi
}

function runbench_for_allcores {
    local logdir=${1:-$LOGDIR}
    pushd $NSDK_ROOT
    echo "Run benchmark on different CORE"
    local runlogdir=$logdir/barebench
    for core in n100
    do
        echo "Build for CORE: $core"
        local appcfg=${DOBENCH_CONFLOC}/app.json
        local hwcfg=${DOBENCH_CONFLOC}/bench_${core}.json
        local corelogdir=$runlogdir/$core

        runbench $appcfg $hwcfg $corelogdir
    done
    genreport $runlogdir
    popd
}

function rundhry_for_allcores {
    local logdir=${1:-$LOGDIR}
    pushd $NSDK_ROOT
    echo "Run for dhrystone different modes"
    local runlogdir=$logdir/dhrystone
    for core in n100
    do
        for dhrymode in ground inline best
        do
            echo "Build for CORE: $core DHRY_MODE=$dhrymode"
            local appcfg=${DOBENCH_CONFLOC}/dhry.json
            local hwcfg=${DOBENCH_CONFLOC}/bench_${core}.json
            local corelogdir=$runlogdir/$dhrymode/$core
            local mkopts="DHRY_MODE=$dhrymode"

            runbench $appcfg $hwcfg $corelogdir $mkopts
        done
    done
    genreport $runlogdir
    popd
}

function run_for_allmodes {
    local logdir=${1:-$LOGDIR}
    echo "Generate for all run modes"
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
        runbench_for_allcores $logdir/$runmode
        rundhry_for_allcores $logdir/$runmode
        unset RUNMODE DOWNLOAD
    done
}

function run_for_one {
    echo "Generate only for run mode=$RUNMODE"
    export RUNMODE=$RUNMODE
    # for runmode=cache, need to run on sram area
    if [ "x$RUNMODE" == "xcache" ] ; then
        export DOWNLOAD=sram
    fi
    local logdir=${1:-$LOGDIR}
    runbench_for_allcores $logdir/$RUNMODE
    rundhry_for_allcores $logdir/$RUNMODE
    unset RUNMODE DOWNLOAD
}

function prebench {
    echo "Do pre-bench steps"
    if [ "x$DATALOC" == "xilm" ] ; then
        sed -i -e 's/REGION_ALIAS("DATA_LMA", ram)/REGION_ALIAS("DATA_LMA", ilm)/' ${NSDK_ROOT}/${LINKSCRIPT}
    fi
}

function postbench {
    echo "Do post-bench steps"
    if [ "x$DATALOC" == "xilm" ] ; then
        git stash
    fi
}

prebench

gen_logdir dobench
describe_env
if [ "x$RUNMODE" == "xall" ] ; then
    run_for_allmodes | tee $LOGDIR/build.log
else
    run_for_one | tee $LOGDIR/build.log
fi
postbench

zip_logdir
