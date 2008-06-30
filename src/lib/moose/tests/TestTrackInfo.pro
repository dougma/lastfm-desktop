CONFIG += qtestlib unicorn moose
QT += testlib xml gui network
TEMPLATE = app
TARGET = 
INCLUDEPATH += ../

include( $$SRC_DIR/common/qmake/include.pro )

DESTDIR = $$DESTDIR/tests

# Input
SOURCES +=  TestTrackInfo.cpp
