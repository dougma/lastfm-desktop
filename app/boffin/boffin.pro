CONFIG += core types unicorn
QT = gui xml sql

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

LIBS += -L$$DESTDIR

