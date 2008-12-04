QT = core testlib xml network
TEMPLATE = app
INCLUDEPATH += ..
CONFIG += ws core types precompile_header
TARGET = test_libscrobble

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES += $$findSources( cpp )
HEADERS += $$findSources( h, .. ) $$findSources( h )

PRECOMPILED_HEADER = pc.h
