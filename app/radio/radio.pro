TEMPLATE = app
TARGET = moralistfad
VERSION = 2.0.0
QT = core gui xml network phonon
LIBS += -llastfm -lunicorn

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

DEFINES += LASTFM_COLLAPSE_NAMESPACE

SOURCES -= LegacyTuner.cpp
HEADERS -= LegacyTuner.h
