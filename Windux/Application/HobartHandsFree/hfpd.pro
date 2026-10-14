TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += ./shared/hfpdshared.pro
SUBDIRS += ./ui/hfpdui.pro

unix {
    SUBDIRS += ./daemon/hfpdd.pro
}

