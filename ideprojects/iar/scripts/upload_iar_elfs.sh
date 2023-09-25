#!/bin/bash

# You can test different download mode such as ilm,flashxip,sram
DOWNLOAD=${1:-ilm}
# TODO: Change CORE is not yet supported
CORE=n300

# you need to modify the default remote to your real machine gdb remote such as localhost:3333
GDBREMOTE=${GDBREMOTE:-whss3.corp.nucleisys.com:20005}

# Export riscv gdb and iarbuild into system PATH, please modify it according to your environment settings
export PATH=/d/Software/NucleiStudio/toolchain/gcc/bin/:$PATH
export PATH="/c/Program Files/IAR Systems/Embedded Workbench 9.1/common/bin":$PATH

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
IARPRODIR=$(readlink -f ${SCRIPTDIR}/..)

if which riscv64-unknown-elf-gdb > /dev/null ; then
    GDB=riscv64-unknown-elf-gdb
else
    GDB=riscv-nuclei-elf-gdb
fi

function tool_check() {
    local tool=$1
    if ! which $tool > /dev/null ; then
        echo "$tool not found in PATH, please check!"
        exit 1
    fi
}

function upload_program() {
    local file=$1
    echo "Upload and test $file"
    $GDB -ex "set remotetimeout 240" -ex "target remote $GDBREMOTE" \
      --batch -ex 'thread apply all monitor reset halt' -ex 'thread apply all info reg pc' \
      -ex 'thread 1' -ex "load $file" -ex "file $file" \
      -ex 'thread apply all set $pc=__alias_hw_reset' -ex 'thread apply all info reg pc' \
      -ex 'thread 1' -ex 'monitor resume' -ex 'quit'
}

function build_iar_project() {
    local iarprj=$1
    local config=${2:-Debug}
    echo "Build project $iarprj, configuration $config"
    iarbuild $iarprj -build Debug -log errors -parallel 8
}

function change_linkscript() {
    local iarprj=$1
    local download=${2:-flashxip}
    echo "Change download mode to $download"
    sed -i "s/IAR\\\\iar_evalsoc_.*\.icf/IAR\\\\iar_evalsoc_${download}.icf/g" $iarprj
}


# check tool existance
tool_check $GDB
tool_check iarbuild

#upload_program baremetal/Debug/Exe/dhrystone.out
#exit 0

folders=(rtos baremetal)

# Push to iar project directory
pushd $IARPRODIR

for folder in "${folders[@]}"
do
    for file in $(find $folder -type f -name "*.ewp")
    do
        change_linkscript $file $DOWNLOAD
        build_iar_project $file
        outfile="$(dirname $file)/Debug/Exe/$(basename $file ewp)out"
        if [ -f $outfile ] ; then
            upload_program $outfile
        fi
        sleep 15
    done
done

popd
