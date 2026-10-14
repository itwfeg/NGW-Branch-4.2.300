# Build the RFID hook and its bundled third-party libraries.

HOBART_APP ?= $(HOME)/projects/Hobart/shared_code/Application
export HOBART_APP

ARCH ?= $(if $(findstring aarch64,$(CROSS_COMPILE)),arm64,$(if $(findstring arm,$(CROSS_COMPILE)),arm,x86_64))
CROSS_COMPILE ?=
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++

OUTPUT_DIR ?= output
BUILD_DIR := $(OUTPUT_DIR)/build
MERCURY_DIR := lib/mercuryapi/api
MERCURY_OUTPUT_DIR := $(BUILD_DIR)/mercury
MERCURY_LIB := $(MERCURY_OUTPUT_DIR)/libmercuryapi.a

TARGET := $(OUTPUT_DIR)/libHobartCustomerHookRfid.so

CPP_SRCS := \
    src/HobartCustomerHook.cpp \
    src/AppHookLog.cpp \
    src/epc_utils.cpp \
    src/Hex.cpp

CPP_OBJS := $(CPP_SRCS:src/%.cpp=$(BUILD_DIR)/%.o)
INI_OBJ := $(BUILD_DIR)/ini.o
INI_READER_OBJ := $(BUILD_DIR)/INIReader.o

INCLUDES := \
    -Ilib \
    -Ilib/inih/cpp \
    -Ilib/spdlog/include \
    -Ilib/mercuryapi/api

DEFINES := \
    -DTMR_ENABLE_BACKGROUND_READS=1 \
    -DTMR_ENABLE_SERIAL_READER_ONLY=1 \
    -DENABLE_READ_AFTER_WRITE=1 \
    -DUSE_TRANSPORT_LISTENER=0 \
    -DENABLE_EMBEDDED_READ=0 \
    -DUSE_GHC_FS

CXXFLAGS ?= -std=gnu++1y -fPIC -Wall -g
CXXFLAGS += $(DEFINES) $(INCLUDES)
CFLAGS ?= -std=gnu99 -fPIC -Wall -g
CFLAGS += $(DEFINES) $(INCLUDES)
LDFLAGS ?= -shared
LDLIBS := $(MERCURY_LIB) -lpthread -ldl -lrt

.PHONY: all clean mercury

all: $(TARGET)

$(TARGET): $(CPP_OBJS) $(INI_OBJ) $(INI_READER_OBJ) $(MERCURY_LIB)
	@mkdir -p $(OUTPUT_DIR)
	$(CXX) $(LDFLAGS) -o $@ $(CPP_OBJS) $(INI_OBJ) $(INI_READER_OBJ) $(LDLIBS)

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(INI_OBJ): lib/inih/ini.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(INI_READER_OBJ): lib/inih/cpp/INIReader.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

mercury: $(MERCURY_LIB)

$(MERCURY_LIB):
	@mkdir -p $(MERCURY_OUTPUT_DIR)
	$(MAKE) -C $(MERCURY_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) OUTPUT_DIR=$(abspath $(MERCURY_OUTPUT_DIR)) libmercuryapi.a

clean:
	rm -rf $(OUTPUT_DIR)
	$(MAKE) -C $(MERCURY_DIR) clean OUTPUT_DIR=$(abspath $(MERCURY_OUTPUT_DIR)) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
