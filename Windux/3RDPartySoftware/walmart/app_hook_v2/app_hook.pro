QT += core gui network sql concurrent

TEMPLATE = lib
DEFINES += HOBARTCUSTOMERHOOK_EXPORTS

CONFIG += c++11

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
    downloadfileurlrequest.cpp \
    errordialogmgr.cpp \
    eventcontrollerrequest.cpp \
    fetchallproductsrequest.cpp \
    hookwalmart.cpp \
    hookwalmartbase.cpp \
    labelparser.cpp \
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
    downloadfileurlrequest.h \
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
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
