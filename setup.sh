NUCLEI_TOOL_ROOT=~/NucleiStudio/toolchain
# Create your setup_config.sh
# and define NUCLEI_TOOL_ROOT like below
# NUCLEI_TOOL_ROOT=~/NucleiStudio_IDE_202310-lin64/NucleiStudio/toolchain
SETUP_CONFIG=setup_config.sh

[ -f $SETUP_CONFIG ] && source $SETUP_CONFIG

[ -f .ci/build_sdk.sh ] && source .ci/build_sdk.sh
[ -f .ci/build_applications.sh ] && source .ci/build_applications.sh

echo "Setup Nuclei SDK Tool Environment"
echo "NUCLEI_TOOL_ROOT=$NUCLEI_TOOL_ROOT"

export PATH=$NUCLEI_TOOL_ROOT/gcc/bin:$NUCLEI_TOOL_ROOT/openocd/bin:$NUCLEI_TOOL_ROOT/qemu/bin:$PATH
