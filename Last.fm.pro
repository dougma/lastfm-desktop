debug:macx-xcode {
    QT = core gui network phonon xml webkit

    include( $$SRC_DIR/common/qmake/include.pro )

    DIRS = app/client lib/core lib/ws lib/unicorn lib/moose lib/radio lib/types lib/scrobble

    for( dir, DIRS ) {
        SOURCES += $$findSources( cpp, $$dir )
        HEADERS += $$findSources( h, $$dir )
        FORMS += $$findSources( ui, $$dir )
		RESOURCES += $$findSources( qrc, $$dir )	
    }

    SOURCES += common/c++/Logger.cpp
    SOURCES -= lib/core/UnicornUtils_win.cpp

    INCLUDEPATH += app/client
    LIBS += -framework SystemConfiguration -framework CoreServices

    TEMPLATE = app
    TARGET = Last.fm
    ICON = app/client/mac/client.icns
    
    # FIXME copied from appclient.pro
    DEFINE = $${LITERAL_HASH}define
    system( echo \\'$$DEFINE VERSION \\\"2.0.0\\\"\\' > app/client/version.h )
    system( echo \\'$$DEFINE PRODUCT_NAME \\\"$$TARGET\\\"\\' >> app/client/version.h )    
}
else {
    TEMPLATE = subdirs

    SUBDIRS += app/client/appclient.pro \ #LEAVE THIS FIRST! --mxcl
               lib/core/libcore.pro \
               lib/ws/libws.pro \
               lib/types/libtypes.pro \
               lib/unicorn/libunicorn.pro \               
               lib/moose/libmoose.pro \
               lib/radio/libradio.pro \
               lib/scrobble/libscrobble.pro
}
