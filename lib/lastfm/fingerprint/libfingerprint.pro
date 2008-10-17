TARGET = fingerprint
TEMPLATE = lib
QT += xml network sql
CONFIG += dll types core ws

include( $$ROOT_DIR/common/qmake/include.pro )

INCLUDEPATH += fplib/include

SOURCES   = $$findSources( cpp )
HEADERS   = $$findSources( h )
FORMS     = $$findSources( ui )
RESOURCES = $$findSources( qrc )

mac*:release {
    LIBS += /opt/local/lib/libfftw3f.a
    LIBS += /opt/local/lib/libmad.a
    LIBS += /opt/local/lib/libsamplerate.a
    INCLUDEPATH += /opt/local/include
} else:win32 {
    # Really not sure about the sanity of this...
    LIBPATH += $$BUILD_DIR/../fplib $$ROOT_DIR/res/libsamplerate $$ROOT_DIR/res/mad
    LIBS += -lfplib$$EXT -llibfftw3f-3 -lmad
    LIBS += -llibsamplerate

    DEFINES += __NO_THREAD_CHECK FINGERPRINT_DLLEXPORT_PRO

    # Remove warnings in debug build
    QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmt.lib
} else {
    CONFIG += link_pkgconfig
    PKGCONFIG += samplerate mad fftw3f
}
