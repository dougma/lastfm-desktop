TEMPLATE = app
QT = testlib
CONFIG += core types
INCLUDEPATH += ..
include( $$ROOT_DIR/admin/include.qmake )

DEFINES += LASTFM_COLLAPSE_NAMESPACE
SOURCES = TestPlayerCommandParser.cpp ../PlayerCommandParser.cpp
