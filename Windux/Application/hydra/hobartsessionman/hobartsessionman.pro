QT -= gui
QT += dbus

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        session.cpp \
        sessionmanager.cpp

HEADERS += \
    session.h \
    sessionmanager.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../events/release/ -levents
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../events/debug/ -levents
else:unix: LIBS += -L$$OUT_PWD/../events/ -levents

INCLUDEPATH += $$PWD/../events
DEPENDPATH += $$PWD/../events

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
