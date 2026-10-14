QT -= gui
QT += network sql concurrent

DEFINES += HOBARTCUSTOMERHOOK_TEST_APPLICATION

CONFIG += c++11 console
CONFIG -= app_bundle

# !! NOTE !!
# To run this project on a Windows machine you will need the following files placed into the build folder:
# 1) dbftX.db
# 2) dbftX.db-shm   [if exists]
# 3) dbftX.db-wal   [if exists]
#    pull the database files from the /usr/local/hobart/db/scale/dbfiles folder on a scale
# 4) FEATURES_INSTALLED
#    pull from /usr/local/hobart/features folder on a scale
# 5) hobart_internal.config
# 6) web_headers.config
#    copy this from the project folder; you will need to edit hobart_internal.config to point to proper server
# 7) mtd3
# 8) mtd6
#    ssh into a scale and run the following commands:
#    $ cat /dev/mtd3ro > /tmp/mtd3
#    $ cat /dev/mtd6ro > /tmp/mtd6
#    then pull the mtd3 and mtd6 files from the /tmp folder; open them in an editor and delete all the extra data after the XML part
# 9) plu_whitelist.csv
#    this can be pulled from a scale in the remote print feature (if installed) or Windux/3RDPartySoftware/walmart/remote_print folder otherwise
# 10) SOFTWARE_VERSION_INFO
# 11) uBootVersion
#     this can be pulled from root folder / on a scale
# 12) scalesettings
#     pull from /usr/local/hobart/db/settingswizard folder on a scale
#


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    HobartCustomerHook.cpp \
    abstractapi.cpp \
    abstractrequest.cpp \
    apiwalmart.cpp \
    callbackrequest.cpp \
    configrequest.cpp \
    coolrec.cpp \
    dbutil.cpp \
    downloadfilerequest.cpp \
    errordialogmgr.cpp \
    eventcontrollerrequest.cpp \
    fetchallproductsrequest.cpp \
    hookwalmart.cpp \
    hookwalmartbase.cpp \
    labelparser.cpp \
    main.cpp \
    networkaccess.cpp \
    networkcheckingthread.cpp \
    nutrifactsparser.cpp \
    productrec.cpp \
    qtappprivate.cpp \
    scaleblockedplus.cpp \
    scaleconfig.cpp \
    scaledb.cpp \
    scalefirmware.cpp \
    scaleperipherals.cpp \
    scalesettings.cpp \
    scaleusagetoday.cpp \
    settingsrequest.cpp

HEADERS += \
    HobartCustomerHook.h \
    abstractapi.h \
    abstractrequest.h \
    apiwalmart.h \
    callbackrequest.h \
    common.h \
    configrequest.h \
    coolrec.h \
    dbutil.h \
    downloadfilerequest.h \
    errordialogmgr.h \
    eventcontrollerrequest.h \
    fetchallproductsrequest.h \
    hookwalmart.h \
    hookwalmartbase.h \
    labelparser.h \
    mutextrylocker.h \
    networkaccess.h \
    networkcheckingthread.h \
    nutrifactsparser.h \
    productrec.h \
    qtappprivate.h \
    scaleblockedplus.h \
    scaleconfig.h \
    scaledb.h \
    scalefirmware.h \
    scaleperipherals.h \
    scalesettings.h \
    scaleusagetoday.h \
    settingsrequest.h \
    stringsconfig.h \
    stringsjson.h

# Dependancies
win32 {
    LIBS += Rpcrt4.lib
} else: unix {
    CONFIG+=link_pkgconfig
    PKGCONFIG+=clio x11 x11-xcb
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
