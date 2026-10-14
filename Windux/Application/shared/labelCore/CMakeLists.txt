cmake_minimum_required(VERSION 3.13.0)

project(labelCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui)
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})

# TODO glob considered bad
file(GLOB SOURCES CONFIGURE_DEPENDS *.cpp *.h)
add_library(labelCore STATIC ${SOURCES})

# TODO fix relative path seems janky
target_include_directories(labelCore PRIVATE ../logManager)
#target_include_directories(labelCore PRIVATE ../../../3RDPartySoftware/chilkat/chilkat-9.5.0-armv7l-linux/include)

target_link_libraries(labelCore logManager)
# TODO what's up with this
target_compile_options(labelCore PRIVATE -Wno-multichar)