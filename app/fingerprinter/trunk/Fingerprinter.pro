TEMPLATE = subdirs

# build directories as they are ordered
CONFIG = ordered

mac* {
    SUBDIRS += src/libs/taglib/ \
               src/libFingerprint/libs/libmad/libmad.pro
}

SUBDIRS += \
        src/libLastFmTools/ \
        src/libFingerprint/ \
        src/


