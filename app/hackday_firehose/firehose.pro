
CONFIG += core ws types
CONFIG -= app_bundle

include( $$ROOT_DIR/common/qmake/include.pro )

INCLUDEPATH += ../client

SOURCES += $$findSources( cpp )
HEADERS += $$findSources( h )
