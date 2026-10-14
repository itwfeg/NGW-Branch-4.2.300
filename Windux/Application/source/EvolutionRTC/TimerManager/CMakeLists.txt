cmake_minimum_required(VERSION 3.13.0)

project(TimerManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(TimerManager STATIC)

target_compile_definitions(TimerManager PRIVATE LINUX_FIX)

target_sources(TimerManager PRIVATE
  Include/Timeout.h
  Include/TimerEvent.h
  Include/Timer.h
  Include/TimerManager.h
  Messages/Timeout.cpp
  Messages/Timer.cpp
  Source/TimerManager.cpp
)

target_include_directories(TimerManager PUBLIC Include)

target_include_directories(TimerManager PRIVATE
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
  ../../../source/EvolutionRTC/TimerManager/Include
  ../../../source/EvolutionRTC/TimerManager/Include
  ../../../source/EvolutionRTC/TimerManager/Include
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
  ../../../source/EvolutionRTC/CanManager/Include
  ../../../source/EvolutionRTC/ProductDev/Include
  ../../../source/EvolutionRTC/KPCBootManager/Include
  ../../../source/EvolutionRTC/KPCBootUpgrader/Include
  ../../../source/EvolutionRTC/KPCBootManager/Messages/Include
  ../../../source/EvolutionRTC/RTCManager/Include
)
