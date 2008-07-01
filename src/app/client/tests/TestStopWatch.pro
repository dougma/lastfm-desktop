CONFIG += qtestlib unicorn moose
QT += testlib xml gui network
TEMPLATE = app
INCLUDEPATH += ..

include( $$SRC_DIR/common/qmake/include.pro )

DESTDIR = $$DESTDIR/tests

SOURCES += TestStopWatch.cpp ../StopWatch.cpp
HEADERS += ../StopWatch.h
