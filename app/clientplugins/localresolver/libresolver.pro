TARGET = resolver
TEMPLATE = lib
CONFIG += qt dll core
QT = core sql
CONFIG += taglib ws types sqlite3 boost

#TODO rename localresolver.pro

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

SOURCES -= test.cpp

win32 {
    LIBS += -lmpr
}
