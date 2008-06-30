#!/bin/bash

RUNDIR=`dirname $0`

TOTERRORS=0
for i in $*
do
  BINDIR=`dirname $i`
  LEN=${#BINDIR}
  let LEN=$LEN+1
  OUTPUTDIR=${i:$LEN}
  OUTPUTFILE=$RUNDIR/output/$OUTPUTDIR.output
  
  $i  -xml > "$OUTPUTFILE"
  
  NUMERRORS=`grep -c \"fail\" $OUTPUTFILE`
  let TOTERRORS=TOTERRORS+NUMERRORS
  
  if [[ NUMERRORS -gt 0 ]];
  then
    echo "$NUMERRORS errors where encountered in $i"
  fi
  
  xsltproc $RUNDIR/QtTest_to_JUnit.xslt $OUTPUTFILE > "$OUTPUTFILE.junit.xml"
done

if [[ TOTERRORS -gt 0 ]];
then
  echo "Total: $TOTERRORS errors"
fi
