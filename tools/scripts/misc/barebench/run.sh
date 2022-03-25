#!/bin/env bash
TOOL_VER=${TOOL_VER:-2022.01}
MAKE_OPTS=${MAKE_OPTS:-""}
NSDK_ROOT=${NSDK_ROOT:-}
NSDK_VER=${NSDK_VER:-}
LOGROOT=${LOGROOT:-gen}
DRYRUN=${DRYRUN:-0}
RUNON=${RUNON-ncycm}

DEVTOOL_ENV=${DEVTOOL_ENV:-/home/share/devtools/env.sh}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)
if [ "x$NSDK_ROOT" == "x" ] ; then
    NSDK_ROOT=$(readlink -f $SCRIPTDIR/../../../..)
fi

FPGALOC=${FPGALOC:-$SCRIPTDIR}
CONFLOC=${CONFLOC:-$SCRIPTDIR}

rm -rf $LOGROOT
if [ ! -d $LOGROOT ] ; then
    mkdir -p $LOGROOT
fi

LOGROOT=$(readlink -f $LOGROOT)

NSDK_RUNNER_PY="$NSDK_ROOT/tools/scripts/nsdk_cli/nsdk_runner.py"

function runbench {
    local yfn=$1
    local logdir=$2
    local mkoptions=${@:3}

    local RUNNER_CMD="python3 $NSDK_RUNNER_PY --appyaml $CONFLOC/$yfn.yaml --logdir $LOGROOT/$logdir --runon $RUNON --cfgloc $CONFLOC --fpgaloc $FPGALOC"

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

