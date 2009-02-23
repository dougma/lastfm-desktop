CONFIG += types unicorn resolver sqlite3 taglib radio boost
QT += opengl sql phonon
VERSION = 0.0.3

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )
DEFINES += LASTFM_COLLAPSE_NAMESPACE
!macx-xcode:generateBuildParameters()

macx-g++:release {
	system( $$ROOT_DIR/admin/dist/mac/Makefile.dmg.pl $$LIBS > Makefile.dmg )
	QMAKE_EXTRA_INCLUDES += Makefile.dmg
    ICON = ../client/mac/client.icns
    CONFIG += app_bundle
}

SOURCES += ../client/Resolver.cpp \
           ../client/XspfTrackSource.cpp \
	       ../client/ResolvingTrackSource.cpp \
	       ../client/XspfResolvingTrackSource.cpp  \
	       ../client/LocalRql.cpp \
	       ../client/LocalRadioTrackSource.cpp

HEADERS += ../client/Resolver.h \
           ../client/XspfTrackSource.h \
	       ../client/ResolvingTrackSource.h \
	       ../client/XspfResolvingTrackSource.h \
	       ../client/LocalRql.h \
	       ../client/LocalRadioTrackSource.h 
	       
