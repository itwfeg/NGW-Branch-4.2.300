cmake_minimum_required(VERSION 3.13.0)

project(sessionHandlerHooks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Network Multimedia MultimediaWidgets)

add_library(sessionHandlerHooks STATIC)

target_include_directories(sessionHandlerHooks PUBLIC
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Network_INCLUDE_DIRS}
  ${Qt5Multimedia_INCLUDE_DIRS}
  ${Qt5MultimediaWidgets_INCLUDE_DIRS}
)

target_include_directories(sessionHandlerHooks PRIVATE
  ../logManager
  ../Session
  .
  ../sessionHandlerWidgets
  ../sessionHandlerUtil
  ../labelCore
  ../platform
  ../videoPlayerCore
)

target_compile_options(sessionHandlerHooks PRIVATE -Wno-multichar)

target_link_libraries(sessionHandlerHooks PUBLIC Session sessionHandlerWidgets)

target_sources(sessionHandlerHooks PRIVATE
  selfservicehook.cpp
  selfservicehookerror.cpp
  selfservicehookerror.h
  selfservicehook.h
  sessionhook.cpp
  sessionhook.h
  sessionhooks.cpp
  sessionhooks.h
  selfservice/selfserveAuditLog.h
  selfservice/selfserveByCount.h
  selfservice/selfserveFinish.h
  selfservice/selfserveImageLookup.h
  selfservice/selfserveInstruct.h
  selfservice/selfserveItemLookup.h
  selfservice/selfserveLogin.h
  selfservice/selfservePlaylist.h
  selfservice/selfserveRandomWeight.h
  selfservice/selfserveStart.h
  selfservice/selfserveWeightsMeasures.h
  selfservice/selfservicebuttonbar.h
  selfservice/selfservicecamerapredictions.h
  selfservice/selfservicedialog.h
  selfservice/selfserviceicons.h
  selfservice/selfserviceitemimage.h
  selfservice/selfserviceitemnutrifacts.h
  selfservice/selfservicenumberpad.h
  selfservice/selfservicesoftkeybutton.h
  selfservice/selfservicesoftkeyswidget.h
  selfservice/selfservicetoggledsoftkeybutton.h
  selfservice/selfserviceweightbar.h
  selfservice/selfservicewmnumberpad.h
  selfservice/selfserveAuditLog.cpp
  selfservice/selfserveByCount.cpp
  selfservice/selfserveFinish.cpp
  selfservice/selfserveImageLookup.cpp
  selfservice/selfserveInstruct.cpp
  selfservice/selfserveItemLookup.cpp
  selfservice/selfserveLogin.cpp
  selfservice/selfservePlaylist.cpp
  selfservice/selfserveRandomWeight.cpp
  selfservice/selfserveStart.cpp
  selfservice/selfserveWeightsMeasures.cpp
  selfservice/selfservicebuttonbar.cpp
  selfservice/selfservicecamerapredictions.cpp
  selfservice/selfservicedialog.cpp
  selfservice/selfserviceicons.cpp
  selfservice/selfserviceitemimage.cpp
  selfservice/selfserviceitemnutrifacts.cpp
  selfservice/selfservicenumberpad.cpp
  selfservice/selfservicesoftkeybutton.cpp
  selfservice/selfservicesoftkeyswidget.cpp
  selfservice/selfservicetoggledsoftkeybutton.cpp
  selfservice/selfserviceweightbar.cpp
  selfservice/selfservicewmnumberpad.cpp
)