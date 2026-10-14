##############################################################
#
# File : makefile
#
# Make file used for building Core/Synchronization/
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
    EvolWrapperCameraCalibration.cpp \
    EvolWrapperCameraHeightScan.cpp \
    EvolWrapperCameraScan.cpp \
    EvolWrapperCamerasTracking.cpp \
    EvolWrapperCameraThreshold.cpp \
    EvolWrapperConfiguration.cpp \
    EvolWrapperControl.cpp \
    EvolWrapperCycle.cpp \
    EvolWrapperCycleCount.cpp \
    EvolWrapperDiagnosticsTest.cpp \
    EvolWrapperDiagnosticsTestResult.cpp \
    EvolWrapperErrorHistory.cpp \
    EvolWrapperErrorInfo.cpp \
    EvolWrapperEventCapture.cpp \
    EvolWrapperGeneralConfig.cpp \
    EvolWrapperGeneralSensors.cpp \
    EvolWrapperHeaterConfig.cpp \
    EvolWrapperInquire.cpp \
    EvolWrapperInterlockSensors.cpp \
    EvolWrapperLoad.cpp \
    EvolWrapperMotorProfile.cpp\
    EvolWrapperNode.cpp \
    EvolWrapperPackageDesc.cpp \
    EvolWrapperPackageDischarge.cpp \
    EvolWrapperPackageHistory.cpp \
    EvolWrapperPackageTimings.cpp \
    EvolWrapperPackageTimingsPointer.cpp \
    EvolWrapperPkgDetectorStatus.cpp \
    EvolWrapperRemoveUpgradeFiles.cpp \
    EvolWrapperReqCameraThreshold.cpp \
    EvolWrapperReqCycleCount.cpp \
    EvolWrapperReqSystemInfo.cpp \
    EvolWrapperReqSystemReset.cpp \
    EvolWrapperRequestPackageTimings.cpp \
    EvolWrapperResetDefaultPackageTimings.cpp \
	EvolWrapperUnlockPackageTimings.cpp \
    EvolWrapperSaveCycleCount.cpp \
    EvolWrapperSensors.cpp \
    EvolWrapperSettings.cpp \
    EvolWrapperStatus.cpp \
    EvolWrapperStatusInfo.cpp \
    EvolWrapperSystemInfo.cpp \
    EvolWrapperSystemInfoErase.cpp \
    EvolWrapperSystemInfoWrite.cpp \
    EvolWrapperSystemStatus.cpp \
    EvolWrapperTemperatureInfo.cpp \
    EvolWrapperTest.cpp \
    EvolWrapperTestComplete.cpp \
    EvolWrapperTrayNames.cpp \
    EvolWrapperWakeup.cpp \
    EvolWrapperWrap.cpp \
    EvolWrapperWriteCameraThreshold.cpp \
    EvolWrapperReqUIModel.cpp \
    EvolWrapperUIModel.cpp \
    NGWLabelReady.cpp \
    NGWLabelTaken.cpp \
    NGWLabelSetup.cpp \
    NGWRequestVersion.cpp \
    NGWVersion.cpp \
    NGWCycleCount.cpp \
    NGWError.cpp \
    NGWWrapBox.cpp \
    NGWWrapBoxComplete.cpp \
    NGWRequestWrapBoxAll.cpp \
    NGWRequestWrapBox.cpp \
    NGWRequestHeightBox.cpp \
    NGWHeightBoxComplete.cpp \
    NGWTimingsSave.cpp \
    NGWChangeParamComplete.cpp \
    NGWTimingsSaveComplete.cpp \
    NGWChangeParam.cpp \
    NGWBootFrame.cpp \
    NGWBootNodeCMD.cpp \
    NGWReqBootStatus.cpp \
    NGWBootStatus.cpp \
    NGWBootVersion.cpp \
    NGWRequestBootVersion.cpp \
    NGWAck.cpp \
    NGWFlush.cpp \
    NGWBulkTransfer.cpp \
    NGWWakeup.cpp \
    NGWGeneric.cpp \
    HeaterConfiguration.cpp \
    NodeInternalTemperature.cpp \
    WrapperAbort.cpp \
    WrapperConfiguration.cpp \
    WrapperControl.cpp \
    WrapperCycle.cpp \
    WrapperDisable.cpp \
    WrapperEnable.cpp \
    WrapperErrorHistory.cpp \
    WrapperErrorHistoryTracking.cpp \
    WrapperErrorInfo.cpp \
    WrapperFilmWidthDimensions.cpp \
    WrapperGeneric.cpp \
    WrapperGripperTiming.cpp \
    WrapperHeaterInfo.cpp \
    WrapperInquire.cpp \
    WrapperIntakeMode.cpp \
    WrapperJog.cpp \
    WrapperLoad.cpp \
    WrapperMode.cpp \
    WrapperOpen.cpp \
    WrapperPackageHistory.cpp \
    WrapperPackageInfo.cpp \
    WrapperPackCodeInfo.cpp \
    WrapperPackSize.cpp \
    WrapperPause.cpp \
    WrapperRequestErrorHistory.cpp \
    WrapperRequestInquire.cpp \
    WrapperRequestPackageHistory.cpp \
    WrapperRequestSensors.cpp \
    WrapperRequestSettings.cpp \
    WrapperRequestStatus.cpp \
    WrapperRequestWakeup.cpp \
    WrapperRequestWrapperKind.cpp \
    WrapperReset.cpp \
    WrapperResume.cpp \
    WrapperSensors.cpp \
    WrapperSensorsTracking.cpp \
    WrapperSettings.cpp \
    WrapperSetWrapMode.cpp \
    WrapperStateInfo.cpp \
    WrapperStatus.cpp \
    WrapperStatusInfo.cpp \
    WrapperStatusTracking.cpp \
    WrapperSystemConfiguration.cpp \
    WrapperSystemInquire.cpp \
    WrapperSystemRequestInquire.cpp \
    WrapperSystemRequestStatus.cpp \
    WrapperSystemRequestWakeup.cpp \
    WrapperSystemStatus.cpp \
    WrapperSystemWakeup.cpp \
    WrapperTest.cpp \
    WrapperTime.cpp \
    WrapperTiming.cpp \
    WrapperWakeup.cpp \
    WrapperWrap.cpp \
    WrapperWrapperKind.cpp \
    NGWEngInfo.cpp \
    NGWEngException.cpp \
    WrapBoxSize.cpp \
    WrPPPWakeup.cpp \
    WrPPPConfig.cpp \
    WrPPPDist.cpp \
    WrPPPRequest.cpp \
    WrPPPCalibration.cpp \
    WrPPPVersion.cpp \
    WrPPPStatus.cpp \
    NGWWrapperPause.cpp \
    WrPPPTest.cpp \
    NGWWrapperResume.cpp \
    NGWDimensionOverrides.cpp \

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
    -I$(HOBART_APP)/source/db/util/thread \
    -I$(HOBART_APP)/source/EvolutionRTC/CodeUpdater/PrinterManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/CodeUpdater/Wrapper/Include \
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
    -I$(HOBART_APP)/source/MetrologicallySignificant \
    -I$(HOBART_APP)/source/util \
    -I$(HOBART_APP)/source/util/Inc \
    -I$(HOBART_APP)/../Smbus/driver

export INC_DIRS


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
