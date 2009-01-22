TARGET = core
TEMPLATE = lib
DEFINES += _CORE_DLLEXPORT

QT = core xml
# UniqueApplication on Windows needs QWidget sadly
win32: QT += gui

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

# AppleScript stuff
macx:LIBS += -framework Carbon

win32:LIBS += user32.lib shell32.lib

headers.files = $$HEADERS
headers.files -= CoreSettings.h
headers.path = $$INSTALL_DIR/include/lastfm/core
INSTALLS = target headers
