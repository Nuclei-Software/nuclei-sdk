#!/bin/env bash
MAKE_OPTS=${MAKE_OPTS:-""}
RUNON=${RUNON-ncycm}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)
COMMON_ENV=$(readlink -f $SCRIPTDIR/../env.sh)
FPGALOC=${FPGALOC:-$SCRIPTDIR}
CONFLOC=${CONFLOC:-$SCRIPTDIR}

source $COMMON_ENV

gen_logdir barebench
describe_env

function runbench {
    local yfn=$1
    local logdir=$2
    local mkoptions=${@:3}

    local RUNNER_CMD="python3 $NSDK_RUNNER_PY --appyaml $CONFLOC/$yfn.yaml --logdir $LOGDIR/$logdir --runon $RUNON --cfgloc $CONFLOC --fpgaloc $FPGALOC"

    if [ "x$mkoptions" != "x" ] ; then
        RUNNER_CMD="${RUNNER_CMD} --make_options \"$mkoptions\""
    fi

    echo $RUNNER_CMD
    if [[ $DRYRUN == 0 ]] ; then
        eval $RUNNER_CMD
    fi
}

function do_barebench {
    pushd $NSDK_ROOT
    echo "Run all baremetal benchmark for all cores"
    runbench barebench barebench ""

    echo "Run dhrystone for different options for all cores"
    for dhrymode in ground inline best
    do
        MKOPTS="DHRY_MODE=$dhrymode"
        local dhrylogdir=dhrystone/$dhrymode
        runbench dhrystone $dhrylogdir "$MKOPTS"
    done

    popd
}

do_barebench | tee $LOGDIR/build.log

zip_logdir
