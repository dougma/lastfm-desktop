TEMPLATE = app
TARGET = Last.fm
CONFIG += unicorn radio core ws types scrobble
QT = core gui xml network phonon webkit svg
# Qt is broken and phonon requires openGL! I emailed them for fix0rs
QT += opengl
# Required because Twiddly requires it
QT += sql
VERSION = 2.0.0

CONFIG += fingerprint

include( $$ROOT_DIR/common/qmake/include.pro )

generateVersionHeader()
QMAKE_EXTRA_INCLUDES += $$generateInstallerMakefile()

SOURCES   += $$findSources( cpp )
HEADERS   += $$findSources( h )
FORMS     += $$findSources( ui )
RESOURCES += $$findSources( qrc )

# included directly into App.cpp, so avoid link error
SOURCES -= legacy/disableHelperApp.cpp

macx* {
	QMAKE_INFO_PLIST = mac/Info.plist
	ICON = mac/client.icns
}
else {
	INCLUDEPATH += .
    win32:LIBS += -lshell32 -luser32
}

win32 {
	RC_FILE = win/client.rc
}

release:win32 {
	root = $$system( cygpath -m '$$ROOT_DIR' )
	qt = $$system( cygpath -m '$$QMAKE_LIBDIR_QT\\..' )

	LONG_VERSION = $$VERSION-$$system( sh $$ROOT_DIR/common/bash/svn_revision.sh )
	
	system( cp win/client.iss.in client.iss )
	system( perl -pi -e       's!\@VERSION\@!$$LONG_VERSION!g' client.iss )
	system( perl -pi -e 's!\@SHORT_VERSION\@!$$VERSION!g' client.iss )
	system( perl -pi -e      's!\@ROOT_DIR\@!$$root!g' client.iss )
	system( perl -pi -e        's!\@QT_DIR\@!$$qt!g' client.iss )
	system( perl -pi -e       's!\@BIN_DIR\@!$$root/_bin!g' client.iss )
}

linux* {
    SOURCES -= $$findSources( cpp, bootstrap )
    HEADERS -= $$findSources( h, bootstrap )
}
