#-------------------------------------------------
#
# Project created by QtCreator 2015-05-01T10:12:53
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bluezviewer
TEMPLATE = app


SOURCES += \
    bluezviewer.cpp \
    mainwindow.cpp \
    sensorselection.cpp \
    dbquery.cpp \
    temperatureviewer.cpp \
    temperaturegraph.cpp \
    inputwidget.cpp \
    keyboardinputwidget.cpp \
    inputwidgethook.cpp \
    configuration.cpp \
    touchscreenstyle.cpp \
    profileselection.cpp \
    profileeditor.cpp \
    sensorpointselection.cpp \
    sensorpointeditor.cpp

HEADERS += \
    mainwindow.h \
    dbtables.h \
    sensorselection.h \
    dbquery.h \
    temperatureviewer.h \
    temperaturegraph.h \
    bluenicadatarec.h \
    inputwidget.h \
    keyboardinputwidget.h \
    inputwidgethook.h \
    configuration.h \
    touchscreenstyle.h \
    colors.h \
    profileselection.h \
    profileeditor.h \
    sensorpointselection.h \
    sensorpointeditor.h
