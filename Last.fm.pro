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

    DIRS = app/client \
           lib/lastfm/core lib/lastfm/ws lib/lastfm/radio lib/lastfm/types lib/lastfm/scrobble \
           lib/unicorn lib/moose 

    for( dir, DIRS ) {
        SOURCES += $$findSources( cpp, $$dir )
        HEADERS += $$findSources( h, $$dir )
        FORMS += $$findSources( ui, $$dir )
		RESOURCES += $$findSources( qrc, $$dir )
    }

    SOURCES += common/c++/Logger.cpp
    SOURCES -= lib/lastfm/core/win/Utils.cpp
	SOURCES -= lib/lastfm/ws/sens_win.cpp lib/lastfm/ws/NdisEvents_win.cpp lib/lastfm/ws/WmiSink_win.cpp

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
               lib/lastfm/scrobble/libscrobble.pro \
               lib/unicorn/libunicorn.pro \
               lib/moose/libmoose.pro \
	           app/client/client.pro

	# make the app client the default project in visual studio
	win32:CONFIG( debug, debug|release ) {
		SUBDIRS -= app/client/client.pro
		SUBDIRS = app/client/client.pro $$SUBDIRS
	}
}
