TARGET = resolver_plugin
TEMPLATE = lib
CONFIG += qt dll core
QT = core sql
CONFIG += taglib ws types sqlite3 boost resolver

#TODO rename localresolver.pro

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

win32 {
    LIBS += -lmpr
}
