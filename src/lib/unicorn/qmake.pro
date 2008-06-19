TARGET = unicorn
TEMPLATE = lib
QT += override

#fixme make libwebservice
QT += network xml

include( $$SRC_DIR/include.pro )

SOURCES += $$system( find . -name \*.cpp )
HEADERS += $$system( find . -name \*.h ) qmake.pro

!win32 {
    SOURCES -= ./UnicornCommonWin.cpp
    HEADERS -= ./UnicornCommonWin.h
}
!mac* {
    SOURCES -= ./UnicornCommonMac.cpp ./AppleScript.cpp ./QtOverride/QSystemTrayIcon.cpp
    HEADERS -= ./UnicornCommonMac.h ./AppleScript.h
}

INCLUDEPATH += .