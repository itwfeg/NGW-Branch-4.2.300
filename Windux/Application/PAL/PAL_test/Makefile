##############################################################
#
# File : makefile
#
# Make file used for building abstraction layer test program.
#
##############################################################

###################################
#defines
###################################
OS = linux

HOBART_APP ?= $(HOME)/projects/Hobart/shared_linux_port/Application
export HOBART_APP
export HOBART_OUT_DIR=$(HOBART_APP)/output

##################################
#make vars
##################################

# Only the root Makefile define the TARGET_LIB (check hobart.mk)
TARGET_LIB := $(addsuffix .a, $(TARGET_BASENAME))

#
#Vars for Source and object files
#
SUB_DIRS :=

C_SRCS := 
CPP_SRCS := PAL_test.cpp PAL_test_Matrix.cpp PAL_test_WorkerThread.cpp
INC_DIRS := -I$(HOBART_APP)/PAL -I$(HOBART_APP)/PAL/PAL_linux

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
CXX_LFLAGS += `curl-config --libs `
AR_FILE := $(HOBART_OUT_DIR)/PAL_PAL_linux.a

exe: $(OUT_TARGET) 
	@echo Linking $(OUT_DIR)/$(OUTPUT)/$@
	$(CXX_LNK) $(CXX_LFLAGS) $(OUT_OBJS) $(AR_FILE) -o PAL_test

