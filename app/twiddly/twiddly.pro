CONFIG -= app_bundle
CONFIG += core types unicorn
QT = core xml sql

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

LIBS += -L$$DESTDIR
SOURCES = main.cpp PlayCountsDatabase.cpp IPod.cpp $$ROOT_DIR/common/c++/Logger.cpp

macx*:SOURCES += ITunesLibrary_mac.cpp

win32 {
    # Would prefer to refer to ITunesTrack.cpp and ITunesComWrapper.cpp in-situ
    # in the ../../plugins/iTunes/ directory, but that triggers bugs in nmake
    # causing it to compile the wrong main.cpp and IPod.cpp!
    # So here we are copying them and their dependencies.
    # Oh, and for some reason, cygwin mutilates their permissions.
    
    system( cp -f ../../plugins/iTunes/ITunesTrack.cpp . )
    system( cp -f ../../plugins/iTunes/ITunesTrack.h . )
    system( cp -f ../../plugins/iTunes/ITunesExceptions.h . )
    system( cp -f ../../plugins/iTunes/ITunesComWrapper.cpp . )
    system( cp -f ../../plugins/iTunes/ITunesComWrapper.h . )
    system( cp -f ../../plugins/iTunes/ITunesEventInterface.h . )
    system( chmod a+r ITunesTrack.cpp )
    system( chmod a+r ITunesTrack.h )
    system( chmod a+r ITunesExceptions.h )
    system( chmod a+r ITunesComWrapper.cpp )
    system( chmod a+r ITunesComWrapper.h )
    system( chmod a+r ITunesEventInterface.h )

    SOURCES += ITunesLibrary_win.cpp \
			   ITunesTrack.cpp \
               ITunesComWrapper.cpp \
               $$ROOT_DIR/plugins/scrobsub/EncodingUtils.cpp \
               $$ROOT_DIR/lib/3rdparty/iTunesCOMAPI/iTunesCOMInterface_i.c

    LIBS += -lcomsuppw

    DEFINES += _WIN32_DCOM
    RC_FILE = Twiddly.rc
}
