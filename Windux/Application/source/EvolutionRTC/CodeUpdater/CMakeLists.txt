cmake_minimum_required(VERSION 3.13.0)

project(CodeUpdater LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(CodeUpdater STATIC)

target_compile_definitions(CodeUpdater PRIVATE LINUX_FIX)

target_link_libraries(CodeUpdater PUBLIC USBManager PrinterManager)

target_sources(CodeUpdater PRIVATE
  CLA/Include/CUCLACommManager.h
  CLA/Include/CUCLAStatus.h
  CLA/Include/UsbCLACodeUpdater.h
  CLA/Source/CUCLACommManager.cpp
  CLA/Source/CUCLAStatus.cpp
  CLA/Source/UsbCLACodeUpdater.cpp
  PrinterManager/Include/CUPrinterCommManager.h
  PrinterManager/Include/CUPrinterEvent.h
  PrinterManager/Include/CUPrinter.h
  PrinterManager/Include/CUPrinterPeripheralMessages.h
  PrinterManager/Source/CUPrinterCommManager.cpp
  Wrapper/Include/CUEvolWrapperStatus.h
  Wrapper/Include/CUWrapperCommManager.h
  Wrapper/Include/UsbWrapperCodeUpdater.h
  Wrapper/Source/CUEvolWrapperStatus.cpp
  Wrapper/Source/CUWrapperCommManager.cpp
  Wrapper/Source/UsbWrapperCodeUpdater.cpp
)

target_include_directories(CodeUpdater PUBLIC
  CLA/Include
  PrinterManager/Include
  Wrapper/Include
)

target_include_directories(CodeUpdater PRIVATE
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
../../../source/db/util/thread
../../../source/EvolutionRTC/CodeUpdater/CLA/Include
../../../source/EvolutionRTC/CodeUpdater/Wrapper/Include
../../../source/EvolutionRTC/LabelApplierManager/Include
../../../source/EvolutionRTC/MarqueeManager/Include
../../../source/EvolutionRTC/MessageCenter
../../../source/EvolutionRTC/NumericManager/Include
../../../source/EvolutionRTC/OperatingModes/Include
../../../source/EvolutionRTC/PngGraphics/Include
../../../source/EvolutionRTC/PrinterManager/Include
../../../source/EvolutionRTC/RendererManager/Include
../../../source/EvolutionRTC/RTCManager/Include
../../../source/EvolutionRTC/TimerManager/Include
../../../source/EvolutionRTC/USBCommManagers/Include
../../../source/EvolutionRTC/USBManager/Include
../../../source/EvolutionRTC/WeigherManager/Include
../../../source/EvolutionRTC/WrapperManager/Include
../../../source/EvolutionRTC/GCDManager/Include
../../../source/HFC
../../../source/HFC/tags
../../../source/HFC/devices
../../../source/MetrologicallySignificant
../../../source/util
../../../source/util/Inc
)
