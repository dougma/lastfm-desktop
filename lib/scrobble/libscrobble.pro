TARGET = scrobble
TEMPLATE = lib
QT = core network xml
CONFIG += core ws types

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h )

DEFINES += _SCROBBLE_DLLEXPORT

headers.files = $$HEADERS
headers.path = $$INSTALL_DIR/include/lastfm/scrobble
INSTALLS += target headers
