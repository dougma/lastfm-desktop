TEMPLATE = app
TARGET = client
CONFIG += unicorn moose radio
QT = core gui xml network phonon
INCLUDEPATH += .

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES   += findSources( cpp )
HEADERS   += findSources( h )
FORMS     += findSources( ui )
RESOURCES += findSources( qrc )
RESOURCES += $$SRC_DIR/common/qrc/common.qrc

!macx:SOURCES -= mac/ITunesListener.cpp mac/ITunesPluginInstaller.cpp
