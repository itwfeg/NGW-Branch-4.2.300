#-------------------------------------------------
#
# Project created by QtCreator 2012-10-22T10:04:23
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = labelViewerTest
TEMPLATE = app

win32-g++: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-g++: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-g++-64: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-timesys-g++: QMAKE_CXXFLAGS += -Wno-multichar

SOURCES += main.cpp\
        labelviewertest.cpp

HEADERS  += labelviewertest.h \
    labelviewertestdata.h

FORMS    += labelviewertest.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../logManager/release/ -llogManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../logManager/debug/ -llogManager
else:symbian: LIBS += -llogManager
else:unix: LIBS += -L$$OUT_PWD/../logManager/ -llogManager

INCLUDEPATH += $$PWD/../logManager
DEPENDPATH += $$PWD/../logManager

# this here below does not appear to work for the mingw32 compiler
#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/release/logManager.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/debug/logManager.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../logManager/liblogManager.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../labelCore/release/ -llabelCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../labelCore/debug/ -llabelCore
else:symbian: LIBS += -llabelCore
else:unix: LIBS += -L$$OUT_PWD/../labelCore/ -llabelCore

INCLUDEPATH += $$PWD/../labelCore
DEPENDPATH += $$PWD/../labelCore

# this here below does not appear to work for the mingw32 compiler
#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../labelCore/release/labelCore.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../labelCore/debug/labelCore.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../labelCore/liblabelCore.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../labelViewerWidget/release/ -llabelViewerWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../labelViewerWidget/debug/ -llabelViewerWidget
else:symbian: LIBS += -llabelViewerWidget
else:unix: LIBS += -L$$OUT_PWD/../labelViewerWidget/ -llabelViewerWidget

INCLUDEPATH += $$PWD/../labelViewerWidget
DEPENDPATH += $$PWD/../labelViewerWidget

# this here below does not appear to work for the mingw32 compiler
#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../labelViewerWidget/release/labelViewerWidget.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../labelViewerWidget/debug/labelViewerWidget.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../labelViewerWidget/liblabelViewerWidget.a
