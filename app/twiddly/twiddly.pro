CONFIG -= app_bundle
CONFIG += core types unicorn
QT = core xml sql

include( $$ROOT_DIR/common/qmake/include.pro )

LIBS += -L$$DESTDIR
SOURCES += main.cpp PlayCountsDatabase.cpp IPod.cpp $$ROOT_DIR/common/c++/Logger.cpp
HEADERS += $$findSources( h )

macx*:SOURCES += ITunesLibrary_mac.cpp

win32 {
    SOURCES += ITunesLibrary_win.cpp \
			   $$ROOT_DIR/plugins/iTunes/ITunesTrack.cpp \
               $$ROOT_DIR/plugins/iTunes/ITunesComWrapper.cpp \
               $$ROOT_DIR/plugins/scrobsub/EncodingUtils.cpp \
               $$ROOT_DIR/lib/3rdparty/iTunesCOMAPI/iTunesCOMInterface_i.c

    LIBS += -lcomsuppw

    DEFINES += _WIN32_DCOM
    RC_FILE = Twiddly.rc
}
