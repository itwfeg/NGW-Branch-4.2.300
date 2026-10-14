cmake_minimum_required(VERSION 3.13.0)

project(hfpdd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(hfpdd)

find_package(Qt5 COMPONENTS Core Network Bluetooth)

target_include_directories(hfpdd PRIVATE ../shared)
target_include_directories(hfpdd PUBLIC
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Bluetooth_INCLUDE_DIRS}
  ${Qt5Network_INCLUDE_DIRS}
)

target_link_libraries(hfpdd
  Qt5::Core Qt5::Network Qt5::Bluetooth
  bluetooth
  hfpdshared
)

target_sources(hfpdd PRIVATE
  bledevice.cpp
  bledevice.h
  blescanneragent.cpp
  blescanneragent.h
  commclient.cpp
  commclient.h
  commserver.cpp
  commserver.h
  dfudevice.cpp
  dfudevice.h
  hciscanneragent.cpp
  hciscanneragent.h
  hciscanneragentthread.cpp
  hciscanneragentthread.h
  hobarttag.cpp
  hobarttag.h
  hobarttagmanager.cpp
  hobarttagmanager.h
  hobarttag_v0.cpp
  hobarttag_v0.h
  hobarttag_v1.cpp
  hobarttag_v1.h
  leadvertinfo.cpp
  leadvertinfo.h
  main.cpp
  otaupgrademanager.cpp
  otaupgrademanager.h
  scanneragent.cpp
  scanneragent.h
  scanneragentwatchdog.cpp
  scanneragentwatchdog.h
)
