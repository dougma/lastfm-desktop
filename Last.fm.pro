debug:macx-xcode {
    QT = core gui network phonon xml webkit

    include( $$SRC_DIR/common/qmake/include.pro )

    DIRS = app/client lib/unicorn lib/moose lib/radio

    for( dir, DIRS ) {
        SOURCES += $$findSources( cpp, $$dir )
        HEADERS += $$findSources( h, $$dir )
        FORMS += $$findSources( ui, $$dir )
		RESOURCES += $$findSources( qrc, $$dir )	
    }

    SOURCES -= lib/unicorn/UnicornUtils_win.cpp

    INCLUDEPATH += app/client
    LIBS += -framework SystemConfiguration -framework CoreServices

    TEMPLATE = app
    TARGET = Last.fm
}
else {
    TEMPLATE = subdirs

    SUBDIRS += app/client/appclient.pro \ #LEAVE THIS FIRST! --mxcl
               lib/unicorn/libunicorn.pro \
               lib/moose/libmoose.pro \
               lib/radio/libradio.pro
}
