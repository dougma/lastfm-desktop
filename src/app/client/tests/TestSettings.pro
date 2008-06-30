CONFIG += qtestlib unicorn moose
QT += testlib xml gui
TEMPLATE = app
TARGET = 
INCLUDEPATH += ../

include( $$SRC_DIR/common/qmake/include.pro )

DESTDIR = $$DESTDIR/tests

# Input
SOURCES +=  TestSettings.cpp \
            ../Settings.cpp
