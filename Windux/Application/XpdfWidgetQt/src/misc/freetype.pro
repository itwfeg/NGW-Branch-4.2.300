QT -= core gui

TARGET = freetype
TEMPLATE = lib
CONFIG += staticlib

DEFINES += FT2_BUILD_LIBRARY

INCLUDEPATH += include

SOURCES += \
    src/base/ftbitmap.c \
    src/base/ftmm.c \
    src/base/ftbbox.c \
    src/base/ftglyph.c \
    src/base/ftbase.c \
    src/base/ftdebug.c \
    src/base/ftinit.c \
    src/base/ftsystem.c \
    src/autofit/autofit.c \
    src/sfnt/sfnt.c \
    src/cff/cff.c \
    src/psnames/psnames.c \
    src/psaux/psaux.c \
    src/truetype/truetype.c \
    src/type1/type1.c \
    src/cid/type1cid.c \
    src/winfonts/winfnt.c \
    src/smooth/smooth.c \
    src/raster/raster.c \
    src/pshinter/pshinter.c \
    src/pcf/pcf.c \
    src/pfr/pfr.c \
    src/type42/type42.c \
    src/bdf/bdf.c \
    src/gzip/ftgzip.c \
    src/lzw/ftlzw.c
