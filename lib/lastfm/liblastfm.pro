TEMPLATE = lib
TARGET = lastfm
QT = core network xml sql
VERSION = 0.2
CONFIG += mad fftw3f samplerate

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

INSTALLS = target
target.path = /lib

# nobody wants a phonon dependency
SOURCES -= radio/Radio.cpp radio/LegacyTuner.cpp
HEADERS -= radio/Radio.h radio/LegacyTuner.h

DEFINES += _RADIO_DLLEXPORT _FINGERPRINT_DLLEXPORT _WS_DLLEXPORT _CORE_DLLEXPORT _TYPES_DLLEXPORT _SCROBBLE_DLLEXPORT

macx*:LIBS += -framework Carbon -framework SystemConfiguration
