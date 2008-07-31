#!/bin/sh
# enter the bundle directory
# you need to have run add_Qt_to_bundle first
# author: max@last.fm, chris@last.fm
################################################################################


cd Contents

QTLIBS=`ls Frameworks | cut -d. -f1`
LIBS=`cd MacOS && ls -fR1 | grep dylib`
################################################################################


function deposx_change 
{
    echo "D \`$1'"

    for y in $QTLIBS
    do
        lib=$(otool -L "$1" | grep $y.framework | cut -d' ' -f1 | xargs echo)
        test -n "$lib" && install_name_tool -change "$lib" \
                                            @executable_path/../Frameworks/$y.framework/Versions/4/$y \
                                            "$1"
    done
    
    for y in $LIBS
    do
        install_name_tool -change $y \
                          @executable_path/$y \
                          "$1"
    done
}
################################################################################


# first all libraries and executables
find MacOS -type f | while read x
do
    y=$(file "$x" | grep 'Mach-O')
    test -n "$y" && deposx_change "$x"
done

# now Qt
for x in $QTLIBS
do
    deposx_change Frameworks/$x.framework/Versions/4/$x
    install_name_tool -id @executable_path/../Frameworks/$x.framework/Versions/4/$x \
                      Frameworks/$x.framework/Versions/4/$x
done
