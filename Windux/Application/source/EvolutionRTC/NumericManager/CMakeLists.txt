cmake_minimum_required(VERSION 3.13.0)

project(NumericManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(NumericManager STATIC)

target_compile_definitions(NumericManager PRIVATE LINUX_FIX)

target_sources(NumericManager PRIVATE
  Include/NumericAnnunciator.h
  Include/NumericConfiguration.h
  Include/NumericEvent.h
  Include/NumericField.h
  Include/NumericGeneric.h
  Include/Numeric.h
  Include/NumericInquire.h
  Include/NumericManager.h
  Include/NumericMode.h
  Include/NumericNumericKind.h
  Include/NumericOpMode.h
  Include/NumericPeripheralMessages.h
  Include/NumericRequestWakeup.h
  Include/NumericStatus.h
  Include/NumericSystemStatus.h
  Include/NumericTest.h
  Include/NumericVirtualImage.h
  Include/NumericWakeup.h
  Include/VacuumFlourescentVirtualImage.h
  Messages/NumericAnnunciator.cpp
  Messages/NumericConfiguration.cpp
  Messages/NumericField.cpp
  Messages/NumericGeneric.cpp
  Messages/NumericInquire.cpp
  Messages/NumericMode.cpp
  Messages/NumericNumericKind.cpp
  Messages/NumericOpMode.cpp
  Messages/NumericRequestWakeup.cpp
  Messages/NumericStatus.cpp
  Messages/NumericSystemStatus.cpp
  Messages/NumericTest.cpp
  Messages/NumericWakeup.cpp
  Source/NumericManager.cpp
  Source/NumericVirtualImage.cpp
  Source/VacummFlourescentVirtualImage.cpp
)

target_include_directories(NumericManager PUBLIC
  Include
)

target_include_directories(NumericManager PRIVATE
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
  ../../../source/EvolutionRTC/NumericManager
  ../../../source/EvolutionRTC/NumericManager/Include
  ../../../source/EvolutionRTC/OperatingModes/Include
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
)