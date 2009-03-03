TARGET = core
TEMPLATE = lib
DEFINES += _CORE_DLLEXPORT
QT = core xml

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

macx:LIBS += -framework Carbon # for mac/AppleScript.*

win32:LIBS += shell32.lib

headers.files = UrlBuilder.h
headers.path = $$INSTALL_DIR/include
INSTALLS = target headers
