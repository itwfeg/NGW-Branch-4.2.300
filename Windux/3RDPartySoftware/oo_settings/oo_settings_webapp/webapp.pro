#-------------------------------------------------
#
# Project created by QtCreator 2015-05-01T10:12:53
#
#-------------------------------------------------

QT       += core gui network webkit script sql

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
    CONFIG += link_pkgconfig console
    PKGCONFIG += x11 x11-xcb
}

win32-g++: QMAKE_CXXFLAGS += -Wno-multichar -std=c++11 
else:linux-g++: QMAKE_CXXFLAGS += -Wno-multichar -std=c++11 
else:linux-g++-64: QMAKE_CXXFLAGS += -Wno-multichar -std=c++11 
else:linux-timesys-g++: QMAKE_CXXFLAGS += -Wno-multichar -std=c++11 
