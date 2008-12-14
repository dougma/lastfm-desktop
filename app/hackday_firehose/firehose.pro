
CONFIG += core
CONFIG -= app_bundle

QT = core gui network xml

include( $$ROOT_DIR/common/qmake/include.pro )

INCLUDEPATH += ../client

SOURCES += $$findSources( cpp )
HEADERS += $$findSources( h )
