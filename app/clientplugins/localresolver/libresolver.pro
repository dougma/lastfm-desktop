TARGET = resolver
TEMPLATE = lib
CONFIG += staticlib qt core
QT = core sql
CONFIG += taglib ws types sqlite3 boost

#TODO rename localresolver.pro

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )
DEFINES += LASTFM_COLLAPSE_NAMESPACE

SOURCES -= test.cpp

win32 {
    LIBS += -lmpr
}
