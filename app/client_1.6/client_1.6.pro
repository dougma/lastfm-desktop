TEMPLATE = app
TARGET = Last.fm
CONFIG += unicorn radio scrobble listener fingerprint
VERSION = 1.5.99

# this header contains all the conversions from old classes to new
PRECOMPILED_HEADER = pc.h
CONFIG += precompile_header

QT = gui \
	 webkit svg \ #review at release
     opengl

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )

INCLUDEPATH += ../client

HEADERS += ../client/ExtractIdentifiersJob.h \
           ../client/StateMachine.h \
           ../client/StopWatch.h \
           ../client/widgets/DiagnosticsDialog.h
SOURCES += ../client/ExtractIdentifiersJob.cpp \
           ../client/StateMachine.cpp \
           ../client/StopWatch.cpp \
           ../client/widgets/DiagnosticsDialog.cpp
FORMS   += ../client/widgets/DiagnosticsDialog.ui