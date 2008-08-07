#!/bin/sh
# author: max@last.fm
# brief:  otools a binary file that is already in the bundle
################################################################################

FRAMEWORKS="QtGui QtCore QtNetwork QtWebKit QtXml phonon"
################################################################################

for y in $FRAMEWORKS
do
    lib=$(otool -L "$1" | grep $y.framework | cut -d' ' -f1 | xargs echo)
    if [ -n "$lib" ]
	then
		install_name_tool -change "$lib" \
                          @executable_path/../Frameworks/$y.framework/Versions/4/$y \
                          "$1"
	fi
done
