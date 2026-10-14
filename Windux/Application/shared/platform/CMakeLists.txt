cmake_minimum_required(VERSION 3.13.0)

project(platform LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets)

add_library(platform STATIC)

include_directories(platform PUBLIC ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

target_include_directories(platform PRIVATE ../logManager)
target_include_directories(platform PUBLIC ${CMAKE_SOURCE_DIR}/../3RDPartySoftware/chilkat/chilkat-9.5.0-armv7l-linux/include/)

target_link_libraries(platform PUBLIC ${CMAKE_SOURCE_DIR}/../3RDPartySoftware/chilkat/chilkat-9.5.0-armv7l-linux/lib/libchilkat-9.5.0.a)

target_sources(platform PRIVATE
  appinit.cpp
  appinit.h
  fileaccess.cpp
  fileaccess.h
  filesystemconfig.h
  filesystem.cpp
  filesystem.h
  folderconfig.cpp
  folderconfig.h
  foldertype.cpp
  foldertype.h
  network.cpp
  network.h
  platform.h
  process.cpp
  process.h
  python.cpp
  python.h
  remoteconnection.h
  remoteconnectionimpl.h
  remotefileaccess.cpp
  remotefileaccess.h
  sighandler.cpp
  sighandler.h
)