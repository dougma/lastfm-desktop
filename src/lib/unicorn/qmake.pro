TARGET = unicorn
TEMPLATE = lib

include( $$SRC_DIR/include.pro )

SOURCES += $$system( ls *.cpp ) md5.c
HEADERS += $$system( ls *.h ) qmake.pro

!win32:SOURCES -= UnicornCommonWin.cpp
!mac*:SOURCES -= UnicornCommonMac.cpp
