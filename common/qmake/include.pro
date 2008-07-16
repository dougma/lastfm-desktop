
include( findsources.pro.inc )

include( breakpad.pro.inc )
include( debug.pro.inc )
#win32:include( manifest.pro.inc )
include( QtOverride.pro.inc )

linux*:CONFIG += link_pkgconfig

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

CONFIG( taglib ) {
    linux*: PKGCONFIG += taglib
    else: LIBS += -L$$COMMON_LIBS_DIR/taglib/taglib -ltag
}

CONFIG( mad ) {
    linux*: PKGCONFIG += mad
    else: LIBS += -L$$COMMON_LIBS_DIR/libmad -lmad
}
