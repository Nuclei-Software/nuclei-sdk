#!/bin/bash

for file in `ls *.a`; do
    if [[ $file == *rv64*xxldspn1x* ]] ; then
        dstfile=${file/xxldspn1x/xxldsp}
        echo "mv $file -> $dstfile"
        mv -f $file $dstfile
    fi
done
