TEMPLATE = lib
TARGET = radio
QT = core phonon
CONFIG += ws types core
DEFINES += _RADIO_DLLEXPORT LASTFM_COLLAPSE_NAMESPACE

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

headers.files = Radio.h RadioStation.h Tuner.h
headers.path = $$INSTALL_DIR/include/lastfm/radio
INSTALLS = target headers
