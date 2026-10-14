cmake_minimum_required(VERSION 3.13.0)

project(backend_comm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(backend_comm STATIC)

target_compile_definitions(backend_comm PRIVATE LINUX_FIX)

target_sources(backend_comm PRIVATE
  CommSession.cpp
  CMFile.cpp
  CMReaderWriter.cpp
  CMReaderWriterEvolUI.cpp
  CMReaderWriterEvolTag.cpp
  CMReaderWriterHandsFreeDevice.cpp
  CMReaderWriterSyncTag.cpp
  CMReaderWriterUltimaDisk.cpp
  CMReaderWriterDBIndependent.cpp
  CMReaderWriterGCD.cpp
  InitializerComm.cpp
  Messenger.cpp
  NetworkConnectivityManager.cpp
  NetworkStatus.cpp
  Ping.cpp
  ServerSocket.cpp
  SessionMessage.cpp
  Socket.cpp
)

target_sources(backend_comm PRIVATE
  cgi/CGICallManager.cpp
  cgi/CGICallPerformedMessage.cpp
  cgi/CGIHTTPAdapter.cpp
  cgi/PerformCGICallMessage.cpp
)

target_sources(backend_comm PRIVATE
  cgi/http/HTTPURI.cpp
  cgi/http/HTTPClient.cpp
  cgi/http/HTTPClientError.cpp
  cgi/http/HTTPRequest.cpp
  cgi/http/HTTPResponse.cpp
  cgi/http/HTTPUnitTester.cpp
)

target_sources(backend_comm PRIVATE
  email/Email.cpp
  email/EmailManager.cpp
  email/SendErrorEmailEvent.cpp
  email/SendGcdFtpErrorEmailEvent.cpp
  email/SendGcdPlaylistErrorEmailEvent.cpp
)

target_sources(backend_comm PRIVATE
  email/SMTP/SMTPClient.cpp
  email/SMTP/SMTPClientInterface.cpp
)

target_include_directories(backend_comm PRIVATE
  .
  cgi
  cgi/http
  email
  email/SMTP
  ../../PAL
  ../../PAL/PAL_linux
  ../../inc/linux
  ../../source/app
  ../../source/db
  ../../source/db/util
  ../../source/db/util/filesys
  ../../source/db/util/thread
  ../../source/core/Synchronization
  ../../source/core
  ../../source/core/VersionManager/include
  ../../source/comm
  ../../HobartHandsFree/shared
  ../../source/EvolutionRTC/HandsFreeDevice/Include
  ../../source/EvolutionRTC/PrinterManager/Include
  ../../source/EvolutionRTC/MessageCenter
  ../../source/EvolutionRTC/RTCManager/Include
  ../../source/EvolutionRTC/OperatingModes/Include
  ../../source/EvolutionRTC/RendererManager/Include
  ../../source/EvolutionRTC/ScannerManager/Include
  ../../source/EvolutionRTC/WrapperManager/Include
  ../../source/EvolutionRTC/WeigherManager/Include
  ../../source/EvolutionRTC/PngGraphics/Include
  ../../source/EvolutionRTC/TimerManager/Include
  ../../source/EvolutionRTC/GCDManager/Include
  ../../source/util
  ../../source/util/Inc
  ../../source/HFC
  ../../source/HFC/messages
  ../../source/HFC/tags
  ../../source/MetrologicallySignificant
)