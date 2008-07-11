TEMPLATE = lib
TARGET = moose
CONFIG += unicorn
QT = core gui xml network

include( $$SRC_DIR/common/qmake/include.pro )

SOURCES = $$system( ls *.cpp )
HEADERS = $$system( ls *.h )

DEFINES += _MOOSE_DLLEXPORT
