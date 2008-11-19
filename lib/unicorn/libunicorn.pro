TARGET = unicorn
TEMPLATE = lib
QT = core gui
CONFIG += ws core types

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES   = $$findSources( cpp )
HEADERS   = $$findSources( h )
FORMS     = $$findSources( ui )
RESOURCES = $$findSources( qrc )

SOURCES  += $$ROOT_DIR/common/c++/Logger.cpp

DEFINES += _UNICORN_DLLEXPORT
