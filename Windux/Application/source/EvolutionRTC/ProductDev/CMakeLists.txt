cmake_minimum_required(VERSION 3.13.0)

project(ProductDev LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ProductDev STATIC)

target_compile_definitions(ProductDev PRIVATE LINUX_FIX)

target_sources(ProductDev PRIVATE
  Include/ProductChangeMsg.h
  Include/ProductCompleteMsg.h
  Include/ProductFinishedPrintingAllLabelsMsg.h
  Include/ProductLookupMsg.h
  Include/ProductPackageCompleteMsg.h
  Include/ProductReadAllAccumulatedItemsMsg.h
  Include/ProductRemoveAccumItemMsg.h
  Include/ProductStartPrintForTransactionMsg.h
  Include/ProductStatusMsg.h
  Include/ProductUpdateMsg.h
  Messages/ProductChangeMsg.cpp
  Messages/ProductCompleteMsg.cpp
  Messages/ProductFinishedPrintingAllLabelsMsg.cpp
  Messages/ProductLookupMsg.cpp
  Messages/ProductPackageCompleteMsg.cpp
  Messages/ProductReadAllAccumulatedItemsMsg.cpp
  Messages/ProductRemoveAccumItemMsg.cpp
  Messages/ProductStartPrintForTransactionMsg.cpp
  Messages/ProductStatusMsg.cpp
  Messages/ProductUpdateMsg.cpp
)

target_include_directories(ProductDev PUBLIC Include)

target_include_directories(ProductDev PRIVATE
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
  ../../../source/EvolutionRTC/OperatingModes/Include
  ../../../source/EvolutionRTC/BoardSite/Include
)
