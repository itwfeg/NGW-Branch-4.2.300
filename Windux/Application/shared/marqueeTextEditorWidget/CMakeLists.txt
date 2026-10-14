cmake_minimum_required(VERSION 3.13.0)

project(marqueeTextEditorWidget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets)
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})

add_library(marqueeTextEditorWidget STATIC)

target_include_directories(marqueeTextEditorWidget PRIVATE ../marqueeLabelWidget)

target_sources(marqueeTextEditorWidget PRIVATE
  marqueesyntaxhighlighter.cpp
  marqueesyntaxhighlighter.h
  marqueetexteditorwidget.cpp
  marqueetexteditorwidget.h
)