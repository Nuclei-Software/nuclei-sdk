#!/bin/env bash
FORCE=${FORCE:-0}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
FPGABITS=bitstreams

N200_BIT=${N200_BIT:-n200_software_best_config_ddr200t.bit}
N300_BIT=${N300_BIT:-n300_software_best_config_ddr200t.bit}
N600_BIT=${N600_BIT:-n600_software_best_config_ku060.bit}
UX600_BIT=${UX600_BIT:-ux600_software_best_config_ku060.bit}
N900_BIT=${N900_BIT:-n900_software_best_config_vcu118.bit}
UX900_BIT=${UX900_BIT:-ux900_software_best_config_vcu118.bit}

function wget_file() {
    local link=$1
    local file=$2

    if [ -f $file ] ; then
        if [ "x$FORCE" == "x0" ] ; then
            echo "$file already exist, no need to download it!"
            return
        fi
        echo "$file existed, will remove it, before download it"
        rm -f $file
    fi
    echo "Download $link to $file"
    wget -cO $file  -K $link
}

function wget_and_link {
    local link=$1
    local file=$2
    local dir=$3

    pushd $SCRIPTDIR
    mkdir -p $dir
    wget_file $link $dir/$file
    rm -f $file
    echo "Make link to $file"
    ln -s $dir/$file $file
    popd
}

function wget2dir {
    local link=$1
    local file=$2
    local dir=$3

    pushd $SCRIPTDIR
    mkdir -p $dir
    wget_file $link $dir/$file
    popd
}

function make_link_latest {
    local bitver=${1:-202206}
    pushd $SCRIPTDIR/$FPGABITS
    rm -f latest
    echo "Link $bitver to latest"
    ln -s $bitver latest
    popd
}

function download_bits_202206 {
    local bitprefix=$FPGABITS/202206
    wget2dir http://doc/cpu_signoff/_downloads/7838d023a633edfce9009ead8f27f808/n300_software_best_config_ddr200t_16M_040a67d4_a1560db8d_202203111641_v3.7.0.bit ${N300_BIT} $bitprefix
    wget2dir http://doc/cpu_signoff/_downloads/bcf1baf4265fa729b6d62ab445d55303/ux900_software_best_config_vpu_vcu118_100M_764e6e25f_e63ce562e_202205301028_v2.3.0.bit ${UX900_BIT} $bitprefix
}

function download_bits_202207 {
    local bitprefix=$FPGABITS/202207
    wget2dir http://doc/cpu_signoff/_downloads/7d46f108bc813af41c8aefda08be50c2/n900_best_config_vcu118_100M_cddf72b59_cecbe311c_202207080925_v2.4.0.bit ${N900_BIT} $bitprefix
    wget2dir http://doc/cpu_signoff/_downloads/b89fcf1200314a5465117b6b8ad3506a/ux900_best_config_vcu118_100M_cddf72b59_cecbe311c_202207071844_v2.4.0.bit ${UX900_BIT} $bitprefix
}

function download_bits_202208 {
    local bitprefix=$FPGABITS/202208
    wget2dir http://doc/cpu_signoff/_downloads/e98209e045802d57b98c0530e7f0113c/n900_best_config_vcu118_100M_7c0b82ce2_439fa315f_202208051729_v2.5.0.bit ${N900_BIT} $bitprefix
    wget2dir http://doc/cpu_signoff/_downloads/e59efa9ad3b746eff4acad26cb177a19/ux900_best_config_vcu118_100M_6effcd3ae_439fa315f_202208061048_v2.5.0.bit ${UX900_BIT} $bitprefix
}

download_bits_202206
download_bits_202207
download_bits_202208

make_link_latest 202208
