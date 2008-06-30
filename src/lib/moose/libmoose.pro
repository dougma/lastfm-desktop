TEMPLATE = lib
TARGET = moose
CONFIG += unicorn
QT += xml network
qt -= gui

include( $$SRC_DIR/common/qmake/include.pro )

SOURCES = $$system( ls *.cpp )
HEADERS = $$system( ls *.h )

DEFINES += _MOOSE_DLLEXPORT
