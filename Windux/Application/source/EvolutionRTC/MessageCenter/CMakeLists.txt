cmake_minimum_required(VERSION 3.13.0)

project(MessageCenter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(MessageCenter STATIC)

target_compile_definitions(MessageCenter PRIVATE LINUX_FIX)

target_sources(MessageCenter PRIVATE
  CEvolMsgManager.h
  CEvolMsgReceiver.cpp
  CEvolMsgReceiver.h
  CMessageCenter.cpp
  CMessageCenter.h
  EventDevEvent.h
  Event.h
  EvolMessage.cpp
  EvolMessage.h
  EvolMessageWrapper.cpp
  EvolMessageWrapper.h
  IEvolMsgHandler.h
  MsgReceiverProxy.cpp
  MsgReceiverProxy.h
  ProxyMessage.cpp
  ProxyMessage.h
)

target_include_directories(MessageCenter PUBLIC .)

target_include_directories(MessageCenter PRIVATE
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
  ../../../source/EvolutionRTC/MessageCenter
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

)
