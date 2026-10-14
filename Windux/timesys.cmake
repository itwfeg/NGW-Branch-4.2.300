set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv7l)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER /opt/hb6mxd_combo/toolchain/bin/armv7l-timesys-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /opt/hb6mxd_combo/toolchain/bin/armv7l-timesys-linux-gnueabihf-g++)

# target environment
set(CMAKE_FIND_ROOT_PATH /opt/hb6mxd_combo/toolchain/)
set(CMAKE_PREFIX_PATH /opt/hb6mxd_combo/toolchain/)
set(CMAKE_MODULE_PATH /opt/hb6mxd_combo/toolchain/lib/cmake/)
set(CMAKE_FRAMEWORK_PATH /opt/hb6mxd_combo/toolchain/)

# don't use the host, only use the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# pkg-config paths
# TODO don't hardcode these???
# seems like these should be autoconfigured and not need to be set manually
#set(ENV{PKG_CONFIG_PATH} "/opt/hb6mxd_combo/toolchain/")
#set(ENV{PKG_CONFIG_LIBDIR} /opt/hb6mxd_combo/toolchain/usr/lib/pkgconfig:/opt/hb6mxd_combo/toolchain/usr/share/pkgconfig)
#set(ENV{PKG_CONFIG_SYSROOT_DIR} /opt/hb6mxd_combo/toolchain/)
#set(ENV{PKG_CONFIG_EXECUTABLE} /opt/hb6mxd_combo/toolchain/bin/pkg-config)
