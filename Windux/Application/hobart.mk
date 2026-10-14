##############################################################
#
# File : hobart.mk
#
# Make file template for Hobart application
#
##############################################################

#
# Generate OUT_SUB_LIBS, OUT_OBJS, and OUT_TARGET
#   OUT_SUB_LIBS: sub-directory generated library
#   OUT_OBJS    : curent direcotry compiled object files
#   OUT_TARGET  : The Target to generate (exp: library including OUT_SUB_LIBS and OUT_OBJS)
#
OBJS = $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.oo)

#remove warnings
export TARGET_ARCH :=
export CPPFLAGS :=

#also export here for PTR 621
export TARGET_BASENAME := $(subst /,_, $(subst $(HOBART_APP)/,, $(CURDIR)))
SUB_LIBS_BASENAME := $(addprefix $(TARGET_BASENAME)_, $(SUB_DIRS))
SUB_LIBS := $(addsuffix .a, $(SUB_LIBS_BASENAME))

ifdef TARGET_SO
TARGET := $(TARGET_SO)
else
TARGET := $(addsuffix .a, $(TARGET_BASENAME))
endif

OUT_DIR = $(HOBART_OUT_DIR)
OUT_SUB_LIBS = $(addprefix $(OUT_DIR)/,$(SUB_LIBS))
OUT_OBJS = $(addprefix $(OUT_DIR)/,$(OBJS))
OUT_TARGET = $(addprefix $(OUT_DIR)/,$(TARGET))

##############################################################
#
# linux build environment
# 
##############################################################

APP_DEFINES ?= 
APP_CFLAGS ?=
APP_CXXFLAGS ?=
APP_LDFLAGS ?=

include $(HOBART_APP)/hobartarch.mk

CC		= $(CROSS_COMPILE)gcc
CXX		= $(CROSS_COMPILE)g++
DEFINES		= -DLINUX_FIX -D__STDC_FORMAT_MACROS -DSQLITE_OMIT_LOAD_EXTENSION $(APP_DEFINES)
CFLAGS		= -pipe $(FFLAGS) $(APP_CFLAGS) -Wall -Wextra $(DEFINES)
CXXFLAGS	= -pipe $(FFLAGS) $(APP_CXXFLAGS) -std=c++11 -Wall -Wextra $(DEFINES)
INCPATH		= $(INC_DIRS) $(XINCPATH)

LINK		= $(CROSS_COMPILE)g++
LDFLAGS		= $(XLDFLAGS) $(APP_LDFLAGS)
LIBS		= $(APP_LIBS) -lrt -lpthread

AR		= $(CROSS_COMPILE)ar
ARFLAGS		= rcs

RANLIB		= $(CROSS_COMPILE)ranlib

STRIP		= $(CROSS_COMPILE)strip

##############################################################
#
#  Target defs
#
##############################################################

all: $(OUT_DIR) $(OUT_TARGET)

rootlib:

subdirs:

#
#build all sub dirs
#
all_dirs:
	@for dir in $(SUB_DIRS); do cd $$dir; $(MAKE) all; cd ..; done

#
# output directory
#
$(OUT_DIR):
	@if test -d $@; then echo "exist"; else mkdir -p $@; fi

#
# Link target library (***** WARNING: Next step: detect it's library or execution file)
#
$(OUT_TARGET): $(OUT_SUB_LIBS) $(OUT_OBJS)
ifneq ($(strip $(OUT_OBJS)), )
	@echo Linking library $(notdir $@)
ifeq ($(suffix $(OUT_TARGET)),.a)
	@$(AR) $(ARFLAGS) $(OUT_TARGET) $(OUT_OBJS)
endif
ifeq ($(suffix $(OUT_TARGET)),.so)
	@$(CXX) $(LDFLAGS) -o $(OUT_TARGET) $(OUT_OBJS)
endif
	@echo ----------
endif

#
# Sub-directory libraries
#
.PHONY: $(SUB_DIRS)
$(SUB_DIRS):
	$(MAKE) -C $@

$(OUT_SUB_LIBS): $(SUB_DIRS)

#
# c build object files
#
$(OUT_DIR)/%.o : %.c
	@echo compiling $<
	@$(CC) $(CFLAGS) -c $< $(INCPATH) -o $@

.c.o:
	@echo compiling $<
	@$(CC) $(CFLAGS) -c $< $(INCPATH) -o $@

#
# c++ build object files
#
$(OUT_DIR)/%.oo : %.cpp
	@echo compiling $<
	$(CXX) $(CXXFLAGS) -c $< $(INCPATH) -o $@

.cpp.o:
	@echo compiling file $<
	$(CXX) $(CXXFLAGS) -c $< $(INCPATH) -o $@

#
# cleanup all output of this makefile
#
clean:
	@echo cleaning directory... 
	@for dir in $(SUB_DIRS); do cd $$dir; $(MAKE) clean; cd ..; done
	@rm -f $(OUT_OBJS)
	@rm -f $(OUT_TARGET)
	@echo Done!

#
# create an exe to test that this library will link properly
# you will need to fill in the ARCHIVES linked in in the 
# projects local Makefile
#
build:
	@for dir in $(SUB_DIRS); do cd $$dir; $(MAKE) testbuild; cd ..; done

testbuild: $(OUT_TARGET)
	$(LINK) $(LDFLAGS) \
	    $(OUT_OBJS) \
	    -Wl,--start-group \
	        $(OUT_DIR)/PAL_PAL_main.a \
	        $(ARCHIVES) \
	    -Wl,--end-group \
	    $(LIBS) \
	    -o main.exe

##################################
# Testing purpose
##################################

test:
	@echo test build
	@echo ----- Current path -----
	@echo $(CURDIR)
	@echo ----- Output path -----
	@echo $(OUT_DIR)
	@echo ----- Current relative path -----
	@echo $(CUR_RELATIVE_DIR)
	@echo ----- Target Basename -----
	@echo $(TARGET_BASENAME)
	@echo ----- Out OBJS -----
	@echo $(OUT_OBJS)
	@echo ----- Sub libs -----
	@echo $(SUB_LIBS)
	@echo ----- Out Sub LIBS -----
	@echo $(OUT_SUB_LIBS)
	@echo ----- Target -----
	@echo $(TARGET)
	@echo ----- HOBART_APP -----
	@echo $(HOBART_APP)
	@echo ------INC_DIRS -----
	@echo $(INCPATH)
	@echo ------ CROSS_COMPILE -----
	@echo $(CROSS_COMPILE)
	@echo ------ CC -----
	@echo $(CC)
	@echo ------ CXX -----
	@echo $(CXX)
	@echo ------ DEFINES -----
	@echo $(DEFINES)
	@echo ------ CFLAGS -----
	@echo $(CFLAGS)
	@echo ------ CXXFLAGS -----
	@echo $(CXXFLAGS)
	@echo ------ INCPATH -----
	@echo $(INCPATH)
	@echo ------ LINK -----
	@echo $(LINK)
	@echo ------ LDFLAGS -----
	@echo $(LDFLAGS)
	@echo ------ LIBS -----
	@echo $(LIBS)
	@echo ------ AR -----
	@echo $(AR)
	@echo ------ ARFLAGS -----
	@echo $(ARFLAGS)
	@echo ------ RANLIB -----
	@echo $(RANLIB)
	@echo ------ STRIP -----
	@echo $(STRIP)








