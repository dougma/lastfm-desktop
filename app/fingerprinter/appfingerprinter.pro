TEMPLATE = app
TARGET = 'Last.fm Fingerprinter'
linux*:TARGET = last.fingerprinter
QT = core gui network xml sql
CONFIG += unicorn fingerprint taglib mad
VERSION = 1.1.0

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )
