TEMPLATE = app
TARGET = Last.fm
CONFIG += unicorn moose radio core ws types scrobble
QT = core gui xml network phonon webkit
VERSION = 2.0.0

include( $$SRC_DIR/common/qmake/include.pro )

generateVersionH()

SOURCES   += $$findSources( cpp )
HEADERS   += $$findSources( h )
FORMS     += $$findSources( ui )
RESOURCES += $$findSources( qrc )
RESOURCES += $$SRC_DIR/common/qrc/common.qrc

# included directly into App.cpp, so avoid link error
SOURCES -= legacy/disableHelperApp.cpp

macx* {
	QMAKE_INFO_PLIST = mac/Info.plist.in
	ICON = mac/client.icns

	!macx-xcode:release {
		system( $$ROOT_DIR/common/dist/mac/Makefile.dmg.pl $$DESTDIR $$VERSION $$QMAKE_LIBDIR_QT $$LIBS > Makefile.dmg )
		QMAKE_EXTRA_INCLUDES += Makefile.dmg
	}
}
else {
	SOURCES -= mac/ITunesListener.cpp mac/ITunesPluginInstaller.cpp
	INCLUDEPATH += .
}
