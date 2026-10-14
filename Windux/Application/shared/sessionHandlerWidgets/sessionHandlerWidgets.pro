#-------------------------------------------------
#
# Project created by QtCreator 2014-01-17T09:09:12
#
#-------------------------------------------------

QT       += widgets network

TARGET = sessionHandlerWidgets
TEMPLATE = lib
CONFIG += staticlib

win32-g++: QMAKE_CXXFLAGS += -Wno-multichar -std=c++11
else:linux-g++: QMAKE_CXXFLAGS += -Wno-multichar -std=c++11
else:linux-g++-64: QMAKE_CXXFLAGS += -Wno-multichar -std=c++11
else:linux-timesys-g++: QMAKE_CXXFLAGS += -Wno-multichar -std=c++11

SOURCES += \
    filedownloader.cpp \
    icons.cpp \
    numericlineedit.cpp \
    pushbutton.cpp \
    refusebin.cpp \
    softkeybutton.cpp \
    softkeybuttonlist.cpp \
    softkeyoverlaymanager.cpp \
    softkeyswidget.cpp \
    softkeytogglemanager.cpp \
    tabbableiplineedit.cpp \
    toolbutton.cpp \
    capacitybarwidgets.cpp \
    backgroundwidgetrenderer.cpp

HEADERS += \
    abstractbutton.h \
    backgroundwidget.h \
    backgroundwidgettypes.h \
    filedownloader.h \
    icons.h \
    numericlineedit.h \
    pushbutton.h \
    refusebin.h \
    softkeybutton.h \
    softkeybuttonlist.h \
    softkeyoverlaymanager.h \
    softkeyswidget.h \
    softkeytogglemanager.h \
    tabbableiplineedit.h \
    toolbutton.h \
    capacitybarwidgets.h \
    backgroundwidgetrenderer.h \
    backgroundwidgetstyles.h


RESOURCES += \
    sessionhandlerwidgets.qrc
    
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Session/release/ -lSession
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Session/debug/ -lSession
else:symbian: LIBS += -lSession
else:unix: LIBS += -L$$OUT_PWD/../Session/ -lSession

INCLUDEPATH += $$PWD/../Session
DEPENDPATH += $$PWD/../Session

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Session/release/Session.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Session/debug/Session.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../Session/libSession.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerUtil/release/ -lsessionHandlerUtil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerUtil/debug/ -lsessionHandlerUtil
else:symbian: LIBS += -lsessionHandlerUtil
else:unix: LIBS += -L$$OUT_PWD/../sessionHandlerUtil/ -lsessionHandlerUtil

INCLUDEPATH += $$PWD/../sessionHandlerUtil
DEPENDPATH += $$PWD/../sessionHandlerUtil

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerUtil/release/sessionHandlerUtil.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerUtil/debug/sessionHandlerUtil.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerUtil/libsessionHandlerUtil.a

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
