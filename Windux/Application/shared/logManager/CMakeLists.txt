cmake_minimum_required(VERSION 3.13.0)

project(logManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets)

# glob considered bad
#file(GLOB SOURCES CONFIGURE_DEPENDS *.cpp *.h)
#message("logManager sources " ${SOURCES})

add_library(logManager STATIC)

target_include_directories(logManager PRIVATE .)
target_include_directories(logManager PUBLIC ${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

target_sources(logManager PRIVATE
  consolelog.cpp
  consolelog.h
  filelog.cpp
  filelog.h
  logappender.cpp
  logappender.h
  log.cpp
  log.h
  logline.cpp
  logline.h
  logmanager.cpp
  logmanager.h
  qtlog.cpp
  qtlog.h
)