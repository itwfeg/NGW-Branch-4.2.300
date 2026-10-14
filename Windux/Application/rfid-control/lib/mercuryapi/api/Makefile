# Mercury API Makefile

# Default architecture
ARCH ?= x86_64

# Cross-compilation setup
ifeq ($(ARCH),arm)
    CROSS_COMPILE ?= arm-linux-gnueabihf-
    ARCH_FLAGS := -march=armv7-a -mfloat-abi=hard -mfpu=neon
else ifeq ($(ARCH),arm64)
    CROSS_COMPILE ?= aarch64-linux-gnu-
    ARCH_FLAGS := -march=armv8-a
else ifeq ($(ARCH),x86_64)
    CROSS_COMPILE ?=
    ARCH_FLAGS := -march=x86-64
else ifeq ($(ARCH),x86)
    CROSS_COMPILE ?=
    ARCH_FLAGS := -m32
else
    $(error Unsupported ARCH value. Please set ARCH to 'arm', 'arm64', 'x86_64', or 'x86'.)
endif

# Toolchain setup
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AR := $(CROSS_COMPILE)ar
RANLIB := $(CROSS_COMPILE)ranlib
STRIP := $(CROSS_COMPILE)strip

# Directories
OUTPUT_DIR ?= .

# Compiler and linker flags
COMMON_DEFINES := -DTMR_ENABLE_BACKGROUND_READS=1 -DTMR_ENABLE_SERIAL_READER_ONLY=1 \
                  -DENABLE_READ_AFTER_WRITE=1 -DUSE_TRANSPORT_LISTENER=0 -DENABLE_EMBEDDED_READ=0
COMMON_FLAGS := -fPIC $(ARCH_FLAGS) $(COMMON_DEFINES)
CFLAGS := $(COMMON_FLAGS) -std=gnu99 -Wall -g
CXXFLAGS := $(COMMON_FLAGS) -std=c++11
LDFLAGS := -lpthread

# Object files to compile
OBJ_NAMES := serial_transport_posix.o serial_transport_tcp_posix.o tmr_strerror.o tmr_param.o \
         hex_bytes.o tm_reader.o tm_reader_async.o serial_reader.o tmr_loadsave_configuration.o \
         serial_reader_l3.o tmr_utils.o osdep_posix.o
OBJS := $(addprefix $(OUTPUT_DIR)/,$(OBJ_NAMES))

# Library names
STATIC_LIB := libmercuryapi.a
SHARED_LIB := libmercuryapi.so

# Targets
.PHONY: all clean

all: $(STATIC_LIB) $(SHARED_LIB)

$(STATIC_LIB): $(OBJS)
	@echo "Building static library with: $(AR)"
	$(AR) rcs $(OUTPUT_DIR)/$@ $^

$(SHARED_LIB): $(OBJS)
	@echo "Building shared library with: $(CXX)"
	$(CXX) $(LDFLAGS) -shared -o $(OUTPUT_DIR)/$@ $^


$(OUTPUT_DIR)/%.o: %.c
	@echo "Compiling $< with: $(CC) $(CFLAGS)"
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/%.o: %.cpp
	@echo "Compiling $< with: $(CXX) $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OUTPUT_DIR)/$(STATIC_LIB) $(OUTPUT_DIR)/$(SHARED_LIB) $(OUTPUT_DIR)/*.o