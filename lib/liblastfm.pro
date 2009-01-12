TEMPLATE = subdirs
SUBDIRS += lastfm/core/libcore.pro \
           lastfm/ws/libws.pro \
           lastfm/types/libtypes.pro \
           lastfm/scrobble/libscrobble.pro \
           lastfm/radio/libradio.pro

dllexport.files = lastfm/public.h
dllexport.path = $$INSTALL_DIR/include/lastfm
INSTALLS = dllexport
