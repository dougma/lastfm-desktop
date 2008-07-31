TEMPLATE = app
TARGET = client
CONFIG += unicorn moose radio core ws types scrobble
QT = core gui xml network phonon webkit
VERSION = 2.0.0

include( $$SRC_DIR/common/qmake/include.pro )

release:TARGET=Last.fm

SOURCES   += $$findSources( cpp )
HEADERS   += $$findSources( h )
FORMS     += $$findSources( ui )
RESOURCES += $$findSources( qrc )
RESOURCES += $$SRC_DIR/common/qrc/common.qrc

!macx*:SOURCES -= mac/ITunesListener.cpp mac/ITunesPluginInstaller.cpp

# included directly into App.cpp
SOURCES -= legacy/disableHelperApp.cpp

ICON = mac/client.icns

DEFINE = $${LITERAL_HASH}define
system( echo \\'$$DEFINE VERSION \\\"$$VERSION\\\"\\' > version.h )
system( echo \\'$$DEFINE PRODUCT_NAME \\\"$$TARGET\\\"\\' >> version.h )
