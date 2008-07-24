TARGET = ws
TEMPLATE = lib
QT = core network xml

include( $$SRC_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h )

DEFINES += _WS_DLLEXPORT
