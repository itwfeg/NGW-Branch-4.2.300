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

HOBART_APP ?= $(HOME)/projects/Hobart/shared_code/Application
export HOBART_APP
export HOBART_OUT_DIR=$(HOBART_APP)/output

#
#Vars for Source and object files
#
SUB_DIRS :=

C_SRCS := \
	sqlite3.c
	
CPP_SRCS :=

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk

all: $(OUT_DIR) $(OUT_TARGET)
	mv $(HOBART_OUT_DIR)/$(TARGET) $(HOBART_OUT_DIR)/libsqlite.a

