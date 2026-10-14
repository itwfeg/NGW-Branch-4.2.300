cmake_minimum_required(VERSION 3.13.0)

project(USBManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(USBManager STATIC)

target_compile_definitions(USBManager PRIVATE LINUX_FIX)

target_link_libraries(USBManager PUBLIC CodeUpdater)

target_sources(USBManager PRIVATE
  Source/UsbManager.cpp
  Source/HobartUsbUpdater.cpp
  Source/IntelHexParser.cpp
  Source/IPeripheralUpdater.cpp
  Source/IUsbCodeUpdater.cpp
  Source/UsbCodeUpdater.cpp
  Source/EvolPrinterCodeUpdater.cpp
  Source/EvolWeigherCodeUpdater.cpp
  Source/UsbNode.cpp
  Source/AtmelSambaUpdater.cpp
  Source/HIDDeviceEvent.cpp
  Source/linux/usb_linux_node.cpp
  Source/linux/CypressEzUsbUpdaterLinux.cpp
  Source/linux/AtmelDeviceHandler.cpp
  Source/linux/UsbManagerLinux.cpp
  Source/linux/CUsbPipe.cpp
  Source/linux/CUsbCommBase.cpp
)

target_include_directories(USBManager PUBLIC Include)

target_include_directories(USBManager PRIVATE
  Include/linux
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
  ../../../source/EvolutionRTC/CodeUpdater/AtmelAT91
  ../../../source/EvolutionRTC/CodeUpdater/PrinterManager/Include
  ../../../source/EvolutionRTC/USBCommManagers/Include
)
