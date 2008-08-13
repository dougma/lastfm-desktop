TEMPLATE = app
TARGET = Last.fm
CONFIG += unicorn moose radio core ws types scrobble
QT = core gui xml network phonon webkit
VERSION = 2.0.0

include( $$ROOT_DIR/common/qmake/include.pro )

generateVersionHeader()
QMAKE_EXTRA_INCLUDES += $$generateInstallerMakefile()

SOURCES   += $$findSources( cpp )
HEADERS   += $$findSources( h )
FORMS     += $$findSources( ui )
RESOURCES += $$findSources( qrc )
RESOURCES += $$SRC_DIR/common/qrc/common.qrc

# included directly into App.cpp, so avoid link error
SOURCES -= legacy/disableHelperApp.cpp

macx* {
	QMAKE_INFO_PLIST = mac/Info.plist
	ICON = mac/client.icns
}
else {
	SOURCES -= mac/ITunesListener.cpp mac/ITunesPluginInstaller.cpp
	INCLUDEPATH += .
}
