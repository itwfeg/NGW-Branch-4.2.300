cmake_minimum_required(VERSION 3.13.0)

project(metro LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(metro STATIC)

target_sources(metro PRIVATE
  HfcWeigherConfig.cpp
  HfcWeigherConfig.h
  HfcWeigherStatus.cpp
  HfcWeigherStatus.h
  MetSig.cpp
  MetSig.h
  Rounding.cpp
  Rounding.h
  WeigherCalibration.cpp
  WeigherCalibrationEnd.cpp
  WeigherCalibrationEnd.h
  WeigherCalibration.h
  WeigherCalibrationStep.cpp
  WeigherCalibrationStep.h
  WeigherConfiguration.cpp
  WeigherConfiguration.h
  WeigherStatus.cpp
  WeigherStatus.h
)

target_include_directories(metro PRIVATE
  ../../PAL
  ../../PAL/PAL_linux
  ../../inc/linux
  ../../source/app
  ../../source/comm
  ../../source/core/Synchronization
  ../../source/core/VersionManager/include
  ../../source/db
  ../../source/db/util
  ../../source/db/util/filesys
  ../../source/db/util/thread
  ../../source/EvolutionRTC/MessageCenter
  ../../source/EvolutionRTC/OperatingModes/Include
  ../../source/EvolutionRTC/PrinterManager/Include
  ../../source/EvolutionRTC/RendererManager/Include
  ../../source/EvolutionRTC/RTCManager/Include
  ../../source/EvolutionRTC/TimerManager/Include
  ../../source/EvolutionRTC/WeigherManager/Include
  ../../source/EvolutionRTC/WrapperManager/Include
  ../../source/HFC
  ../../source/HFC/devices
  ../../source/HFC/tags
  ../../source/MetrologicallySignificant
  ../../source/util
  ../../source/util/Inc
  ../../../Smbus/driver
)
