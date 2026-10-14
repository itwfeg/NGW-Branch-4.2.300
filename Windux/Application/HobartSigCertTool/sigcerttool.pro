#-------------------------------------------------
#
# Project created by QtCreator 2015-09-10T08:35:23
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = sigcerttool
CONFIG   += console
CONFIG   -= app_bundle

win32: LIBS += -ladvapi32 -lcrypt32

TEMPLATE = app

SOURCES += main.cpp
