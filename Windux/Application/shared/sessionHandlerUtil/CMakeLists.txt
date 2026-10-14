cmake_minimum_required(VERSION 3.13.0)

project(sessionHandlerUtil LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Network)
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})

add_library(sessionHandlerUtil STATIC)

target_include_directories(sessionHandlerUtil PRIVATE common)

target_sources(sessionHandlerUtil PRIVATE
  shellzip.cpp
  ean/eanstring.cpp
  ean/eanapi.cpp
  openauth.cpp
  qrcode/qrmetrics.cpp
  qrcode/qreccblock.cpp
  qrcode/qrmode.cpp
  qrcode/qrecc.cpp
  qrcode/poly.cpp
  qrcode/qrmodedata.cpp
  qrcode/qrmodule.cpp
  qrcode/qrapi.cpp
  common/bitmap.cpp
  common/bitstring.cpp
  common/buffer.cpp
  shellzip.h
  pixmap.h
  ean/eanapi.h
  ean/eanstring.h
  ean/eantype.h
  qrcode/poly.h
  qrcode/qrecc.h
  qrcode/qreccblock.h
  qrcode/qrversion.h
  qrcode/qrapi.h
  qrcode/qrmodule.h
  qrcode/qrmodedata.h
  qrcode/qrecclevel.h
  qrcode/qrmetrics.h
  qrcode/qrmode.h
  openauth.h
  palette.h
  font.h
  common/buffer.h
  common/bitstring.h
  common/bitmap.h
  margins.h
)