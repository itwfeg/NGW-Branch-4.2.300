#-------------------------------------------------
#
# Project created by QtCreator 2017-10-09T10:39:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = autocalibrationrig
TEMPLATE = app

CONFIG += link_pkgconfig \
          resources_big

SOURCES += \
    calibui.cpp \
    main.cpp \
    failure.cpp \
    generate.cpp \
    accelcalib.cpp \
    common.cpp \
    matrix.cpp \
    vector.cpp \
    poly.cpp \
    autocalibrationrig.cpp \
    hobart/can/candev.cpp \
    hobart/can/candevman.cpp \
    hobart/can/cansocket.cpp \
    hobart/can/hobartcandev.cpp \
    hobart/can/hobartcanprinter.cpp \
    hobart/can/hobartcanweigher.cpp \
    hobart/can/kpc.cpp \
    hobart/rs485/calrigrs485.cpp \
    hobart/hobartprinter.cpp \
    hobart/hobartweigher.cpp \
    hobart/mtrd24motor.cpp \
    readThread.cpp

HEADERS += \
    calibui.h \
    failure.h \
    flash.h \
    generate.h \
    accelcalib.h \
    common.h \
    matrix.h \
    vector.h \
    poly.h \
    autocalibrationrig.h \
    hobart/can/candev.h \
    hobart/can/candevman.h \
    hobart/can/cansocket.h \
    hobart/can/hobartcandev.h \
    hobart/can/hobartcanprinter.h \
    hobart/can/hobartcanweigher.h \
    hobart/can/kpc.h \
    hobart/hobartprinter.h \
    hobart/hobartweigher.h \
    hobart/rs485/calrigrs485.h \
    hobart/mtrd24motor.h \
    readThread.h

FORMS += \
    calibui.ui \
    failure.ui

RESOURCES += \
    autocalibrationrig.qrc
target.path += /usr/bin/
INSTALLS += target
