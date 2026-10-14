QT += core network
QT -= gui

TARGET = squishexecutive
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    seconfiguration.cpp \
    squishexecutive.cpp \
    applicationattacher.cpp \
    runnerprocess.cpp \
    recorder.cpp \
    signalhandler.cpp

HEADERS += \
    seconfiguration.h \
    squishexecutive.h \
    applicationattacher.h \
    runnerprocess.h \
    recorder.h \
    signalhandler.h

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
