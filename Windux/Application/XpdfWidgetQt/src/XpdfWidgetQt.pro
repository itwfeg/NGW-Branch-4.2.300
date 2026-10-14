TARGET = XpdfWidgetQt
TEMPLATE = lib
CONFIG += staticlib

# * Set FREETYPE_INCLUDE_DIR so that $${FREETYPE_INCLUDE_DIR}/ft2build.h
#   exists.
# * Set FREETYPE_LIB_DIR so that $${FREETYPE_LIB_DIR}/freetype.lib
#   (Windows) or $${FREETYPE_LIB_DIR}/libfreetype.a (Linux, Mac)
#   exists.
#-- on Linux with system FreeType:
#FREETYPE_INCLUDE_DIR = /usr/include/freetype2
#FREETYPE_LIB_DIR = /usr/lib
#-- on Windows with FreeType built locally:
#FREETYPE_INCLUDE_DIR = $$OUT_PWD/../freetype-2.10.0/include
#FREETYPE_LIB_DIR = $$OUT_PWD/../freetype-2.10.0/release
#-- on Mac with FreeType built locally:
#FREETYPE_INCLUDE_DIR = $$OUT_PWD/../freetype-2.10.0/include
#FREETYPE_LIB_DIR = $$OUT_PWD/../freetype-2.10.0

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets
}

# workaround for a conflict between the Qt5 QDateTime and windows.h
win32:DEFINES += NOMINMAX

INCLUDEPATH += . \
    goo \
    fofi \
    splash \
    xpdf \
    xpdf-qt

SOURCES = \
    goo/FixedPoint.cc \
    goo/gfile.cc \
    goo/GHash.cc \
    goo/GList.cc \
    goo/gmem.cc \
    goo/gmempp.cc \
    goo/GString.cc \
    goo/parseargs.c \
    fofi/FoFiBase.cc \
    fofi/FoFiEncodings.cc \
    fofi/FoFiIdentifier.cc \
    fofi/FoFiTrueType.cc \
    fofi/FoFiType1.cc \
    fofi/FoFiType1C.cc \
    splash/Splash.cc \
    splash/SplashBitmap.cc \
    splash/SplashClip.cc \
    splash/SplashFont.cc \
    splash/SplashFontEngine.cc \
    splash/SplashFontFile.cc \
    splash/SplashFontFileID.cc \
    splash/SplashFTFont.cc \
    splash/SplashFTFontEngine.cc \
    splash/SplashFTFontFile.cc \
    splash/SplashPath.cc \
    splash/SplashPattern.cc \
    splash/SplashScreen.cc \
    splash/SplashState.cc \
    splash/SplashXPath.cc \
    splash/SplashXPathScanner.cc \
    xpdf/AcroForm.cc \
    xpdf/Annot.cc \
    xpdf/Array.cc \
    xpdf/BuiltinFont.cc \
    xpdf/BuiltinFontTables.cc \
    xpdf/Catalog.cc \
    xpdf/CharCodeToUnicode.cc \
    xpdf/CMap.cc \
    xpdf/Decrypt.cc \
    xpdf/Dict.cc \
    xpdf/DisplayState.cc \
    xpdf/Error.cc \
    xpdf/FontEncodingTables.cc \
    xpdf/Form.cc \
    xpdf/Function.cc \
    xpdf/Gfx.cc \
    xpdf/GfxFont.cc \
    xpdf/GfxState.cc \
    xpdf/GlobalParams.cc \
    xpdf/HighlightFile.cc \
    xpdf/JArithmeticDecoder.cc \
    xpdf/JBIG2Stream.cc \
    xpdf/JPXStream.cc \
    xpdf/Lexer.cc \
    xpdf/Link.cc \
    xpdf/NameToCharCode.cc \
    xpdf/Object.cc \
    xpdf/OptionalContent.cc \
    xpdf/Outline.cc \
    xpdf/OutputDev.cc \
    xpdf/Page.cc \
    xpdf/Parser.cc \
    xpdf/PDF417Barcode.cc \
    xpdf/PDFCore.cc \
    xpdf/PDFDoc.cc \
    xpdf/PDFDocEncoding.cc \
    xpdf/PSTokenizer.cc \
    xpdf/SecurityHandler.cc \
    xpdf/SplashOutputDev.cc \
    xpdf/Stream.cc \
    xpdf/TextOutputDev.cc \
    xpdf/TextString.cc \
    xpdf/TileCache.cc \
    xpdf/TileCompositor.cc \
    xpdf/TileMap.cc \
    xpdf/UnicodeMap.cc \
    xpdf/UnicodeRemapping.cc \
    xpdf/UnicodeTypeTable.cc \
    xpdf/UTF8.cc \
    xpdf/XFAForm.cc \
    xpdf/XRef.cc \
    xpdf/Zoox.cc \
    xpdf-qt/QtPDFCore.cc \
    xpdf-qt/XpdfWidget.cc

