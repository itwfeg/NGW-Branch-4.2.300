set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv7l)

# C and C++ compilers.
set(CMAKE_C_COMPILER /opt/hb6mxd_combo/toolchain/bin/armv7l-timesys-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /opt/hb6mxd_combo/toolchain/bin/armv7l-timesys-linux-gnueabihf-g++)

# Target environment.
set(CMAKE_FIND_ROOT_PATH /opt/hb6mxd_combo/toolchain/)
set(CMAKE_PREFIX_PATH /opt/hb6mxd_combo/toolchain/)
set(CMAKE_MODULE_PATH /opt/hb6mxd_combo/toolchain/lib/cmake/)
set(CMAKE_FRAMEWORK_PATH /opt/hb6mxd_combo/toolchain/)

# Don't use the host, only use the target environment.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
