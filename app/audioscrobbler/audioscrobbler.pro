TEMPLATE = app
TARGET = audioscrobbler
VERSION = 2.0.0
LIBS += -llastfm -lunicorn -llistener

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

DEFINES += LASTFM_COLLAPSE_NAMESPACE
