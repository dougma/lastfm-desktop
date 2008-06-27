TEMPLATE = app
TARGET = AudioScrobbler
CONFIG += unicorn moose radio
QT += xml network phonon
INCLUDEPATH += .

include( $$SRC_DIR/include.pro )

SOURCES += $$system( sh $$TOOLS_DIR/findsources cpp )
HEADERS += $$system( sh $$TOOLS_DIR/findsources h )
FORMS += $$system( sh $$TOOLS_DIR/findsources ui )
RESOURCES += $$system( sh $$TOOLS_DIR/findsources qrc )
RESOURCES += $$SRC_DIR/common/qrc/common.qrc

!macx:SOURCES -= mac/ITunesListener.cpp mac/ITunesPluginInstaller.cpp
