TARGET = unicorn
TEMPLATE = lib

#fixme make libwebservice
QT += network xml
#fixme make UnicornGui
QT += gui

include( $$SRC_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h ) \
           QtOverride/QHttp QtOverride/QSystemTrayIcon QtOverride/QMessageBox
FORMS   = $$findSources( ui )

!win32 {
    SOURCES -= UnicornCommonWin.cpp
    HEADERS -= UnicornCommonWin.h
}
!macx {
    SOURCES -= UnicornCommonMac.cpp AppleScript.cpp QtOverride/QSystemTrayIcon.cpp
    HEADERS -= UnicornCommonMac.h AppleScript.h
}

macx*:LIBS += -framework SystemConfiguration -framework CoreServices

DEFINES += _UNICORN_DLLEXPORT
