TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = lib/unicorn/libunicorn.pro \
          lib/radio/libradio.pro \ 
          lib/moose/libmoose.pro \
          lib/moose/tests/tests.pro \
          app/client/appclient.pro \
          app/client/tests/tests.pro
