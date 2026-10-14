##############################################################
#
# File : makefile
#
# Make file used for building db/cb/access/
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
    stdfile.cpp \
    stdindex.cpp \
    stdrecord.cpp


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
