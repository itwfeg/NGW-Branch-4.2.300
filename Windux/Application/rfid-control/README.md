# rfid-control

This project contains the code required to communicate with the ThingMagic® m7e Nano RFID Module. It provides a library and executable for interfacing with RFID hardware in GT and HT scales.

## Prerequisites

- Weigh Wrap's Ubuntu VM with ARM cross-compilation toolchain. (Tested in Ubuntu 22.04 [here](https://wwhobart.atlassian.net/wiki/spaces/GS/pages/201949236/ARK+Ubuntu+Image+and+Toolchain+-+VMware+Installation))
- Additional packages: `g++-9-arm-linux-gnueabihf`

## Build Instructions

### General Build Options

The Makefile supports the following options:
- `ARCH`: Target architecture (x86_64, arm, arm64). Defaults to x86_64.
- `BUILD_TYPE`: Type of build (lib for shared library, exe for executable). Defaults to lib.
- `SOURCE_FILE`: Source file to compile. Defaults to HobartCustomerHook.cpp.
- `FORCE_MERCURY_REBUILD`: Set to 1 to force rebuilding of MercuryAPI libraries.

### Building for Different Architectures

1. Clean any previous builds:
   ```
   make clean
   ```

2. Build the project:
   - For x86_64 (default):
     ```
     make
     ```
   - For ARM (32-bit):
     ```
     make ARCH=arm
     ```
   - For ARM64:
     ```
     make ARCH=arm64
     ```

### Additional Build Options

- To build an executable instead of a shared library:
  ```
  make exe
  or
  make BUILD_TYPE=exe
  ```
- To specify a different source file:
  ```
  make SOURCE_FILE=path/to/your/source.cpp
  ```
- You can combine options:
  ```
  make exe ARCH=arm SOURCE_FILE=custom_source.cpp
  or
  make ARCH=arm BUILD_TYPE=exe SOURCE_FILE=custom_source.cpp
  ```

### Cleaning

To clean the build artifacts:
```
make clean
```
This will remove both the shared library and executable files.


## Troubleshooting

- If you encounter errors related to the ARM toolchain when building for x86/x64, ensure you haven't sourced the ARM toolchain in your current terminal session.
- If MercuryAPI fails to build, try setting `FORCE_MERCURY_REBUILD=1` in your make command.
