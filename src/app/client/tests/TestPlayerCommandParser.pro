CONFIG += qtestlib
QT += testlib xml
TEMPLATE = app
TARGET = 
INCLUDEPATH += ../

include( $$SRC_DIR/include.pro )

DESTDIR = $$BIN_DIR/tests

libs += -lunicorn$$EXT -lmoose$$EXT

# Input
SOURCES +=  TestPlayerCommandParser.cpp
