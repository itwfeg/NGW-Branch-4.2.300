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
    AccessKeyActivation.cpp \
    AccessKeyDeactivation.cpp \
    SystemCalibrationSwitch.cpp \
    SystemConfiguration.cpp \
    SystemConfigureGCD.cpp \
    SystemDisable.cpp \
    SystemEEPBlockRead.cpp \
    SystemEEPBlockWrite.cpp \
    SystemEnable.cpp \
    SystemGCDPowerOff.cpp \
    SystemGCDPowerOn.cpp \
    SystemGeneric.cpp \
    SystemInquire.cpp \
    SystemMode.cpp \
    SystemRead.cpp \
    SystemRequestInquire.cpp \
    SystemRequestStatus.cpp \
    SystemRequestWakeup.cpp \
    SystemReset.cpp \
    SystemStatus.cpp \
    SystemStatusInfo.cpp \
    SystemWakeup.cpp \
    SystemWrite.cpp \
    UsbKbdAttachChange.cpp

INC_DIRS := \
    -I$(HOBART_APP)/PAL \
    -I$(HOBART_APP)/PAL/PAL_linux \
    -I$(HOBART_APP)/inc/linux \
    -I../Include \
    -I$(HOBART_APP)/source/db \
    -I$(HOBART_APP)/source/db/util \
    -I$(HOBART_APP)/source/EvolutionRTC/MessageCenter \
    -I$(HOBART_APP)/source/app \
    -I$(HOBART_APP)/source/HFC \
    -I$(HOBART_APP)/source/util \
    -I$(HOBART_APP)/source/util/Inc

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
