TEMPLATE = lib
TARGET = breakpad
CONFIG -= gui

include( $$SRC_DIR/common/qmake/include.pro )

INCLUDEPATH += . external/src
SOURCES = BreakPad.cpp
HEADERS = BreakPad.h

!win32 {
	SOURCES += $$SYSTEM( ls external/src/client/*.cc )
	SOURCES += external/src/common/convert_UTF.c external/src/common/string_conversion.cc
}

mac* {
    SOURCES += external/src/common/mac/dump_syms.mm
    SOURCES += external/src/common/mac/file_id.cc
    SOURCES += external/src/common/mac/macho_id.cc
    SOURCES += external/src/common/mac/macho_utilities.cc
    SOURCES += external/src/common/mac/macho_walker.cc
    SOURCES += external/src/common/mac/string_utilities.cc
    
    SOURCES += external/src/client/mac/handler/breakpad_nlist_64.cc
    SOURCES += external/src/client/mac/handler/dynamic_images.cc
    SOURCES += external/src/client/mac/handler/exception_handler.cc
    SOURCES += external/src/client/mac/handler/minidump_generator.cc
    SOURCES += external/src/client/mac/handler/protected_memory_allocator.cc
    
    LIBS += -lcrypto
}

win32 {
    SOURCES += external/src/client/windows/handler/exception_handler.cc
    SOURCES += external/src/client/windows/crash_generation/crash_generation_client.cc
    SOURCES += external/src/common/windows/guid_string.cc

    LIBS += ole32.lib
    
    DEFINES += BREAKPAD_DLLEXPORT_PRO
}

linux* {
    SOURCES += $$SYSTEM( find external/src/client/linux -name \*test\* -prune -o -name \*.cc -print )
    
    common = external/src/common/linux/
    SOURCES += $$common/dump_symbols.cc $$common/file_id.cc $$common/guid_creator.cc

    SOURCES += external/src/common/md5.c
}
