
TARGET = dsc-checksum
SRC_DIRS ?= ./

#SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
SRCS := \
	dsc-checksum.cpp \
	SrecParser.cpp

OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

ifeq ($(BUILD_TYPE),debug)
	FFLAGS = -O0 -g
else
	FFLAGS=-O3
endif

CXXFLAGS ?= $(INC_FLAGS) $(FFLAGS) -MMD -MP

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)

