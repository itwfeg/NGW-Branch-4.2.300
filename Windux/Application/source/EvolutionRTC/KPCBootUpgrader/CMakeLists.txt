cmake_minimum_required(VERSION 3.13.0)

project(KPCBootUpgrader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(KPCBootUpgrader STATIC)

target_compile_definitions(KPCBootUpgrader PRIVATE LINUX_FIX)

target_sources(KPCBootUpgrader PRIVATE
  Include/KpcBootUpgraderEvent.h
  Include/KPCBootUpgrader.h
  Messages/Include/KPCBootUpgraderAck.h
  Messages/Include/KPCBootUpgraderBulkMsg.h
  Messages/Include/KPCBootUpgraderCmd.h
  Messages/Include/KPCBootUpgraderFlush.h
  Messages/Include/KPCBootUpgraderFrame.h
  Messages/Include/KPCBootUpgraderFrameHeader.h
  Messages/Include/KPCBootUpgraderGeneric.h
  Messages/Include/KPCBootUpgraderStatus.h
  Messages/Include/KPCUpgradeInfo.h
  Messages/Include/KPCUpgradeProgress.h
  Messages/KPCBootUpgraderAck.cpp
  Messages/KPCBootUpgraderBulkMsg.cpp
  Messages/KPCBootUpgraderCmd.cpp
  Messages/KPCBootUpgraderFlush.cpp
  Messages/KPCBootUpgraderFrame.cpp
  Messages/KPCBootUpgraderFrameHeader.cpp
  Messages/KPCBootUpgraderGeneric.cpp
  Messages/KPCBootUpgraderStatus.cpp
  Messages/KPCUpgradeInfo.cpp
  Messages/KPCUpgradeProgress.cpp
  Source/KPCBootUpgrader.cpp
)

target_include_directories(KPCBootUpgrader PUBLIC
  Include
  Messages/Include
)

target_include_directories(KPCBootUpgrader PRIVATE
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
  ../../../source/HFC
  ../../../source/HFC/tags
  ../../../source/HFC/devices
  ../../../source/util
  ../../../source/util/Inc
  ../../../../Smbus/driver
  ../../../source/EvolutionRTC/BoardSite/Include
  ../../../source/EvolutionRTC/GCDManager/Include
)