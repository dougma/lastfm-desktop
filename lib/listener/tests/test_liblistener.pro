TEMPLATE = app
QT = testlib
CONFIG += core types
INCLUDEPATH += ..
include( $$ROOT_DIR/admin/include.qmake )

SOURCES = TestPlayerCommandParser.cpp ../PlayerCommandParser.cpp
