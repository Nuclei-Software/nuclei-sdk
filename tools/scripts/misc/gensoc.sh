#!/bin/env bash
SOC=${1:-evalsoc}
NEWSOC=${2:-ctsoc}

SCRIPTDIR=$(dirname $(readlink -f $BASH_SOURCE))
SCRIPTDIR=$(readlink -f $SCRIPTDIR)
if [ "x$NSDK_ROOT" == "x"  ] ; then
    NSDK_ROOT=$(readlink -f $SCRIPTDIR/../../..)
fi

# cd to SoC folder
pushd $NSDK_ROOT/SoC

if [ -d $NEWSOC ] ; then
    echo "$NEWSOC already exist, exit!"
    exit
fi

cp -r $SOC $NEWSOC
pushd $NEWSOC
for dir in `find . -type d -name "*${SOC}*"`
do
    echo "Found directory $dir"
done

for fl in `find . -type f -name "*${SOC}*"`
do
    nfl="${fl//${SOC}/${NEWSOC}}"
    echo "Rename $fl to $nfl"
    mv $fl $nfl
done

for fl in `find . -type f`
do
    echo "Sed and replace in $fl"
    sed -i -e "s/${SOC}/${NEWSOC}/g" $fl
    sed -i -e "s/${SOC^^}/${NEWSOC^^}/g" $fl
    sed -i -e "s/Demo/Eval/g" $fl
done
popd

popd
