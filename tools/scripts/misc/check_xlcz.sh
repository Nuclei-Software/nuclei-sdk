#!/bin/env bash

APPDIR=${1:-application}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)
if [ "x$NSDK_ROOT" == "x"  ] ; then
    NSDK_ROOT=$(readlink -f $SCRIPTDIR/../../..)
fi

pushd $NSDK_ROOT/$APPDIR > /dev/null

totaladdibnecnt=0

echo "app,corecfg,xlczcnt,addibnecnt"
for ext in _xxlcz _zca_zcb_zcf_zcmp_zcmt_xxlcz ; do
    for appdir in `find . -name Makefile | xargs dirname`; do
        skipcases=("demo_dsp" "demo_vnice")
        for case in "${skipcases[@]}"; do
            if [[ "$appdir" == *"$case"* ]] ; then
                # see https://blog.csdn.net/focus_lyh/article/details/112319193
                continue 2
            fi
        done
        for core in n300 n300f nx900 nx900f ; do
            archext=$ext
            if [[ "$core" == *"x"* ]] || [[ ! "$core" == *"f"* ]] ; then
                archext=${archext/_zcf/}
            fi
            pushd $appdir > /dev/null
            make SILENT=1 clean > /dev/null
            dasmfile=$(make SILENT=1 CORE=$core ARCH_EXT=$archext -j dasm 2>&1 | grep dasm | cut -d ">" -f2)
            if [ "x$dasmfile" == "x" ] || [ ! -f $dasmfile ] ; then
                errmsg=$(make SILENT=1 CORE=$core ARCH_EXT=$archext -j dasm 2>&1 | grep 'ld: cannot find -lnmsis')
                if [ "x$errmsg" == "x" ] ; then
                    echo "ERROR: Unable to build $appdir for CORE=$core ARCH_EXT=$archext"
                    make SILENT=1 CORE=$core ARCH_EXT=$archext | grep "Error:"
                    exit 1
                else
                    continue
                fi
            else
                xlczinscnt=$(cat $dasmfile | grep -e "xl\." | wc -l)
                addibnecnt=$(cat $dasmfile | grep -e "xl\.addibne" | wc -l)
            fi
            totaladdibnecnt=$(($addibnecnt+$totaladdibnecnt))
            echo "$appdir,$core$archext,$xlczinscnt,$addibnecnt"
            popd > /dev/null
        done
    done
done

popd > /dev/null

if [[ $totaladdibnecnt -ge 0 ]] ; then
    echo "Generate totally $totaladdibnecnt xl.addibne instructions"
    exit 0
fi

echo "ERORR: No xl.addibne generated!"
exit 1
