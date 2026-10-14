cmake_minimum_required(VERSION 3.13.0)

project(Session LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Network Gui Widgets Multimedia MultimediaWidgets)

add_library(Session STATIC)

target_include_directories(Session PUBLIC
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Network_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Multimedia_INCLUDE_DIRS}
  ${Qt5MultimediaWidgets_INCLUDE_DIRS}
)

target_include_directories(Session PRIVATE ../logManager ../labelCore ../platform .)

target_compile_options(Session PRIVATE -Wno-multichar)

target_sources(Session PRIVATE
  Devices.h
  Events.h
  HobSocket.cpp
  HobSocket.h
  MessageParser.cpp
  MessageParser.h
  Messages.h
  ScaleClasses.cpp
  ScaleClasses.h
  ScaleConfiguration.cpp
  ScaleConfiguration.h
  Session.cpp
  Session.h
  SessionMessageMap.h
  TokenParser.cpp
  TokenParser.h
  UiUtilities.cpp
  UiUtilities.h
)