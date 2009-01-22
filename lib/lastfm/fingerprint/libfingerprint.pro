TARGET = fingerprint
TEMPLATE = lib
QT += xml network sql
CONFIG += dll types core ws

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

INCLUDEPATH += fplib/include

mac*:release {
    LIBS += /opt/local/lib/libfftw3f.a
    LIBS += /opt/local/lib/libmad.a
    LIBS += /opt/local/lib/libsamplerate.a
    INCLUDEPATH += /opt/local/include
}
else:win32 {
    INCLUDEPATH += $$ROOT_DIR/lib/3rdparty

    LIBS += -llibsamplerate -llibfftw3f-3 -lmad

    DEFINES += __NO_THREAD_CHECK _FINGERPRINT_DLLEXPORT

    QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmt.lib
} else {
    CONFIG += link_pkgconfig
    PKGCONFIG += samplerate mad fftw3f
}
