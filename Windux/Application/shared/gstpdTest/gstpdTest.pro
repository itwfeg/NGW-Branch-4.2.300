#-------------------------------------------------
#
# Project created by QtCreator 2015-06-08T15:22:45
#
#-------------------------------------------------

QT       += core dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gstpdTest
TEMPLATE = app

SOURCES += main.cpp \
    test.cpp

HEADERS += \
    test.h

#
# gstpdCore
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../gstpdCore/release/ -lgstpdCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../gstpdCore/debug/ -lgstpdCore
else:symbian: LIBS += -lgstpdCore
else:unix: LIBS += -L$$OUT_PWD/../gstpdCore/ -lgstpdCore

INCLUDEPATH += $$PWD/../gstpdCore
DEPENDPATH += $$PWD/../gstpdCore

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../gstpdCore/release/gstpdCore.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../gstpdCore/debug/gstpdCore.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../gstpdCore/libgstpdCore.a

#
# logManager
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../logManager/release/ -llogManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../logManager/debug/ -llogManager
else:symbian: LIBS += -llogManager
else:unix: LIBS += -L$$OUT_PWD/../logManager/ -llogManager

INCLUDEPATH += $$PWD/../logManager
DEPENDPATH += $$PWD/../logManager

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/release/logManager.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/debug/logManager.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../logManager/liblogManager.a

#
# GST
#

unix:!symbian {
    CONFIG += link_pkgconfig
    PKGCONFIG += gtk+-2.0 gstreamer-1.0
    LIBS += -lgstapp-1.0 -lm
}

