TEMPLATE = app
TARGET = moralistfad
CONFIG += unicorn radio
VERSION = 2.0.0

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

DEFINES += LASTFM_COLLAPSE_NAMESPACE

SOURCES -= LegacyTuner.cpp
HEADERS -= LegacyTuner.h
