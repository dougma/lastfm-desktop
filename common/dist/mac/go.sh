#!/bin/bash

source common/bash/utils.sh.inc


if [[ -z $QTDIR ]]
then
	QTDIR=`which qmake`
	QTDIR=`dirname $QTDIR`
	QTDIR=`dirname $QTDIR`
	test -L "$QTDIR" && QTDIR=`readlink $QTDIR`
	export QTDIR
fi


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

header Modifying Info.plist
perl -pi -e 's/0\.0\.0\.0/'$VERSION'/g' $BUNDLE/Contents/Info.plist
perl -pi -e 's/0\.0\.0/'`echo $VERSION | cut -d'.' -f1,2,3`'/g' $BUNDLE/Contents/Info.plist


ROOT=`pwd`
pushd $BUNDLE
    header "Copying Qt frameworks"
    $ROOT/common/dist/mac/add_Qt_to_bundle.sh 'QtGui QtCore QtNetwork QtWebKit QtXml phonon QtDBus' imageformats

    header "deposx"
    $ROOT/common/dist/mac/deposx.sh
popd

header "Creating DMG"
$ROOT/common/dist/mac/gimme_dmg.sh
