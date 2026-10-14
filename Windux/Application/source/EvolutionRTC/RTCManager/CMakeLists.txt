cmake_minimum_required(VERSION 3.13.0)

project(RTCManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(RTCManager STATIC)

target_compile_definitions(RTCManager PRIVATE LINUX_FIX)

target_link_libraries(RTCManager PUBLIC
  MarqueeManager
  OperatingModes
  PrinterManager
  NumericManager
)

target_sources(RTCManager PRIVATE
  Include/AccessKeyActivation.h
  Include/AccessKeyDeactivation.h
  Include/RealTimeController.h
  Include/SystemCalibrationSwitch.h
  Include/SystemConfiguration.h
  Include/SystemConfigureGCD.h
  Include/SystemDisable.h
  Include/SystemEEPBlockRead.h
  Include/SystemEEPBlockWrite.h
  Include/SystemEnable.h
  Include/SystemEvent.h
  Include/SystemGCDPowerOff.h
  Include/SystemGCDPowerOn.h
  Include/SystemGeneric.h
  Include/System.h
  Include/SystemInquire.h
  Include/SystemMode.h
  Include/SystemRead.h
  Include/SystemRequestInquire.h
  Include/SystemRequestStatus.h
  Include/SystemRequestWakeup.h
  Include/SystemReset.h
  Include/SystemStatus.h
  Include/SystemStatusInfo.h
  Include/SystemWakeup.h
  Include/SystemWrite.h
  Include/UsbKbdAttachChange.h
  Messages/AccessKeyActivation.cpp
  Messages/AccessKeyDeactivation.cpp
  Messages/SystemCalibrationSwitch.cpp
  Messages/SystemConfiguration.cpp
  Messages/SystemConfigureGCD.cpp
  Messages/SystemDisable.cpp
  Messages/SystemEEPBlockRead.cpp
  Messages/SystemEEPBlockWrite.cpp
  Messages/SystemEnable.cpp
  Messages/SystemGCDPowerOff.cpp
  Messages/SystemGCDPowerOn.cpp
  Messages/SystemGeneric.cpp
  Messages/SystemInquire.cpp
  Messages/SystemMode.cpp
  Messages/SystemRead.cpp
  Messages/SystemRequestInquire.cpp
  Messages/SystemRequestStatus.cpp
  Messages/SystemRequestWakeup.cpp
  Messages/SystemReset.cpp
  Messages/SystemStatus.cpp
  Messages/SystemStatusInfo.cpp
  Messages/SystemWakeup.cpp
  Messages/SystemWrite.cpp
  Messages/UsbKbdAttachChange.cpp
  Source/RealTimeController.cpp
)

target_include_directories(RTCManager PUBLIC Include)

target_include_directories(RTCManager PRIVATE
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
  ../../../source/EvolutionRTC/RTCManager/Include
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
  ../../../source/EvolutionRTC/CanManager/Include
  ../../../source/EvolutionRTC/ProductDev/Include
  ../../../source/EvolutionRTC/KPCBootManager/Include
  ../../../source/EvolutionRTC/KPCBootUpgrader/Include
  ../../../source/EvolutionRTC/KPCBootManager/Messages/Include
)
