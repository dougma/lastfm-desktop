
CONFIG( unicorn ):horn = yes
contains( TARGET, unicorn ):horn = yes

contains( horn, yes ) {
    win32:QMAKE_INCDIR_QT = $$SRC_DIR/lib/unicorn/QtOverride $$QMAKE_INCDIR_QT
    else:macx-xcode:QMAKE_INCDIR = $$SRC_DIR/lib/unicorn/QtOverride $$QMAKE_INCDIR
    else:unix:QMAKE_CXX = $$QMAKE_CXX -I$$SRC_DIR/lib/unicorn/QtOverride
}
