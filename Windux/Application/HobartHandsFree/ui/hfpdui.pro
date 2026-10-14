
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = hfpdui
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    inputwidget.cpp \
    keyboardinputwidget.cpp \
    inputwidgethook.cpp \
    touchscreenstyle.cpp \
    sensorselection.cpp \
    commclient.cpp \
    configuration.cpp \
    configeditorwidget.cpp \
    firmwareupgradedialog.cpp

HEADERS += \
    mainwindow.h \
    inputwidget.h \
    keyboardinputwidget.h \
    inputwidgethook.h \
    touchscreenstyle.h \
    colors.h \
    sensorselection.h \
    commclient.h \
    configuration.h \
    configeditorwidget.h \
    firmwareupgradedialog.h

RESOURCES += \
    hfpdui.qrc

#
# hfpdshared
#
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../shared/release/ -lhfpdshared
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../shared/debug/ -lhfpdshared
else:unix:!symbian: LIBS += -L$$OUT_PWD/../shared/ -lhfpdshared

INCLUDEPATH += $$PWD/../shared
DEPENDPATH += $$PWD/../shared

# ws2
win32: LIBS += Ws2_32.lib

