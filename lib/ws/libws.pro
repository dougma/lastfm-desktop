TARGET = ws
TEMPLATE = lib
QT = core network xml gui
CONFIG += core

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h )

DEFINES += _WS_DLLEXPORT

!win32:SOURCES -= sens_win.cpp
!win32:HEADERS -= sens_win.h
win32:DEFINES += _ATL_DLL

win32:LIBS += winhttp.lib
