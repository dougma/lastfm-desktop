TEMPLATE = lib
TARGET = lastfm
QT = core network xml phonon
VERSION = 0.2

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

INSTALLS = target
target.path = $$INSTALL_DIR/lib


# for now because it doesn't work anyway
SOURCES -= fingerprint/Sha256.cpp \
           fingerprint/MP3_Source_Qt.cpp \
           fingerprint/fplib/src/OptFFT.cpp \
           fingerprint/fplib/src/FingerprintExtractor.cpp \
           fingerprint/fplib/src/Filter.cpp \
           fingerprint/Fingerprint.cpp \
           fingerprint/Collection.cpp
          