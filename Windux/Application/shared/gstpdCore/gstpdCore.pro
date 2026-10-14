#-------------------------------------------------
#
# Project created by QtCreator 2015-06-09T11:28:12
#
#-------------------------------------------------

QT       += core
QT       -= gui

unix:!symbian {
    QT += dbus
}

TARGET = gstpdCore
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    abstractmediaplayer.cpp \
    dbusmediaplayer.cpp \
    gstmp.cpp \
    custgstmp.cpp \
    opergstmp.cpp \
    dbusmediaplayerclient.cpp

HEADERS += \
    abstractmediaplayer.h \
    dbusmediaplayer.h \
    gstmp.h \
    custgstmp.h \
    opergstmp.h \
    dbusmediaplayerclient.h
    
unix {
    target.path = /usr/lib
    INSTALLS += target
}

#
# logManager
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../logManager/release/ -llogManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../logManager/debug/ -llogManager
else:symbian: LIBS += -llogManager
else:unix: LIBS += -L$$OUT_PWD/../logManager/ -llogManager

INCLUDEPATH += $$PWD/../logManager
DEPENDPATH += $$PWD/../logManager

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/release/logManager.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/debug/logManager.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../logManager/liblogManager.a

#
# gstreamer
#

unix:!symbian {
    CONFIG += link_pkgconfig
    PKGCONFIG += gtk+-2.0 gstreamer-1.0
    LIBS += -lgstapp-1.0 -lm
}

#
# freescale
#

linux-timesys-g++ {
    DEFINES += ARM_MXC_FREESCALE
}
