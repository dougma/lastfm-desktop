#!/bin/bash

RUNDIR=`dirname $0`

for i in $*
do
  BINDIR=`dirname $i`
  LEN=${#BINDIR}
  let LEN=$LEN+1
  OUTPUTDIR=${i:$LEN}
  OUTPUTFILE=$RUNDIR/output/$OUTPUTDIR.output
  
  $i  -xml > "$OUTPUTFILE"
  xsltproc $RUNDIR/QtTest_to_JUnit.xslt $OUTPUTFILE > "$OUTPUTFILE.junit.xml"
done
