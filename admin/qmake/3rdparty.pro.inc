CONFIG( taglib ) {
    CONFIG += link_pkgconfig
    PKGCONFIG += taglib
    win32:debug:LIBS += tagd.lib    # tagd.lib will precede PKGCONFIG added libs
}

CONFIG( mad ) {
    win32: LIBS += -L$$COMMON_LIBS_DIR/libmad -lmad
    else {
        CONFIG += link_pkgconfig
        PKGCONFIG += mad
    } 
}

CONFIG( boost ) {
    macx*:INCLUDEPATH += /opt/local/include
}

CONFIG( fftw3f ) {
    win32: LIBS += -L$$COMMON_LIBS_DIR/libfftw -lfftw3f
    else {
        CONFIG += link_pkgconfig
        PKGCONFIG += fftw3f
    }
}

CONFIG( samplerate ) {
    win32: LIBS += -L$$COMMON_LIBS_DIR/libsamplerate -lsamplerate
    else: {
        CONFIG += link_pkgconfig
        PKGCONFIG += samplerate
    }
}

CONFIG( sqlite3 ) {
    !macx*{
        CONFIG += link_pkgconfig
        PKGCONFIG += sqlite3
    }
    else:LIBS += -lsqlite3
}
