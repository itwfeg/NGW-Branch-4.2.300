cmake_minimum_required(VERSION 3.13.0)

project(PAL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(PAL STATIC
  PAL_absclass.h
  PAL_errcode.h
  PAL_linux/PAL_absclass.cpp
  PAL_linux/PAL_errno.cpp
)

target_include_directories(PAL PUBLIC . PAL_linux)

