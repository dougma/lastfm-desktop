TEMPLATE = app
TARGET = client
CONFIG += unicorn moose radio core ws types
QT = core gui xml network phonon webkit opengl
INCLUDEPATH += .

include( $$SRC_DIR/common/qmake/include.pro )

SOURCES   += $$findSources( cpp )
HEADERS   += $$findSources( h )
FORMS     += $$findSources( ui )
RESOURCES += $$findSources( qrc )
RESOURCES += $$SRC_DIR/common/qrc/common.qrc

!macx*:SOURCES -= mac/ITunesListener.cpp mac/ITunesPluginInstaller.cpp

# included directly into App.cpp
SOURCES -= legacy/disableHelperApp.cpp