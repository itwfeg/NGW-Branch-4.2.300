cmake_minimum_required(VERSION 3.13.0)

project(videoPlayerCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Multimedia MultimediaWidgets)
include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Multimedia_INCLUDE_DIRS}
  ${Qt5MultimediaWidgets_INCLUDE_DIRS}
)

find_package(PkgConfig)
pkg_search_module(DBUS REQUIRED dbus-1)
pkg_check_modules(DBUS REQUIRED dbus-1)

link_directories(${DBUS_LIBRARY_DIRS})

pkg_search_module(GLIB REQUIRED glib-2.0) 
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)
pkg_check_modules(GST_VIDEO REQUIRED gstreamer-video-1.0)

add_library(videoPlayerCore STATIC )

target_include_directories(videoPlayerCore PRIVATE ../logManager ../platform ../gstpdCore)
target_include_directories(videoPlayerCore PRIVATE ${GST_INCLUDE_DIRS})

target_link_libraries(videoPlayerCore PRIVATE ${DBUS_LIBRARIES} ${GST_APP_LIBRARIES} ${GST_VIDEO_LIBRARIES} ${GLIB_LIBRARIES})
target_link_libraries(videoPlayerCore PRIVATE gstpdCore)

target_sources(videoPlayerCore PRIVATE
  seekslider/seekslider.cpp
  seekslider/seekslider.h
  seekslider/seeksliderimpl.h
  videoplayer/external/appconfig.cpp
  videoplayer/external/appconfig.h
  videoplayer/external/externalvideoplayer.cpp
  videoplayer/external/externalvideoplayer.h
  videoplayer/external/gstlaunchappconfig.cpp
  videoplayer/external/gstlaunchappconfig.h
  videoplayer/external/gstpdappconfig.cpp
  videoplayer/external/gstpdappconfig.h
  videoplayer/external/gstpdexternalvideoplayer.cpp
  videoplayer/external/gstpdexternalvideoplayer.h
  videoplayer/external/videoplayerappconfig.cpp
  videoplayer/external/videoplayerappconfig.h
  videoplayer/multimedia/multimediavideoplayerconfig.h
  videoplayer/multimedia/multimediavideoplayer.cpp
  videoplayer/multimedia/multimediavideoplayer.h
  videoplayer/phonon/phononvideoplayerconfig.h
  videoplayer/phonon/phononvideoplayer.cpp
  videoplayer/phonon/phononvideoplayer.h
  videoplayer/videoplayerconfig.h
  videoplayer/videoplayer.cpp
  videoplayer/videoplayer.h
  videoplayerwidget.cpp
  videoplayerwidget.h
  videoplayer/wmf/wmfvideoplayer.cpp
  videoplayer/wmf/wmfvideoplayer.h
  volumeslider/volumeslider.cpp
  volumeslider/volumeslider.h
  volumeslider/volumesliderimpl.h
)