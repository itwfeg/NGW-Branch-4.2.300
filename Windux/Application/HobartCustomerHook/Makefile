##############################################################
#
# File : makefile
#
# Make file used for building HobartCustomerHook
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

TARGET_SO := libHobartCustomerHook.so

APP_DEFINES := -DHOBARTCUSTOMERHOOK_EXPORTS
APP_CXXFLAGS := -fPIC -g
APP_LDFLAGS := -shared

#
#Vars for Source and object files
#
SUB_DIRS :=

C_SRCS := 

CPP_SRCS := \
    HobartCustomerHook.cpp

INC_DIRS :=

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
