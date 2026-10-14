#-------------------------------------------------
#
# Project created by QtCreator 2012-09-26T15:35:34
#
#-------------------------------------------------

QT       += core gui widgets sql

TARGET = settingsWizardCore
TEMPLATE = lib
CONFIG += staticlib

SOURCES += mainwindow.cpp \
    network.cpp

HEADERS  += mainwindow.h \
    network.h

FORMS    += mainwindow.ui

TRANSLATIONS += qmsettings_sp.ts \
    qmsettings_fr.ts

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

RESOURCES += \
    settingsWizardCore.qrc
