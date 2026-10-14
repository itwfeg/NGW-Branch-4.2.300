##############################################################
#
# File : makefile
#
# Make file used for building EvolutionRTC/LabelApplierManager/Messages/
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
    EvolLabelApplierConfiguration.cpp \
    ILACloseWindow.cpp \
    ILACycle.cpp \
    ILAForceLabel.cpp \
    ILAOff.cpp \
    ILAOpenWindow.cpp \
    ILARequestSpeed.cpp \
    ILASpeed.cpp \
    LabelApplierCloseWindow.cpp \
    LabelApplierConfiguration.cpp \
    LabelApplierDisable.cpp \
    LabelApplierEnable.cpp \
    LabelApplierGeneric.cpp \
    LabelApplierInquire.cpp \
    LabelApplierLabelApplierKind.cpp \
    LabelApplierLabelReady.cpp \
    LabelApplierLabelTaken.cpp \
    LabelApplierMode.cpp \
    LabelApplierOff.cpp \
    LabelApplierOn.cpp \
    LabelApplierOpenWindow.cpp \
    LabelApplierPackageReady.cpp \
    LabelApplierPositionInfo.cpp \
    LabelApplierPrint.cpp \
    LabelApplierRequestCloseWindow.cpp \
    LabelApplierRequestInquire.cpp \
    LabelApplierRequestLabelApplierKind.cpp \
    LabelApplierRequestOff.cpp \
    LabelApplierRequestSensors.cpp \
    LabelApplierRequestStatus.cpp \
    LabelApplierRequestTest.cpp \
    LabelApplierRequestWakeup.cpp \
    LabelApplierReset.cpp \
    LabelApplierSensors.cpp \
    LabelApplierSensorsTracking.cpp \
    LabelApplierSetup.cpp \
    LabelApplierStatus.cpp \
    LabelApplierSystemConfiguration.cpp \
    LabelApplierSystemInquire.cpp \
    LabelApplierSystemRequestInquire.cpp \
    LabelApplierSystemRequestStatus.cpp \
    LabelApplierSystemRequestWakeup.cpp \
    LabelApplierSystemStatus.cpp \
    LabelApplierSystemWakeup.cpp \
    LabelApplierTest.cpp \
    LabelApplierUltimaAutoMode.cpp \
    LabelApplierWakeup.cpp \
	CLARemoveUpgradeFiles.cpp \
	CLAReqSystemInfo.cpp \
	CLASystemInfo.cpp \
	CLASystemInfoErase.cpp \
	CLASystemInfoWrite.cpp \
	CLASystemStatus.cpp \
	CLATestComplete.cpp \
	CompactLabelApplierConfiguration.cpp \
	CompactLabelApplierInquire.cpp \
	CompactLabelApplierSensors.cpp \
	CLAPackageDischarge.cpp


#INC_DIRS := \
#    -I$(HOBART_APP)/PAL \
#    -I$(HOBART_APP)/PAL/PAL_linux \
#    -I$(HOBART_APP)/inc/linux \
#    -I$(HOBART_APP)/source/app \
#    -I$(HOBART_APP)/source/comm \
#    -I$(HOBART_APP)/source/core \
#    -I$(HOBART_APP)/source/core/Synchronization \
#    -I$(HOBART_APP)/source/core/VersionManager/include \
#    -I$(HOBART_APP)/source/db \
#    -I$(HOBART_APP)/source/db/util \
#    -I$(HOBART_APP)/source/db/util/thread \
#    -I$(HOBART_APP)/source/EvolutionRTC/CodeUpdater/PrinterManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/LabelApplierManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/MarqueeManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/MessageCenter \
#    -I$(HOBART_APP)/source/EvolutionRTC/NumericManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/OperatingModes/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/PngGraphics/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/PrinterManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/RendererManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/RTCManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/ScannerManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/TimerManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/USBCommManagers/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/USBManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/WeigherManager/Include \
#    -I$(HOBART_APP)/source/EvolutionRTC/WrapperManager/Include \
#    -I$(HOBART_APP)/source/HFC \
#    -I$(HOBART_APP)/source/HFC/tags \
#    -I$(HOBART_APP)/source/HFC/devices \
#    -I$(HOBART_APP)/source/util \
#    -I$(HOBART_APP)/source/util/Inc \

#    -I$(HOBART_APP)/source/HFC/devices \
#    -I$(HOBART_APP)/../smbus/driver


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
