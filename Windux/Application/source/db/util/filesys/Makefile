##############################################################
#
# File : makefile
#
# Make file used for building db/util/filesys/
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

#
#Vars for Source and object files
#
SUB_DIRS :=
C_SRCS :=
CPP_SRCS := \
    directory.cpp \
    file.cpp \
    filename.cpp

INC_DIRS := \
    -I../.. \
    -I$(HOBART_APP)/PAL/ \
    -I$(HOBART_APP)/PAL/PAL_linux \
    -I$(HOBART_APP)/source/util

#
# define ARCHIVES for 'make testbuild'
#
ARCHIVES := \
    $(HOBART_OUT_DIR)/source_db_util_platform.a \
    $(HOBART_OUT_DIR)/source_db_util_time.a \
    $(HOBART_OUT_DIR)/source_db_util_checksums.a \
    $(HOBART_OUT_DIR)/source_db_util_e_string.a \


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
