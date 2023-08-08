#!/usr/bin/env bash

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)

NMSIS_LIBDIR=$(readlink -f $SCRIPTDIR/../../../NMSIS/Library)

function rename_nmsis_lib() {
    local libname=${1:-dsp}
    for xlen in 32 64
    do
        for arch in imac imafc imafdc
        do
            for ext in bpv bv bp pv p v b
            do
                newext=""
                vext=_zve32f
                if [ "$xlen" == "64" ] ; then
                    if [[ $arch == *fd* ]] ; then
                        vext=v
                    else
                        vext=_zve64f
                    fi
                fi
                case $ext in
                    bpv)
                        newext=${vext}_zba_zbb_zbc_zbs_xxldspn1x
                        ;;
                    bp)
                        newext=_zba_zbb_zbc_zbs_xxldspn1x
                        ;;
                    bv)
                        newext=${vext}_zba_zbb_zbc_zbs
                        ;;
                    pv)
                        newext=${vext}_xxldspn1x
                        ;;
                    p)
                        newext=_xxldspn1x
                        ;;
                    b)
                        newext=_zba_zbb_zbc_zbs
                        ;;
                    v)
                        newext=${vext}
                        ;;
                esac
                srclib=libnmsis_${libname}_rv${xlen}${arch}${ext}.a
                dstlib=libnmsis_${libname}_rv${xlen}${arch}${newext}.a
                if [ -f $srclib ] && [ "$ext" != "$newext" ] ; then
                    echo "Rename $srclib -> $dstlib"
                    mv $srclib $dstlib
                fi
            done
        done
    done
}

for libname in dsp nn
do
    pushd $NMSIS_LIBDIR/${libname^^}/GCC
    echo ">>> Renaming library for NMSIS $libname library"
    rename_nmsis_lib $libname
    popd
done
