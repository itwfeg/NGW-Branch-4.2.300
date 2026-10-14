##############################################################
#
# File : makefile
#
# Make file used for building db/util/accountant/
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
    accountant.cpp

INC_DIRS := \
    -I../.. \
    -I../debug \
    -I$(HOBART_APP)/PAL/ \
    -I$(HOBART_APP)/PAL/PAL_linux 

#
# define ARCHIVES for 'make testbuild'
#
ARCHIVES := \
    $(HOBART_OUT_DIR)/PAL_PAL_linux.a \
    $(HOBART_OUT_DIR)/source_db_util_e_string.a \
    $(HOBART_OUT_DIR)/source_db_util_platform.a \
    $(HOBART_OUT_DIR)/source_db_util_filesys.a \
    $(HOBART_OUT_DIR)/source_db_util_checksums.a \
    $(HOBART_OUT_DIR)/source_db_util_time.a \
    $(HOBART_OUT_DIR)/source_util.a \
    $(HOBART_OUT_DIR)/source_db_util_properties.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_MessageCenter.a \
    $(HOBART_OUT_DIR)/source_util_Src.a \
    $(HOBART_OUT_DIR)/source_HFC.a \


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
