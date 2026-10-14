#-------------------------------------------------
#
# Project created by QtCreator 2015-05-01T10:12:53
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = wmsmart
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    inputwidget.cpp \
    keyboardinputwidget.cpp \
    inputwidgethook.cpp \
    settingsviewerwidget.cpp \
    touchscreenstyle.cpp \
    webserviceslogviewerwidget.cpp \
    wmconfigviewerwidget.cpp

HEADERS += \
    mainwindow.h \
    inputwidget.h \
    keyboardinputwidget.h \
    inputwidgethook.h \
    settingsviewerwidget.h \
    touchscreenstyle.h \
    colors.h \
    webserviceslogviewerwidget.h \
    wmconfigviewerwidget.h

#
# X11
#

unix:!symbian {
    CONFIG += link_pkgconfig
    PKGCONFIG += x11 x11-xcb
}
