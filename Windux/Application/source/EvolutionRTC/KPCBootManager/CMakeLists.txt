cmake_minimum_required(VERSION 3.13.0)

project(KPCBootManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(KPCBootManager STATIC)

target_compile_definitions(KPCBootManager PRIVATE LINUX_FIX)

target_sources(KPCBootManager PRIVATE
  Include/KpcBootEvent.h
  Include/KPCBootManager.h
  Messages/Include/KPCBootAck.h
  Messages/Include/KPCBootAppVersion.h
  Messages/Include/KPCBootBulkMsg.h
  Messages/Include/KPCBootCmd.h
  Messages/Include/KPCBootFlush.h
  Messages/Include/KPCBootFrame.h
  Messages/Include/KPCBootFrameHeader.h
  Messages/Include/KPCBootGeneric.h
  Messages/Include/KPCBootReady.h
  Messages/Include/KPCBootStatus.h
  Messages/Include/KPCBootVersion.h
  Messages/KPCBootAck.cpp
  Messages/KPCBootAppVersion.cpp
  Messages/KPCBootBulkMsg.cpp
  Messages/KPCBootCmd.cpp
  Messages/KPCBootFlush.cpp
  Messages/KPCBootFrame.cpp
  Messages/KPCBootFrameHeader.cpp
  Messages/KPCBootGeneric.cpp
  Messages/KPCBootReady.cpp
  Messages/KPCBootStatus.cpp
  Messages/KPCBootVersion.cpp
  Source/KPCBootManager.cpp
)

target_include_directories(KPCBootManager PUBLIC
  Include
  Messages/Include
)

target_include_directories(KPCBootManager PRIVATE
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
  ../../../source/db/util/thread
  ../../../source/EvolutionRTC/CanManager/Include
  ../../../source/EvolutionRTC/CanManager/Messages/Include
  ../../../source/EvolutionRTC/KPCBootManager/Include
  ../../../source/EvolutionRTC/KPCBootManager/Messages/Include
  ../../../source/EvolutionRTC/KPCBootUpgrader/Include
  ../../../source/EvolutionRTC/KPCBootUpgrader/Messages/Include
  ../../../source/EvolutionRTC/CodeUpdater/PrinterManager/Include
  ../../../source/EvolutionRTC/CodeUpdater/Wrapper/Include
  ../../../source/EvolutionRTC/LabelApplierManager/Include
  ../../../source/EvolutionRTC/MarqueeManager/Include
  ../../../source/EvolutionRTC/MessageCenter
  ../../../source/EvolutionRTC/NumericManager/Include
  ../../../source/EvolutionRTC/OperatingModes/Include
  ../../../source/EvolutionRTC/PngGraphics/Include
  ../../../source/EvolutionRTC/PrinterManager/Include
  ../../../source/EvolutionRTC/ProductDev/Include
  ../../../source/EvolutionRTC/RendererManager/Include
  ../../../source/EvolutionRTC/RFIDManager/Include
  ../../../source/EvolutionRTC/RTCManager/Include
  ../../../source/EvolutionRTC/TimerManager/Include
  ../../../source/EvolutionRTC/USBCommManagers/Include
  ../../../source/EvolutionRTC/USBManager/Include
  ../../../source/EvolutionRTC/WeigherManager/Include
  ../../../source/EvolutionRTC/WrapperManager/Include
  ../../../source/EvolutionRTC/BoardSite/Include
  ../../../source/HFC
  ../../../source/HFC/tags
  ../../../source/HFC/devices
  ../../../source/util
  ../../../source/util/Inc
  ../../../../Smbus/driver
)