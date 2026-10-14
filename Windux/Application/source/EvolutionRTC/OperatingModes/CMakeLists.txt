cmake_minimum_required(VERSION 3.13.0)

project(OperatingModes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(OperatingModes STATIC)

target_compile_definitions(OperatingModes PRIVATE LINUX_FIX)

target_link_libraries(OperatingModes PUBLIC LabelApplierManager WeigherManager WrapperManager)

target_sources(OperatingModes PRIVATE
  Include/AccumulateItemsMode.h
  Include/AutoWeighEvolWrapperMode.h
  Include/AutoWeighMode.h
  Include/AutoWeighWrapperMode.h
  Include/BuildAMealMode.h
  Include/IdleMode.h
  Include/IlaWeighMode.h
  Include/ManualWeighMode.h
  Include/OperatingMode.h
  Include/OperationModeAccumulateItems.h
  Include/OperationModeAutoManualApply.h
  Include/OperationModeAutoWeigh.h
  Include/OperationModeBuildAMeal.h
  Include/OperationModeGeneric.h
  Include/OperationModeIdle.h
  Include/OperationModeManualWeigh.h
  Include/OperationModeMonitor.h
  Include/OperationModeOperate.h
  Include/OperationModePluEntry.h
  Include/OperationModePowerDown.h
  Include/OperationModeProductionEntry.h
  Include/OperationModeSemiWeigh.h
  Include/OperationModeService.h
  Include/OperationModeStatus.h
  Include/OperationModeSupervisor.h
  Include/OperationModeWrapOnly.h
  Include/OpModeEvent.h
  Include/OpModePackage.h
  Include/ProductionEntryMode.h
  Include/SemiWeighMode.h
  Include/ServiceMode.h
  Messages/OperationModeAccumulateItems.cpp
  Messages/OperationModeAutoManualApply.cpp
  Messages/OperationModeAutoWeigh.cpp
  Messages/OperationModeBuildAMeal.cpp
  Messages/OperationModeGeneric.cpp
  Messages/OperationModeIdle.cpp
  Messages/OperationModeManualWeigh.cpp
  Messages/OperationModeMonitor.cpp
  Messages/OperationModeOperate.cpp
  Messages/OperationModePluEntry.cpp
  Messages/OperationModePowerDown.cpp
  Messages/OperationModeProductionEntry.cpp
  Messages/OperationModeSemiWeigh.cpp
  Messages/OperationModeService.cpp
  Messages/OperationModeStatus.cpp
  Messages/OperationModeSupervisor.cpp
  Messages/OperationModeWrapOnly.cpp
  Source/AccumulateItemsMode.cpp
  Source/AutoWeighEvolWrapperMode.cpp
  Source/AutoWeighMode.cpp
  Source/AutoWeighWrapperMode.cpp
  Source/BuildAMealMode.cpp
  Source/IdleMode.cpp
  Source/IlaWeighMode.cpp
  Source/ManualWeighMode.cpp
  Source/OperatingMode.cpp
  Source/OpModePackage.cpp
  Source/ProductionEntryMode.cpp
  Source/SemiWeighMode.cpp
  Source/ServiceMode.cpp

)

target_include_directories(OperatingModes PUBLIC Include)

target_include_directories(OperatingModes PRIVATE
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
  ../../../source/EvolutionRTC/OperatingModes
  ../../../source/EvolutionRTC/OperatingModes/Include
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
  ../../../source/EvolutionRTC/CanManager/Messages/Include
  ../../../source/EvolutionRTC/NumericManager/Include
  ../../../source/EvolutionRTC/GCDManager/Include
)
