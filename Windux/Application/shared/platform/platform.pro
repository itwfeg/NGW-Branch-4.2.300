#-------------------------------------------------
#
# Project created by QtCreator 2012-10-17T14:11:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = platform
TEMPLATE = lib
CONFIG += staticlib

SOURCES += process.cpp \
    filesystem.cpp \
    python.cpp \
    network.cpp \
    foldertype.cpp \
    folderconfig.cpp \
    fileaccess.cpp \
    remotefileaccess.cpp \
    sighandler.cpp \
    appinit.cpp

HEADERS += platform.h \
    process.h \
    filesystem.h \
    python.h \
    filesystemconfig.h \
    network.h \
    foldertype.h \
    folderconfig.h \
    fileaccess.h \
    remotefileaccess.h \
    remoteconnection.h \
    remoteconnectionimpl.h \
    sighandler.h \
    appinit.h

disable_chilkat {
    DEFINES += DISABLE_CHILKAT
}

disable_python {
    DEFINES += DISABLE_PYTHON
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

#
# chilkat
#
!disable_chilkat {

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

} # disable_chilkat

#
# logManager
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../logManager/release/ -llogManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../logManager/debug/ -llogManager
else:symbian: LIBS += -llogManager
else:unix: LIBS += -L$$OUT_PWD/../logManager/ -llogManager

INCLUDEPATH += $$PWD/../logManager
DEPENDPATH += $$PWD/../logManager

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/logManager/release/liblogManager.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/logManager/debug/liblogManager.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/logManager/release/logManager.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/logManager/debug/logManager.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/logManager/liblogManager.a
