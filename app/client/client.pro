TEMPLATE = app
TARGET = Last.fm
CONFIG += unicorn radio scrobble listener fingerprint
VERSION = 2.0.0

QT = gui \
	 webkit svg \ #review at release
     opengl \ #phonon requires this bizarrely
     sql #doug's headers have an incredible vast dependency chain :P

macx-xcode:debug {
	#fucked up stuff to make a semi-useful xcodeproj
	#statically linking, essentially; it's a headache
	CONFIG += mad fftw3f samplerate taglib sqlite3
	CONFIG -= unicorn radio scrobble listener fingerprint
}

include( $$ROOT_DIR/admin/include.qmake )
include( _files.qmake )
!macx-xcode:generateBuildParameters()

INCLUDEPATH += .

macx* {
	QMAKE_INFO_PLIST = mac/Info.plist
	debug:ICON = mac/client.icns
	release:ICON = mac/client_debug.icns
}
macx-xcode:debug {
    # must be copied to $$ROOT_DIR before processing because qmake is broken
    # and the xcode generator is poor so we have elaborate workarounds
	system( admin/qpp lib )
	include( _files.qmake )
	system( admin/qpp app/clientplugins )
	include( _files.qmake )

	LIBS += -framework SystemConfiguration -framework CoreServices
	SOURCES += common/c++/Logger.cpp
    ICON = app/client/$$ICON
    QMAKE_INFO_PLIST = app/client/$$QMAKE_INFO_PLIST
	QT += sql
	INCLUDEPATH += app/client
	DEFINES += NPLUGINS
}
macx-g++:release {
    QT += sql # to make Makefile.dmg copy the QtSql framework into the bundle
	system( $$ROOT_DIR/admin/dist/mac/Makefile.dmg.pl $$LIBS > Makefile.dmg )
	QMAKE_EXTRA_INCLUDES += Makefile.dmg
}

win32 {
    RC_FILE = win/client.rc
    LIBS += -lshell32 -luser32
    PRECOMPILED_HEADER = $$ROOT_DIR/common/precompiled.h
    release:system( $$ROOT_DIR/admin/dist/win/isspp win/client.iss.in $$ROOT_DIR/Last.fm.iss )
}

linux* {
    SOURCES -= $$system( ls bootstrap/*.cpp )
    HEADERS -= $$system( ls bootstrap/*.h )
}
