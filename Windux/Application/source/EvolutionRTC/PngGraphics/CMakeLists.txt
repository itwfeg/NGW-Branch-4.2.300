cmake_minimum_required(VERSION 3.13.0)

project(PngGraphics LANGUAGES C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(PngGraphics STATIC)

target_compile_definitions(PngGraphics PRIVATE LINUX_FIX)

target_sources(PngGraphics PRIVATE
  Include/deflate.h
  Include/gconver.h
  Include/hob2png.h
  Include/infblock.h
  Include/infcodes.h
  Include/inffast.h
  Include/inftrees.h
  Include/infutil.h
  Include/png2hob.h
  Include/pngconf.h
  Include/png.h
  Include/zconf.h
  Include/zlib.h
  Include/zutil.h
  Source/adler32.c
  Source/crc32.c
  Source/deflate.c
  Source/hob2png.c
  Source/infblock.c
  Source/infcodes.c
  Source/inffast.c
  Source/inflate.c
  Source/inftrees.c
  Source/infutil.c
  Source/png2hob.c
  Source/png.c
  Source/pngerror.c
  Source/pngget.c
  Source/pngmem.c
  Source/pngread.c
  Source/pngrio.c
  Source/pngrtran.c
  Source/pngrutil.c
  Source/pngset.c
  Source/pngtrans.c
  Source/pngwio.c
  Source/pngwrite.c
  Source/pngwtran.c
  Source/pngwutil.c
  Source/trees.c
  Source/zutil.c
)

target_include_directories(PngGraphics PUBLIC Include)

target_include_directories(PngGraphics PRIVATE
  Messages
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
  ../../../source/EvolutionRTC/CodeUpdater/PrinterManager/Include
  ../../../source/EvolutionRTC/LabelApplierManager/Include
  ../../../source/EvolutionRTC/MarqueeManager/Include
  ../../../source/EvolutionRTC/PngGraphics
  ../../../source/EvolutionRTC/PngGraphics/Include
  ../../../source/EvolutionRTC/PngGraphics/Include
  ../../../source/EvolutionRTC/PngGraphics/Include
  ../../../source/EvolutionRTC/PrinterManager/Include
  ../../../source/EvolutionRTC/ProductDev/Include
  ../../../source/EvolutionRTC/RendererManager/Include
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
)
