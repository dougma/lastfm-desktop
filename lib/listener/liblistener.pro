TARGET = listener
TEMPLATE = lib
QT = core xml network
CONFIG += core types scrobble

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

DEFINES += _LISTENER_DLLEXPORT LASTFM_COLLAPSE_NAMESPACE

win32:LIBS += Advapi32.lib
