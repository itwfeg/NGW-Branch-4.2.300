#-------------------------------------------------
#
# Project created by QtCreator 2015-07-22T11:57:19
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fppServerApp
TEMPLATE = app


SOURCES += main.cpp\
    healthcheckcommswidget.cpp \
    healthcheckwidget.cpp \
        mainwindow.cpp \
    webserverwidget.cpp \
    clientcommswidget.cpp \
    clientcommdialog.cpp \
    smtpserverwidget.cpp \
    clientalertswidget.cpp \
    proxyserverwidget.cpp \
    clientproxycommswidget.cpp

HEADERS  += mainwindow.h \
    healthcheckcommswidget.h \
    healthcheckwidget.h \
    webserverwidget.h \
    clientcommswidget.h \
    clientcommdialog.h \
    webserverstate.h \
    smtpserverwidget.h \
    clientalertswidget.h \
    proxyserverwidget.h \
    clientproxycommswidget.h
