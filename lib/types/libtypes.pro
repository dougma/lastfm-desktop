TARGET = types
TEMPLATE = lib
QT = core
CONFIG += core ws

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h )

DEFINES += _TYPES_DLLEXPORT
