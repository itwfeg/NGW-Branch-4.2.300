##############################################################
#
# File : makefile
#
# Make file used for building db/util/
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
SUB_DIRS := \
    accountant \
    checksums \
    cloptions \
    e_exception \
    e_string \
    filesys \
    nibble \
    platform \
    properties \
    time

#    containers/        only header file(s)
#    debug/trace.cpp    is excluded from build in Win32 (Alamy: change it to db_util_debug/)
#    locale/            this directory is not compiled in Win32
#    ptr/               only header file(s)
#    thread/            only source for VxWorks

C_SRCS :=
CPP_SRCS :=


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
