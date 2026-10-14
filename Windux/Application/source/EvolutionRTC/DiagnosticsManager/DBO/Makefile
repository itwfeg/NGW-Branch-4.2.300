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
	Config.cpp \
	Diag.cpp \
	DiagStep.cpp \
	EventLog.cpp \
	PrinterDotUsage.cpp \
	PrinterDotWear.cpp \
	PrinterHeadStatus.cpp \
	ServiceAction.cpp \
	ServiceCat.cpp \
	ServiceMaintLog.cpp \
	ServiceMethod.cpp \
	ServicePart.cpp \
	ValueMax.cpp \
	Wizards.cpp \
	WrapperAuditLog.cpp \
	WrapperTimings.cpp \
	WrapperTrays.cpp \

INC_DIRS := \
	-I$(HOBART_APP)/PAL \
	-I$(HOBART_APP)/PAL/PAL_linux \
	-I$(HOBART_APP)/source/db \
	-I$(HOBART_APP)/source/db/util \
	-I$(HOBART_APP)/source/EvolutionRTC/DiagnosticsManager \
	-I$(HOBART_APP)/source/EvolutionRTC/PrinterManager/Include \
	-I$(HOBART_APP)/source/HFC \
	-I$(HOBART_APP)/source/util \
	-I$(HOBART_APP)/../3RDPartySoftware/sqlite-3071401 \

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
