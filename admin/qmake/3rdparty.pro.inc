CONFIG( taglib ) {
    CONFIG += link_pkgconfig
    PKGCONFIG += taglib
    win32:debug:LIBS += tagd.lib    # tagd.lib will precede PKGCONFIG added libs
}

CONFIG( mad ) {
    win32{
		LIBS += -L$$COMMON_LIBS_DIR/libmad -lmad
	}else:CONFIG( app_bundle ){
		LIBS += /opt/local/lib/libmad.a
		INCLUDEPATH += /opt/local/include
	}else{
        CONFIG += link_pkgconfig
        PKGCONFIG += mad
    } 
}

CONFIG( boost ) {
    macx*{
        INCLUDEPATH += /opt/local/include
        LIBS += /opt/local/lib/libboost_thread-mt.a
    }
    unix: !macx* {
        LIBS += -lboost_thread
    }
}

CONFIG( fftw3f ) {
    win32{
		LIBS += -L$$COMMON_LIBS_DIR/libfftw -lfftw3f
	}else:CONFIG( app_bundle ){
		LIBS += /opt/local/lib/libfftw3f.a
		INCLUDEPATH += /opt/local/include
	}else{
        CONFIG += link_pkgconfig
        PKGCONFIG += fftw3f
    }
}

CONFIG( samplerate ) {
    win32{
		LIBS += -L$$COMMON_LIBS_DIR/libsamplerate -lsamplerate
	}else:CONFIG( app_bundle ){
		LIBS += /opt/local/lib/libsamplerate.a
		INCLUDEPATH += /opt/local/include
	}else{
        CONFIG += link_pkgconfig
        PKGCONFIG += samplerate
    }
}

CONFIG( sqlite3 ) {
    win32 {
        CONFIG += link_pkgconfig
        PKGCONFIG += sqlite3
    }
    else:LIBS += -lsqlite3
}
