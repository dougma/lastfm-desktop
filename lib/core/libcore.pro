TARGET = core
TEMPLATE = lib
QT = core gui xml

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h )

DEFINES += _CORE_DLLEXPORT

!macx {
    SOURCES -= mac/AppleScript.cpp mac/Growl.cpp mac/CFStringToQString.cpp
	HEADERS -= mac/AppleScript.h mac/Growl.h mac/CFStringToQString.h
}
!win32 {
    SOURCES -= win/Utils.cpp
	HEADERS -= win/Utils.h
}

INSTALLS = $$defaultInstallTargets()
