TEMPLATE = lib
TARGET = radio
CONFIG += unicorn
QT += xml network
QT -= gui

include( $$SRC_DIR/common/qmake/include.pro )

SOURCES += $$findSources( cpp )
HEADERS += $$findSources( h )
DEFINES += _RADIO_DLLEXPORT
