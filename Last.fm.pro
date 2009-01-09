debug:macx-xcode {
    QT = core gui network phonon xml webkit sql svg

    TEMPLATE = app
    TARGET = Last.fm
    ICON = app/client/mac/client_debug.icns
	VERSION	= 2.0.0
	QMAKE_INFO_PLIST = app/client/mac/Info.plist

    CONFIG += mad fftw3f samplerate taglib sqlite3

    include( $$ROOT_DIR/common/qmake/include.pro )

	generateVersionHeader()
	system( mv version.h app/client )

    DIRS = app/client \
           lib/lastfm/core lib/lastfm/ws lib/lastfm/radio lib/lastfm/types lib/lastfm/scrobble lib/lastfm/fingerprint \
           lib/unicorn lib/listener \
           app/clientplugins/localresolver

    for( dir, DIRS ) {
        SOURCES += $$findSources( cpp, $$dir )
        HEADERS += $$findSources( h, $$dir )
        FORMS += $$findSources( ui, $$dir )
		RESOURCES += $$findSources( qrc, $$dir )
    }

    SOURCES += common/c++/Logger.cpp

    INCLUDEPATH += app/client lib
    LIBS += -framework SystemConfiguration -framework CoreServices
}
else {
    TEMPLATE = subdirs

    CONFIG += ordered
    SUBDIRS += lib/lastfm/core/libcore.pro \
               lib/lastfm/ws/libws.pro \
               lib/lastfm/types/libtypes.pro \
               lib/lastfm/radio/libradio.pro \
               lib/lastfm/fingerprint/libfingerprint.pro \
               lib/lastfm/scrobble/libscrobble.pro \
               lib/unicorn/libunicorn.pro \
               lib/listener/liblistener.pro \
	           app/client \
               app/twiddly \
               app/clientplugins/localresolver/libresolver.pro

    win32 {
        RC_FILE = app/client/win/client.rc
    }

	# make the app client the default project in visual studio
	debug:win32 {
		SUBDIRS -= app/client
		SUBDIRS = app/client $$SUBDIRS

		PRECOMPILED_HEADER = common/precompiled.h
	}
	
	linux*:SUBDIRS -= app/twiddly
}
