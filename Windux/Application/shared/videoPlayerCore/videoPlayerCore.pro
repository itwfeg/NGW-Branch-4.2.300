#-------------------------------------------------
#
# Project created by QtCreator 2012-10-17T14:20:40
#
#-------------------------------------------------

QT       += core widgets

# Qt5 uses multimedia
# Qt4 uses phonon
greaterThan( QT_MAJOR_VERSION, 4 ): QT += multimedia multimediawidgets
else: QT += phonon

TARGET = videoPlayerCore
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    seekslider/seekslider.cpp \
    videoplayer/external/appconfig.cpp \
    videoplayer/external/externalvideoplayer.cpp \
    videoplayer/external/gstlaunchappconfig.cpp \
    videoplayer/external/gstpdappconfig.cpp \
    videoplayer/external/videoplayerappconfig.cpp \
    videoplayer/multimedia/multimediavideoplayer.cpp \
    videoplayer/phonon/phononvideoplayer.cpp \
    videoplayer/videoplayer.cpp \
    videoplayer/wmf/wmfvideoplayer.cpp \
    videoplayerwidget.cpp \
    volumeslider/volumeslider.cpp \
    videoplayer/external/gstpdexternalvideoplayer.cpp

HEADERS += \
    seekslider/seekslider.h \
    seekslider/seeksliderimpl.h \
    videoplayer/external/appconfig.h \
    videoplayer/external/externalvideoplayer.h \
    videoplayer/external/gstlaunchappconfig.h \
    videoplayer/external/gstpdappconfig.h \
    videoplayer/external/gstpdexternalvideoplayer.h \
    videoplayer/external/videoplayerappconfig.h \
    videoplayer/multimedia/multimediavideoplayer.h \
    videoplayer/multimedia/multimediavideoplayerconfig.h \
    videoplayer/phonon/phononvideoplayer.h \
    videoplayer/phonon/phononvideoplayerconfig.h \
    videoplayer/videoplayer.h \
    videoplayer/videoplayerconfig.h \
    videoplayer/wmf/wmfvideoplayer.h \
    videoplayerwidget.h \
    volumeslider/volumeslider.h \
    volumeslider/volumesliderimpl.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

#
# gstpdCore
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../gstpdCore/release/ -lgstpdCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../gstpdCore/debug/ -lgstpdCore
else:symbian: LIBS += -lgstpdCore
else:unix: LIBS += -L$$OUT_PWD/../gstpdCore/ -lgstpdCore

INCLUDEPATH += $$PWD/../gstpdCore
DEPENDPATH += $$PWD/../gstpdCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/gstpdCore/release/libgstpdCore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/gstpdCore/debug/libgstpdCore.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/gstpdCore/release/gstpdCore.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/gstpdCore/debug/gstpdCore.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/gstpdCore/libgstpdCore.a

#
# platform
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../platform/release/ -lplatform
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../platform/debug/ -lplatform
else:symbian: LIBS += -lplatform
else:unix: LIBS += -L$$OUT_PWD/../platform/ -lplatform

INCLUDEPATH += $$PWD/../platform
DEPENDPATH += $$PWD/../platform

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/platform/release/libplatform.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/platform/debug/libplatform.a
else:win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/platform/release/platform.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../shared/platform/debug/platform.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../shared/platform/libplatform.a

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
