TARGET = resolver
TEMPLATE = lib
CONFIG += qt dll core taglib
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

win32:LIBS += -lmpr
