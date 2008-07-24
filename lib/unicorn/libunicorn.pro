TARGET = unicorn
TEMPLATE = lib
QT = core gui
CONFIG += ws core

include( $$SRC_DIR/common/qmake/include.pro )

SOURCES   = $$findSources( cpp )
HEADERS   = $$findSources( h )
FORMS     = $$findSources( ui )
RESOURCES = $$findSources(qrc )

!win32 {
    SOURCES -= UnicornUtils_win.cpp
}
!macx {
    SOURCES -= UnicornUtils_mac.cpp
}

DEFINES += _UNICORN_DLLEXPORT
