#-------------------------------------------------
#
# Project created by QtCreator 2012-10-09T09:43:27
#
#-------------------------------------------------

QT       += core gui network webkit

# Qt5 uses multimedia
# Qt4 uses phonon
greaterThan( QT_MAJOR_VERSION, 4 ): QT += widgets multimedia multimediawidgets webkitwidgets
else: QT += phonon

TARGET = playlistWidget
TEMPLATE = lib
CONFIG += staticlib

SOURCES += mediaplaylistfileparser.cpp \
    mediaplaylist.cpp \
    playlistwidget.cpp

HEADERS += mediaplaylistfileparser.h \
    mediaplaylist.h \
    playlistwidget.h

disable_video {
    DEFINES += DISABLE_VIDEO
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
# videoPlayerCore
#
!disable_video {

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

} # disable_video

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

