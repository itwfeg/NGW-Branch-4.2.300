##############################################################
#
# File : makefile
#
# Make file used for building Core/Synchronization/Commands/
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
    AbortBatchCommand.cpp \
    ApplyAndDeleteBatchCommand.cpp \
    ApplyBatchCommand.cpp \
    Command.cpp \
    CommandWrapper.cpp \
    CommCommand.cpp \
    DeleteBatchCommand.cpp \
    DownloadLocalBatchesCommand.cpp \
    NotifyLocalBatchesPresentCommand.cpp \
    NotifyOnlineCommand.cpp \
    PurgeBatchesCommand.cpp \
    RequestDeregistrationCommand.cpp \
    RequestRegistrationCommand.cpp \
    SynchronizePrimaryScaleCommand.cpp \
    SynchronizeSecondaryScaleCommand.cpp \

#   Alamy: We don't need the 2 files. They are not compiled in Windows, and they cause compiling error in Linux.
#    RequestSynchronizationCommand.cpp \
#    UploadLocalBatchesCommand.cpp 
#

INC_DIRS := \
    -I$(HOBART_APP)/PAL \
    -I$(HOBART_APP)/PAL/PAL_linux \
    -I$(HOBART_APP)/source/app \
    -I$(HOBART_APP)/source/comm \
    -I$(HOBART_APP)/source/core \
    -I$(HOBART_APP)/source/core/Synchronization \
    -I$(HOBART_APP)/source/core/VersionManager/include \
    -I$(HOBART_APP)/source/db \
    -I$(HOBART_APP)/source/EvolutionRTC/LabelApplierManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/MarqueeManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/MessageCenter \
    -I$(HOBART_APP)/source/EvolutionRTC/NumericManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/OperatingModes/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PngGraphics/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PrinterManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RendererManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RTCManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/ScannerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/TimerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WeigherManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WrapperManager/Include \
    -I$(HOBART_APP)/source/HFC \
    -I$(HOBART_APP)/source/HFC/devices \
    -I$(HOBART_APP)/source/HFC/tags \
    -I$(HOBART_APP)/source/util \
    -I$(HOBART_APP)/source/util/Inc \
    -I$(HOBART_APP)/../smbus/driver

#
# define ARCHIVES for 'make testbuild'
#
ARCHIVES := \
    $(HOBART_OUT_DIR)/source_core_Synchronization_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_MessageCenter.a \
    $(HOBART_OUT_DIR)/PAL_PAL_linux.a


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
