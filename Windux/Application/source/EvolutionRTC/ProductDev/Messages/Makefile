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
    ProductChangeMsg.cpp \
    ProductCompleteMsg.cpp \
    ProductFinishedPrintingAllLabelsMsg.cpp \
    ProductLookupMsg.cpp \
    ProductPackageCompleteMsg.cpp \
    ProductReadAllAccumulatedItemsMsg.cpp \
    ProductRemoveAccumItemMsg.cpp \
    ProductStartPrintForTransactionMsg.cpp \
    ProductStatusMsg.cpp \
    ProductUpdateMsg.cpp


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
