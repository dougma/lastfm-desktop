TARGET = ws
TEMPLATE = lib
QT = core network xml gui
CONFIG += core

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h )

DEFINES += _WS_DLLEXPORT

!win32 {
    SOURCES -= win/NdisEvents.cpp win/WmiSink.cpp win/Pac.cpp WsAccessManagerInit.cpp
    HEADERS -= win/NdisEvents.h win/WmiSink.h win/Pac.h
} else {
    DEFINES += _ATL_DLL
    LIBS += winhttp.lib wbemuuid.lib
}

macx*:LIBS += -framework SystemConfiguration

headers.files = WsAccessManager.h WsError.h WsKeys.h WsRequestBuilder.h WsReply.h
headers.path = $$INSTALL_DIR/include/lastfm/ws
INSTALLS = target headers
