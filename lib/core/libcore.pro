TARGET = core
TEMPLATE = lib
QT = core gui xml

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h )

DEFINES += _CORE_DLLEXPORT

!macx {
    SOURCES -= mac/AppleScript.cpp UnicornUtils_mac.cpp mac/Growl.cpp
	HEADERS -= mac/AppleScript.h mac/Growl.h
}
!win32 {
    SOURCES -= UnicornUtils_win.cpp
}
