##############################################################
#
# File : makefile
#
# Make file used for building EvolutionRTC/CodeUpdater/
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
    Wrapper \
    PrinterManager \
    CLA

#    AtmelAT91 \        ***** WARNING: need to handle the binary only library

C_SRCS :=
CPP_SRCS :=


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
