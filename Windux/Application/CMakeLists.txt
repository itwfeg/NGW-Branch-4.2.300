cmake_minimum_required(VERSION 3.16.0)

project(windux LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

################################################################################

# scaleui

find_package(Qt5 COMPONENTS Core REQUIRED)

# resources can't be added to static libraries, so add them to scaleui binary
include(bigResources.cmake)
qt5_add_big_resources(RESOURCES_ADDED
  shared/sessionHandler/sessionhandler.qrc
  shared/sessionHandlerWidgets/sessionhandlerwidgets.qrc
  shared/marqueeTextEditorWidget/marqueetexteditorwidget.qrc
)

add_executable(scaleui operatorInterface/scaleui/main.cpp ${RESOURCES_ADDED})

target_include_directories(scaleui PRIVATE
  inc/linux
  shared/platform shared/logManager
  shared/sessionHandlerHooks
  shared/sessionHandler
  shared/Session
)

add_subdirectory(shared/logManager)
add_subdirectory(shared/comm)
add_subdirectory(shared/gstpdCore)
add_subdirectory(shared/platform)
add_subdirectory(shared/labelCore)
add_subdirectory(shared/labelViewerWidget)
add_subdirectory(shared/marqueeLabelWidget)
add_subdirectory(shared/marqueeTextEditorWidget)
add_subdirectory(shared/playlistWidget)
add_subdirectory(shared/videoPlayerCore)
add_subdirectory(shared/sessionHandlerUtil)
add_subdirectory(shared/sessionHandlerWidgets)
add_subdirectory(shared/sessionHandlerHooks)
add_subdirectory(shared/sessionHandler)
add_subdirectory(shared/Session)
add_subdirectory(XpdfWidgetQt)

find_package(Qt5 COMPONENTS Gui PrintSupport Widgets Sql Network Script WebKit WebKitWidgets Core Multimedia MultimediaWidgets DBus Quick REQUIRED)
find_package(PkgConfig)
pkg_check_modules(FREETYPE REQUIRED freetype2)
pkg_search_module(GLIB REQUIRED glib-2.0)

target_link_libraries(scaleui
  X11 Xext dbus-1 ${FREETYPE_LIBRARIES} ${GLIB_LIBRARIES}
  XpdfWidgetQt
  Qt5::Core Qt5::Gui Qt5::PrintSupport Qt5::Widgets Qt5::Sql Qt5::Network Qt5::Script Qt5::WebKit Qt5::WebKitWidgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::DBus Qt5::Quick
  logManager
  labelCore
  comm
  platform
  labelViewerWidget
  marqueeLabelWidget
  marqueeTextEditorWidget
  playlistWidget
  videoPlayerCore
  sessionHandlerUtil
  sessionHandlerWidgets
  sessionHandlerHooks
  sessionHandler
  Session
)

################################################################################

# chobart_d

add_executable(chobart_d)

target_compile_definitions(chobart_d PRIVATE LINUX_FIX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_compile_options(chobart_d PUBLIC -frtti)

target_include_directories(chobart_d PRIVATE
  inc/linux
  source/EvolutionRTC/MessageCenter
  source/EvolutionRTC/RendererManager/Include
  source/EvolutionRTC/WrapperManager/Include
  source/EvolutionRTC/WeigherManager/Include
  source/EvolutionRTC/RTCManager/Include
  source/EvolutionRTC/OperatingModes/Include
  source/EvolutionRTC/PrinterManager/Include
  source/EvolutionRTC/TimerManager/Include
  source/core/VersionManager/include
  source/util/Inc
  source/comm
  source/util
  source/HFC
  source/db
  source/db/util
  source/db/util/filesys
  source/db/util/thread
  source/core/Synchronization
  source/MetrologicallySignificant
  source/HFC/devices
  source/EvolutionRTC/GCDManager/Include
  source/EvolutionRTC/LabelApplierManager/Include
  source/EvolutionRTC/NumericManager/Include
  source/EvolutionRTC/MarqueeManager/Include
  source/EvolutionRTC/HandsFreeDevice/Include
  source/EvolutionRTC/USBManager/Include
  source/EvolutionRTC/CanManager/Messages/Include
  source/EvolutionRTC/KPCBootManager/Include
  source/EvolutionRTC/KPCBootManager/Messages/Include
  source/EvolutionRTC/KPCBootUpgrader/Include
  source/EvolutionRTC/KPCBootUpgrader/Messages/Include
  source/HFC/reports
  source/HFC/messages
  source/comm/email
  source/EvolutionRTC/USBCommManagers/Include
  source/core
  source/EvolutionRTC/RFIDManager/Include
  source/EvolutionRTC/ProductDev/Include
  source/EvolutionRTC/DiagnosticsManager/Messages
  source/EvolutionRTC/CanManager/Include
  source/EvolutionRTC/BoardSite/Include
  HobartHandsFree/shared
  HobartCustomerHook
  source/EvolutionRTC/ScannerManager/Include
  source/EvolutionRTC/DiagnosticsManager
  source/HFC/tags
  PAL
)

add_subdirectory(HobartHandsFree)
add_subdirectory(PAL)
add_subdirectory(source)
add_subdirectory(HobartCustomerHook)

target_link_directories(chobart_d PRIVATE ../3RDPartySoftware/digimarc/lib)

find_package(PkgConfig)
pkg_check_modules(FREETYPE REQUIRED freetype2)
pkg_check_modules(ZLIB REQUIRED zlib)

# N.B. very touchy about the linking order
target_link_libraries(chobart_d
  db
  util
  metro
  core
  PAL
  dlsdk
  hfpdshared
  HFC
  backend_comm
  HobartCustomerHook
  BoardSite
  CanManager
  DiagnosticsManager
  GCDManager
  KPCBootManager
  KPCBootUpgrader
  LabelApplierManager
  MarqueeManager
  MessageCenter
  NumericManager
  OperatingModes
  PngGraphics
  PrinterManager
  ProductDev
  RendererManager
  RFIDManager
  RTCManager
  ScannerManager
  TimerManager
  USBCommManagers
  USBManager
  WeigherManager
  WrapperManager
  CodeUpdater

  pthread
  udev
  usb
  usb-1.0
  sqlite3
  ${FREETYPE_LIBRARIES}
  ${ZLIB_LIBRARIES}
  curl
)
