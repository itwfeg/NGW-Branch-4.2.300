
QT += core bluetooth network
QT -= gui

# bluez library does not like c++11
#CONFIG += c++11

TARGET = hfpdd
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    blescanneragent.cpp \
    hciscanneragent.cpp \
    hciscanneragentthread.cpp \
    leadvertinfo.cpp \
    scanneragent.cpp \
    scanneragentwatchdog.cpp \
    hobarttagmanager.cpp \
    bledevice.cpp \
    hobarttag.cpp \
    hobarttag_v0.cpp \
    hobarttag_v1.cpp \
    commserver.cpp \
    commclient.cpp \
    otaupgrademanager.cpp \
    dfudevice.cpp

HEADERS += \
    blescanneragent.h \
    hciscanneragent.h \
    hciscanneragentthread.h \
    leadvertinfo.h \
    scanneragent.h \
    scanneragentwatchdog.h \
    hobarttagmanager.h \
    bledevice.h \
    hobarttag.h \
    hobarttag_v0.h \
    hobarttag_v1.h \
    commserver.h \
    commclient.h \
    otaupgrademanager.h \
    dfudevice.h

#
# hfpdshared
#
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../shared/release/ -lhfpdshared
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../shared/debug/ -lhfpdshared
else:unix:!symbian: LIBS += -L$$OUT_PWD/../shared/ -lhfpdshared

INCLUDEPATH += $$PWD/../shared
DEPENDPATH += $$PWD/../shared

#
# bluez
#
unix:!symbian: LIBS += -lbluetooth
