CONFIG += qtestlib unicorn moose
QT += testlib xml gui network
TEMPLATE = app
INCLUDEPATH += ../

include( $$SRC_DIR/common/qmake/include.pro )

# Input
SOURCES +=  TestSettings.cpp ../Settings.cpp
