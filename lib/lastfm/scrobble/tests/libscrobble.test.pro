QT = core testlib xml network
TEMPLATE = app
INCLUDEPATH += ..
CONFIG += ws core types
TARGET = test_scrobbler

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES += $$findSources( cpp )
HEADERS += $$findSources( h, .. )
