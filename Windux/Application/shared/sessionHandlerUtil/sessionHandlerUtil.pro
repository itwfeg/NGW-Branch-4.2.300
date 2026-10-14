#-------------------------------------------------
#
# Project created by QtCreator 2014-01-17T09:09:12
#
#-------------------------------------------------

QT       += widgets network

TARGET = sessionHandlerUtil
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += common

win32-g++: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-g++: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-g++-64: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-timesys-g++: QMAKE_CXXFLAGS += -Wno-multichar

SOURCES += \
    shellzip.cpp \
    openauth.cpp \
    common/bitmap.cpp \
    common/bitstring.cpp \
    common/buffer.cpp \
    ean/eanapi.cpp \
    ean/eanstring.cpp \
    qrcode/poly.cpp \
    qrcode/qrapi.cpp \
    qrcode/qrecc.cpp \
    qrcode/qreccblock.cpp \
    qrcode/qrmetrics.cpp \
    qrcode/qrmode.cpp \
    qrcode/qrmodedata.cpp \
    qrcode/qrmodule.cpp

HEADERS += \
    font.h \
    margins.h \
    palette.h \
    pixmap.h \
    shellzip.h \
    openauth.h \
    common/bitmap.h \
    common/bitstring.h \
    common/buffer.h \
    ean/eanapi.h \
    ean/eanstring.h \
    ean/eantype.h \
    qrcode/poly.h \
    qrcode/qrapi.h \
    qrcode/qrecc.h \
    qrcode/qreccblock.h \
    qrcode/qrecclevel.h \
    qrcode/qrmetrics.h \
    qrcode/qrmode.h \
    qrcode/qrmodedata.h \
    qrcode/qrmodule.h \
    qrcode/qrversion.h
