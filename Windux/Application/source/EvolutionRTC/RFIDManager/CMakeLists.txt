cmake_minimum_required(VERSION 3.13.0)

project(RFIDManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(RFIDManager STATIC)

target_compile_definitions(RFIDManager PRIVATE LINUX_FIX)

target_sources(RFIDManager PRIVATE
  Include/RFIDEvent.h
  Include/RFIDManager.h
  Include/RFIDStatus.h
  Include/Tiris.h
  Include/TIRISReadOnlyId.h
  Messages/RFIDStatus.cpp
  Messages/TIRISReadOnlyId.cpp
  Source/RFIDManager.cpp
  Source/RFIDManagerLinux.cpp
)

target_include_directories(RFIDManager PUBLIC Include)

target_include_directories(RFIDManager PRIVATE
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
  ../../../source/EvolutionRTC/RendererManager/Include
)
