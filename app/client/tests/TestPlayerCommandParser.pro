CONFIG += qtestlib unicorn moose
QT += testlib xml network
TEMPLATE = app
INCLUDEPATH += ../

include( $$SRC_DIR/common/qmake/include.pro )

# Input
SOURCES +=  TestPlayerCommandParser.cpp \
            ../PlayerCommandParser.cpp
