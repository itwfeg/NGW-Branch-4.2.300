#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T08:08:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wrapperPacketSim
TEMPLATE = app

INCLUDEPATH += ./

SOURCES += \
    main.cpp \
    wrapperpacketdialog.cpp \
    DiagnosticsEdgeDevice.cpp \
    Util/BasicSocket.cpp \
    Util/Dict.cpp \
    Util/NetworkVal.cpp \
    Util/Timestamp.cpp \
    EdgeComm/EdgePacket.cpp \
    edgedevicecommthread.cpp \
    e_string/utilstring.cpp

HEADERS += \
    wrapperpacketdialog.h \
    DiagnosticsEdgeDevice.h \
    Util/BasicSocket.h \
    Util/Dict.h \
    Util/NetworkVal.h \
    Util/Timestamp.h \
    EdgeComm/DiagnosticsMsg.h \
    EdgeComm/EdgePacket.h \
    EdgeComm/EventMsg.h \
    EdgeComm/Header.h \
    EdgeComm/HealthInfoMsg.h \
    EdgeComm/PowerUpMsg.h \
    EdgeComm/StatusCheckMsg.h \
    edgedevicecommthread.h \
    e_string/utilstring.h

LIBS += -lws2_32
