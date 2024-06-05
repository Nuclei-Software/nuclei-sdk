#/bin/env bash

CFGDIR=${1:-full}

echo "Add u600/u900 configs for $CFGDIR in 3s"
sleep 3

pushd $CFGDIR

cp -f n600.json u600.json
cp -f n900.json u900.json

sed -i "s/n600/u600/g" u600.json
sed -i "s/n900/u900/g" u900.json

pushd
