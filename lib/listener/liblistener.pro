TARGET = listener
TEMPLATE = lib
QT = core xml network
LIBS += -llastfm

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

DEFINES += _LISTENER_DLLEXPORT LASTFM_COLLAPSE_NAMESPACE

win32:LIBS += Advapi32.lib

SOURCES -= mac/ITunesListener.cpp
HEADERS -= mac/ITunesListener.h
