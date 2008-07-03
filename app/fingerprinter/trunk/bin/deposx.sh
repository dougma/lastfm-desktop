rm -Rf Fingerprinter.app/Contents/Frameworks
rm -Rf Fingerprinter.app/Contents/MacOS/cache
rm -Rf Fingerprinter.app/Contents/MacOS/services
rm -Rf Fingerprinter.app/Contents/MacOS/extensions
rm -Rf Fingerprinter.app/Contents/MacOS/data
rm -Rf Fingerprinter.app/Contents/MacOS/*.log
rm -Rf Fingerprinter.app/Contents/MacOS/*.xml

mkdir Fingerprinter.app/Contents/Frameworks
mkdir Fingerprinter.app/Contents/MacOS/sqldrivers

QTDIR=`which qmake`
QTDIR=`dirname $QTDIR`
QTDIR=`dirname $QTDIR`
export QTDIR=`readlink -f $QTDIR`

cp -Rf $QTDIR/lib/QtCore.framework Fingerprinter.app/Contents/Frameworks/
cp -Rf $QTDIR/lib/QtGui.framework Fingerprinter.app/Contents/Frameworks/
cp -Rf $QTDIR/lib/QtXml.framework Fingerprinter.app/Contents/Frameworks/
cp -Rf $QTDIR/lib/QtNetwork.framework Fingerprinter.app/Contents/Frameworks/
cp -Rf $QTDIR/lib/QtSql.framework Fingerprinter.app/Contents/Frameworks/
cp -Rf $QTDIR/plugins/imageformats Fingerprinter.app/Contents/MacOS/
cp -Rf $QTDIR/plugins/sqldrivers/*sqlite* Fingerprinter.app/Contents/MacOS/sqldrivers

cp -Rf libLastFmTools.1.0.0.dylib Fingerprinter.app/Contents/MacOS/libLastFmTools.1.dylib
cp -Rf libLastFmFingerprint.1.0.0.dylib Fingerprinter.app/Contents/MacOS/libLastFmFingerprint.1.dylib
cp -Rf libtaglib.1.0.0.dylib Fingerprinter.app/Contents/MacOS/libtaglib.1.dylib
cp -Rf libmad.1.0.0.dylib Fingerprinter.app/Contents/MacOS/libmad.1.dylib
cp -Rf Info.plist Fingerprinter.app/Contents/
cp -Rf data Fingerprinter.app/Contents/MacOS/
cp -Rf libiconv.2.4.0.dylib Fingerprinter.app/Contents/MacOS/libiconv.2.dylib


find . -name \*_debug -o -name \*_debug.dylib | xargs rm
find Fingerprinter.app/Contents -name .svn | xargs rm -rf
find Fingerprinter.app -name Headers -o -name \*.prl -o -name \*_debug | xargs rm -rf

#strip uploader.app/Contents/MacOS/Last.fm

function deposxid {
    install_name_tool -change $QTDIR/lib/QtCore.framework/Versions/4/QtCore \
                @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
                $1
    
    install_name_tool -change $QTDIR/lib/QtGui.framework/Versions/4/QtGui \
                @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
                $1
    
    install_name_tool -change $QTDIR/lib/QtXml.framework/Versions/4/QtXml \
                @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
                $1
    
    install_name_tool -change $QTDIR/lib/QtNetwork.framework/Versions/4/QtNetwork \
                @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork \
                $1
    
    install_name_tool -change $QTDIR/lib/QtSql.framework/Versions/4/QtSql \
                @executable_path/../Frameworks/QtSql.framework/Versions/4/QtSql \
                $1
    
    install_name_tool -change libLastFmTools.1.dylib \
                @executable_path/libLastFmTools.1.dylib \
                $1

    install_name_tool -change libLastFmFingerprint.1.dylib \
                @executable_path/libLastFmFingerprint.1.dylib \
                $1

	install_name_tool -change libtaglib.1.dylib \
			    @executable_path/libtaglib.1.dylib \
			    $1
			
    install_name_tool -change libmad.1.dylib \
			    @executable_path/libmad.1.dylib \
			    $1
			
    install_name_tool -change /sw/lib/libiconv.2.dylib \
                @executable_path/libiconv.2.dylib \
                $1

}

deposxid Fingerprinter.app/Contents/MacOS/Fingerprinter
deposxid Fingerprinter.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui
deposxid Fingerprinter.app/Contents/Frameworks/QtNetwork.framework/Versions/4/QtNetwork
deposxid Fingerprinter.app/Contents/Frameworks/QtXml.framework/Versions/4/QtXml
deposxid Fingerprinter.app/Contents/Frameworks/QtSql.framework/Versions/4/QtSql
deposxid Fingerprinter.app/Contents/MacOS/imageformats/libqgif.dylib
deposxid Fingerprinter.app/Contents/MacOS/imageformats/libqjpeg.dylib  
deposxid Fingerprinter.app/Contents/MacOS/imageformats/libqmng.dylib
deposxid Fingerprinter.app/Contents/MacOS/sqldrivers/libqsqlite.dylib

deposxid Fingerprinter.app/Contents/MacOS/libLastFmTools.1.dylib
deposxid Fingerprinter.app/Contents/MacOS/libLastFmFingerprint.1.dylib
deposxid Fingerprinter.app/Contents/MacOS/libtaglib.1.dylib
deposxid Fingerprinter.app/Contents/MacOS/libmad.1.dylib
deposxid Fingerprinter.app/Contents/MacOS/libiconv.2.dylib

install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
            Fingerprinter.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore

install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
            Fingerprinter.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui

install_name_tool -id @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
            Fingerprinter.app/Contents/Frameworks/QtXml.framework/Versions/4/QtXml

install_name_tool -id @executable_path/../Frameworks/QtSql.framework/Versions/4/QtSql \
            Fingerprinter.app/Contents/Frameworks/QtSql.framework/Versions/4/QtSql

install_name_tool -id @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork \
            Fingerprinter.app/Contents/Frameworks/QtNetwork.framework/Versions/4/QtNetwork

