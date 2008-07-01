CONFIG += qtestlib unicorn moose
QT += testlib xml network
TEMPLATE = app
TARGET =
INCLUDEPATH += ../

include( $$SRC_DIR/common/qmake/include.pro )

# Input
SOURCES +=  TestPlayerCommandParser.cpp \
            ../PlayerCommandParser.cpp
