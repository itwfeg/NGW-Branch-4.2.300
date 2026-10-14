QT -= gui
QT += dbus

TEMPLATE = lib
DEFINES += EVENTS_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    event.cpp \
    eventfactory.cpp \
    fileresourceevent.cpp \
    jsondocumentevent.cpp \
    messagecenterclient.cpp \
    sessioneventtype.cpp

HEADERS += \
    databaseeventtype.h \
    databasejson.h \
    devicetype.h \
    eventfactory.h \
    events_global.h \
    event.h \
    fileresourceevent.h \
    jsondocumentevent.h \
    messagecenterclient.h \
    resourceeventtype.h \
    sessioneventtype.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../dbusinterfaces/release/ -ldbusinterfaces
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../dbusinterfaces/debug/ -ldbusinterfaces
else:unix: LIBS += -L$$OUT_PWD/../dbusinterfaces/ -ldbusinterfaces

INCLUDEPATH += $$OUT_PWD/../dbusinterfaces
DEPENDPATH += $$PWD/../dbusinterfaces

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../dbusinterfaces/release/libdbusinterfaces.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../dbusinterfaces/debug/libdbusinterfaces.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../dbusinterfaces/release/dbusinterfaces.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../dbusinterfaces/debug/dbusinterfaces.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../dbusinterfaces/libdbusinterfaces.a

