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
    apiwalmart.cpp \
    customerhook.cpp \
    errordialogmgr.cpp \
    healthstatusthread.cpp \
    labelparser.cpp \
    networkaccess.cpp \
    networkcheckingthread.cpp \
    qtappprivate.cpp

HEADERS += \
    HobartCustomerHook.h \
    apiwalmart.h \
    customerhook.h \
    errordialogmgr.h \
    healthstatusthread.h \
    labelparser.h \
    mutextrylocker.h \
    networkaccess.h \
    networkcheckingthread.h \
    qtappprivate.h

# Dependancies
unix {
    CONFIG+=link_pkgconfig
    PKGCONFIG+=clio x11 x11-xcb
}

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
