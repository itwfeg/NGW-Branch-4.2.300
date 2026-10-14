##############################################################
#
# File : makefile
#
# Make file used for building Core/
#
##############################################################

###################################
#defines
###################################
OS = linux

#HOBART_APP ?= $(HOME)/projects/Hobart/shared_code/Application
HOBART_APP ?= $(shell echo `pwd`/Application)
export HOBART_APP
export HOBART_OUT_DIR=$(HOBART_APP)/output

#
#Vars for Source and object files
#
SUB_DIRS = Application

UI_TARGETS = Application/customerInterface Application/operatorInterface
DAEMON_TARGETS = Application/source/app

all: $(SUB_DIRS)
	@for dir in $(UI_TARGETS); do cd $$dir; $(MAKE); cd ../..; done
	@for dir in $(DAEMON_TARGETS); do cd $$dir; $(MAKE) daemon; cd ../../..; done

export C_SRCS =
export CPP_SRCS =
export INC_DIRS

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
