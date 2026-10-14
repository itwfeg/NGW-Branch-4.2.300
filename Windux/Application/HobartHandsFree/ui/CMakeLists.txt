cmake_minimum_required(VERSION 3.16.0)

project(hfpdui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Network Widgets REQUIRED)

include(../../bigResources.cmake)
qt5_add_big_resources(hfpdui_RESOURCES hfpdui.qrc)

add_executable(hfpdui ${hfpdui_RESOURCES})

target_include_directories(hfpdui PRIVATE ../shared)
target_include_directories(hfpdui PUBLIC
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Network_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
)

target_link_libraries(hfpdui
  Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets
  hfpdshared
)

target_sources(hfpdui PRIVATE
  colors.h
  commclient.cpp
  commclient.h
  configeditorwidget.cpp
  configeditorwidget.h
  configuration.cpp
  configuration.h
  firmwareupgradedialog.cpp
  firmwareupgradedialog.h
  inputwidget.cpp
  inputwidget.h
  inputwidgethook.cpp
  inputwidgethook.h
  keyboardinputwidget.cpp
  keyboardinputwidget.h
  main.cpp
  mainwindow.cpp
  mainwindow.h
  sensorselection.cpp
  sensorselection.h
  touchscreenstyle.cpp
  touchscreenstyle.h
)

