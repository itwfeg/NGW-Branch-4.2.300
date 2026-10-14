#-------------------------------------------------
#
# Project created by QtCreator 2015-02-19T08:25:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia multimediawidgets

TARGET = cameraTest
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp

HEADERS  += mainwindow.h

#
# cameraScanner
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../cameraScanner/release/ -lcameraScanner
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../cameraScanner/debug/ -lcameraScanner
else:symbian: LIBS += -lcameraScanner
else:unix: LIBS += -L$$OUT_PWD/../cameraScanner/ -lcameraScanner

INCLUDEPATH += $$PWD/../cameraScanner
DEPENDPATH += $$PWD/../cameraScanner

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../cameraScanner/release/cameraScanner.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../cameraScanner/debug/cameraScanner.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../cameraScanner/libcameraScanner.a

# third party software location
THIRD_PARTY_SOFTWARE = $$PWD/../../../3RDPartySoftware

#
# imaging source afu050 camera driver
#

win32 {
    LIBS += -L$$THIRD_PARTY_SOFTWARE/imagingsource/tisgrabber/bin/win32 -ltisgrabber
    INCLUDEPATH += $$THIRD_PARTY_SOFTWARE/imagingsource/tisgrabber/include
    DEPENDPATH += $$THIRD_PARTY_SOFTWARE/imagingsource/tisgrabber
    PRE_TARGETDEPS += $$THIRD_PARTY_SOFTWARE/imagingsource/tisgrabber/bin/win32/tisgrabber.dll
} else:linux-timesys-g++ {
    LIBS += -L$$THIRD_PARTY_SOFTWARE/imagingsource/libafu050/timesys_armle -lafu050
    INCLUDEPATH += $$THIRD_PARTY_SOFTWARE/imagingsource/libafu050
    DEPENDPATH += $$THIRD_PARTY_SOFTWARE/imagingsource/libafu050
    PRE_TARGETDEPS += $$THIRD_PARTY_SOFTWARE/imagingsource/libafu050/timesys_armle/libafu050.so
    # additional requirements
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0 gtk+-2.0 gstreamer-1.0
    LIBS += -lgstapp-1.0 -lm
} else:unix:!symbian {
    HARDWARE_PLATFORM = $$system(uname -i)
    contains( HARDWARE_PLATFORM, x86_64 ) {
        # 64-bit Linux
        LIBS += -L$$THIRD_PARTY_SOFTWARE/imagingsource/libafu050/ubuntu_14.04_x86_64 -lafu050
        INCLUDEPATH += $$THIRD_PARTY_SOFTWARE/imagingsource/libafu050
        DEPENDPATH += $$THIRD_PARTY_SOFTWARE/imagingsource/libafu050
        PRE_TARGETDEPS += $$THIRD_PARTY_SOFTWARE/imagingsource/libafu050/ubuntu_14.04_x86_64/libafu050.so
    } else {
        # 32-bit Linux
        LIBS += -L$$THIRD_PARTY_SOFTWARE/imagingsource/libafu050/ubuntu_14.04_i686 -lafu050
        INCLUDEPATH += $$THIRD_PARTY_SOFTWARE/imagingsource/libafu050
        DEPENDPATH += $$THIRD_PARTY_SOFTWARE/imagingsource/libafu050
        PRE_TARGETDEPS += $$THIRD_PARTY_SOFTWARE/imagingsource/libafu050/ubuntu_14.04_i686/libafu050.so
    }
    # additional requirements
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0 gtk+-2.0 gstreamer-1.0
    LIBS += -lgstapp-1.0 -lm
}

#
# softek
#

win32 {
    LIBS += -L$$THIRD_PARTY_SOFTWARE/softek/win/bin -lSoftekBarcodeDLL
    INCLUDEPATH += $$THIRD_PARTY_SOFTWARE/softek/win/include
    DEPENDPATH += $$THIRD_PARTY_SOFTWARE/softek/win
    PRE_TARGETDEPS += $$THIRD_PARTY_SOFTWARE/softek/win/bin/SoftekBarcodeDLL.lib
} else:linux-timesys-g++ {
    LIBS += -L$$THIRD_PARTY_SOFTWARE/softek/linux/build/angstrom_arm/lib -lbardecode
    INCLUDEPATH += $$THIRD_PARTY_SOFTWARE/softek/linux/include
    DEPENDPATH += $$THIRD_PARTY_SOFTWARE/softek/linux
    PRE_TARGETDEPS += $$THIRD_PARTY_SOFTWARE/softek/linux/build/angstrom_arm/lib/bardecode.a
    # requirements of libbardecode.so
    LIBS += -ltiff -ljpeg -lz -lm
} else:unix:!symbian {
    HARDWARE_PLATFORM = $$system(uname -i)
    contains( HARDWARE_PLATFORM, x86_64 ) {
        # 64-bit Linux
        LIBS += -L$$THIRD_PARTY_SOFTWARE/softek/linux/build/ubuntu_12.04_x86_64/lib -lbardecode
        INCLUDEPATH += $$THIRD_PARTY_SOFTWARE/softek/linux/include
        DEPENDPATH += $$THIRD_PARTY_SOFTWARE/softek/linux
        PRE_TARGETDEPS += $$THIRD_PARTY_SOFTWARE/softek/linux/build/ubuntu_12.04_x86_64/lib/bardecode.a
    } else {
        # 32-bit Linux
        LIBS += -L$$THIRD_PARTY_SOFTWARE/softek/linux/build/ubuntu_12.04_i686/lib -lbardecode
        INCLUDEPATH += $$THIRD_PARTY_SOFTWARE/softek/linux/include
        DEPENDPATH += $$THIRD_PARTY_SOFTWARE/softek/linux
        PRE_TARGETDEPS += $$THIRD_PARTY_SOFTWARE/softek/linux/build/ubuntu_12.04_i686/lib/bardecode.a
    }
    # requirements of libbardecode.so
    LIBS += -ltiff -ljpeg -lz -lm
}
