#!/bin/bash

source common/bash/utils.sh.inc
export ROOT=`pwd`
export VERSION='2.0.0'
export BUNDLE=_bin/Last.fm.app


header "Copying remaining stuff into $BUNDLE"
for x in `find _bin -maxdepth 1 -name \*.dylib | grep '^[^0-9]*\.[0-9]\.dylib'` \
         `find _bin -maxdepth 1 -name \*.dylib -prune -o -type f -print`
do
    # the Mach-O bit is to stop us copying shell scripts and DLLs etc.
    tmp=$(file -L "$x" | grep 'Mach-O')
    if [ -n "$tmp" ]
    then
        echo "C $x"
        cp "$x" $BUNDLE/Contents/MacOS
    fi
done
cp COPYING $BUNDLE/Contents/
cp ChangeLog.txt $BUNDLE/Contents/ChangeLog


pushd $BUNDLE
    header "Copying Qt frameworks"
    $ROOT/common/dist/mac/add_Qt_to_bundle.sh 'QtGui QtCore QtNetwork QtWebKit QtXml phonon QtDBus' imageformats

    header "deposx"
    $ROOT/common/dist/mac/deposx.sh
popd

header "Creating DMG"
$ROOT/common/dist/mac/gimme_dmg.sh
