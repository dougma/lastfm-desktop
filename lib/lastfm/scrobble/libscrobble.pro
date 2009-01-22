TARGET = scrobble
TEMPLATE = lib
QT = core network xml
CONFIG += core ws types
DEFINES += _SCROBBLE_DLLEXPORT

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

headers.files = Scrobbler.h Scrobble.h ScrobblePoint.h
headers.path = $$INSTALL_DIR/include/lastfm
INSTALLS = target headers
