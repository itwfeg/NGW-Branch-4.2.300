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


##################################
#make vars
##################################

#
#Vars for Source and object files
#
SUB_DIRS := app comm core db EvolutionRTC/MessageCenter EvolutionRTC HFC MetrologicallySignificant util

C_SRCS :=
CPP_SRCS :=

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
