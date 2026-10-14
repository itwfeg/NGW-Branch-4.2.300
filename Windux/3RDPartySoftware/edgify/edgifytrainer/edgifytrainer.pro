#-------------------------------------------------
#
# Project created by QtCreator 2015-05-01T10:12:53
#
#-------------------------------------------------

QT += core gui network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = edgifytrainer
TEMPLATE = app

SOURCES += \
    configurewidget.cpp \
    devicediscovery.cpp \
    main.cpp \
    mainwindow.cpp \
    inputwidget.cpp \
    keyboardinputwidget.cpp \
    inputwidgethook.cpp \
    predictionwidget.cpp \
    terminalwidget.cpp \
    touchscreenstyle.cpp \
    webserviceslogviewerwidget.cpp

HEADERS += \
    configurewidget.h \
    devicediscovery.h \
    mainwindow.h \
    inputwidget.h \
    keyboardinputwidget.h \
    inputwidgethook.h \
    predictionwidget.h \
    terminalwidget.h \
    touchscreenstyle.h \
    colors.h \
    webserviceslogviewerwidget.h

#
# Hook
#

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../edgify_hook/release/ -lHobartCustomerHookEdgify
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../edgify_hook/debug/ -lHobartCustomerHookEdgify
else:unix: LIBS += -L$$OUT_PWD/../edgify_hook/ -lHobartCustomerHookEdgify

INCLUDEPATH += $$PWD/../edgify_hook
DEPENDPATH += $$PWD/../edgify_hook

#
# X11
#

unix:!symbian {
    CONFIG += link_pkgconfig
    PKGCONFIG += x11 x11-xcb
}

