##############################################################
#
# File : makefile
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
    poly.cpp \
    qrapi.cpp \
    qrecc.cpp \
    qreccblock.cpp \
    qrmetrics.cpp \
    qrmode.cpp \
    qrmodedata.cpp \
    qrmodule.cpp
    

INC_DIRS := \
    -I$(HOBART_APP)/source/EvolutionRTC/RendererManager/common
    

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
