
include( findsources.pro.inc )

include( breakpad.pro.inc )
include( debug.pro.inc )
#win32:include( manifest.pro.inc )
include( QtOverride.pro.inc )

CONFIG( service ) {
    CONFIG += plugin
    TARGET = $$TARGET.service
}

CONFIG( extension ) {
    CONFIG += plugin
    TARGET = $$TARGET.extension
}

CONFIG( unicorn ) {
    LIBS += -lunicorn
}

CONFIG( moose ) {
    LIBS += -lmoose
}

CONFIG( radio ) {
    LIBS += -lradio
}

CONFIG( qtestlib ) {
    DESTDIR = $$DESTDIR/tests
}
