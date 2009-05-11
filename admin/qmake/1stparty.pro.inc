CONFIG( unicorn ) {
    LIBS += -lunicorn
    QT += gui
    CONFIG += lastfm
}
CONFIG( listener ) {
    LIBS += -llistener
    CONFIG += lastfm
}
CONFIG( fingerprint ) {
    LIBS += -llastfm_fingerprint
    CONFIG += lastfm
}
CONFIG( lastfm ) {
    QT += core network xml
    LIBS += -llastfm
    win32 {
        CONFIG += link_pkgconfig
        PKGCONFIG += lastfm
    }
}
