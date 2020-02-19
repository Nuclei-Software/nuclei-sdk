NUCLEI_TOOL_ROOT=~/Nuclei
NMSIS_ROOT=../NMSIS
# Create your setup_config.sh
# and define NUCLEI_TOOL_ROOT and NMSIS_ROOT like below
# NUCLEI_TOOL_ROOT=/home/develop/Software/Nuclei
# NMSIS_ROOT=/home/develop/Source/NMSIS
SETUP_CONFIG=setup_config.sh

[ -f $SETUP_CONFIG ] && source $SETUP_CONFIG

[ -f .ci/build_sdk.sh ] && source .ci/build_sdk.sh
[ -f .ci/build_applications.sh ] && source .ci/build_applications.sh

echo "Setup Nuclei SDK Tool Environment"
echo "NUCLEI_TOOL_ROOT=$NUCLEI_TOOL_ROOT"

export PATH=$NUCLEI_TOOL_ROOT/gcc/bin:$NUCLEI_TOOL_ROOT/openocd/bin:$PATH
export NMSIS_ROOT=$(readlink -f $NMSIS_ROOT)
