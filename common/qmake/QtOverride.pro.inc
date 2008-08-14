
CONFIG( core ) {
    win32:QMAKE_INCDIR_QT = $$ROOT_DIR/lib/core/QtOverride $$QMAKE_INCDIR_QT
    else:macx-xcode:QMAKE_INCDIR = $$ROOT_DIR/lib/core/QtOverride $$QMAKE_INCDIR
    else:unix:QMAKE_CXX = $$QMAKE_CXX -I$$ROOT_DIR/lib/core/QtOverride
}
