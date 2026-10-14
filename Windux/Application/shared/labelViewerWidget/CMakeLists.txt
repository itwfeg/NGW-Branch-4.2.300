cmake_minimum_required(VERSION 3.13.0)

project(labelViewerWidget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets)
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})


# TODO glob considered bad
file(GLOB SOURCES CONFIGURE_DEPENDS *.cpp *.h)
add_library(labelViewerWidget STATIC ${SOURCES})

# TODO fix relative path seems janky
target_include_directories(labelViewerWidget PRIVATE ../logManager)
target_include_directories(labelViewerWidget PRIVATE ../labelCore)

#target_include_directories(labelViewerWidget PRIVATE ../../../3RDPartySoftware/chilkat/chilkat-9.5.0-armv7l-linux/include)

target_link_libraries(labelViewerWidget logManager labelCore)
# TODO what's up with this
target_compile_options(labelViewerWidget PRIVATE -Wno-multichar)