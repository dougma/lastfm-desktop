TARGET = resolver
TEMPLATE = lib
CONFIG += qt dll core
QT = core sql
CONFIG += taglib ws types sqlite3

#TODO rename localresolver.pro

include( $$ROOT_DIR/common/qmake/include.pro )

SOURCES  = $$findSources( cpp )
HEADERS  = $$findSources( h )

SOURCES -= test.cpp

win32:LIBS += -lmpr

QMAKE_CXXFLAGS += -Zi 
QMAKE_LFLAGS += /DEBUG /OPT:ICF /OPT:REF