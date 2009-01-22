win32:QMAKE_INCDIR_QT = $$ROOT_DIR/common/qt/override $$QMAKE_INCDIR_QT
else:!macx-xcode:QMAKE_CXX = $$QMAKE_CXX -I$$ROOT_DIR/common/qt/override
