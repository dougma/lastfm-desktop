#!/bin/bash
# @author <max@last.fm>
# expects to located in _bin, otherwise it doesn't work

d=`dirname $0`

source $d/../admin/utils.bash


case `uname` in
    Darwin) export DYLD_LIBRARY_PATH=$d:DYLD_LIBRARY_PATH;;
    Linux)  export LD_LIBRARY_PATH=$d:$LD_LIBRARY_PATH;;
esac

# reverse sorted because test_client takes 3 minutes and it is boring to wait
for x in `find . -type f -perm +1 -name test_\* | sort -r`
do
    header $x
    $x
done
