##############################################################
#
# File : makefile
#
# Make file used for building abstraction layer.
#
##############################################################

###################################
#defines
###################################
OS = linux

HOBART_APP ?= $(HOME)/projects/Hobart/shared_code/Application
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
CPP_SRCS := PAL_absclass.cpp PAL_errno.cpp

INC_DIRS := \
    -I$(HOBART_APP)/PAL \
    -I$(HOBART_APP)/PAL/PAL_linux \
    `pkg-config --cflags libcurl`


export INC_DIRS

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
