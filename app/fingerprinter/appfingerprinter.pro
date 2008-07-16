TEMPLATE = app
TARGET = 'Last.fm Fingerprinter'
linux*:TARGET = last.fingerprinter
QT = core gui network xml sql
CONFIG += unicorn fingerprint

LASTFM_COMMON_LIBS += taglib mad

include( $$ROOT_DIR/common/qmake/include.pro )

HEADERS = $$findSources( h )
SOURCES = $$findSources( cpp )
FORMS = $$findSources( ui )
