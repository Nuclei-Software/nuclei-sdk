#!/bin/env bash
SIMU_OPTS=${SIMU_OPTS:-"SIMULATION=1 SIMU=xlspike"}
RUNTARGET=${RUNTARGET-xlspike}
DATALOC=${DATALOC:-dlm}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)
COMMON_ENV=$(readlink -f $SCRIPTDIR/../env.sh)
DOBENCH_CONFLOC=$SCRIPTDIR

source $COMMON_ENV


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
    local BENCH_CMD="python3 $NSDK_BENCH_PY --appcfg $appcfg --hwcfg $hwcfg --parallel=-j --logdir $logdir --make_options \"$SIMU_OPTS $mkopts\" $RUN_OPTS"
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
    for core in n200 n300 n600 n900 nx600 nx900
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
    for core in n200 n300 n600 n900 nx600 nx900
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
        runbench_for_allcores $logdir/$runmode
        rundhry_for_allcores $logdir/$runmode
        unset RUNMODE
    done
}

function run_for_one {
    echo "Generate only for run mode=$RUNMODE"
    export RUNMODE=$RUNMODE
    local logdir=${1:-$LOGDIR}
    runbench_for_allcores $logdir/$RUNMODE
    rundhry_for_allcores $logdir/$RUNMODE

}

function prebench {
    echo "Do pre-bench steps"
    if [ "x$DATALOC" == "xilm" ] ; then
        local patch=$DOBENCH_CONFLOC/data_in_ilm.patch
        git am $patch
    fi
}

function postbench {
    echo "Do post-bench steps"
    if [ "x$DATALOC" == "xilm" ] ; then
        local stashmsg=$(git stash)
        git reset --hard HEAD~1
        if [[ "x$stashmsg" == *"xSaved"* ]] ; then
            git stash pop
        fi
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
