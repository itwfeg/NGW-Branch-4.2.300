cmake_minimum_required(VERSION 3.13.0)

project(USBCommManagers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(USBCommManagers STATIC)

target_compile_definitions(USBCommManagers PRIVATE LINUX_FIX)

target_link_libraries(USBCommManagers PUBLIC RTCManager CodeUpdater)

target_sources(USBCommManagers PRIVATE
  Include/APCCommManager.h
  Include/CLACommManager.h
  Include/CommHeader.h
  Include/ESCCommManager.h
  Include/EvolWrapperCommManager.h
  Include/FPCCommManager.h
  Include/PrinterCommManager.h
  Include/USBEvolHeader.h
  Include/WeigherCommManager.h
  Messages/CommHeader.cpp
  Messages/USBEvolHeader.cpp
  Source/APCCommManager.cpp
  Source/CLACommManager.cpp
  Source/ESCCommManager.cpp
  Source/EvolWrapperCommManager.cpp
  Source/FPCCommManager.cpp
  Source/PrinterCommManager.cpp
  Source/WeigherCommManager.cpp
)

target_include_directories(USBCommManagers PUBLIC Include)

target_include_directories(USBCommManagers PRIVATE
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
  ../../../source/EvolutionRTC/CanManager/Include
  ../../../source/EvolutionRTC/ProductDev/Include
  ../../../source/EvolutionRTC/KPCBootManager/Include
  ../../../source/EvolutionRTC/KPCBootUpgrader/Include
  ../../../source/EvolutionRTC/KPCBootManager/Messages/Include
  ../../../source/EvolutionRTC/RTCManager/Include
  ../../../source/EvolutionRTC/CodeUpdater/CLA/Include
  ../../../source/EvolutionRTC/CodeUpdater/Wrapper/Include
)
