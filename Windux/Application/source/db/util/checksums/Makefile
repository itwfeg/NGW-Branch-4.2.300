##############################################################
#
# File : makefile
#
# Make file used for building db/util/checksums/
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
    checksum.cpp

INC_DIRS := \
    -I../.. \
    -I$(HOBART_APP)/PAL/ \
    -I$(HOBART_APP)/PAL/PAL_linux 

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
