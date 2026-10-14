
QT += core
QT -= gui

CONFIG += c++11

TARGET = hfpdshared
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    message.cpp \
    messagefactory.cpp \
    nordicfirmware.cpp

HEADERS += \
    message.h \
    messagefactory.h \
    ping.h \
    pong.h \
    devicelist.h \
    devicelistreq.h \
    deviceconnect.h \
    devicedisconnect.h \
    devicestate.h \
    devicereadallconfig.h \
    devicereadconfig.h \
    devicecmdresult.h \
    deviceconfig.h \
    devicewriteconfig.h \
    deviceclearallmessages.h \
    deviceerror.h \
    devicesendmessage.h \
    deviceopnamelist.h \
    deviceflashfirmwareota.h \
    nordicfirmware.h \
    devicefirmwarerequest.h \
    devicefirmwareprogress.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
