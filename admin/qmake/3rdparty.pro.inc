mac {
    LIBS += -L/opt/local/lib
    INCLUDEPATH += /opt/local/include
}

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
	}else{
        CONFIG += link_pkgconfig
        PKGCONFIG += mad
    } 
}

CONFIG( boost ) {
    mac:CONFIG(app_bundle){
        LIBS += /opt/local/lib/libboost_thread.a
    }
    else:unix {
        LIBS += -lboost_thread
    }
}

CONFIG( fftw3f ) {
    win32{
		LIBS += -L$$COMMON_LIBS_DIR/libfftw -lfftw3f
	}else:CONFIG( app_bundle ){
		LIBS += /opt/local/lib/libfftw3f.a
	}else{
        CONFIG += link_pkgconfig
        PKGCONFIG += fftw3f
    }
}

CONFIG( samplerate ) {
    win32{
		LIBS += -L$$COMMON_LIBS_DIR/libsamplerate -lsamplerate
	}else:mac:CONFIG( app_bundle ){
		LIBS += libsamplerate.a -L/opt/local/lib
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

CONFIG( yajl ) {
	win32 {
		CONFIG += link_pkgconfig
		debug:PKGCONFIG += yajl-debug
		release:PKGCONFIG += yajl-release
	}
	else:CONFIG(app_bundle) {
		LIBS += /opt/local/lib/libyajl_s.a
	}
	else:LIBS += -lyajl
}
