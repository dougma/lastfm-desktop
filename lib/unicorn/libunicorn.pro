TARGET = unicorn
TEMPLATE = lib
QT = core gui
CONFIG += ws core types

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

SOURCES += $$ROOT_DIR/common/c++/Logger.cpp
DEFINES += _UNICORN_DLLEXPORT LASTFM_COLLAPSE_NAMESPACE
