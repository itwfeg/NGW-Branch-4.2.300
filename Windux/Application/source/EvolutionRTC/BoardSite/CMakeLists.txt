cmake_minimum_required(VERSION 3.13.0)

project(BoardSite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(BoardSite STATIC)

target_sources(BoardSite PRIVATE
  Source/BoardSiteTestManager.cpp
  Messages/BoardSiteGeneric.cpp
  Messages/BoardSitePercentComplete.cpp
  Messages/BoardSiteReqTestMode.cpp
  Messages/BoardSiteTestComplete.cpp
  Messages/BoardSiteTestCompleteUI.cpp
  Messages/BoardSiteTest.cpp
  Messages/BoardSiteTestStatus.cpp
)

target_include_directories(BoardSite PUBLIC Include)

target_include_directories(BoardSite PRIVATE
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