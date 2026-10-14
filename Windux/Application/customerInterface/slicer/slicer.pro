#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T15:33:40
#
#-------------------------------------------------

QT       += \
    core gui \
    multimedia \
    multimediawidgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = slicerui
TEMPLATE = app

QMAKE_CXXFLAGS += -fpermissive
QMAKE_CXXFLAGS += -g

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    scale.cpp \
    simulatedscaleinterface.cpp \
    slicerstatemachine.cpp \
    kniferunin.cpp \
    onpausebuttonsim.cpp \
    weighconfig.cpp \
    weighcalib.cpp \
    mainmenu.cpp \
    msgbox.cpp \
    speedkeyconfig.cpp \
    msgboxconfig.cpp \
    supervisormenu.cpp \
    servicemenu.cpp \
    hobart/usb/weigherconfiguration.cpp \
    keysignalhandler.cpp \
    keypad.cpp \
    inputwidgethook.cpp \
    speedkey.cpp\
    calibratetypepopup.cpp \
    standardpopup.cpp \
    savecalibration.cpp \
    message.cpp \
    usbkey.cpp \
    alertmessagepopup.cpp \
    setuputility.cpp \
    timeedit.cpp \
    generalconfig.cpp \
    popupmessage.cpp \
    slicerstatistics.cpp \
    slicerstatisticsviewer.cpp \
    slicerversion.cpp \
    scripter.cpp \
    passwordentry.cpp

HEADERS += \
    mainwindow.h \
    scale.h \
    simulatedscaleinterface.h \
    scaleinterface.h \
    slicerstatemachine.h \
    kniferunin.h \
    onpausebuttonsim.h \
    weighconfig.h \
    weighcalib.h \
    mainmenu.h \
    msgbox.h \
    speedkeyconfig.h \
    msgboxconfig.h \
    supervisormenu.h \
    servicemenu.h \
    hobart/usb/weigherconfiguration.h \
    keysignalhandler.h \
    keypad.h \
    inputwidgethook.h \
    speedkey.h\
    calibratetypepopup.h \
    standardpopup.h \
    savecalibration.h \
    message.h \
    usbkey.h \
    alertmessagepopup.h \
    setuputility.h \
    timeedit.h \
    generalconfig.h \
    inputvalidator.h \
    popupmessage.h \
    slicerstatistics.h \
    slicerstatisticsviewer.h \
    slicerversion.h \
    configsettings.h \
    scripter.h \
    passwordentry.h

FORMS += mainwindow.ui \
    weighconfig.ui \
    weighcalib.ui \
    mainmenu.ui \
    speedkeyconfig.ui \
    msgboxconfig.ui \
    supervisormenu.ui \
    servicemenu.ui \
    calibratetypepopup.ui \
    savecalibration.ui \
    standardpopup.ui \
    keypad.ui \
    alertmessagepopup.ui \
    setuputility.ui \
    timeedit.ui \
    generalconfig.ui \
    popupmessage.ui \
    slicerstatisticsviewer.ui \
    passwordentry.ui

CONFIG += link_pkgconfig using_hobart
CONFIG += c++11

#
# Raspberry Pi
#
using_rpi {

DEFINES += USING_RPI
LIBS += -lwiringPi

SOURCES += \
    rpi/led.cpp \
    rpi/rotaryencoderknob.cpp \
    rpi/inputgpio.cpp \
    rpi/button.cpp \
    rpi/kniferunin.cpp \
    rpi/onpausebuttonsim.cpp

HEADERS += \
    rpi/led.h \
    rpi/rotaryencoderknob.h \
    rpi/raspberrypitypes.h \
    rpi/inputgpio.h \
    rpi/button.h \
    rpi/kniferunin.h \
    rpi/onpausebuttonsim.h

}

#
# A&D EK-12Ki Class II Weigher
#
using_rpi {
using_ek_12ki {

DEFINES += USING_EK_12KI

SOURCES += \
    and/and_ek12ki_serialinterface.cpp \
    and/hx711.cpp

HEADERS += \
    and/and_ek12ki_serialinterface.h \
    and/hx711.h

}
}

#
# Hobart ACR Board and USB Weigher
#
# For debugging on Ubuntu system you will need the following packages:
# $ sudo apt-get install pkg-config libusb-1.0-0-dev libgl1-mesa-dev
using_hobart {

DEFINES += USING_HOBART

SOURCES += \
    hobart/usb/usbdevman.cpp \
    hobart/usb/usbdev.cpp \
    hobart/usb/usbdevfactory.cpp \
    hobart/usb/hobartusbdev.cpp \
    hobart/usb/atmelsamba.cpp \
    hobart/usb/hobartweigher.cpp \
    hobart/usb/hobartprinter.cpp \
    hobart/usb/fpc.cpp \
    hobart/usb/prepack.cpp \
    hobart/led.cpp \
    hobart/knifepower.cpp \
    hobart/usbweigher.cpp

HEADERS += \
    hobart/usb/usbdevman.h \
    hobart/usb/usbdev.h \
    hobart/usb/usbdevfactory.h \
    hobart/usb/hobartusbdev.h \
    hobart/usb/atmelsamba.h \
    hobart/usb/hobartweigher.h \
    hobart/usb/hobartprinter.h \
    hobart/usb/fpc.h \
    hobart/usb/prepack.h \
    hobart/led.h \
    hobart/knifepower.h \
    hobart/usbweigher.h

PKGCONFIG += libusb-1.0
}


RESOURCES += \
    resources.qrc

TRANSLATIONS += \
    languages/lang_en_ca.ts \
    languages/lang_fr_ca.ts \
    languages/lang_es_mx.ts

DEFINES += "SVN_VERSION=\"\\\"$$system(./version.sh)\\\"\""

DISTFILES += \
    msgsettings.ini \
    res/menu12.png \
    res/Status.png

