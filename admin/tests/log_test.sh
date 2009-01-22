#!/bin/bash
# author <pete@last.fm>
# meant to be run from the /tests dir

RUNDIR=`pwd`
mkdir $RUNDIR/output

TOTERRORS=0

pushd ../_bin > /dev/null

for i in $*
do
    BINDIR=`dirname $i`
    LEN=${#BINDIR}
    let LEN=$LEN+1
    OUTPUTDIR=${i:$LEN}
    OUTPUTFILE=$RUNDIR/output/$OUTPUTDIR.output
    
    $i  -xml > "$OUTPUTFILE"
    
    # check for program errors
    RETURNCODE=$?
    if [[ RETURNCODE -gt 0 ]];
    then
        echo "Bailing out - bad return code: $RETURNCODE"
        echo "$i"
        exit
    fi
    
    # count number of test failures
    NUMERRORS=`grep -c \"fail\" $OUTPUTFILE`
    XPASS=`grep -c \"xpass\" $OUTPUTFILE`
    let NUMERRORS=$NUMERRORS+$XPASS
    let TOTERRORS=TOTERRORS+NUMERRORS
    
    if [[ NUMERRORS -gt 0 ]];
    then
        echo "$NUMERRORS errors where encountered in $i"
    fi
    
    # transform outputted xml into junit-xml
    xsltproc $RUNDIR/QtTest_to_JUnit.xslt $OUTPUTFILE > "$OUTPUTFILE.junit.xml"
done

popd > /dev/null


if [[ TOTERRORS -gt 0 ]];
then
    echo "Total: $TOTERRORS errors"
fi
