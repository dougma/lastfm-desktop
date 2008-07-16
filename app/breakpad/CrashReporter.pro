TEMPLATE = app
QT = core gui network xml
TARGET = CrashReporter

include( $$ROOT_DIR/common/qmake/include.pro )

win32:LIBS += -luser32

FORMS = CrashReporter.ui
HEADERS = CrashReporter.h
SOURCES = main.cpp CrashReporter.cpp
