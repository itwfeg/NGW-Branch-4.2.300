cmake_minimum_required(VERSION 3.13.0)

project(HobartCustomerHook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(HobartCustomerHook SHARED)

target_compile_definitions(HobartCustomerHook PRIVATE LINUX_FIX)
target_compile_definitions(HobartCustomerHook PRIVATE HOBARTCUSTOMERHOOK_EXPORTS)

target_sources(HobartCustomerHook PRIVATE
  HobartCustomerHook.h
  HobartCustomerHook.cpp
)
