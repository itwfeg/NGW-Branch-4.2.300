#-------------------------------------------------
#
# Project created by QtCreator 2012-09-26T15:35:34
#
#-------------------------------------------------

QT       += core gui widgets sql

TARGET = settingsWizard
TEMPLATE = app


SOURCES += main.cpp

HEADERS  +=

FORMS    +=

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../settingsWizardCore/release/ -lsettingsWizardCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../settingsWizardCore/debug/ -lsettingsWizardCore
else:symbian: LIBS += -lsettingsWizardCore
else:unix: LIBS += -L$$OUT_PWD/../settingsWizardCore/ -lsettingsWizardCore

INCLUDEPATH += $$PWD/../settingsWizardCore
DEPENDPATH += $$PWD/../settingsWizardCore

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../settingsWizardCore/release/settingsWizardCore.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../settingsWizardCore/debug/settingsWizardCore.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../settingsWizardCore/libsettingsWizardCore.a
