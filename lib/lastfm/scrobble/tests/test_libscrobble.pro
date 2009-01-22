QT = core testlib xml network
TEMPLATE = app
INCLUDEPATH += ..
CONFIG += ws core types precompile_header

include( $$ROOT_DIR/admin/include.qmake )

SOURCES += TestScrobbler.cpp
HEADERS += MockHttp.h ../Scrobbler.h ../ScrobblerHttp.h

PRECOMPILED_HEADER = pc.h
