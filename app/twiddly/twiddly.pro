CONFIG -= app_bundle
CONFIG += core types
QT = core xml sql

include( $$ROOT_DIR/common/qmake/include.pro )

LIBS += -L$$DESTDIR

SOURCES += main.cpp \
           PlayCountsDatabase.cpp \
           IPod.cpp
           
macx* {
    SOURCES += ITunesLibrary_mac.cpp
    release {
        QMAKE_POST_LINK = cd $$DESTDIR/*.app/Contents && cp ../../$$TARGET MacOS && cd Resources && ln -s ../Contents/$$TARGET
    }
}
    

win32 {
    SOURCES += ITunesLibrary_win.cpp \
			   common/ITunesTrack.cpp \
               common/ITunesComWrapper.cpp \
               common/EncodingUtils.cpp \ # IT'S A DRY SIN
               iTunesCOMAPI/iTunesCOMInterface_i.c

    LIBS += -lcomsuppw

    DEFINES += _WIN32_DCOM
    RC_FILE = Twiddly.rc

    # Twiddly currently builds to iPodScrobblerd in debug builds, it's easier to handle
    # the manifest merging that way, but we rename it to iPodScrobbler afterwards as
    # that is how it was before (ask Max).
    CONFIG(debug, debug|release) {
        QMAKE_POST_LINK += && copy $$ROOT_DIR\bin\iPodScrobblerd.exe $$ROOT_DIR\bin\iPodScrobbler.exe
    }
}
