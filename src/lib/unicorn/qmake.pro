TARGET = unicorn
TEMPLATE = lib

# See UnicornDllExportMacro.h
DEFINES += UNICORN_DLLEXPORT_PRO

include( $$SRC_DIR/include.pro )

SOURCES += $$system( ls *.cpp ) md5.c
HEADERS += $$system( ls *.h )

!win32:SOURCES -= UnicornCommonWin.cpp
!mac*:SOURCES -= UnicornCommonMac.cpp
