TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = lib/lastfm/core/libcore.pro \
          lib/lastfm/ws/libws.pro \
          lib/lastfm/types/libtypes.pro \
          lib/lastfm/radio/libradio.pro \
          lib/lastfm/scrobble/libscrobble.pro \
          lib/unicorn/libunicorn.pro \
          lib/listener/liblistener.pro \
          app/audioscrobbler \
          app/radio \
          app/twiddly

debug:win32 {
	# make the client the default project in visual studio
	SUBDIRS -= app/client
	SUBDIRS = app/client $$SUBDIRS
}

linux*:SUBDIRS -= app/twiddly

CONFIG( tests ) {
    SUBDIRS += \
        lib/lastfm/core/tests/test_libcore.pro \
        lib/lastfm/types/tests/test_libtypes.pro \
        lib/lastfm/scrobble/tests/test_libscrobble.pro \
        lib/listener/tests/test_liblistener.pro \
        app/client/tests/test_client.pro
}
