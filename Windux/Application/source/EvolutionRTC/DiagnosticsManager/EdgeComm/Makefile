##############################################################
#
# File : makefile
#
# Make file used for building EvolutionRTC/OperatingModes/
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
    EdgePacket.cpp \

INC_DIRS := \
	-I$(HOBART_APP)/PAL \
	-I$(HOBART_APP)/PAL/PAL_linux \
	-I$(HOBART_APP)/source/db \
	-I$(HOBART_APP)/source/db/util \
	-I$(HOBART_APP)/source/EvolutionRTC/DiagnosticsManager \
	-I$(HOBART_APP)/source/util \

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
