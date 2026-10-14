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
    RealTimeController.cpp

#    -I$(HOBART_APP)/ \

INC_DIRS := \
    -I$(HOBART_APP)/PAL \
    -I$(HOBART_APP)/PAL/PAL_linux \
    -I$(HOBART_APP)/inc/linux \
    -I$(HOBART_APP)/source/app \
    -I$(HOBART_APP)/source/core \
    -I$(HOBART_APP)/source/comm \
    -I$(HOBART_APP)/source/HFC \
    -I$(HOBART_APP)/source/HFC/tags \
    -I$(HOBART_APP)/source/util \
    -I$(HOBART_APP)/source/util/Inc \
    -I$(HOBART_APP)/source/db \
    -I$(HOBART_APP)/source/db/util \
    -I$(HOBART_APP)/source/EvolutionRTC/BoardSite/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/CanManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/CanManager/Messages/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/KPCBootManager/Include/ \
    -I$(HOBART_APP)/source/EvolutionRTC/KPCBootManager/Messages/Include/ \
    -I$(HOBART_APP)/source/EvolutionRTC/KPCBootUpgrader/Include/ \
    -I$(HOBART_APP)/source/EvolutionRTC/KPCBootUpgrader/Messages/Include/ \
    -I$(HOBART_APP)/source/EvolutionRTC/MessageCenter \
    -I$(HOBART_APP)/source/EvolutionRTC/ProductDev/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RTCManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RendererManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/USBCommManagers/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/MarqueeManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/NumericManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/OperatingModes/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/ScannerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WeigherManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WeigherManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/LabelApplierManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PrinterManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WrapperManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/USBManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/GCDManager/Include \
    -I$(HOBART_APP)/source/core/VersionManager/include \
    -I$(HOBART_APP)/source/MetrologicallySignificant

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk

	    #$(OUT_DIR)/source_HFC.a \

exe: $(OUT_TARGET)
	g++ -lrt \
	    $(OUT_OBJS) \
	    $(OUT_DIR)/PAL_PAL_main.a \
	    $(OUT_DIR)/PAL_PAL_linux.a \
	    $(OUT_DIR)/source_HFC.a \
	    $(OUT_DIR)/source_util.a \
	    $(OUT_DIR)/source_util_Src.a \
	    $(OUT_DIR)/source_db_util.a \
	    -o main.exe

