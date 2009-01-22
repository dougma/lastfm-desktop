CONFIG( unicorn ) {
  	LIBS += -lunicorn
	CONFIG += types
	QT += gui
}
CONFIG( listener ) {
	LIBS += -llistener
	CONFIG += types
}
CONFIG( radio ) {
	LIBS += -lradio
	CONFIG += types
	QT += phonon
}
CONFIG( scrobble ) {
	LIBS += -lscrobble
	CONFIG += types
}
CONFIG( fingerprint ) {
	LIBS += -lfingerprint
	CONFIG += types
}
CONFIG( types ) {
	LIBS += -ltypes
	CONFIG += ws
}
CONFIG( ws ) {
    LIBS += -lws
	CONFIG += core
    QT += network
}
CONFIG( core ) {
	LIBS += -lcore
	QT += core xml
}

# so #include <lastfm/*.h> works
INCLUDEPATH += $$ROOT_DIR/lib
