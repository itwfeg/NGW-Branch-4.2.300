#-------------------------------------------------
#
# Project created by QtCreator 2014-01-17T09:09:12
#
#-------------------------------------------------

QT       += widgets network

# Qt5 uses multimedia
# Qt4 uses phonon
greaterThan( QT_MAJOR_VERSION, 4 ): QT += multimedia multimediawidgets
else: QT += phonon

TARGET = sessionHandlerHooks
TEMPLATE = lib
CONFIG += staticlib

win32-g++: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-g++: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-g++-64: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-timesys-g++: QMAKE_CXXFLAGS += -Wno-multichar

SOURCES += \
    selfservice/selfservicecamerapredictions.cpp \
    selfserviceGen2/selfserveAuditLogG2.cpp \
    selfserviceGen2/selfserveByCountG2.cpp \
    selfserviceGen2/selfserveByCountQtyG2.cpp \
    selfserviceGen2/selfserveDialogG2.cpp \
    selfserviceGen2/selfserveImageLookupG2.cpp \
    selfserviceGen2/selfserveImageLookupViewAllG2.cpp \
    selfserviceGen2/selfserveInstructG2.cpp \
    selfserviceGen2/selfserveItemLookupG2.cpp \
    selfserviceGen2/selfserveRandomWeightG2.cpp \
    selfserviceGen2/selfserveStartG2.cpp \
    selfserviceGen2/selfserveWeightsMeasuresG2.cpp \
    selfserviceGen2/selfservicecamerapredictionsG2.cpp \
    selfserviceGen2/selfservicedialogG2.cpp \
    selfserviceGen2/selfservicesoftkeybuttonG2.cpp \
    selfserviceGen2/selfservicesoftkeyswidgetG2.cpp \
    sessionhooks.cpp \
    sessionhook.cpp \
    devtesthook.cpp \
    selfservicehook.cpp \
    selfservicehookerror.cpp \
    selfservice/selfserveByCount.cpp \
    selfservice/selfserveFinish.cpp \
    selfservice/selfserveImageLookup.cpp \
    selfservice/selfserveItemLookup.cpp \
    selfservice/selfserveRandomWeight.cpp \
    selfservice/selfserveStart.cpp \
    selfservice/selfserveWeightsMeasures.cpp \
    selfservice/selfservicedialog.cpp \
    selfservice/selfservicebuttonbar.cpp \
    selfservice/selfserviceicons.cpp \
    selfservice/selfservicenumberpad.cpp \
    selfservice/selfservicesoftkeybutton.cpp \
    selfservice/selfservicetoggledsoftkeybutton.cpp \
    selfservice/selfservicesoftkeyswidget.cpp \
    selfservice/selfserviceweightbar.cpp \
    selfservice/selfserviceitemnutrifacts.cpp \
    selfservice/selfserviceitemimage.cpp \
    selfservice/selfservicewmnumberpad.cpp \
    selfservice/selfserveInstruct.cpp \
    selfservice/selfservePlaylist.cpp \
    selfservice/selfserveAuditLog.cpp \
    selfservice/selfserveLogin.cpp

HEADERS += \
    selfservice/selfservicecamerapredictions.h \
    selfserviceGen2/selfserveAuditLogG2.h \
    selfserviceGen2/selfserveByCountG2.h \
    selfserviceGen2/selfserveByCountQtyG2.h \
    selfserviceGen2/selfserveDialogG2.h \
    selfserviceGen2/selfserveImageLookupG2.h \
    selfserviceGen2/selfserveImageLookupViewAllG2.h \
    selfserviceGen2/selfserveInstructG2.h \
    selfserviceGen2/selfserveItemLookupG2.h \
    selfserviceGen2/selfserveRandomWeightG2.h \
    selfserviceGen2/selfserveStartG2.h \
    selfserviceGen2/selfserveWeightsMeasuresG2.h \
    selfserviceGen2/selfservicecamerapredictionsG2.h \
    selfserviceGen2/selfservicedialogG2.h \
    selfserviceGen2/selfservicesoftkeybuttonG2.h \
    selfserviceGen2/selfservicesoftkeyswidgetG2.h \
    sessionhooks.h \
    sessionhook.h \
    devtesthook.h \
    selfservicehook.h \
    selfservicehookerror.h \
    selfservice/selfserveByCount.h \
    selfservice/selfserveFinish.h \
    selfservice/selfserveImageLookup.h \
    selfservice/selfserveItemLookup.h \
    selfservice/selfserveRandomWeight.h \
    selfservice/selfserveStart.h \
    selfservice/selfserveWeightsMeasures.h \
    selfservice/selfservicedialog.h \
    selfservice/selfservicebuttonbar.h \
    selfservice/selfserviceicons.h \
    selfservice/selfservicenumberpad.h \
    selfservice/selfservicesoftkeybutton.h \
    selfservice/selfservicetoggledsoftkeybutton.h \
    selfservice/selfservicesoftkeyswidget.h \
    selfservice/selfserviceweightbar.h \
    selfservice/selfserviceitemnutrifacts.h \
    selfservice/selfserviceitemimage.h \
    selfservice/selfservicewmnumberpad.h \
    selfservice/selfserveInstruct.h \
    selfservice/selfservePlaylist.h \
    selfservice/selfserveAuditLog.h \
    selfservice/selfserveLogin.h

