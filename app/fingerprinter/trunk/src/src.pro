TEMPLATE = app
RC_FILE = fingerprinter.rc
TARGET = Fingerprinter
QT = core gui network xml sql

CONFIG += unicorn fingerprint

include( $$ROOT_DIR/common/qmake/include.pro )

linux* {
    TARGET = last.fingerprinter
}

INCLUDEPATH += \
    src/ \
    libFingerprint/recommendation-commons/ \
    libFingerprint/libs/libmad \
    libFingerprint/libs/fftw

FORMS = $$findSources( ui )
HEADERS = $$findSources( h )
SOURCES =$$findSources( cpp )

macx {
    LIBS += -ltaglib$$EXT -lmad$$EXT
    INCLUDEPATH += \
    libs/taglib/ \
    libs/taglib/taglib/ \
    libs/taglib/taglib/toolkit

    HEADERS += macstyleoverrides.h
    SOURCES += macstyleoverrides.cpp

    ICON = ../bin/data/icons/fingerprinter.icns
}
else:unix: {
    CONFIG += link_pkgconfig
    PKGCONFIG += taglib
}
win32 {
    INCLUDEPATH += libs/taglib/win32/include
    HEADERS += winstyleoverrides.h
    SOURCES += winstyleoverrides.cpp
    LIBS += -ltaglib -lshfolder	
}
