TEMPLATE = lib
TARGET = moose
CONFIG += unicorn
QT += xml network

include( $$SRC_DIR/include.pro )

SOURCES = $$system( ls *.cpp )
HEADERS = $$system( ls *.h ) qmake.pro

DEFINES += _MOOSE_DLLEXPORT

RESOURCES += $$SRC_DIR/common/qrc/common.qrc