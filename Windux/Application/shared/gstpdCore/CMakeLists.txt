cmake_minimum_required(VERSION 3.13.0)

project(gstpdCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Network DBus)

find_package(PkgConfig REQUIRED)
pkg_search_module(DBUS REQUIRED dbus-1)
pkg_check_modules(DBUS REQUIRED dbus-1)
pkg_check_modules(GLIB REQUIRED glib-2.0) 
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GTK REQUIRED gtk+-2.0)
pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)
pkg_check_modules(GST_VIDEO REQUIRED gstreamer-video-1.0)

include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Network_INCLUDE_DIRS}
  ${Qt5DBus_INCLUDE_DIRS}
  ${GST_INCLUDE_DIRS}
  ${GLIB_INCLUDE_DIRS}
)

add_library(gstpdCore STATIC)

target_sources(gstpdCore PRIVATE
  abstractmediaplayer.cpp
  abstractmediaplayer.h
  custgstmp.cpp
  custgstmp.h
  dbusmediaplayerclient.cpp
  dbusmediaplayerclient.h
  dbusmediaplayer.cpp
  dbusmediaplayer.h
  gstmp.cpp
  gstmp.h
  opergstmp.cpp
  opergstmp.h
)

target_include_directories(gstpdCore PRIVATE ../logManager)

target_link_libraries(gstpdCore logManager ${GST_LIBRARIES} ${GLIB_LIBRARIES} ${GST_APP_LIBRARIES} ${GST_VIDEO_LIBRARIES} ${DBUS_LIBRARIES})
