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

ifdef TARGET_LIB
    # When TARGET_LIB is defined, it's the root Makefile
    # ...... to do ......
endif

TARGET_BASENAME := $(subst /,_, $(subst $(HOBART_APP)/,, $(CURDIR)))
SUB_LIBS_BASENAME := $(addprefix $(TARGET_BASENAME)_, $(SUB_DIRS))
SUB_LIBS := $(addsuffix .a, $(SUB_LIBS_BASENAME))
TARGET := $(addsuffix .a, $(TARGET_BASENAME))

OUT_DIR = $(HOBART_OUT_DIR)
OUT_SUB_LIBS = $(addprefix $(OUT_DIR)/,$(SUB_LIBS))
OUT_OBJS = $(addprefix $(OUT_DIR)/,$(OBJS))
OUT_TARGET = $(addprefix $(OUT_DIR)/,$(TARGET))

INC_DIRS := $(INC_DIRS) \
    `pkg-config --cflags freetype2`

#---------------------------------
#linux specific vars
#---------------------------------
#
#C Compiler vars
#
#%if $(OS) == linux

CC = gcc 
CLINK = gcc

#
#C++ Compiler vars
#
# let gcc decide on the compiler to invoke as that is it's purpose
CXX = gcc
# g++ is the linker, not ar. ar is an archiver
CXX_LNK = g++ 
# don't put '-c' as a CXX_FLAG as '-c' is actually not a flag but the 
# compiler action itself and these flags will need to be duplicated
# if we ever build a dynamic library during linking
CXX_FLAGS = -O3 -Wall -Wextra -DLINUX_FIX -D__STDC_FORMAT_MACROS #-DEVOL_NDEBUG#-Werror #-libstdc++ -fno-implicit-templates -DDONT_INCLUDE_TEMPLATE_CODE #-fno-implicit-inline-templates
CXX_LFLAGS = $(CXX_FLAGS) `pkg-config --libs freetype2` `curl-config --libs` -lrt
CXX_FOR_TESTING_FLG = -E
AR = ar
AR_FLAGS = rcs

#---------------------------------
#Windows specific vars
#---------------------------------
#%else
#CC = 
#LINK =
#CFLAGS =
#CPP_FLAGS =
#OFLAGS =
#%endif


##################################
#Target defs
##################################

#all: all_dirs $(OUT_DIR) $(OUT_OBJS)
#	@echo Linking $(TARGET)
#	@$(CXX_LNK) $(CXX_LFLAGS) $(OUT_TARGET) $(OUT_OBJS)

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
	@if test -d $@; then echo "exist"; else mkdir $@; fi

#
# Link target library (***** WARNING: Next step: detect it's library or execution file)
#
$(OUT_TARGET): $(OUT_SUB_LIBS) $(OUT_OBJS)
ifneq ($(strip $(OUT_OBJS)), )
	@echo Linking library $(notdir $@)
	@$(AR) $(AR_FLAGS) $(OUT_TARGET) $(OUT_OBJS)
#	@echo Remove sub-directory library: $(SUB_LIBS)
#	@for lib in $(OUT_SUB_LIBS); do rm $$lib; done
	@echo ----------
endif

#
# Sub-directory libraries
#
.PHONY: $(SUB_DIRS)
$(SUB_DIRS):
	$(MAKE) -C $@

$(OUT_SUB_LIBS): $(SUB_DIRS)
#	@for dir in $(SUB_DIRS); do \
#	    (cd $$dir; $(MAKE) all) || exit 1; \
#	done

#	@for dir in $(SUB_DIRS); do cd $$dir; $(MAKE) all; cd ..; done

#
#c build object files
#
$(OUT_DIR)/%.o : %.c
	@echo compiling $<
	@$(CC) -c $< $(INC_DIRS) -o $@

.c.o:
	@echo compiling $<
	@$(CC) -c $< $(INC_DIRS) -o $@

#
#c++ build object files
#
$(OUT_DIR)/%.oo : %.cpp
	@echo compiling $<
	$(CXX) $(CXX_FLAGS) -c $< $(INC_DIRS) -o $@

.cpp.o:
	@echo compiling file $<
	$(CXX) $(CXX_FLAGS) -c $< $(INC_DIRS) -o $@

#
#cleanup all output of this makefile
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
build:
	@for dir in $(SUB_DIRS); do cd $$dir; $(MAKE) testbuild; cd ..; done

testbuild: $(OUT_TARGET)
	g++ $(CXX_LFLAGS) \
	    $(OUT_OBJS) \
	    -Wl,--start-group \
	    $(OUT_DIR)/PAL_PAL_main.a \
	    $(ARCHIVES) \
	    -Wl,--end-group \
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
	@echo $(INC_DIRS)
