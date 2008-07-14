TARGET = unicorn
TEMPLATE = lib

#fixme make libwebservice
QT += network xml
#fixme make UnicornGui
QT += gui

include( $$SRC_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h )
FORMS   = $$findSources( ui )

!win32 {
    SOURCES -= UnicornUtils_win.cpp
}
!macx {
    SOURCES -= UnicornUtils_mac.cpp AppleScript.cpp QtOverride/QSystemTrayIcon.cpp
    HEADERS -= AppleScript.h
}

macx*:LIBS += -framework SystemConfiguration -framework CoreServices

DEFINES += _UNICORN_DLLEXPORT
