#!/bin/env bash
TOOL_VER=${TOOL_VER:-2022.01}
MAKE_OPTS=${MAKE_OPTS:-""}
NSDK_ROOT=${NSDK_ROOT:-}
NSDK_VER=${NSDK_VER:-}
LOGROOT=${LOGROOT:-gen}
DRYRUN=${DRYRUN:-0}
RUNON=${RUNTARGET-ncycm}

DEVTOOL_ENV=${DEVTOOL_ENV:-/home/share/devtools/env.sh}

SCRIPTDIR=$(readlink -f $(dirname $0))
if [ "x$NSDK_ROOT" == "x" ] ; then
    NSDK_ROOT=$(readlink -f $SCRIPTDIR/../../../..)
fi

if [ -d $LOGROOT ] ; then
    mkdir -p $LOGROOT
fi


NSDK_RUNNER_PY="$NSDK_ROOT/tools/scripts/nsdk_cli/nsdk_runner.py"

RUNNER_CMD="python3 $NSDK_RUNNER_PY --appyaml $SCRIPTDIR/barebench.yaml --logdir $LOGROOT --runon $RUNON --cfgloc $SCRIPTDIR"

echo $RUNNER_CMD
if [[ $DRYRUN == 0 ]] ; then
    eval $RUNNER_CMD
fi
