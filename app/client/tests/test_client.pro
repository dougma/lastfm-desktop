CONFIG += core
QT += testlib
TEMPLATE = app
INCLUDEPATH += ..

include( $$ROOT_DIR/admin/include.qmake )

SOURCES = test_main.cpp

SOURCES += ../Settings.cpp
HEADERS += TestSettings.h ../Settings.h

SOURCES += ../StopWatch.cpp
HEADERS += TestStopWatch.h ../StopWatch.h
