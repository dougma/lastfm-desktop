TARGET = unicorn
TEMPLATE = lib

#fixme make libwebservice
QT += network xml

#fixme make UnicornGui
QT += gui

include( $$SRC_DIR/include.pro )

SOURCES += $$system( find . -name \*.cpp )
HEADERS += $$system( find . -name \*.h ) \
           qmake.pro \
           QtOverride/QHttp QtOverride/QSystemTrayIcon QtOverride/QMessageBox

!win32 {
    SOURCES -= ./UnicornCommonWin.cpp
    HEADERS -= ./UnicornCommonWin.h
}
!mac* {
    SOURCES -= ./UnicornCommonMac.cpp ./AppleScript.cpp ./QtOverride/QSystemTrayIcon.cpp
    HEADERS -= ./UnicornCommonMac.h ./AppleScript.h
}

DEFINES += _UNICORN_DLLEXPORT