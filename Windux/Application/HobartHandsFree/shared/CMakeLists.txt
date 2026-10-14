cmake_minimum_required(VERSION 3.13.0)

project(hfpdshared LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core)

add_library(hfpdshared STATIC
  deviceclearallmessages.h
  devicecmdresult.h
  deviceconfig.h
  deviceconnect.h
  devicedisconnect.h
  deviceerror.h
  devicefirmwareprogress.h
  devicefirmwarerequest.h
  deviceflashfirmwareota.h
  devicelist.h
  devicelistreq.h
  deviceopnamelist.h
  devicereadallconfig.h
  devicereadconfig.h
  devicesendmessage.h
  devicestate.h
  devicewriteconfig.h
  message.cpp
  messagefactory.cpp
  messagefactory.h
  message.h
  nordicfirmware.cpp
  nordicfirmware.h
  ping.h
  pong.h
)

target_include_directories(hfpdshared PUBLIC .)
