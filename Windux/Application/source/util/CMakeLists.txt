cmake_minimum_required(VERSION 3.13.0)

project(util LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(util STATIC)

target_compile_definitions(util PUBLIC LINUX_FIX)

find_package(PkgConfig)
pkg_check_modules(FREETYPE REQUIRED freetype2)

target_include_directories(util PRIVATE "${FREETYPE_INCLUDE_DIRS}")

target_sources(util PRIVATE
  Bool.h
  CHobartTimer.cpp
  CHobartTimer.h
  crypt.h
  CZip.cpp
  CZip.h
  Debug.cpp
  Debug.h
  Double.h
  Hex.cpp
  Hex.h
  HobartMath.cpp
  HobartMath.h
  Integer.h
  ioapi.h
  ITimerHandler.h
  Long.h
  MemShow.cpp
  MemShow.h
  MessageLogger.cpp
  MessageLogger.h
  Power.h
  PythonScripter.cpp
  PythonScripter.h
  ServiceSpeaker.cpp
  ServiceSpeaker.h
  SimpleTimer.cpp
  SimpleTimer.h
  TimerContainer.cpp
  TimerContainer.h
  Trace.cpp
  Trace.h
  UInteger.h
  ULong.h
  unzip.h
  zip.h
  unzip.c
  minizip.c
  zip.c
  ioapi.c
  miniunz.c
  Inc/BaseNT.h
  Inc/CCriticalSection.h
  Inc/CFile.h
  Inc/CKnownLengthByteStream.h
  Inc/CMSGByteStream.h
  Inc/CMSGIntByteStream_Util.h
  Inc/CMSGSafeStream.h
  Inc/CMSGStrByteStream.h
  Inc/CMSGStream.h
  Inc/CMSGStream_impl.h
  Inc/CRawByteStream.h
  Inc/CRawMemoryStream.h
  Inc/CReadWriteObject.h
  Inc/CRelativeTimer.h
  Inc/CSemStream.h
  Inc/CSmartFindFileHandle.h
  Inc/CSmartFontHandle.h
  Inc/CSmartPtr.h
  Inc/CTLSThread.h
  Inc/CVersionInfo.h
  Inc/DADFResourceUtils.h
  Inc/ErrorHandling.h
  Inc/GenericHandle.h
  Inc/GSDS_Volume_F.h
  Inc/GSDS_Volume_F_Impl.h
  Inc/MSGEvent.h
  Inc/MSGMutex.h
  Inc/MSGSemaphore.h
  Inc/MsgTime.h
  Inc/MSGTime_impl.h
  Inc/MSGTimeUtil.h
  Inc/OutputLogger.h
  Inc/SmartHandle.h
  Inc/Statistic.h
  Inc/TypeName.h
  Inc/utilunicode.h
  Src/Cfile.cpp
  Src/CSmartFontHandle.cpp
  Src/CTLSThread.cpp
  Src/CVersionInfo.cpp
  Src/DADFResourceUtils.cpp
  Src/ErrorHandling.cpp
  Src/OutputLogger.cpp
  Src/utilunicode.cpp
)

target_include_directories(util PRIVATE
  ../../PAL
  ../../PAL/PAL_linux
  ../../inc/linux
  ../../source/app
  ../../source/comm
  ../../source/core
  ../../source/core/Synchronization
  ../../source/core/VersionManager/include
  ../../source/db
  ../../source/db/util
  ../../source/db/util/e_string
  ../../source/db/util/filesys
  ../../source/db/util/thread
  ../../source/EvolutionRTC/CodeUpdater/PrinterManager/Include
  ../../source/EvolutionRTC/LabelApplierManager/Include
  ../../source/EvolutionRTC/MarqueeManager/Include
  ../../source/EvolutionRTC/MessageCenter
  ../../source/EvolutionRTC/NumericManager/Include
  ../../source/EvolutionRTC/OperatingModes/Include
  ../../source/EvolutionRTC/PrinterManager/Include
  ../../source/EvolutionRTC/ProductDev/Include
  ../../source/EvolutionRTC/RendererManager/Include
  ../../source/EvolutionRTC/RFIDManager/Include
  ../../source/EvolutionRTC/RTCManager/Include
  ../../source/EvolutionRTC/ScannerManager/Include
  ../../source/EvolutionRTC/TimerManager/Include
  ../../source/EvolutionRTC/USBCommManagers/Include
  ../../source/EvolutionRTC/USBManager/Include
  ../../source/EvolutionRTC/WeigherManager/Include
  ../../source/EvolutionRTC/WrapperManager/Include
  ../../source/HFC
  ../../source/HFC/tags
  ../../source/HFC/devices
  ../../source/util
  ../../source/util/Inc
  ../../source/MetrologicallySignificant
)
