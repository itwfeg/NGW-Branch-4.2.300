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
SUB_DIRS := \
    linux

C_SRCS :=
CPP_SRCS := \
    UsbManager.cpp \
    HobartUsbUpdater.cpp \
    IntelHexParser.cpp \
    IPeripheralUpdater.cpp \
    IUsbCodeUpdater.cpp \
    UsbCodeUpdater.cpp \
    EvolPrinterCodeUpdater.cpp \
    EvolWeigherCodeUpdater.cpp \
    UsbNode.cpp \
    AtmelSambaUpdater.cpp \
    HIDDeviceEvent.cpp \

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
    -I$(HOBART_APP)/source/EvolutionRTC/CodeUpdater/AtmelAT91 \
    -I$(HOBART_APP)/source/EvolutionRTC/CodeUpdater/PrinterManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/LabelApplierManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/MarqueeManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/MessageCenter \
    -I$(HOBART_APP)/source/EvolutionRTC/NumericManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/OperatingModes/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PngGraphics/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PrinterManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/ProductDev/Include/ \
    -I$(HOBART_APP)/source/EvolutionRTC/RendererManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RTCManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/ScannerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/TimerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/USBCommManagers/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/USBManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/USBManager/Include/linux \
    -I$(HOBART_APP)/source/EvolutionRTC/WeigherManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WrapperManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/GCDManager/Include \
    -I$(HOBART_APP)/source/HFC \
    -I$(HOBART_APP)/source/HFC/tags \
    -I$(HOBART_APP)/source/HFC/devices \
    -I$(HOBART_APP)/source/MetrologicallySignificant \
    -I$(HOBART_APP)/source/util \
    -I$(HOBART_APP)/source/util/Inc \


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk

ARCHIVES := \
    $(OUT_DIR)/PAL_PAL_linux.a \
    $(OUT_DIR)/source_util.a \
    $(OUT_DIR)/source_util_Src.a \
    $(OUT_DIR)/source_HFC.a \
    $(OUT_DIR)/source_HFC_devices.a \
    $(OUT_DIR)/source_HFC_messages.a \
    $(OUT_DIR)/source_HFC_reports.a \
    $(OUT_DIR)/source_HFC_tags.a \
    $(OUT_DIR)/source_core_VersionManager.a \
    $(OUT_DIR)/source_core_VersionManager_messages.a \
    $(OUT_DIR)/source_core_VersionManager_source.a \
    $(OUT_DIR)/source_core.a \
    $(OUT_DIR)/source_core_StateMachine.a \
    $(OUT_DIR)/source_core_Synchronization.a \
    $(OUT_DIR)/source_core_Synchronization_Commands.a \
    $(OUT_DIR)/source_core_Synchronization_Messages.a \
    $(OUT_DIR)/source_EvolutionRTC_RTCManager.a \
    $(OUT_DIR)/source_EvolutionRTC_MessageCenter.a \
    $(OUT_DIR)/source_db_util_filesys.a \
    $(OUT_DIR)/source_db_util_time.a \
    $(OUT_DIR)/source_db_util_platform.a \
    $(OUT_DIR)/source_db_util_e_string.a \
    $(OUT_DIR)/source_db_util_checksums.a \
    $(OUT_DIR)/source_db_util_properties.a

exe: $(OUT_TARGET)
	g++ -lrt $(OUT_OBJS) $(ARCHIVES) -o main.exe
