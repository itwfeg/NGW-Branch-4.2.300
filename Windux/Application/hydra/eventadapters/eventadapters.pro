QT -= gui

TEMPLATE = lib
DEFINES += EVENTADAPTERS_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    eventadapter.cpp \
    tageventadapter.cpp \
    transformprodtext.cpp \
    transformresource.cpp

HEADERS += \
    eventadapter.h \
    eventadapters_global.h \
    evoltags.h \
    tageventadapter.h \
    transformprodtext.h \
    transformresource.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../events/release/ -levents
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../events/debug/ -levents
else:unix: LIBS += -L$$OUT_PWD/../events/ -levents

INCLUDEPATH += $$PWD/../events
DEPENDPATH += $$PWD/../events
