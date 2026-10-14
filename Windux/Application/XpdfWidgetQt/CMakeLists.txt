cmake_minimum_required(VERSION 3.13.0)

project(XpdfWidgetQt LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Network)
include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})

find_package(PkgConfig)
pkg_check_modules(FREETYPE REQUIRED freetype2)
pkg_search_module(GLIB REQUIRED glib-2.0)

include_directories("${FREETYPE_INCLUDE_DIRS}")

add_library(XpdfWidgetQt STATIC)

target_link_libraries(XpdfWidgetQt PUBLIC ${GLIB_LIBRARIES} Qt5::Core Qt5::Widgets Qt5::Network)

target_include_directories(XpdfWidgetQt PRIVATE
  src/
  src/splash
  src/xpdf
  src/xpdf-qt
  src/goo
  src/fofi
)

target_sources(XpdfWidgetQt PRIVATE
  src/aconf2.h
  src/aconf.h
  src/aconf-linux.h
  src/aconf-mac.h
  src/aconf-win32.h
  src/fofi/FoFiBase.cc
  src/fofi/FoFiBase.h
  src/fofi/FoFiEncodings.cc
  src/fofi/FoFiEncodings.h
  src/fofi/FoFiIdentifier.cc
  src/fofi/FoFiIdentifier.h
  src/fofi/FoFiTrueType.cc
  src/fofi/FoFiTrueType.h
  src/fofi/FoFiType1.cc
  src/fofi/FoFiType1C.cc
  src/fofi/FoFiType1C.h
  src/fofi/FoFiType1.h
  src/goo/FixedPoint.cc
  src/goo/FixedPoint.h
  src/goo/gfile.cc
  src/goo/gfile.h
  src/goo/GHash.cc
  src/goo/GHash.h
  src/goo/GList.cc
  src/goo/GList.h
  src/goo/gmem.cc
  src/goo/gmem.h
  src/goo/gmempp.cc
  src/goo/gmempp.h
  src/goo/GMutex.h
  src/goo/GString.cc
  src/goo/GString.h
  src/goo/gtypes.h
  src/goo/parseargs.c
  src/goo/parseargs.h
  src/splash/SplashBitmap.cc
  src/splash/SplashBitmap.h
  src/splash/Splash.cc
  src/splash/SplashClip.cc
  src/splash/SplashClip.h
  src/splash/SplashErrorCodes.h
  src/splash/SplashFont.cc
  src/splash/SplashFontEngine.cc
  src/splash/SplashFontEngine.h
  src/splash/SplashFontFile.cc
  src/splash/SplashFontFile.h
  src/splash/SplashFontFileID.cc
  src/splash/SplashFontFileID.h
  src/splash/SplashFont.h
  src/splash/SplashFTFont.cc
  src/splash/SplashFTFontEngine.cc
  src/splash/SplashFTFontEngine.h
  src/splash/SplashFTFontFile.cc
  src/splash/SplashFTFontFile.h
  src/splash/SplashFTFont.h
  src/splash/SplashGlyphBitmap.h
  src/splash/Splash.h
  src/splash/SplashMath.h
  src/splash/SplashPath.cc
  src/splash/SplashPath.h
  src/splash/SplashPattern.cc
  src/splash/SplashPattern.h
  src/splash/SplashScreen.cc
  src/splash/SplashScreen.h
  src/splash/SplashState.cc
  src/splash/SplashState.h
  src/splash/SplashTypes.h
  src/splash/SplashXPath.cc
  src/splash/SplashXPath.h
  src/splash/SplashXPathScanner.cc
  src/splash/SplashXPathScanner.h
  src/xpdf/AcroForm.cc
  src/xpdf/AcroForm.h
  src/xpdf/Annot.cc
  src/xpdf/Annot.h
  src/xpdf/Array.cc
  src/xpdf/Array.h
  src/xpdf/BuiltinFont.cc
  src/xpdf/BuiltinFont.h
  src/xpdf/BuiltinFontTables.cc
  src/xpdf/BuiltinFontTables.h
  src/xpdf/Catalog.cc
  src/xpdf/Catalog.h
  src/xpdf/CharCodeToUnicode.cc
  src/xpdf/CharCodeToUnicode.h
  src/xpdf/CharTypes.h
  src/xpdf/CMap.cc
  src/xpdf/CMap.h
  src/xpdf/CompactFontTables.h
  src/xpdf/config.h
  src/xpdf/Decrypt.cc
  src/xpdf/Decrypt.h
  src/xpdf/Dict.cc
  src/xpdf/Dict.h
  src/xpdf/DisplayState.cc
  src/xpdf/DisplayState.h
  src/xpdf/Error.cc
  src/xpdf/ErrorCodes.h
  src/xpdf/Error.h
  src/xpdf/FontEncodingTables.cc
  src/xpdf/FontEncodingTables.h
  src/xpdf/Form.cc
  src/xpdf/Form.h
  src/xpdf/Function.cc
  src/xpdf/Function.h
  src/xpdf/Gfx.cc
  src/xpdf/GfxFont.cc
  src/xpdf/GfxFont.h
  src/xpdf/Gfx.h
  src/xpdf/GfxState.cc
  src/xpdf/GfxState.h
  src/xpdf/GlobalParams.cc
  src/xpdf/GlobalParams.h
  src/xpdf/HighlightFile.cc
  src/xpdf/HighlightFile.h
  src/xpdf/HTMLGen.cc
  src/xpdf/HTMLGen.h
  src/xpdf/ImageOutputDev.cc
  src/xpdf/ImageOutputDev.h
  src/xpdf/JArithmeticDecoder.cc
  src/xpdf/JArithmeticDecoder.h
  src/xpdf/JBIG2Stream.cc
  src/xpdf/JBIG2Stream.h
  src/xpdf/JPXStream.cc
  src/xpdf/JPXStream.h
  src/xpdf/Lexer.cc
  src/xpdf/Lexer.h
  src/xpdf/Link.cc
  src/xpdf/Link.h
  src/xpdf/NameToCharCode.cc
  src/xpdf/NameToCharCode.h
  src/xpdf/NameToUnicodeTable.h
  src/xpdf/Object.cc
  src/xpdf/Object.h
  src/xpdf/OptionalContent.cc
  src/xpdf/OptionalContent.h
  src/xpdf/Outline.cc
  src/xpdf/Outline.h
  src/xpdf/OutputDev.cc
  src/xpdf/OutputDev.h
  src/xpdf/Page.cc
  src/xpdf/Page.h
  src/xpdf/Parser.cc
  src/xpdf/Parser.h
  src/xpdf/PDF417Barcode.cc
  src/xpdf/PDF417Barcode.h
  src/xpdf/PDFCore.cc
  src/xpdf/PDFCore.h
  src/xpdf/pdfdetach.cc
  src/xpdf/PDFDoc.cc
  src/xpdf/PDFDocEncoding.cc
  src/xpdf/PDFDocEncoding.h
  src/xpdf/PDFDoc.h
  src/xpdf/pdffonts.cc
  src/xpdf/pdfimages.cc
  src/xpdf/pdfinfo.cc
  src/xpdf/pdftohtml.cc
  src/xpdf/pdftopng.cc
  src/xpdf/pdftoppm.cc
  src/xpdf/pdftops.cc
  src/xpdf/pdftotext.cc
  src/xpdf/PreScanOutputDev.cc
  src/xpdf/PreScanOutputDev.h
  src/xpdf/PSOutputDev.cc
  src/xpdf/PSOutputDev.h
  src/xpdf/PSTokenizer.cc
  src/xpdf/PSTokenizer.h
  src/xpdf-qt/QtPDFCore.cc
  src/xpdf-qt/QtPDFCore.h
  src/xpdf-qt/QtQuickPDFCore.cc
  src/xpdf-qt/QtQuickPDFCore.h
  src/xpdf-qt/XpdfApp.cc
  src/xpdf-qt/XpdfApp.h
  src/xpdf-qt/xpdf.cc
  src/xpdf-qt/XpdfItem.cc
  src/xpdf-qt/XpdfItem.h
  src/xpdf-qt/XpdfViewer.cc
  src/xpdf-qt/XpdfViewer.h
  src/xpdf-qt/XpdfWidget.cc
  src/xpdf-qt/XpdfWidget.h
  src/xpdf-qt/XpdfWidgetPrint.cc
  src/xpdf-qt/XpdfWidgetPrint.h
  src/xpdf/SecurityHandler.cc
  src/xpdf/SecurityHandler.h
  src/xpdf/SplashOutputDev.cc
  src/xpdf/SplashOutputDev.h
  src/xpdf/Stream.cc
  src/xpdf/Stream-CCITT.h
  src/xpdf/Stream.h
  src/xpdf/TextOutputDev.cc
  src/xpdf/TextOutputDev.h
  src/xpdf/TextString.cc
  src/xpdf/TextString.h
  src/xpdf/TileCache.cc
  src/xpdf/TileCache.h
  src/xpdf/TileCompositor.cc
  src/xpdf/TileCompositor.h
  src/xpdf/TileMap.cc
  src/xpdf/TileMap.h
  src/xpdf/UnicodeMap.cc
  src/xpdf/UnicodeMap.h
  src/xpdf/UnicodeMapTables.h
  src/xpdf/UnicodeRemapping.cc
  src/xpdf/UnicodeRemapping.h
  src/xpdf/UnicodeTypeTable.cc
  src/xpdf/UnicodeTypeTable.h
  src/xpdf/UTF8.cc
  src/xpdf/UTF8.h
  src/xpdf/WebFont.cc
  src/xpdf/WebFont.h
  src/xpdf/XFAForm.cc
  src/xpdf/XFAForm.h
  src/xpdf/XRef.cc
  src/xpdf/XRef.h
  src/xpdf/Zoox.cc
  src/xpdf/Zoox.h
)