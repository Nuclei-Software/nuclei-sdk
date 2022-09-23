#!/bin/env bash
MAKE_OPTS=${MAKE_OPTS:-""}
RUNON=${RUNON-fpga}
CONFIG=${CONFIG-"n900,ux900"}
RUNYAML=${RUNYAML-}
BACKUP=${BACKUP:-Backups}
CFGSET=${CFGSET:-full}
BITSET=${BITSET:-latest}
RUNMODE=${RUNMODE-all}
VERBOSE=${VERBOSE-}

MYSCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
MYSCRIPTDIR=$(readlink -f $MYSCRIPTDIR)
COMMON_ENV=$(readlink -f $MYSCRIPTDIR/../env.sh)
FPGALOC=
CFGLOC=

if [ "x$RUNYAML" != "x" ] ; then
    RUNYAML=$(readlink -f $RUNYAML)
    if [ ! -f $RUNYAML ] ; then
        echo "$RUNYAML not exist, please check!"
        exit 1
    fi
fi

source $COMMON_ENV

gen_logdir barebench
describe_env

function setup_suite {
    local cfgloc=${1:-mini}
    local fpgaloc=${2:-latest}
    local configpath=${MYSCRIPTDIR}/configs/${cfgloc}
    local fpgapath=${MYSCRIPTDIR}/bitstreams/$fpgaloc

    if [ ! -d $configpath ] ; then
        echo "Can't find proper test suite $cfgloc located in $configpath"
        echo "Please pass correct CFGSET value, such as CFGSET=mini or CFGSET=full"
        exit 1
    fi
    if [ ! -d $fpgapath ] ; then
        echo "Can't find proper test suite $fpgaloc located in $fpgapath"
        echo "Please pass correct FPGASET value, such as FPGASET=latest or FPGASET=202206"
        exit 1
    fi
    CFGLOC=$configpath
    FPGALOC=$fpgapath
}

function runbench {
    local yfn=$1
    local logdir=$2
    local mkoptions=${@:3}

    local RUNNER_CMD="python3 $NSDK_RUNNER_PY --appyaml ${MYSCRIPTDIR}/$yfn.yaml --logdir $LOGDIR/$logdir --runon $RUNON --cfgloc $CFGLOC --fpgaloc $FPGALOC"

    if [ "x$RUNYAML" != "x" ] ; then
        RUNNER_CMD="${RUNNER_CMD} --runyaml $RUNYAML"
    fi
    if [ "x$CONFIG" != "x" ] ; then
        RUNNER_CMD="${RUNNER_CMD} --config \"$CONFIG\""
    fi
    if [ "x$VERBOSE" == "x1" ] ; then
        RUNNER_CMD="${RUNNER_CMD} --verbose"
    fi

    if [ "x$mkoptions" != "x" ] ; then
        RUNNER_CMD="${RUNNER_CMD} --make_options \"$mkoptions\""
    fi

    echo $RUNNER_CMD
    if [[ $DRYRUN == 0 ]] ; then
        export OLDRUNMODE=$RUNMODE
        unset RUNMODE
        eval $RUNNER_CMD
        export RUNMODE=$OLDRUNMODE
        unset OLDRUNMODE
    fi
}

function do_barebench {
    pushd $NSDK_ROOT

    if [ "x${RUNMODE}" == "xall" ] || [ "x${RUNMODE}" == "xbare" ] ; then
        echo "Run all baremetal benchmark for all cores"
        runbench barebench barebench ""
    fi

    if [ "x${RUNMODE}" == "xall" ] || [ "x${RUNMODE}" == "xclib" ] ; then
        echo "Run all baremetal benchmark for all cores"
        runbench clibbench clibbench ""
    fi

    if [ "x${RUNMODE}" == "xall" ] || [ "x${RUNMODE}" == "xdhry" ] ; then
        echo "Run dhrystone for different options for all cores"
        for dhrymode in ground inline best
        do
            MKOPTS="DHRY_MODE=$dhrymode"
            local dhrylogdir=dhrystone/$dhrymode
            runbench dhrystone $dhrylogdir "$MKOPTS"
        done
        RUNCMD="python3 $NSDK_REPORT_PY --logdir $LOGDIR/dhrystone --split --run"
        if [[ $DRYRUN == 0 ]] ; then
            eval $RUNCMD
        fi
    fi

    # generate report for all baremetal benchmark/libncrt/dhrystone benchmarks
    RUNCMD="python3 $NSDK_REPORT_PY --logdir $LOGDIR --split --run"
    if [[ $DRYRUN == 0 ]] ; then
       eval $RUNCMD
    fi
    popd
}

setup_suite ${CFGSET} ${FPGASET}
do_barebench | tee $LOGDIR/build.log

zip_logdir
