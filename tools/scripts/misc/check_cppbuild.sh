#!/bin/bash

# exit if any command fails
set -e

# Check header files in NMSIS/Core/Include is compatible with C++
# arg1 is the cpu arch
# arg2 is the abi
# arg3 and later are extra build flags. Cause some APIs in header file may protected
# by macros, so you can provide extra macros to test these APIs.
check_cpp_build ()
{
    local arch=${1-rv32imac}
    local abi=${2-ilp32}
    # provide extra build flags if any
    local extra_flags=${@:3}

    riscv64-unknown-elf-g++ -march=${arch} -mabi=${abi} -INMSIS/Core/Include -ISoC/evalsoc/Common/Include -Werror -c -o /tmp/test.o -x c++ ${extra_flags} - <<< $'#include "nuclei_sdk_soc.h"\n int main(){return 0;}'

    echo "Checking arch=${arch} abi=${abi} extra_flags=\"${extra_flags}\" Pass!"
}

SCRIPTDIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

pushd ${SCRIPTDIR}/../../../ > /dev/null

echo "Start C++ compatibility check:"

check_cpp_build rv32imafdc_zve32f_xxldspn3x ilp32d
check_cpp_build rv32imafdc_zve32f_xxldspn3x ilp32d -DXLCFG_CCM=1
check_cpp_build rv32imafdc_zve32f_xxldspn3x ilp32d -DXLCFG_SMPCC=1
check_cpp_build rv32imafdc_zve32f_xxldspn3x ilp32d -DXLCFG_HPM=1
check_cpp_build rv64imafdcv_xxldsp lp64d
check_cpp_build rv64imafdcv_xxldsp lp64d -DXLCFG_CCM=1
check_cpp_build rv64imafdcv_xxldsp lp64d -DXLCFG_SMPCC=1
check_cpp_build rv64imafdcv_xxldsp lp64d -DXLCFG_HPM=1

popd > /dev/null

exit 0
