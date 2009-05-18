CONFIG += lastfm unicorn boost yajl
QT += opengl sql phonon
VERSION = 1.0.0

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )
DEFINES += LASTFM_COLLAPSE_NAMESPACE

macx-g++{
    release {
        QMAKE_INFO_PLIST = Info.plist.in
        system( $$ROOT_DIR/admin/dist/mac/Makefile.dmg.pl $$LIBS > Makefile.dmg )
        QMAKE_EXTRA_INCLUDES += Makefile.dmg
        ICON = ../client/mac/client.icns
        CONFIG += app_bundle
    }
}
