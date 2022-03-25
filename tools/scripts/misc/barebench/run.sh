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

pushd $NSDK_ROOT
runbench barebench barebench ""

for dhrymode in ground inline best
do
    MKOPTS="DHRY_MODE=$dhrymode"
    dhrylogdir=dhrystone/$dhrymode
    runbench dhrystone $dhrylogdir "$MKOPTS"
done

popd

zip_logdir
