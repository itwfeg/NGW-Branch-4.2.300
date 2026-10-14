cmake_minimum_required(VERSION 3.13.0)

project(MarqueeManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(MarqueeManager STATIC)

target_compile_definitions(MarqueeManager PRIVATE LINUX_FIX)

target_sources(MarqueeManager PRIVATE
  Include/MarqueeConfiguration.h
  Include/MarqueeEvent.h
  Include/MarqueeGeneric.h
  Include/Marquee.h
  Include/MarqueeInquire.h
  Include/MarqueeManager.h
  Include/MarqueeMarqueeKind.h
  Include/MarqueeMode.h
  Include/MarqueeOpMode.h
  Include/MarqueePeripheralMessages.h
  Include/MarqueeRequestWakeup.h
  Include/MarqueeStatus.h
  Include/MarqueeSystemStatus.h
  Include/MarqueeTest.h
  Include/MarqueeUpdate.h
  Include/MarqueeWakeup.h
  Messages/MarqueeConfiguration.cpp
  Messages/MarqueeGeneric.cpp
  Messages/MarqueeInquire.cpp
  Messages/MarqueeMarqueeKind.cpp
  Messages/MarqueeMode.cpp
  Messages/MarqueeOpMode.cpp
  Messages/MarqueeRequestWakeup.cpp
  Messages/MarqueeStatus.cpp
  Messages/MarqueeSystemStatus.cpp
  Messages/MarqueeTest.cpp
  Messages/MarqueeUpdate.cpp
  Messages/MarqueeWakeup.cpp
  Source/MarqueeManager.cpp
)

target_include_directories(MarqueeManager PUBLIC
  Include
)

target_include_directories(MarqueeManager PRIVATE
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
  ../../../source/EvolutionRTC/MarqueeManager/Include
  ../../../source/EvolutionRTC/MarqueeManager/Messages/Include
  ../../../source/EvolutionRTC/CodeUpdater/PrinterManager/Include
  ../../../source/EvolutionRTC/CodeUpdater/Wrapper/Include
  ../../../source/EvolutionRTC/MarqueeManager/Include
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
  ../../../source/MetrologicallySignificant
  ../../../source/db/util/filesys
)