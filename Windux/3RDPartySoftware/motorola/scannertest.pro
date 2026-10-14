#-------------------------------------------------
#
# Project created by QtCreator 2015-07-15T14:59:49
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = scannertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    motorolassiscanner.cpp \
    serialiowin.cpp \
    serialiolinux.cpp

HEADERS += \
    motorolassiscanner.h \
    serialiowin.h \
    serialiolinux.h
