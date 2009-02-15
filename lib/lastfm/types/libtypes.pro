TARGET = types
TEMPLATE = lib
QT = core
CONFIG += core ws

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

DEFINES += _TYPES_DLLEXPORT

headers.files = Track.h Mbid.h Artist.h Album.h FingerprintId.h Playlist.h Tag.h User.h Xspf.h
headers.path = $$INSTALL_DIR/include/lastfm/types
INSTALLS = target headers
