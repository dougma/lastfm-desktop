#!/bin/sh

BINDIR="../_bin"

case `uname` in
    Darwin) ;;
    Linux) export LD_LIBRARY_PATH=$BINDIR:$LD_LIBRARY_PATH;;
    *) export PATH=$BINDIR:$PATH;;
esac

find $BINDIR/tests -type f -perm +1 | xargs ./logTest.sh
