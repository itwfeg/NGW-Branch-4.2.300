##############################################################
#
# File : makefile
#
# Make file used for building util/
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

# Only the root Makefile define the TARGET_LIB (check hobart.mk)
TARGET_LIB := $(addsuffix .a, $(TARGET_BASENAME))

#
#Vars for Source and object files
#
SUB_DIRS := \

#    devices \
#    messages \
#    reports \
#    tags

C_SRCS :=
CPP_SRCS := CEvolMsgReceiver.cpp \
    CMessageCenter.cpp \
    EvolMessage.cpp \
    EvolMessageWrapper.cpp \
    MsgReceiverProxy.cpp \
    ProxyMessage.cpp

INC_DIRS := \
    -I$(HOBART_APP)/PAL \
    -I$(HOBART_APP)/PAL/PAL_linux \
    -I$(HOBART_APP)/inc/linux \
    -I$(HOBART_APP)/source/app \
    -I$(HOBART_APP)/source/comm \
    -I$(HOBART_APP)/source/core \
    -I$(HOBART_APP)/source/core/Synchronization \
    -I$(HOBART_APP)/source/core/VersionManager/include \
    -I$(HOBART_APP)/source/db \
    -I$(HOBART_APP)/source/db/util \
    -I$(HOBART_APP)/source/db/util/e_string \
    -I$(HOBART_APP)/source/db/util/filesys \
    -I$(HOBART_APP)/source/db/util/thread \
    -I$(HOBART_APP)/source/EvolutionRTC/CodeUpdater/PrinterManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/LabelApplierManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/MarqueeManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/MessageCenter \
    -I$(HOBART_APP)/source/EvolutionRTC/NumericManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/OperatingModes/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PngGraphics/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PrinterManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/ProductDev/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RendererManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RFIDManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RTCManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/ScannerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/TimerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/USBCommManagers/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/USBManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WeigherManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WrapperManager/Include \
    -I$(HOBART_APP)/source/HFC \
    -I$(HOBART_APP)/source/HFC/tags \
    -I$(HOBART_APP)/source/HFC/devices \
    -I$(HOBART_APP)/source/util \
    -I$(HOBART_APP)/source/util/Inc \
    -I$(HOBART_APP)/../Smbus/driver

export INC_DIRS

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
	    #$(HOBART_APP)/output/source_EvolutionRTC_MessageCenter.a \
	    #$(HOBART_APP)/output/source_util.a \

ARCHIVES := \
	    $(OUT_DIR)/PAL_PAL_linux.a \
	    $(OUT_DIR)/source_EvolutionRTC_USBManager_Source.a \
	    $(OUT_DIR)/source_HFC.a \
	    $(OUT_DIR)/source_util_Src.a \

exe: $(OUT_TARGET)
	g++ -lrt \
	    $(OUT_OBJS) \
	    $(OUT_DIR)/PAL_PAL_main.a \
	    $(OUT_DIR)/PAL_PAL_linux.a \
	    $(OUT_DIR)/source_EvolutionRTC_USBManager_Source.a \
	    $(OUT_DIR)/source_HFC.a \
	    $(OUT_DIR)/source_util_Src.a \
	    -o main.exe
