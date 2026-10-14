#-------------------------------------------------
#
# Project created by QtCreator 2015-04-09T10:18:14
#
#-------------------------------------------------

QT       += core sql
QT       -= gui

TARGET = bluez
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#DEFINES += _DEBUG_HTTP
#DEFINES += _DEBUG_SENSOR

SOURCES += bluez.cpp \
    hcidev.cpp \
    hcidevthread.cpp \
    leadvertinfo.cpp \
    httputil.cpp \
    bluenicauploader.cpp \
    bluenicadatarec.cpp \
    bluenicatempdatarec.cpp \
    bluenicaconfigdatarec.cpp \
    dbuploader.cpp

HEADERS += \
    hcidev.h \
    hcidevthread.h \
    leadvertinfo.h \
    httputil.h \
    bluenicauploader.h \
    bluenica.h \
    bluenicadatarec.h \
    bluenicaconfigdatarec.h \
    bluenicatempdatarec.h \
    dbuploader.h \
    dbtables.h

#
# bluez
#

linux-timesys-g++ {
    LIBS += -lbluetooth
} else:unix:!symbian {
    LIBS += -lbluetooth
}

