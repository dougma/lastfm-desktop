TARGET = core
TEMPLATE = lib
QT = core xml

# UniqueApplication on Windows needs QWidget sadly
win32: QT += gui

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES = $$findSources( cpp )
HEADERS = $$findSources( h )

DEFINES += _CORE_DLLEXPORT

macx {
	# AppleScript stuff
	LIBS += -framework Carbon
}

win32:LIBS += user32.lib shell32.lib

headers.files = $$HEADERS
headers.files -= CoreSettings.h
headers.path = $$INSTALL_DIR/include/lastfm/core
INSTALLS = target headers
