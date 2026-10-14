cmake_minimum_required(VERSION 3.13.0)

project(CanManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(CanManager STATIC)

target_link_libraries(CanManager PUBLIC BoardSite)

target_sources(CanManager PRIVATE
  Include/CanLibrary.h
  Include/CanManager.h
  Include/CanTranslator.h
  Include/deviceProductIDs.h
  Include/GpioDeviceReset.h
  Include/NGWUpdateManager.h
  Include/Sam7xCANUpdater.h
  Include/SystemStartManager.h
  Include/UnitTest.h
  Messages/Include/KpcSystemErrors.h
  Messages/Include/KpcSystemEvents.h
  Messages/Include/NGWStartUpgrade.h
  Messages/Include/NGWUpgradeFinished.h
  Messages/Include/NGWUpgradeInfo.h
  Messages/Include/NGWUpgradeProgress.h
  Messages/Include/NGWUpgradeRequired.h
  Messages/Include/NGWUpgradeStatus.h
  Messages/Include/NGWUpgradeUIReady.h
  Messages/Include/Sam7xUpdaterMsg.h
  Messages/KpcSystemErrors.cpp
  Messages/NGWStartUpgrade.cpp
  Messages/NGWUpgradeFinished.cpp
  Messages/NGWUpgradeInfo.cpp
  Messages/NGWUpgradeProgress.cpp
  Messages/NGWUpgradeRequired.cpp
  Messages/NGWUpgradeStatus.cpp
  Messages/NGWUpgradeUIReady.cpp
  Messages/Sam7xUpdaterMsg.cpp
  Source/CanLibrary.cpp
  Source/CanManager.cpp
  Source/CanTranslator.cpp
  Source/GpioDeviceReset.cpp
  Source/NGWUpdateManager.cpp
  Source/Sam7xCANUpdater.cpp
  Source/SystemStartManager.cpp
  Source/UnitTest.cpp
)

target_include_directories(CanManager PUBLIC Include Messages/Include)

target_include_directories(CanManager PRIVATE
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
  ../../../source/EvolutionRTC/BoardSite/Include
  ../../../source/EvolutionRTC/CanManager/Include
  ../../../source/EvolutionRTC/CanManager/Messages/Include
  ../../../source/EvolutionRTC/KPCBootManager/Include
  ../../../source/EvolutionRTC/KPCBootManager/Messages/Include
  ../../../source/EvolutionRTC/KPCBootUpgrader/Include
  ../../../source/EvolutionRTC/KPCBootUpgrader/Messages/Include
  ../../../source/EvolutionRTC/CodeUpdater/PrinterManager/Include
  ../../../source/EvolutionRTC/CodeUpdater/Wrapper/Include
  ../../../source/EvolutionRTC/LabelApplierManager/Include
  ../../../source/EvolutionRTC/GCDManager/Include
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
  ../../../source/MetrologicallySignificant
  ../../../source/util
  ../../../source/util/Inc
  ../../../../Smbus/driver
)
