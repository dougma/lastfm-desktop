CONFIG += types unicorn resolver sqlite3 taglib
QT += opengl sql phonon
VERSION = 0.0.3

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )
DEFINES += LASTFM_COLLAPSE_NAMESPACE

macx*:QMAKE_INFO_PLIST = mac/Info.plist

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
	       ../client/LocalRadioTrackSource.h \
	       ../../lib/lastfm/radio/AbstractTrackSource.h
	       
