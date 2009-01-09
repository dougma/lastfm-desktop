TARGET = listener
TEMPLATE = lib
QT = core xml network
CONFIG += types scrobble

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES   = $$findSources( cpp )
HEADERS   = $$findSources( h )

DEFINES += _LISTENER_DLLEXPORT
