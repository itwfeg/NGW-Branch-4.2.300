QT -= gui
QT += dbus

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    testobject.h

SOURCES += \
    main.cpp \
    testobject.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../events/release/ -levents
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../events/debug/ -levents
else:unix: LIBS += -L$$OUT_PWD/../events/ -levents

INCLUDEPATH += $$PWD/../events
DEPENDPATH += $$PWD/../events
