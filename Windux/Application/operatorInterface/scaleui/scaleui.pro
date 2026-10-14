#-------------------------------------------------
#
# Project created by QtCreator 2012-11-13T16:22:01
#
#-------------------------------------------------

QT       += core gui widgets network sql webkit script
QT       += printsupport
QT       += script

unix:!symbian {
    QT += dbus
}

# Qt5 uses multimedia
# Qt4 uses phonon
greaterThan( QT_MAJOR_VERSION, 4 ): QT += webkitwidgets multimedia multimediawidgets
else: QT += phonon

TARGET = scaleui
TEMPLATE = app

SOURCES += main.cpp

HEADERS  +=

FORMS    +=

#
# sessionHandler
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/sessionHandler/release/ -lsessionHandler
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/sessionHandler/debug/ -lsessionHandler
else:symbian: LIBS += -lsessionHandler
else:unix: LIBS += -L$$OUT_PWD/../../shared/sessionHandler/ -lsessionHandler

INCLUDEPATH += $$PWD/../../shared/sessionHandler
DEPENDPATH += $$PWD/../../shared/sessionHandler

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandler/release/libsessionHandler.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandler/debug/libsessionHandler.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandler/release/sessionHandler.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandler/debug/sessionHandler.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandler/libsessionHandler.a

#
# sessionHandlerHooks
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/sessionHandlerHooks/release/ -lsessionHandlerHooks
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/sessionHandlerHooks/debug/ -lsessionHandlerHooks
else:symbian: LIBS += -lsessionHandlerHooks
else:unix: LIBS += -L$$OUT_PWD/../../shared/sessionHandlerHooks/ -lsessionHandlerHooks

INCLUDEPATH += $$PWD/../../shared/sessionHandlerHooks
DEPENDPATH += $$PWD/../../shared/sessionHandlerHooks

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerHooks/release/libsessionHandlerHooks.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerHooks/debug/libsessionHandlerHooks.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerHooks/release/sessionHandlerHooks.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerHooks/debug/sessionHandlerHooks.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerHooks/libsessionHandlerHooks.a

#
# sessionHandlerWidgets
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/sessionHandlerWidgets/release/ -lsessionHandlerWidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/sessionHandlerWidgets/debug/ -lsessionHandlerWidgets
else:symbian: LIBS += -lsessionHandlerWidgets
else:unix: LIBS += -L$$OUT_PWD/../../shared/sessionHandlerWidgets/ -lsessionHandlerWidgets

INCLUDEPATH += $$PWD/../../shared/sessionHandlerWidgets
DEPENDPATH += $$PWD/../../shared/sessionHandlerWidgets

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerWidgets/release/libsessionHandlerWidgets.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerWidgets/debug/libsessionHandlerWidgets.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerWidgets/release/sessionHandlerWidgets.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerWidgets/debug/sessionHandlerWidgets.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerWidgets/libsessionHandlerWidgets.a

#
# sessionHandlerUtil
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/sessionHandlerUtil/release/ -lsessionHandlerUtil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/sessionHandlerUtil/debug/ -lsessionHandlerUtil
else:symbian: LIBS += -lsessionHandlerUtil
else:unix: LIBS += -L$$OUT_PWD/../../shared/sessionHandlerUtil/ -lsessionHandlerUtil

INCLUDEPATH += $$PWD/../../shared/sessionHandlerUtil
DEPENDPATH += $$PWD/../../shared/sessionHandlerUtil

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerUtil/release/libsessionHandlerUtil.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerUtil/debug/libsessionHandlerUtil.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerUtil/release/sessionHandlerUtil.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerUtil/debug/sessionHandlerUtil.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/sessionHandlerUtil/libsessionHandlerUtil.a

#
# Session
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/Session/release/ -lSession
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/Session/debug/ -lSession
else:symbian: LIBS += -lSession
else:unix: LIBS += -L$$OUT_PWD/../../shared/Session/ -lSession

INCLUDEPATH += $$PWD/../../shared/Session
DEPENDPATH += $$PWD/../../shared/Session

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/Session/release/libSession.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/Session/debug/libSession.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/Session/release/Session.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/Session/debug/Session.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/Session/libSession.a

#
# playlistWidget
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/playlistWidget/release/ -lplaylistWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/playlistWidget/debug/ -lplaylistWidget
else:unix:!macx: LIBS += -L$$OUT_PWD/../../shared/playlistWidget/ -lplaylistWidget

INCLUDEPATH += $$PWD/../../shared/playlistWidget
DEPENDPATH += $$PWD/../../shared/playlistWidget

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/playlistWidget/release/libplaylistWidget.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/playlistWidget/debug/libplaylistWidget.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/playlistWidget/release/playlistWidget.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/playlistWidget/debug/playlistWidget.lib
else:unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../../shared/playlistWidget/libplaylistWidget.a

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
# marqueeTextEditorWidget
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/marqueeTextEditorWidget/release/ -lmarqueeTextEditorWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/marqueeTextEditorWidget/debug/ -lmarqueeTextEditorWidget
else:symbian: LIBS += -lmarqueeTextEditorWidget
else:unix: LIBS += -L$$OUT_PWD/../../shared/marqueeTextEditorWidget/ -lmarqueeTextEditorWidget

