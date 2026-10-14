cmake_minimum_required(VERSION 3.13.0)

project(playlistWidget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Network WebKit Widgets WebKitWidgets)

add_library(playlistWidget STATIC)

target_include_directories(playlistWidget PUBLIC
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${Qt5Network_INCLUDE_DIRS}
  ${Qt5WebKit_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5WebKitWidgets_INCLUDE_DIRS}
)

target_include_directories(playlistWidget PRIVATE
  ../logManager
  ../platform
  ../videoPlayerCore
)

target_sources(playlistWidget PRIVATE
  mediaplaylist.cpp
  mediaplaylistfileparser.cpp
  mediaplaylistfileparser.h
  mediaplaylist.h
  playlistwidget.cpp
  playlistwidget.h
)