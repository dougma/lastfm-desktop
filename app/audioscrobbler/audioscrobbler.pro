TEMPLATE = app
TARGET = audioscrobbler
CONFIG += unicorn scrobble listener
VERSION = 2.0.0

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

DEFINES += LASTFM_COLLAPSE_NAMESPACE
