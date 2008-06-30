TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = lib/unicorn/libunicorn.pro \
          lib/radio/libradio.pro \ 
          lib/moose/libmoose.pro \
          app/client/appclient.pro
