TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = lib/unicorn \
          lib/listener \
          app/configurator \
          app/audioscrobbler \
          app/radio \
          app/twiddly \
          app/boffin

debug:win32 {
    # reorder things to set the default project in visual studio
    SUBDIRS -= app/configurator
    SUBDIRS = app/configurator $$SUBDIRS
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
