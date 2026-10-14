#-------------------------------------------------
#
# Project created by QtCreator 2015-05-01T10:12:53
#
#-------------------------------------------------

QT       += core gui network webkit script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = webapp
TEMPLATE = app


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    inputwidget.cpp \
    keyboardinputwidget.cpp \
    inputwidgethook.cpp \
    touchscreenstyle.cpp \
    proxyautoconfig.cpp \
    webviewnetwork.cpp \
    webviewnetworkproxyfactory.cpp \
    webpage.cpp \
    webview.cpp \
    webviewcookiejar.cpp

HEADERS += \
    mainwindow.h \
    inputwidget.h \
    keyboardinputwidget.h \
    inputwidgethook.h \
    touchscreenstyle.h \
    colors.h \
    proxyautoconfig.h \
    webviewnetwork.h \
    webviewnetworkproxyfactory.h \
    webpage.h \
    webview.h \
    webviewcookiejar.h

#
# X11
#

unix:!symbian {
    CONFIG += link_pkgconfig
    PKGCONFIG += x11 x11-xcb
}
