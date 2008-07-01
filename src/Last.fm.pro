TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += lib/unicorn/libunicorn.pro \
           lib/moose/libmoose.pro \
           lib/radio/libradio.pro \ 
           app/client/appclient.pro
