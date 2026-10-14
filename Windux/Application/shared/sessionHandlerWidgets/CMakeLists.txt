cmake_minimum_required(VERSION 3.13.0)

project(sessionHandlerWidgets LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Network)
include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})

find_package(PkgConfig)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

add_library(sessionHandlerWidgets STATIC)

target_include_directories(sessionHandlerWidgets PRIVATE
  ../sessionHandlerUtil
  ../Session
  ../logManager
  ../platform
  ../labelCore
)

target_include_directories(sessionHandlerWidgets PRIVATE ${GST_INCLUDE_DIRS})

target_compile_options(sessionHandlerWidgets PRIVATE -Wno-multichar)

target_sources(sessionHandlerWidgets PRIVATE
  abstractbutton.h
  backgroundwidget.h
  backgroundwidgetrenderer.cpp
  backgroundwidgetrenderer.h
  backgroundwidgetstyles.h
  backgroundwidgettypes.h
  capacitybarwidgets.cpp
  capacitybarwidgets.h
  filedownloader.cpp
  filedownloader.h
  icons.cpp
  icons.h
  numericlineedit.cpp
  numericlineedit.h
  pushbutton.cpp
  pushbutton.h
  qrc_sessionhandlerwidgets.cpp
  refusebin.cpp
  refusebin.h
  sellbysoftkeybutton.cpp
  sellbysoftkeybutton.h
  softkeybutton.cpp
  softkeybutton.h
  softkeybuttonlist.cpp
  softkeybuttonlist.h
  softkeyoverlaymanager.cpp
  softkeyoverlaymanager.h
  softkeytogglemanager.cpp
  softkeytogglemanager.h
  softkeyswidget.cpp
  softkeyswidget.h
  tabbableiplineedit.cpp
  tabbableiplineedit.h
  toolbutton.cpp
  toolbutton.h
)
