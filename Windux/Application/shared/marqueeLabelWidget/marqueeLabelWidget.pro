#-------------------------------------------------
#
# Project created by QtCreator 2012-10-09T09:43:27
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = marqueeLabelWidget
TEMPLATE = lib
CONFIG += staticlib

SOURCES += marqueelabelwidget.cpp \
    marqueetext.cpp \
    marqueetextlist.cpp

HEADERS += marqueelabelwidget.h \
    marqueetext.h \
    marqueetextlist.h \
    marqueetextfragment.h \
    marqueetextconstants.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
