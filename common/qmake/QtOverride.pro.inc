
CONFIG( core ) {
    win32:QMAKE_INCDIR_QT = $$SRC_DIR/lib/core/QtOverride $$QMAKE_INCDIR_QT
    else:macx-xcode:QMAKE_INCDIR = $$SRC_DIR/lib/core/QtOverride $$QMAKE_INCDIR
    else:unix:QMAKE_CXX = $$QMAKE_CXX -I$$SRC_DIR/lib/core/QtOverride
}
