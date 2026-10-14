#-------------------------------------------------
#
# Project created by QtCreator 2012-10-09T10:43:13
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = marqueeTextEditorWidget
TEMPLATE = lib
CONFIG += staticlib

SOURCES += marqueetexteditorwidget.cpp \
    marqueesyntaxhighlighter.cpp

HEADERS += marqueetexteditorwidget.h \
    marqueesyntaxhighlighter.h

INCLUDEPATH += $$PWD/../marqueeLabelWidget

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

RESOURCES += \
    marqueetexteditorwidget.qrc
