#-------------------------------------------------
#
# Project created by QtCreator 2012-10-22T10:20:15
#
#-------------------------------------------------

TARGET = labelCore
TEMPLATE = lib
CONFIG += staticlib

win32-g++: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-g++: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-g++-64: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-timesys-g++: QMAKE_CXXFLAGS += -Wno-multichar

SOURCES += \
    tagdatastream.cpp \
    tagdatarecord.cpp \
    dataobjectcontainer.cpp \
    dataobject.cpp \
    systemstring.cpp \
    graphic.cpp \
    nutrifactcolumn.cpp \
    nutrifact.cpp \
    labelformtype.cpp \
    labelformflags.cpp \
    labelform.cpp \
    label.cpp \
    datastore.cpp

HEADERS += \
    tagdatastream.h \
    tagdatarecord.h \
    tagdatafunctioncode.h \
    tagdatacommand.h \
    dataobjectcontainer.h \
    dataobjecttraits.h \
    processtagdatacontainer.h \
    dataobjecttype.h \
    dataobject.h \
    lockingdataobject.h \
    mapdataobjectcontainer.h \
    systemstring.h \
    graphic.h \
    encoding.h \
    nutrifact.h \
    nutrifactcolumn.h \
    labelformtype.h \
    labelformflags.h \
    labelform.h \
    label.h \
    datastore.h \
    dataobjectcontainerfactory.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

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
