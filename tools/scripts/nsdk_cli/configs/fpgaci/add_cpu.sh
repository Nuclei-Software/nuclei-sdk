#/bin/env bash

function add_cpu() {
    local cfgdir=$1

    pushd $cfgdir > /dev/null

    [ -f n600.json ] && cp -f n600.json u600.json
    [ -f n900.json ] && cp -f n900.json u900.json

    [ -f u600.json ] && sed -i "s/n600/u600/g" u600.json
    [ -f u900.json ] && sed -i "s/n900/u900/g" u900.json

    pushd > /dev/null
}
