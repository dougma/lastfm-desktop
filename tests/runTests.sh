#!/bin/sh

BINDIR="../_bin"

export LD_LIBRARY_PATH=$BINDIR:$LD_LIBRARY_PATH

find $BINDIR/tests -type f -perm +1 | xargs ./logTest.sh
