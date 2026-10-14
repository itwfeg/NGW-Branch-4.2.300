#-------------------------------------------------
#
# Project created by QtCreator 2012-10-10T16:38:41
#
#-------------------------------------------------

QT       += core widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = logManager
TEMPLATE = lib
CONFIG += staticlib

SOURCES += logmanager.cpp \
    logappender.cpp \
    filelog.cpp \
    logline.cpp \
    log.cpp \
    qtlog.cpp \
    consolelog.cpp

HEADERS += logmanager.h \
    log.h \
    logappender.h \
    filelog.h \
    qtlog.h \
    logline.h \
    consolelog.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