INCLUDEPATH += $$PWD/../../shared/marqueeTextEditorWidget
DEPENDPATH += $$PWD/../../shared/marqueeTextEditorWidget

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/marqueeTextEditorWidget/release/libmarqueeTextEditorWidget.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/marqueeTextEditorWidget/debug/libmarqueeTextEditorWidget.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/marqueeTextEditorWidget/release/marqueeTextEditorWidget.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/marqueeTextEditorWidget/debug/marqueeTextEditorWidget.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/marqueeTextEditorWidget/libmarqueeTextEditorWidget.a

#
# marqueeLabelWidget
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/marqueeLabelWidget/release/ -lmarqueeLabelWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/marqueeLabelWidget/debug/ -lmarqueeLabelWidget
else:symbian: LIBS += -lmarqueeLabelWidget
else:unix: LIBS += -L$$OUT_PWD/../../shared/marqueeLabelWidget/ -lmarqueeLabelWidget

INCLUDEPATH += $$PWD/../../shared/marqueeLabelWidget
DEPENDPATH += $$PWD/../../shared/marqueeLabelWidget

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/marqueeLabelWidget/release/libmarqueeLabelWidget.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/marqueeLabelWidget/debug/libmarqueeLabelWidget.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/marqueeLabelWidget/release/marqueeLabelWidget.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/marqueeLabelWidget/debug/marqueeLabelWidget.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/marqueeLabelWidget/libmarqueeLabelWidget.a

#
# labelViewerWidget
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/labelViewerWidget/release/ -llabelViewerWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/labelViewerWidget/debug/ -llabelViewerWidget
else:symbian: LIBS += -llabelViewerWidget
else:unix: LIBS += -L$$OUT_PWD/../../shared/labelViewerWidget/ -llabelViewerWidget

INCLUDEPATH += $$PWD/../../shared/labelViewerWidget
DEPENDPATH += $$PWD/../../shared/labelViewerWidget

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/labelViewerWidget/release/liblabelViewerWidget.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/labelViewerWidget/debug/liblabelViewerWidget.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/labelViewerWidget/release/labelViewerWidget.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/labelViewerWidget/debug/labelViewerWidget.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/labelViewerWidget/liblabelViewerWidget.a

#
# labelCore
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../shared/labelCore/release/ -llabelCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../shared/labelCore/debug/ -llabelCore
else:symbian: LIBS += -llabelCore
else:unix: LIBS += -L$$OUT_PWD/../../shared/labelCore/ -llabelCore

INCLUDEPATH += $$PWD/../../shared/labelCore
DEPENDPATH += $$PWD/../../shared/labelCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/labelCore/release/liblabelCore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/labelCore/debug/liblabelCore.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/labelCore/release/labelCore.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/labelCore/debug/labelCore.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/labelCore/liblabelCore.a

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
# XpdfWidgetQt
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../XpdfWidgetQt/src/release/ -lXpdfWidgetQt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../XpdfWidgetQt/src/debug/ -lXpdfWidgetQt
else:symbian: LIBS += -lXpdfWidgetQt
else:unix: LIBS += -L$$OUT_PWD/../../XpdfWidgetQt/src/ -lXpdfWidgetQt

INCLUDEPATH += $$PWD/../../XpdfWidgetQt/src
DEPENDPATH += $$PWD/../../XpdfWidgetQt/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../XpdfWidgetQt/src/release/libXpdfWidgetQt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../XpdfWidgetQt/src/debug/libXpdfWidgetQt.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../XpdfWidgetQt/src/release/XpdfWidgetQt.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../XpdfWidgetQt/src/debug/XpdfWidgetQt.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../XpdfWidgetQt/src/libXpdfWidgetQt.a

#
# freetype2
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../XpdfWidgetQt/freetype-2.4.11/release/ -lFreeTypeLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../XpdfWidgetQt/freetype-2.4.11/debug/ -lFreeTypeLib
else:unix:!symbian {
    CONFIG += link_pkgconfig
    PKGCONFIG += freetype2
    #LIBS += -lfreetype -lm
}

#
# GST, X11
#

unix:!symbian {
    CONFIG += link_pkgconfig
    PKGCONFIG += gtk+-2.0 gstreamer-1.0 x11 x11-xcb
    #LIBS += -lgstapp-1.0 -lm
}

#
# windows api
#

win32:CONFIG(release, debug|release): LIBS += -ladvapi32 -lcrypt32 -lole32 -loleaut32 -luuid -lshlwapi -lws2_32 -luser32 -ldxva2 -levr -lmf -lmfplat -lmfplay -lmfreadwrite -lmfuuid -lstrmiids
else:win32:CONFIG(debug, debug|release): LIBS += -ladvapi32 -lcrypt32 -lole32 -loleaut32 -luuid -lshlwapi -lws2_32 -luser32 -ldxva2 -levr -lmf -lmfplat -lmfplay -lmfreadwrite -lmfuuid -lstrmiids
