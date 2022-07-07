#!/bin/env bash
SOC=${1:-demosoc}
NEWSOC=${2:-evalsoc}

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
