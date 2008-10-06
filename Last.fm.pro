debug:macx-xcode {
    QT = core gui network phonon xml webkit sql svg

    TEMPLATE = app
    TARGET = Last.fm
    ICON = app/client/mac/client.icns
	VERSION	= 2.0.0
	QMAKE_INFO_PLIST = app/client/mac/Info.plist

    CONFIG += mad fftw3f samplerate

    include( $$ROOT_DIR/common/qmake/include.pro )

	generateVersionHeader()
	system( mv version.h app/client )

    DIRS = app/client lib/core lib/ws lib/unicorn lib/moose lib/radio lib/types lib/scrobble lib/fingerprint

    for( dir, DIRS ) {
        SOURCES += $$findSources( cpp, $$dir )
        HEADERS += $$findSources( h, $$dir )
        FORMS += $$findSources( ui, $$dir )
		RESOURCES += $$findSources( qrc, $$dir )
    }

    SOURCES += common/c++/Logger.cpp
    SOURCES -= lib/core/win/Utils.cpp
	SOURCES -= lib/ws/sens_win.cpp lib/ws/NdisEvents_win.cpp lib/ws/WmiSink_win.cpp

    INCLUDEPATH += app/client
    LIBS += -framework SystemConfiguration -framework CoreServices
}
else {
    TEMPLATE = subdirs

	# QMake is broken on OSX and doesn't figure out the intra-project
	# dependencies

    CONFIG += ordered
    SUBDIRS += lib/core/libcore.pro \
               lib/ws/libws.pro \
               lib/types/libtypes.pro \
               lib/unicorn/libunicorn.pro \
               lib/moose/libmoose.pro \
               lib/radio/libradio.pro \
               lib/scrobble/libscrobble.pro \
	       app/client/appclient.pro

	# make the app client the default project in visual studio
	win32:CONFIG( debug, debug|release ) {
		SUBDIRS -= app/client/appclient.pro
		SUBDIRS = app/client/appclient.pro $$SUBDIRS
	}
	
	win32:SUBDIRS -= lib/fingerprint/libfingerprint.pro
}
