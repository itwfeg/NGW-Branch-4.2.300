#-------------------------------------------------
#
# Project created by Larry Pevoar, based on oo_setting_webapp 2022-02-25
#
#-------------------------------------------------

QT       += core gui network script sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = hteapphookconfig
TEMPLATE = app


SOURCES += \
    ../app_hook_v2/dbutil.cpp \
    ../app_hook_v2/htedb.cpp \
    main.cpp \
    mainwindow.cpp \
    inputwidget.cpp \
    keyboardinputwidget.cpp \
    inputwidgethook.cpp \
    touchscreenstyle.cpp

HEADERS += \
    ../app_hook_v2/common.h \
    ../app_hook_v2/dbutil.h \
    ../app_hook_v2/htedb.h \
    ../app_hook_v2/stringsconfig.h \
    ../app_hook_v2/stringsjson.h \
    mainwindow.h \
    inputwidget.h \
    keyboardinputwidget.h \
    inputwidgethook.h \
    touchscreenstyle.h \
    colors.h

# Dependancies
win32 {
    LIBS += Rpcrt4.lib
} else: unix {
    CONFIG+=link_pkgconfig
    PKGCONFIG+=clio x11 x11-xcb
}

INCLUDEPATH += $$PWD/..
DEFINES += DONTUSESCALECONFIG

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
