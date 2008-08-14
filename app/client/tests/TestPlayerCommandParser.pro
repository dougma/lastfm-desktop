CONFIG += qtestlib unicorn moose
QT += testlib xml network
TEMPLATE = app
INCLUDEPATH += ../

include( $$ROOT_DIR/common/qmake/include.pro )

# Input
SOURCES +=  TestPlayerCommandParser.cpp \
            ../PlayerCommandParser.cpp
