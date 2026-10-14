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
# 4) hobart_internal.config
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
    hookwalmartrfid.cpp \
    main.cpp \
    networkaccess.cpp \
    qtappprivate.cpp

HEADERS += \
    HobartCustomerHook.h \
    common.h \
    hookwalmartrfid.h \
    mutextrylocker.h \
    networkaccess.h \
    qtappprivate.h

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
