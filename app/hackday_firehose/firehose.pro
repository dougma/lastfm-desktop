CONFIG += core
CONFIG -= app_bundle

QT = core gui network xml

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

INCLUDEPATH += ../client
