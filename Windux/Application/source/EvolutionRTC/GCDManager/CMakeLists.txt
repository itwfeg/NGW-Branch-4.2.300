cmake_minimum_required(VERSION 3.13.0)

project(GCDManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(GCDManager STATIC)

target_compile_definitions(GCDManager PRIVATE LINUX_FIX)

target_sources(GCDManager PRIVATE
  Include/GCDDevDataMsg.h
  Include/GCDDevFileUploadMsg.h
  Include/GCDDevGetFileInfoMsg.h
  Include/GCDDevHeartbeatReqMsg.h
  Include/GCDDevLanguageAndWeighModeMsg.h
  Include/GCDDevLoadDisplayObjectXMLMsg.h
  Include/GCDDevLoadPlaylistMsg.h
  Include/GCDDevNoParamMsg.h
  Include/GCDDevSetPlaybackWaitTimeMsg.h
  Include/GCDDevSetTimeInfo.h
  Include/GCDDevSetVolumeMsg.h
  Include/GCDDevStringParamMsg.h
  Include/GCDDevTestColorMsg.h
  Include/GCDEvent.h
  Include/GCDFtpUpdateMsg.h
  Include/GCDGCDKind.h
  Include/GCD.h
  Include/GCDStatus.h
  Include/GCDSystemMsg.h
  Include/GCDSystemStatus.h
  Messages/GCDDevDataMsg.cpp
  Messages/GCDDevFileUploadMsg.cpp
  Messages/GCDDevGetFileInfoMsg.cpp
  Messages/GCDDevHeartbeatReqMsg.cpp
  Messages/GCDDevLoadDisplayObjectXMLMsg.cpp
  Messages/GCDDevLoadPlaylistMsg.cpp
  Messages/GCDDevNoParamMsg.cpp
  Messages/GCDDevSetPlaybackWaitTimeMsg.cpp
  Messages/GCDDevSetTimeInfo.cpp
  Messages/GCDDevSetVolumeMsg.cpp
  Messages/GCDDevStringParamMsg.cpp
  Messages/GCDDevTestColorMsg.cpp
  Messages/GCDFtpUpdateMsg.cpp
  Messages/GCDGCDKind.cpp
  Messages/GCDLanguageAndWeighModeMsg.cpp
  Messages/GCDSystemMsg.cpp
  Messages/GCDSystemStatus.cpp
)

target_include_directories(GCDManager PUBLIC
  Include
)

target_include_directories(GCDManager PRIVATE
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
  ../../../source/EvolutionRTC/CodeUpdater/PrinterManager/Include
  ../../../source/EvolutionRTC/LabelApplierManager/Include
  ../../../source/EvolutionRTC/MarqueeManager/Include
  ../../../source/EvolutionRTC/MessageCenter
  ../../../source/EvolutionRTC/NumericManager/Include
  ../../../source/EvolutionRTC/OperatingModes/Include
  ../../../source/EvolutionRTC/PngGraphics/Include
  ../../../source/EvolutionRTC/PrinterManager/Include
  ../../../source/EvolutionRTC/RendererManager/Include
  ../../../source/EvolutionRTC/RTCManager/Include
  ../../../source/EvolutionRTC/ScannerManager/Include
  ../../../source/EvolutionRTC/TimerManager/Include
  ../../../source/EvolutionRTC/USBCommManagers/Include
  ../../../source/EvolutionRTC/USBManager/Include
  ../../../source/EvolutionRTC/WeigherManager/Include
  ../../../source/EvolutionRTC/WrapperManager/Include
  ../../../source/EvolutionRTC/GCDManager/Include
  ../../../source/HFC
  ../../../source/HFC/tags
  ../../../source/HFC/devices
  ../../../source/MetrologicallySignificant
  ../../../source/util
  ../../../source/util/Inc
)