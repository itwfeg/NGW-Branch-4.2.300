cmake_minimum_required(VERSION 3.13.0)

project(comm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Network)
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})

# TODO glob considered bad
file(GLOB SOURCES CONFIGURE_DEPENDS *.cpp *.h)
add_library(comm STATIC ${SOURCES})

# TODO fix relative path seems janky
target_include_directories(comm PRIVATE ../logManager)
target_link_libraries(comm logManager)
