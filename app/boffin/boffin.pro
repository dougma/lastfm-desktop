CONFIG += types unicorn radio boost
CONFIG -= app_bundle
QT += opengl sql phonon
VERSION = 0.0.6

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )
DEFINES += LASTFM_COLLAPSE_NAMESPACE
!macx-xcode:generateBuildParameters()

macx-g++:release {
    QMAKE_INFO_PLIST = Info.plist.in
	system( $$ROOT_DIR/admin/dist/mac/Makefile.dmg.pl $$LIBS > Makefile.dmg )
	QMAKE_EXTRA_INCLUDES += Makefile.dmg
    ICON = ../client/mac/client.icns
    CONFIG += app_bundle
}

!win {
    LIBS +=
}
