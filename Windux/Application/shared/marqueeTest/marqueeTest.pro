#-------------------------------------------------
#
# Project created by QtCreator 2012-10-09T09:39:49
#
#-------------------------------------------------

QT       += core gui

TARGET = marqueeTest
TEMPLATE = app


SOURCES += main.cpp\
        marqueetest.cpp

HEADERS  += marqueetest.h

FORMS    += marqueetest.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../marqueeLabelWidget/release/ -lmarqueeLabelWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../marqueeLabelWidget/debug/ -lmarqueeLabelWidget
else:symbian: LIBS += -lmarqueeLabelWidget
else:unix: LIBS += -L$$OUT_PWD/../marqueeLabelWidget/ -lmarqueeLabelWidget

INCLUDEPATH += $$PWD/../marqueeLabelWidget
DEPENDPATH += $$PWD/../marqueeLabelWidget

# this here below does not appear to work for the mingw32 compiler
#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../marqueeLabelWidget/release/marqueeLabelWidget.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../marqueeLabelWidget/debug/marqueeLabelWidget.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../marqueeLabelWidget/libmarqueeLabelWidget.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../marqueeTextEditorWidget/release/ -lmarqueeTextEditorWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../marqueeTextEditorWidget/debug/ -lmarqueeTextEditorWidget
else:symbian: LIBS += -lmarqueeTextEditorWidget
else:unix: LIBS += -L$$OUT_PWD/../marqueeTextEditorWidget/ -lmarqueeTextEditorWidget

INCLUDEPATH += $$PWD/../marqueeTextEditorWidget
DEPENDPATH += $$PWD/../marqueeTextEditorWidget

# this here below does not appear to work for the mingw32 compiler
#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../marqueeTextEditorWidget/release/marqueeTextEditorWidget.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../marqueeTextEditorWidget/debug/marqueeTextEditorWidget.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../marqueeTextEditorWidget/libmarqueeTextEditorWidget.a
