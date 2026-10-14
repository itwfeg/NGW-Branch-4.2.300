TARGET = XpdfWidgetQtDemo
TEMPLATE = app

# * Set FREETYPE_INCLUDE_DIR so that $${FREETYPE_INCLUDE_DIR}/ft2build.h
#   exists.
# * Set FREETYPE_LIB_DIR so that $${FREETYPE_LIB_DIR}/freetype.lib
#   (Windows) or $${FREETYPE_LIB_DIR}/libfreetype.a (Linux, Mac)
#   exists.
#-- on Linux with system FreeType:
#FREETYPE_INCLUDE_DIR = /usr/include/freetype2
#FREETYPE_LIB_DIR = /usr/lib
#-- on Windows with FreeType built locally:
#FREETYPE_INCLUDE_DIR = $$OUT_PWD/../../freetype-2.10.0/include
#FREETYPE_LIB_DIR = $$OUT_PWD/../../freetype-2.10.0/release
#-- on Mac with FreeType built locally:
#FREETYPE_INCLUDE_DIR = $$OUT_PWD/../freetype-2.10.0/include
#FREETYPE_LIB_DIR = $$OUT_PWD/../freetype-2.10.0

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets
}

SOURCES = \
	DemoWindow.cc \
	demo.cc

HEADERS = \
	DemoWindow.h

win32 {
  CONFIG(release, debug|release): {CONFIG_TYPE=release}
  CONFIG(debug, debug|release): {CONFIG_TYPE=debug}
  PRE_TARGET_DEPS += $$OUT_PWD/../$${CONFIG_TYPE}/XpdfWidgetQt.lib
  LIBS += -L$$OUT_PWD/../$${CONFIG_TYPE} -lXpdfWidgetQt
  LIBS += -ladvapi32 -luser32 -lgdi32
}
unix {
  PRE_TARGETDEPS += $$OUT_PWD/../libXpdfWidgetQt.a
  LIBS += -L$$OUT_PWD/.. -lXpdfWidgetQt
}
LIBS += -L$${FREETYPE_LIB_DIR} -lfreetype

INCLUDEPATH += $$OUT_PWD/../xpdf-qt
DEPENDPATH += $$OUT_PWD/../xpdf-qt
INCLUDEPATH += $$OUT_PWD/../goo
DEPENDPATH += $$OUT_PWD/../goo
INCLUDEPATH += $$OUT_PWD/../
DEPENDPATH += $$OUT_PWD/../

INCLUDEPATH += $${FREETYPE_INCLUDE_DIR}
DEPENDPATH += $${FREETYPE_INCLUDE_DIR}
