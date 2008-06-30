TEMPLATE = lib
TARGET = radio
CONFIG += unicorn
QT += xml network
QT -= gui

include( $$SRC_DIR/common/qmake/include.pro )

SOURCES += $$system( ls *.cpp )
HEADERS += $$system( ls *.h )
DEFINES += _RADIO_DLLEXPORT
