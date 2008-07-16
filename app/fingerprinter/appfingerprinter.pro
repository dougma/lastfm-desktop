TEMPLATE = app
TARGET = 'Last.fm Fingerprinter'
linux*:TARGET = last.fingerprinter
QT = core gui network xml sql
CONFIG += unicorn fingerprint taglib mad

include( $$ROOT_DIR/common/qmake/include.pro )

HEADERS = $$findSources( h )
SOURCES = $$findSources( cpp )
FORMS = $$findSources( ui )
