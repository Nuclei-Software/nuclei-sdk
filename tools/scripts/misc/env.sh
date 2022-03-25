
TOOL_VER=${TOOL_VER:-2022.01}
DRYRUN=${DRYRUN:-0}
NSDK_ROOT=${NSDK_ROOT:-}
NSDK_VER=${NSDK_VER-}
LOGROOT=${LOGROOT:-gen}
DEVTOOL_ENV=${DEVTOOL_ENV:-/home/share/devtools/env.sh}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)
if [ "x$NSDK_ROOT" == "x"  ] ; then
    NSDK_ROOT=$(readlink -f $SCRIPTDIR/../../..)
fi

function pushd {
    command pushd "$@" > /dev/null
}

function popd {
    command popd  "$@" > /dev/null
}

function env_setup {
    if [ -f $DEVTOOL_ENV  ] ; then
        TOOL_VER=$TOOL_VER source $DEVTOOL_ENV
    else
        pushd $NSDK_ROOT
        source setup.sh
        popd
    fi
}

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


LOGROOT=$(readlink -f $LOGROOT)
LOGDIR=$(readlink -f $LOGROOT)/$NSDK_VER
LOGZIPNAME=$NSDK_VER
LOGZIP=${LOGZIPNAME}.zip

function gen_logdir {
    local logdirname=${1:-}
    LOGDIR=$LOGDIR/$logdirname
    LOGZIPNAME=${logdirname}_$LOGZIPNAME
    LOGZIP=${LOGZIPNAME}.zip
    if [ -d $LOGDIR ] ; then
        echo "WARNING: Removed existing $LOGDIR"
        rm -rf $LOGDIR
    fi
    if [ ! -d $LOGDIR ] ; then
        mkdir -p $LOGDIR
    fi
    echo "INFO: Creating log directory in $LOGDIR"
}

function zip_logdir {
    local logdir=${1:-$LOGDIR}
    local logzip=${2:-$LOGZIP}

    logzip=$(readlink -f $logzip)
    logdirname=$logdir/..
    logdirbase=$(basename $logdir)
    pushd $logdirname
    zip -r $logzip $logdirbase
    popd

    echo "INFO: Generate zip file $logzip using $logdir"
    echo "Copy to your local machine using command below"
    echo "scp $(whoami)@$(hostname):$logzip ."
}

function describe_env {
    describe_build $LOGDIR/build.txt
    describe_sdk $LOGDIR/build.txt
    describe_repo $NSDK_ROOT $LOGDIR/build.txt
}

NSDK_RUNNER_PY="$NSDK_ROOT/tools/scripts/nsdk_cli/nsdk_runner.py"
NSDK_BENCH_PY="$NSDK_ROOT/tools/scripts/nsdk_cli/nsdk_bench.py"
NSDK_REPORT_PY="$NSDK_ROOT/tools/scripts/nsdk_cli/nsdk_report.py"

env_setup

echo "Nuclei SDK location: $NSDK_ROOT, SDK Version $NSDK_VER, Tool version: $TOOL_VER"
