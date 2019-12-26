function nsdk_build() {
    local TARGET=${1:-"clean all"}
    local PROGRAM=${2:-"application/baremetal/helloworld"}
    local CORE=${3:-"n307"}
    local DOWNLOAD=${4:-"ilm"}

    BUILD_CMD="make ${TARGET} PROGRAM=${PROGRAM} CORE=${CORE} DOWNLOAD=${DOWNLOAD} "
    echo "Help: nsdk_build [TARGET] [PROGRAM] [CORE] [DOWNLOAD]"
    echo "Execute Nuclei build command in 1s: ${BUILD_CMD}"
    sleep 1
    eval $BUILD_CMD
}
