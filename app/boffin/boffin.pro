CONFIG += core types unicorn resolver radio sqlite3 taglib
QT = gui xml sql
VERSION = 0.0.1

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

macx*:QMAKE_INFO_PLIST = mac/Info.plist
