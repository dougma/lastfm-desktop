CONFIG += qtestlib moose unicorn
QT += testlib xml network
TEMPLATE = app
INCLUDEPATH += ../ ../scrobbler .

include( $$SRC_DIR/common/qmake/include.pro )

# Input
HEADERS +=  ../scrobbler/Scrobbler.h \
            ../scrobbler/ScrobblerHandshake.h \
            ../scrobbler/ScrobblerSubmission.h \
            ../scrobbler/NowPlaying.h \
            ../scrobbler/ScrobbleCache.h \
            ../scrobbler/ScrobblerHttp.h \
            mock/PrivateMockScrobblerHttp.h

SOURCES +=  TestScrobbler.cpp \
            ../scrobbler/Scrobbler.cpp \
            ../scrobbler/ScrobblerHandshake.cpp \
            ../scrobbler/ScrobblerSubmission.cpp \
            ../scrobbler/NowPlaying.cpp \
            mock/MockScrobbleCache.cpp \
            mock/MockScrobblerHttp.cpp \
            mock/PrivateMockScrobblerHttp.cpp
