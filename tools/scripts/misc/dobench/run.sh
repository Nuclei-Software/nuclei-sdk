#!/bin/env bash
TOOL_VER=${TOOL_VER:-2022.01}
SIMU_OPTS=${SIMU_OPTS:-"SIMULATION=1 SIMU=xlspike"}
NSDK_ROOT=${NSDK_ROOT:-}
NSDK_VER=${NSDK_VER:-}
LOGROOT=${LOGROOT:-gen}
DRYRUN=${DRYRUN:-0}
RUNTARGET=${RUNTARGET-xlspike}

DEVTOOL_ENV=${DEVTOOL_ENV:-/home/share/devtools/env.sh}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
if [ "x$NSDK_ROOT" == "x" ] ; then
    NSDK_ROOT=$(readlink -f $SCRIPTDIR/../../../..)
fi

if [ ! -d $LOGROOT ] ; then
    mkdir -p $LOGROOT
fi

function describe_repo {
    local repodir=${1}
    local repodesc=${2:-repogit.txt}
    repodir=$(readlink -f $repodir)
    if [ -d ${repodir}/.git  ] ; then
        pushd ${repodir}
        echo "Git Repo $repodir Information:" >> ${repodesc}
        gitver=$(git describe --tags --always --abbrev=10 --dirty)
        gitslog=$(git log --oneline -1)
        echo "git describe version: $gitver" >> ${repodesc}
        echo "git shortlog: $gitslog" >> ${repodesc}
        git submodule >> ${repodesc}
        popd
    else
        echo "$repodir not a git repo" >> ${repodesc}
    fi
}

function describe_build {
    logfile=$1
    echo -n "Build Date: " > $logfile
    date >> $logfile
    echo "Nuclei GNU Toolchain Version:" >> $logfile
    riscv-nuclei-elf-gcc -v >> $logfile 2>&1
}

function describe_sdk {
    logfile=$1
    echo -n "Nuclei SDK(npk) " >> $logfile
    cat $NSDK_ROOT/npk.yml | grep version 2>&1 >> $logfile
}

function get_sdk_ver {
    local repodir=${1}
    repodir=$(readlink -f $repodir)
    if [ -d ${repodir}/.git  ] ; then
        pushd ${repodir} > /dev/null
        gitver=$(git describe --tags --always --abbrev=10 --dirty)
        popd > /dev/null
        echo $gitver
    else
        cat $NSDK_ROOT/npk.yml | grep version 2>&1
    fi
}

if [ "x$NSDK_VER" == "x" ] ; then
    NSDK_VER=$(get_sdk_ver $NSDK_ROOT)
fi

echo "Nuclei SDK location: $NSDK_ROOT, SDK Version $NSDK_VER"

LOGDIR=$(readlink -f $LOGROOT)/$NSDK_VER
NSDK_BENCH_PY="$NSDK_ROOT/tools/scripts/nsdk_cli/nsdk_bench.py"
NSDK_REPORT_PY="$NSDK_ROOT/tools/scripts/nsdk_cli/nsdk_report.py"

if [ -f $DEVTOOL_ENV ] ; then
    TOOL_VER=$TOOL_VER source $DEVTOOL_ENV
else
    pushd $NSDK_ROOT
    source setup.sh
    popd
fi

mkdir -p $LOGDIR

describe_build $LOGDIR/build.txt
describe_sdk $LOGDIR/build.txt
describe_repo $NSDK_ROOT $LOGDIR/build.txt

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

echo "Run for all benchmark scripts"
pushd $NSDK_ROOT
for core in n200 n300 n600 n900 nx600 nx900
do
    echo "Build for CORE: $core"
    appcfg=$SCRIPTDIR/app.json
    hwcfg=$SCRIPTDIR/bench_${core}.json
    logdir=$LOGDIR/barebench/$core

    runbench $appcfg $hwcfg $logdir
done

REPORT_CMD="python3 $NSDK_REPORT_PY --logdir $LOGDIR/barebench --split"
if [ "x$RUNTARGET" != "x" ] ; then
    REPORT_CMD="$REPORT_CMD --run"
fi
echo $REPORT_CMD
if [[ $DRYRUN == 0 ]] ; then
    eval $REPORT_CMD
fi

echo "Run for dhrystone different modes"
for core in n200 n300 n600 n900 nx600 nx900
do
    for dhrymode in ground inline best
    do
        echo "Build for CORE: $core DHRY_MODE=$dhrymode"
        appcfg=$SCRIPTDIR/dhry.json
        hwcfg=$SCRIPTDIR/bench_${core}.json
        logdir=$LOGDIR/dhrystone/$dhrymode/$core
        mkopts="DHRY_MODE=$dhrymode"

        runbench $appcfg $hwcfg $logdir $mkopts
    done
done

REPORT_CMD="python3 $NSDK_REPORT_PY --logdir $LOGDIR/dhrystone --split"
if [ "x$RUNTARGET" != "x" ] ; then
    REPORT_CMD="$REPORT_CMD --run"
fi
echo $REPORT_CMD
if [[ $DRYRUN == 0 ]] ; then
    eval $REPORT_CMD
fi

popd

benchzip=barebench_$NSDK_VER.zip
echo "collect all generated elf and verilog files to $benchzip"
zip -q -r $benchzip $LOGDIR
echo "Copy to your local machine using command below"
echo "scp $(whoami)@$(hostname):$(pwd)/$benchzip ."

