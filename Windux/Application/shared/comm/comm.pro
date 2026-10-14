#-------------------------------------------------
#
# Project created by QtCreator 2012-10-17T14:19:33
#
#-------------------------------------------------

QT       += core network

TARGET = comm
TEMPLATE = lib
CONFIG += staticlib

SOURCES += devicemsg.cpp \
    devicemsgfactory.cpp \
    gcdmsg.cpp \
    gcdmsgfactory.cpp \
    commserver.cpp

HEADERS += msg.h \
    msgdispatcher.h \
    devicemsg.h \
    devicemsgfactory.h \
    devicemsggcd.h \
    devicemsggcdsession.h \
    devicemsgsession.h \
    devicemsgweigher.h \
    gcdmsg.h \
    gcdmsgfactory.h \
    commserver.h \
    devicemsgsoftkey.h \
    devicemsgerror.h \
    devicemsggraphic.h \
    devicemsgconfiguration.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../logManager/release/ -llogManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../logManager/debug/ -llogManager
else:symbian: LIBS += -llogManager
else:unix: LIBS += -L$$OUT_PWD/../logManager/ -llogManager

INCLUDEPATH += $$PWD/../logManager
DEPENDPATH += $$PWD/../logManager

# this here below does not appear to work for the mingw32 compiler
#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/release/logManager.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/debug/logManager.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../logManager/liblogManager.a
