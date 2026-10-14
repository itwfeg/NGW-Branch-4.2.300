cmake_minimum_required(VERSION 3.22.0)
project(mercuryapi LANGUAGES C CXX)

set(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} -fPIC \
   -DTMR_ENABLE_BACKGROUND_READS=1 \
   -DTMR_ENABLE_SERIAL_READER_ONLY=1 \
   -DENABLE_READ_AFTER_WRITE=1 \
   -DUSE_TRANSPORT_LISTENER=0 \
   -DENABLE_EMBEDDED_READ=0 \
   -std=gnu99 -Wall -g"
)
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -fPIC \
   -DTMR_ENABLE_BACKGROUND_READS=1 \
   -DTMR_ENABLE_SERIAL_READER_ONLY=1 \
   -DENABLE_READ_AFTER_WRITE=1 \
   -DUSE_TRANSPORT_LISTENER=0 \
   -DENABLE_EMBEDDED_READ=0 \
   -std=c++11"
)

set(MERCURY_SOURCES
  serial_transport_posix.c
  serial_transport_tcp_posix.c
  tmr_strerror.c
  tmr_param.c
  hex_bytes.c
  tm_reader.c
  tm_reader_async.c
  serial_reader.c
  tmr_loadsave_configuration.c
  serial_reader_l3.c
  tmr_utils.c
  osdep_posix.c
)

add_library(mercuryapi_static STATIC ${MERCURY_SOURCES})
target_include_directories(mercuryapi_static PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/LTK/LTKC/Library
  ${CMAKE_CURRENT_SOURCE_DIR}/LTK/LTKC/Library/LLRP.org
)
target_link_libraries(mercuryapi_static PRIVATE pthread)
set_target_properties(mercuryapi_static PROPERTIES
  OUTPUT_NAME             "mercuryapi"
  POSITION_INDEPENDENT_CODE ON
)

add_library(mercuryapi_shared SHARED ${MERCURY_SOURCES})
target_include_directories(mercuryapi_shared PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/LTK/LTKC/Library
  ${CMAKE_CURRENT_SOURCE_DIR}/LTK/LTKC/Library/LLRP.org
)
target_link_libraries(mercuryapi_shared PRIVATE pthread)
set_target_properties(mercuryapi_shared PROPERTIES
  OUTPUT_NAME             "mercuryapi"
  POSITION_INDEPENDENT_CODE ON
)

