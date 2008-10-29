TARGET = resolver
TEMPLATE = lib
CONFIG += qt dll core
QT += sql

include( $$ROOT_DIR/common/qmake/include.pro )

INCLUDEPATH +=	\
        qt \
        fplib/include

SOURCES   = $$findSources( cpp )
HEADERS   = $$findSources( h )
#FORMS     = $$findSources( ui )
#RESOURCES = $$findSources( qrc )

SOURCES -= test.cpp

LIBS += -L$$BIN_DIR 
win32:LIBS += -lmpr
win32:debug:LIBS += -ltagd
win32:!debug:LIBS += -ltag
