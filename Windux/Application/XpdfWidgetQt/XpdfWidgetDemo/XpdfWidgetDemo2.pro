QT       += widgets

TARGET = XpdfWidgetDemo2
TEMPLATE = app


SOURCES += main.cc\
        xpdfwidgetdemo2.cc

HEADERS  += xpdfwidgetdemo2.h

FORMS    += xpdfwidgetdemo2.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../XpdfWidgetQtLib-build-Desktop-Qt_4_8_1_for_Desktop_-_MinGW__Qt_SDK__Debug/release/ -lXpdfWidgetQtLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../XpdfWidgetQtLib-build-Desktop-Qt_4_8_1_for_Desktop_-_MinGW__Qt_SDK__Debug/debug/ -lXpdfWidgetQtLib
else:unix: LIBS += -L$$PWD/../XpdfWidgetQtLib-build-Desktop-Qt_4_8_1_for_Desktop_-_MinGW__Qt_SDK__Debug/ -lXpdfWidgetQtLib

INCLUDEPATH += $$PWD/../XpdfWidgetQtSrc-3.03.18/xpdf-qt
DEPENDPATH += $$PWD/../XpdfWidgetQtSrc-3.03.18/xpdf-qt
INCLUDEPATH += $$PWD/../XpdfWidgetQtSrc-3.03.18/
DEPENDPATH += $$PWD/../XpdfWidgetQtSrc-3.03.18/

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../FreeTypeLib-build-Desktop-Qt_4_8_1_for_Desktop_-_MinGW__Qt_SDK__Debug/release -lFreeTypeLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../FreeTypeLib-build-Desktop-Qt_4_8_1_for_Desktop_-_MinGW__Qt_SDK__Debug/debug -lFreeTypeLib
else:unix: LIBS += -L$$PWD/../FreeTypeLib-build-Desktop-Qt_4_8_1_for_Desktop_-_MinGW__Qt_SDK__Debug/ -lFreeTypeLib

INCLUDEPATH += $$PWD/../freetype-2.4.11/include
DEPENDPATH += $$PWD/../freetype-2.4.11/include

win32: LIBS += -ladvapi32
