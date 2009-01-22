TARGET = types
TEMPLATE = lib
QT = core
CONFIG += core ws

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

DEFINES += _TYPES_DLLEXPORT

headers.files = $$HEADERS
headers.path = $$INSTALL_DIR/include/lastfm/types
INSTALLS = target headers
