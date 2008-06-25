TEMPLATE = app
TARGET = AudioScrobbler
CONFIG += unicorn moose 
QT += xml network

include( $$SRC_DIR/include.pro )

SOURCES += $$system( sh $$TOOLS_DIR/findsources cpp )
HEADERS += $$system( sh $$TOOLS_DIR/findsources h )
FORMS += $$system( sh $$TOOLS_DIR/findsources ui )
RESOURCES += $$system( sh $$TOOLS_DIR/findsources qrc )
RESOURCES += $$SRC_DIR/common/qrc/common.qrc

# how else to add it to the project?
win32:HEADERS += qmake.pro

!macx:SOURCES -= mac/ITunesListener.cpp mac/ITunesPluginInstaller.cpp

INCLUDEPATH += .
