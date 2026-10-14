cmake_minimum_required(VERSION 3.13.0)

project(RendererManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(RendererManager STATIC)

target_compile_definitions(RendererManager PRIVATE LINUX_FIX)

target_link_directories(RendererManager PRIVATE ../../../../3RDPartySoftware/digimarc/lib)
target_link_libraries(RendererManager PUBLIC PngGraphics dlsdk util)

pkg_check_modules(FREETYPE REQUIRED freetype2)
target_include_directories(RendererManager PUBLIC "${FREETYPE_INCLUDE_DIRS}")

target_sources(RendererManager PRIVATE
  common/bitstring.h
  common/bitstring.cpp
  common/buffer.h
  common/buffer.cpp
  common/rbitmap.h
  common/rbitmap.cpp
  datamatrix/dmapi.cpp
  datamatrix/dmapi.h
  datamatrix/dmecc.cpp
  datamatrix/dmecc.h
  datamatrix/dmecclevel.h
  datamatrix/dmmetrics.cpp
  datamatrix/dmmetrics.h
  datamatrix/dmmode.cpp
  datamatrix/dmmodedata.cpp
  datamatrix/dmmodedata.h
  datamatrix/dmmode.h
  datamatrix/dmmodule.cpp
  datamatrix/dmmodule.h
  datamatrix/dmsymbolsize.h
  ean/eanapi.cpp
  ean/eanapi.h
  ean/eanstring.cpp
  ean/eanstring.h
  ean/eantype.h
  Include/Barcode.h
  Include/BitBlt.h
  Include/BitMap.h
  Include/Blob.h
  Include/CRendererUtil.h
  Include/DataMatrixBarcode.h
  Include/DrawingPrimitives.h
  Include/Ean128CBarcode.h
  Include/EanBarcode.h
  Include/FormRecord.h
  Include/Frame.h
  Include/GenericBarcode.h
  Include/GraphicHeader.h
  Include/GraphicImage.h
  Include/HobartLabel.h
  Include/hoblabel.h
  Include/ImageControl.h
  Include/ImageRecord.h
  Include/Label.h
  Include/OffsetRecord.h
  Include/Pen.h
  Include/Point.h
  Include/QRBarcode.h
  Include/Rectangle.h
  Include/RendererCloseSession.h
  Include/RendererEvent.h
  Include/RendererGeneric.h
  Include/Renderer.h
  Include/RendererInitDotHeadWear.h
  Include/RendererManager.h
  Include/RendererNewDotHeadWear.h
  Include/RendererRenderComplete.h
  Include/RendererRenderStart.h
  Include/RendererRequestRendering.h
  Include/RendererRequestSession.h
  Include/RendererSessionId.h
  Include/RendererSetDestination.h
  Include/RendererSetSource.h
  Include/ResourceHeader.h
  Include/Rss14.h
  Include/SwapBytes.h
  Include/TempHeader.h
  Include/UltimaObjects.h
  Include/UpcBarcode.h
  Include/UpcD3Barcode.h
  Include/WordWrapLineMetrics.h
  Messages/RendererCloseSession.cpp
  Messages/RendererGeneric.cpp
  Messages/RendererInitDotHeadWear.cpp
  Messages/RendererNewDotHeadWear.cpp
  Messages/RendererRenderComplete.cpp
  Messages/RendererRenderStart.cpp
  Messages/RendererRequestRendering.cpp
  Messages/RendererRequestSession.cpp
  Messages/RendererSessionId.cpp
  Messages/RendererSetDestination.cpp
  Messages/RendererSetSource.cpp
  qrcode/poly.cpp
  qrcode/poly.h
  qrcode/qrapi.cpp
  qrcode/qrapi.h
  qrcode/qreccblock.cpp
  qrcode/qreccblock.h
  qrcode/qrecc.cpp
  qrcode/qrecc.h
  qrcode/qrecclevel.h
  qrcode/qrmetrics.cpp
  qrcode/qrmetrics.h
  qrcode/qrmode.cpp
  qrcode/qrmodedata.cpp
  qrcode/qrmodedata.h
  qrcode/qrmode.h
  qrcode/qrmodule.cpp
  qrcode/qrmodule.h
  qrcode/qrversion.h
  Source/BitMap.cpp
  Source/Blob.cpp
  Source/CopyOp.cpp
  Source/CRendererUtil.cpp
  Source/DataMatrixBarcode.cpp
  Source/DrawingPrimitives.cpp
  Source/Ean128CBarcode.cpp
  Source/EanBarcode.cpp
  Source/FormRecord.cpp
  Source/GenericBarcode.cpp
  Source/GraphicHeader.cpp
  Source/GraphicImage.cpp
  Source/HobartLabel.cpp
  Source/ImageControl.cpp
  Source/OffSetRecord.cpp
  Source/Pen.cpp
  Source/Point.cpp
  Source/QRBarcode.cpp
  Source/Rectangle.cpp
  Source/Renderer.cpp
  Source/RendererManager.cpp
  Source/ResourceHeader.cpp
  Source/RotateOp.cpp
  Source/Rss14.cpp
  Source/UpcBarcode.cpp
  Source/UpcD3Barcode.cpp
  Source/WordWrapLineMetrics.cpp
)

target_include_directories(RendererManager PUBLIC Include)

target_include_directories(RendererManager PRIVATE
  Messages
  datamatrix
  qrcode
  common
  ../../../PAL
  ../../../PAL/PAL_linux
  ../../../inc/linux
  ../../../source/app
  ../../../source/comm
  ../../../source/core
  ../../../source/core/Synchronization
  ../../../source/core/VersionManager/include
  ../../../source/db
  ../../../source/db/util
  ../../../source/db/util/e_string
  ../../../source/db/util/filesys
  ../../../source/db/util/thread
  ../../../source/EvolutionRTC/LabelApplierManager/Include
  ../../../source/EvolutionRTC/MarqueeManager/Include
  ../../../source/EvolutionRTC/RFIDManager/Include
  ../../../source/EvolutionRTC/RTCManager/Include
  ../../../source/EvolutionRTC/ScannerManager/Include
  ../../../source/EvolutionRTC/TimerManager/Include
  ../../../source/EvolutionRTC/USBCommManagers/Include
  ../../../source/EvolutionRTC/USBManager/Include
  ../../../source/EvolutionRTC/WeigherManager/Include
  ../../../source/EvolutionRTC/WrapperManager/Include
  ../../../source/HFC
  ../../../source/HFC/tags
  ../../../source/HFC/devices
  ../../../source/util
  ../../../source/util/Inc
  ../../../../Smbus/driver
  ../../../source/EvolutionRTC/MessageCenter
  ../../../source/MetrologicallySignificant
  ../../../source/EvolutionRTC/CanManager/Messages/Include
  ../../../source/EvolutionRTC/NumericManager/Include
  ../../../source/EvolutionRTC/GCDManager/Include
  ../../../source/EvolutionRTC/OperatingModes/Include
  ../../../source/EvolutionRTC/BoardSite/Include
  ../../../source/EvolutionRTC/PngGraphics/Include
  ../../../source/EvolutionRTC/PrinterManager/Include
  ../../../source/EvolutionRTC/DiagnosticsManager/Messages
  ../../../../3RDPartySoftware/digimarc/include
  ../../../source/EvolutionRTC
)
