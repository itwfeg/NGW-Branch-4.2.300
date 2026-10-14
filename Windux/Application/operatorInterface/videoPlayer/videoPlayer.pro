#-------------------------------------------------
#
# Project created by QtCreator 2012-10-17T14:55:55
#
#-------------------------------------------------

QT       += core widgets

unix:!symbian {
    QT += dbus
}

# Qt5 uses multimedia
# Qt4 uses phonon
greaterThan( QT_MAJOR_VERSION, 4 ): QT += multimedia multimediawidgets
else: QT += phonon

TARGET = videoPlayer
TEMPLATE = app


SOURCES += main.cpp \
    videoplayerdialog.cpp

HEADERS  += videoplayerdialog.h

FORMS    += videoplayerdialog.ui

RESOURCES += \
    images.qrc

#
# videoPlayerCore
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/videoPlayerCore/release/ -lvideoPlayerCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/videoPlayerCore/debug/ -lvideoPlayerCore
else:symbian: LIBS += -lvideoPlayerCore
else:unix: LIBS += -L$$OUT_PWD/../../shared/videoPlayerCore/ -lvideoPlayerCore

INCLUDEPATH += $$PWD/../../shared/videoPlayerCore
DEPENDPATH += $$PWD/../../shared/videoPlayerCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/videoPlayerCore/release/libvideoPlayerCore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/videoPlayerCore/debug/libvideoPlayerCore.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/videoPlayerCore/release/videoPlayerCore.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/videoPlayerCore/debug/videoPlayerCore.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/videoPlayerCore/libvideoPlayerCore.a

#
# gstpdCore
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/gstpdCore/release/ -lgstpdCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/gstpdCore/debug/ -lgstpdCore
else:symbian: LIBS += -lgstpdCore
else:unix: LIBS += -L$$OUT_PWD/../../shared/gstpdCore/ -lgstpdCore

INCLUDEPATH += $$PWD/../../shared/gstpdCore
DEPENDPATH += $$PWD/../../shared/gstpdCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/gstpdCore/release/libgstpdCore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/gstpdCore/debug/libgstpdCore.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/gstpdCore/release/gstpdCore.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/gstpdCore/debug/gstpdCore.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/gstpdCore/libgstpdCore.a

#
# platform
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/platform/release/ -lplatform
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/platform/debug/ -lplatform
else:symbian: LIBS += -lplatform
else:unix: LIBS += -L$$OUT_PWD/../../shared/platform/ -lplatform

INCLUDEPATH += $$PWD/../../shared/platform
DEPENDPATH += $$PWD/../../shared/platform

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/platform/release/libplatform.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/platform/debug/libplatform.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/platform/release/platform.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/platform/debug/platform.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/platform/libplatform.a

#
# chilkat
#

win32:CONFIG(release, debug|release) {
    MSVC_VER = $$(VisualStudioVersion)
    equals( MSVC_VER, 14.0 ) {
        message("msvc 2015 release")
        contains( QMAKE_TARGET.arch, x86_64 ) {
            message("x86_64 build")
            LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-vc2015/libs -lChilkatRelDll_x64
            INCLUDEPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-vc2015/include
            DEPENDPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-vc2015
            PRE_TARGETDEPS += $$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-vc2015/libs/ChilkatRelDll_x64.lib
        } else {
            message("x86 build")
            LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc2015/libs -lChilkatRelDll
            INCLUDEPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc2015/include
            DEPENDPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc2015
            PRE_TARGETDEPS += $$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc2015/libs/ChilkatRelDll.lib
        }
    } else {
        message("msvc 2010 release")
        LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc10-sp1/libs -lChilkatRelDll
        INCLUDEPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc10-sp1/include
        DEPENDPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc10-sp1
        PRE_TARGETDEPS += $$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc10-sp1/libs/ChilkatRelDll.lib
    }
} else:win32:CONFIG(debug, debug|release) {
    MSVC_VER = $$(VisualStudioVersion)
    equals( MSVC_VER, 14.0 ) {
        message("msvc 2015 debug")
        contains( QMAKE_TARGET.arch, x86_64 ) {
            message("x86_64 build")
            LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-vc2015/libs -lChilkatDbgDll_x64
            INCLUDEPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-vc2015/include
            DEPENDPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-vc2015
            PRE_TARGETDEPS += $$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-vc2015/libs/ChilkatDbgDll_x64.lib
        } else {
            message("x86 build")
            LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc2015/libs -lChilkatDbgDll
            INCLUDEPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc2015/include
            DEPENDPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc2015
            PRE_TARGETDEPS += $$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc2015/libs/ChilkatDbgDll.lib
        }
    } else {
        message("msvc 2010 debug")
        LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc10-sp1/libs -lChilkatDbgDll
        INCLUDEPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc10-sp1/include
        DEPENDPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc10-sp1
        PRE_TARGETDEPS += $$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-vc10-sp1/libs/ChilkatDbgDll.lib
    }
} else:linux-timesys-g++ {
    LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-armv7l-linux/lib -lchilkat-9.5.0
    INCLUDEPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-armv7l-linux/include
    DEPENDPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-armv7l-linux
    PRE_TARGETDEPS += $$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-armv7l-linux/lib/libchilkat-9.5.0.a
} else:unix:!symbian {
    HARDWARE_PLATFORM = $$system(uname -i)
    contains( HARDWARE_PLATFORM, x86_64 ) {
        # 64-bit Linux
        LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-linux-gcc/lib -lchilkat-9.5.0
        INCLUDEPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-linux-gcc/include
        DEPENDPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-linux-gcc
        PRE_TARGETDEPS += $$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86_64-linux-gcc/lib/libchilkat-9.5.0.a
    } else {
        # 32-bit Linux
        LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-linux-gcc/lib -lchilkat-9.5.0
        INCLUDEPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-linux-gcc/include
        DEPENDPATH += $$PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-linux-gcc
        PRE_TARGETDEPS += $$OUT_PWD/../../../3RDPartySoftware/chilkat/chilkat-9.5.0-x86-linux-gcc/lib/libchilkat-9.5.0.a
    }
}

#
# logManager
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/logManager/release/ -llogManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/logManager/debug/ -llogManager
else:symbian: LIBS += -llogManager
else:unix: LIBS += -L$$OUT_PWD/../../shared/logManager/ -llogManager

INCLUDEPATH += $$PWD/../../shared/logManager
DEPENDPATH += $$PWD/../../shared/logManager

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/logManager/release/liblogManager.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/logManager/debug/liblogManager.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/logManager/release/logManager.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/logManager/debug/logManager.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/logManager/liblogManager.a

#
# GST
#

unix:!symbian {
    CONFIG += link_pkgconfig
    PKGCONFIG += gtk+-2.0 gstreamer-1.0
    LIBS += -lgstapp-1.0 -lm
}

#
# windows api
#

win32:CONFIG(release, debug|release): LIBS += -ladvapi32 -lcrypt32 -lole32 -loleaut32 -luuid -lshlwapi -lws2_32 -ldxva2 -levr -lmf -lmfplat -lmfplay -lmfreadwrite -lmfuuid -lstrmiids
else:win32:CONFIG(debug, debug|release): LIBS += -ladvapi32 -lcrypt32 -lole32 -loleaut32 -luuid -lshlwapi -lws2_32 -ldxva2 -levr -lmf -lmfplat -lmfplay -lmfreadwrite -lmfuuid -lstrmiids
