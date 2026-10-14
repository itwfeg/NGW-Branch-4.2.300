cmake_minimum_required(VERSION 3.22.0)

option(BUILD_HT "Build HobartCustomerHookRfid for HT" OFF)

if(BUILD_HT)
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/hb6mxd-toolchain.cmake)
  set(CXX_FLAGS "-std=gnu++1y -DUSE_GHC_FS")
else()
  set(CXX_FLAGS "-std=gnu++17")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

project(HobartCustomerHookRfid LANGUAGES CXX C)

set(CMAKE_C_FLAGS
    "-std=gnu99 \
     -DTMR_ENABLE_BACKGROUND_READS=1 \
     -DTMR_ENABLE_SERIAL_READER_ONLY=1 \
     -DENABLE_READ_AFTER_WRITE=1 \
     -DUSE_TRANSPORT_LISTENER=0 \
     -DENABLE_EMBEDDED_READ=0 \
     -fPIC -Wall -g"
)
set(CMAKE_CXX_FLAGS
    "${CXX_FLAGS} \
     -DTMR_ENABLE_BACKGROUND_READS=1 \
     -DTMR_ENABLE_SERIAL_READER_ONLY=1 \
     -DENABLE_READ_AFTER_WRITE=1 \
     -DUSE_TRANSPORT_LISTENER=0 \
     -DENABLE_EMBEDDED_READ=0 \
     -fPIC -Wall -g"
)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../GitInfo.cmake)
  include(${CMAKE_CURRENT_SOURCE_DIR}/../GitInfo.cmake)
  get_version_from_git_tag(${CMAKE_CURRENT_SOURCE_DIR} GIT_VER)
  set_version_from_git_tag(${GIT_VER})
  set_git_info(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_NAME})
else()
  set(PROJECT_VERSION "0.0.0")
  set(PROJECT_VERSION_MAJOR 0)
  set(PROJECT_VERSION_MINOR 0)
  set(PROJECT_VERSION_PATCH 0)
  set(${PROJECT_NAME}_GIT_INFO "+0-unknown-0000000000")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/mercuryapi/api)

add_library(inih STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/inih/ini.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/inih/cpp/INIReader.cpp
)
target_include_directories(inih PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/inih
)
set_target_properties(inih PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(spdlog_headers INTERFACE)
target_include_directories(spdlog_headers INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include
)

add_library(${PROJECT_NAME} SHARED
  src/AppHookLog.h
  src/Config.h
  src/epc_utils.h
  src/Hex.h
  src/HobartCustomerHook.h
  src/ManufacturerDecoders.h
  src/manufacturer_names.h
  src/RFIDDecoder.h

  src/HobartCustomerHook.cpp
  src/AppHookLog.cpp
  src/epc_utils.cpp
  src/Hex.cpp
)

add_dependencies(${PROJECT_NAME} mercuryapi_static)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/inih/cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include

  ${CMAKE_CURRENT_SOURCE_DIR}/lib/mercuryapi/api
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/mercuryapi/api/LTK/LTKC/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/mercuryapi/api/LTK/LTKC/Library
)

find_library(PTHREAD_LIB pthread REQUIRED)
find_library(DL_LIBRARY dl REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PTHREAD_LIB} ${DL_LIBRARY} mercuryapi_static spdlog_headers inih)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION /usr/lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/rfid-hook/ DESTINATION "/usr/share/rfid-hook" FILES_MATCHING PATTERN "*")

set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}${${PROJECT_NAME}_GIT_INFO}")
set(CPACK_GENERATOR "DEB")

set(CPACK_PACKAGE_NAME "rfid-hook")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sean Jordan <sean.jordan@itwfeg.com>")
set(CPACK_PACKAGE_VENDOR "Hobart")
set(CPACK_PACKAGE_CONTACT "sean.jordan@itwfeg.com")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

##########################################
# Need to look for a better way to do this
set(CPACK_DEB_COMPONENT_INSTALL OFF)
set(CPACK_COMPONENTS_ALL "")

set(
  CPACK_INSTALL_CMAKE_PROJECTS
  "${CMAKE_CURRENT_BINARY_DIR};${PROJECT_NAME};ALL;/"
)
##########################################

set(
  CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL armv7l)
  set(DETECTED_ARCHITECTURE armhf)
elseif((CMAKE_SYSTEM_PROCESSOR STREQUAL cortexa72) OR (CMAKE_SYSTEM_PROCESSOR STREQUAL armv8a) OR (CMAKE_SYSTEM_PROCESSOR STREQUAL aarch64))
  set(DETECTED_ARCHITECTURE arm64)
else()
  message(WARNING "using default system architecture for package generation!")
endif()

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DETECTED_ARCHITECTURE}")

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_BINARY_DIR}/CPackConfig-${PROJECT_NAME}.cmake")
endif()

include(CPack)

