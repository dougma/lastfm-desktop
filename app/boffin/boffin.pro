CONFIG += core types unicorn resolver radio sqlite3 taglib
QT = gui xml sql
VERSION = 0.0.1

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
	       ../client/LocalRadioTrackSource.h
	       