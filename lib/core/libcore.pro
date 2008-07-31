TARGET = core
TEMPLATE = lib
QT = core gui xml

include( $$SRC_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h )

DEFINES += _CORE_DLLEXPORT

!macx {
    SOURCES -= AppleScript.cpp QtOverride/QSystemTrayIcon.cpp UnicornUtils_mac.cpp
    HEADERS -= AppleScript.h
}
!win32 {
    SOURCES -= UnicornUtils_win.cpp
}
