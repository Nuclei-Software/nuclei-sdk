#!/bin/env bash
LOGTO=${1:-bench}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)
COMMON_ENV=$(readlink -f $SCRIPTDIR/../env.sh)

# generate log directory
if [ -d $LOGTO ] ; then
    echo "Remove previous generated elfs"
    rm -rf $LOGTO
fi
mkdir -p $LOGTO

LOGTO=$(readlink -f $LOGTO)
FPGALOGDIR=$LOGTO/fpga
RTLLOGDIR=$LOGTO/rtl

pushd $SCRIPTDIR
echo "Generate baremetal benchmark elf files for FPGA running into $FPGALOGDIR"
SIMU_OPTS="" LOGROOT=$FPGALOGDIR RUNTARGET= ./run.sh

echo "Generate baremetal benchmark elf files for cycle model or rtl simulation running into $RTLLOGDIR"
SIMU_OPTS="SIMULATION=1 SIMU=xlspike" LOGROOT=$RTLLOGDIR RUNTARGET= ./run.sh

popd

source ${COMMON_ENV}
zip_logdir $LOGTO dobench_${NSDK_VER}_$(date -u +"%Y%m%dT%H%M%S").zip

