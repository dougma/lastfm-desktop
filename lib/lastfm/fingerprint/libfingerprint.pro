TARGET = fingerprint
TEMPLATE = lib
QT += xml network sql
CONFIG += dll types mad fftw3f samplerate

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

headers.files = Fingerprint.h
headers.path = /include/lastfm
INSTALLS = target headers

INCLUDEPATH += fplib/include

win32 {
    INCLUDEPATH += $$ROOT_DIR/lib/3rdparty

    LIBS += -llibsamplerate -llibfftw3f-3 -lmad

    DEFINES += __NO_THREAD_CHECK _FINGERPRINT_DLLEXPORT

    QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmt.lib
}