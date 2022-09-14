OUT=${1:-cmpelfs_Os/medany_sr}
FLAGS=${FLAGS:-"-Os -Wno-implicit -msave-restore"}
for bench in coremark dhrystone whetstone
do
    echo "Testing $bench"
    cd $bench
    GENROOT=$OUT BFLAGS="$FLAGS" bash compare_codesz.sh
    cd -
done