HEADERS = \
    goo/FixedPoint.h \
    goo/gfile.h \
    goo/GHash.h \
    goo/GList.h \
    goo/gmem.h \
    goo/gmempp.h \
    goo/GMutex.h \
    goo/GString.h \
    goo/gtypes.h \
    goo/parseargs.h \
    fofi/FoFiBase.h \
    fofi/FoFiEncodings.h \
    fofi/FoFiIdentifier.h \
    fofi/FoFiTrueType.h \
    fofi/FoFiType1.h \
    fofi/FoFiType1C.h \
    splash/Splash.h \
    splash/SplashBitmap.h \
    splash/SplashClip.h \
    splash/SplashErrorCodes.h \
    splash/SplashFont.h \
    splash/SplashFontEngine.h \
    splash/SplashFontFile.h \
    splash/SplashFontFileID.h \
    splash/SplashFTFont.h \
    splash/SplashFTFontEngine.h \
    splash/SplashFTFontFile.h \
    splash/SplashGlyphBitmap.h \
    splash/SplashMath.h \
    splash/SplashPath.h \
    splash/SplashPattern.h \
    splash/SplashScreen.h \
    splash/SplashState.h \
    splash/SplashTypes.h \
    splash/SplashXPath.h \
    splash/SplashXPathScanner.h \
    xpdf/AcroForm.h \
    xpdf/Annot.h \
    xpdf/Array.h \
    xpdf/BuiltinFont.h \
    xpdf/BuiltinFontTables.h \
    xpdf/Catalog.h \
    xpdf/CharCodeToUnicode.h \
    xpdf/CharTypes.h \
    xpdf/CMap.h \
    xpdf/CompactFontTables.h \
    xpdf/config.h \
    xpdf/Decrypt.h \
    xpdf/Dict.h \
    xpdf/DisplayState.h \
    xpdf/Error.h \
    xpdf/ErrorCodes.h \
    xpdf/FontEncodingTables.h \
    xpdf/Form.h \
    xpdf/Function.h \
    xpdf/Gfx.h \
    xpdf/GfxFont.h \
    xpdf/GlobalParams.h \
    xpdf/GfxState.h \
    xpdf/HighlightFile.h \
    xpdf/JArithmeticDecoder.h \
    xpdf/JBIG2Stream.h \
    xpdf/JPXStream.h \
    xpdf/Lexer.h \
    xpdf/Link.h \
    xpdf/NameToCharCode.h \
    xpdf/NameToUnicodeTable.h \
    xpdf/Object.h \
    xpdf/OptionalContent.h \
    xpdf/Outline.h \
    xpdf/OutputDev.h \
    xpdf/Page.h \
    xpdf/Parser.h \
    xpdf/PDF417Barcode.h \
    xpdf/PDFCore.h \
    xpdf/PDFDoc.h \
    xpdf/PDFDocEncoding.h \
    xpdf/PSTokenizer.h \
    xpdf/SecurityHandler.h \
    xpdf/SplashOutputDev.h \
    xpdf/Stream.h \
    xpdf/Stream-CCITT.h \
    xpdf/TextOutputDev.h \
    xpdf/TextString.h \
    xpdf/TileCache.h \
    xpdf/TileCompositor.h \
    xpdf/TileMap.h \
    xpdf/UnicodeMap.h \
    xpdf/UnicodeMapTables.h \
    xpdf/UnicodeTypeTable.h \
    xpdf/UTF8.h \
    xpdf/XFAForm.h \
    xpdf/XRef.h \
    xpdf/Zoox.h \
    xpdf-qt/QtPDFCore.h \
    xpdf-qt/XpdfWidget.h \
    aconf.h \
    aconf2.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../freetype-2.4.11/release/ -lFreeTypeLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../freetype-2.4.11/debug/ -lFreeTypeLib
else:unix:!symbian {
    CONFIG += link_pkgconfig
    PKGCONFIG += freetype2
    #LIBS += -lfreetype -lm
}

win32: INCLUDEPATH += $$PWD/../freetype-2.4.11/include
win32: DEPENDPATH += $$PWD/../freetype-2.4.11/include

INCLUDEPATH += $${FREETYPE_INCLUDE_DIR}
DEPENDPATH += $${FREETYPE_INCLUDE_DIR}
