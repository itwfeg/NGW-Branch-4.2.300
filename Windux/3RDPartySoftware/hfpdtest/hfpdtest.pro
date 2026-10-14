#-------------------------------------------------
#
# Project created by QtCreator 2018-04-12T10:28:56
#
#-------------------------------------------------

QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hfpdtest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    devicediscovery.cpp \
    deviceinfo.cpp \
    scanneragent/blescanneragent.cpp \
    scanneragent/hciscanneragent.cpp \
    scanneragent/hciscanneragentthread.cpp \
    scanneragent/leadvertinfo.cpp \
    scanneragent/scanneragent.cpp \
    device/bledevice.cpp \
    device/hfpdtag.cpp \
    device/hfpdtag_v2.cpp

HEADERS  += mainwindow.h \
    devicediscovery.h \
    deviceinfo.h \
    scanneragent/blescanneragent.h \
    scanneragent/hciscanneragent.h \
    scanneragent/hciscanneragentthread.h \
    scanneragent/leadvertinfo.h \
    scanneragent/scanneragent.h \
    device/bledevice.h \
    device/hfpdtag.h \
    device/hfpdtag_v2.h

#
# bluez
#

unix:!symbian: LIBS += -lbluetooth