FORMS += \
    selfservice/selfserveByCount.ui \
    selfservice/selfserveFinish.ui \
    selfservice/selfserveImageLookup.ui \
    selfservice/selfserveItemLookup.ui \
    selfservice/selfserveRandomWeight.ui \
    selfservice/selfserveStart.ui \
    selfservice/selfserveWeightsMeasures.ui \
    selfservice/selfservePlaylist.ui \
    selfservice/selfserveInstruct.ui \
    selfservice/selfserveLogin.ui \
    selfserviceGen2/selfserveAuditLogG2.ui \
    selfserviceGen2/selfserveByCountG2.ui \
    selfserviceGen2/selfserveByCountQtyG2.ui \
    selfserviceGen2/selfserveDialogG2.ui \
    selfserviceGen2/selfserveImageLookupG2.ui \
    selfserviceGen2/selfserveImageLookupViewAllG2.ui \
    selfserviceGen2/selfserveInstructG2.ui \
    selfserviceGen2/selfserveItemLookupG2.ui \
    selfserviceGen2/selfserveRandomWeightG2.ui \
    selfserviceGen2/selfserveStartG2.ui \
    selfserviceGen2/selfserveWeightsMeasuresG2.ui \
    selfserviceGen2/selfservicecamerapredictionsG2.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerWidgets/release/ -lsessionHandlerWidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerWidgets/debug/ -lsessionHandlerWidgets
else:symbian: LIBS += -lsessionHandlerWidgets
else:unix: LIBS += -L$$OUT_PWD/../sessionHandlerWidgets/ -lsessionHandlerWidgets

INCLUDEPATH += $$PWD/../sessionHandlerWidgets
DEPENDPATH += $$PWD/../sessionHandlerWidgets

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerWidgets/release/sessionHandlerWidgets.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerWidgets/debug/sessionHandlerWidgets.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerWidgets/libsessionHandlerWidgets.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerUtil/release/ -lsessionHandlerUtil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerUtil/debug/ -lsessionHandlerUtil
else:symbian: LIBS += -lsessionHandlerUtil
else:unix: LIBS += -L$$OUT_PWD/../sessionHandlerUtil/ -lsessionHandlerUtil

INCLUDEPATH += $$PWD/../sessionHandlerUtil
DEPENDPATH += $$PWD/../sessionHandlerUtil

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerUtil/release/sessionHandlerUtil.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerUtil/debug/sessionHandlerUtil.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerUtil/libsessionHandlerUtil.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Session/release/ -lSession
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Session/debug/ -lSession
else:symbian: LIBS += -lSession
else:unix: LIBS += -L$$OUT_PWD/../Session/ -lSession

INCLUDEPATH += $$PWD/../Session
DEPENDPATH += $$PWD/../Session

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Session/release/Session.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Session/debug/Session.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../Session/libSession.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../videoPlayerCore/release/ -lvideoPlayerCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../videoPlayerCore/debug/ -lvideoPlayerCore
else:symbian: LIBS += -lvideoPlayerCore
else:unix: LIBS += -L$$OUT_PWD/../videoPlayerCore/ -lvideoPlayerCore

INCLUDEPATH += $$PWD/../videoPlayerCore
DEPENDPATH += $$PWD/../videoPlayerCore

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../videoPlayerCore/release/videoPlayerCore.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../videoPlayerCore/debug/videoPlayerCore.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../videoPlayerCore/libvideoPlayerCore.a



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../playlistWidget/release/ -lplaylistWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../playlistWidget/debug/ -lplaylistWidget
else:unix:!macx: LIBS += -L$$OUT_PWD/../playlistWidget/ -lplaylistWidget

INCLUDEPATH += $$PWD/../playlistWidget
DEPENDPATH += $$PWD/../playlistWidget

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../playlistWidget/release/libplaylistWidget.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../playlistWidget/debug/libplaylistWidget.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../playlistWidget/release/playlistWidget.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../playlistWidget/debug/playlistWidget.lib
#else:unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../playlistWidget/libplaylistWidget.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../labelCore/release/ -llabelCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../labelCore/debug/ -llabelCore
else:symbian: LIBS += -llabelCore
else:unix: LIBS += -L$$OUT_PWD/../labelCore/ -llabelCore

INCLUDEPATH += $$PWD/../labelCore
DEPENDPATH += $$PWD/../labelCore

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../labelCore/release/labelCore.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../labelCore/debug/labelCore.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../labelCore/liblabelCore.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../platform/release/ -lplatform
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../platform/debug/ -lplatform
else:symbian: LIBS += -lplatform
else:unix: LIBS += -L$$OUT_PWD/../platform/ -lplatform

INCLUDEPATH += $$PWD/../platform
DEPENDPATH += $$PWD/../platform

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../platform/release/platform.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../platform/debug/platform.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../platform/libplatform.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../logManager/release/ -llogManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../logManager/debug/ -llogManager
else:symbian: LIBS += -llogManager
else:unix: LIBS += -L$$OUT_PWD/../logManager/ -llogManager

INCLUDEPATH += $$PWD/../logManager
DEPENDPATH += $$PWD/../logManager

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/release/logManager.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/debug/logManager.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../logManager/liblogManager.a

