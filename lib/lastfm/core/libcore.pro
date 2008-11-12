TARGET = core
TEMPLATE = lib
QT = core xml

# UniqueApplication on Windows needs QWidget sadly
win32: QT += gui

# AppleScript stuff
macx*: LIBS += -framework Carbon

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

headers.files = $$HEADERS
headers.files -= CoreSettings.h
headers.path = $$INSTALL_DIR/include/lastfm/core
INSTALLS = target headers
