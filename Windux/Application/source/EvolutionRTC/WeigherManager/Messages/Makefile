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
    UsbWeigherSystemStatus.cpp \
    WeigherCachedStatus.cpp \
    WeigherColdReset.cpp \
    WeigherControl.cpp \
    WeigherDisable.cpp \
    WeigherEnable.cpp \
    WeigherGeneric.cpp \
    WeigherInquire.cpp \
    WeigherLock.cpp \
    WeigherMode.cpp \
    WeigherNormal.cpp \
    WeigherRequestInquire.cpp \
    WeigherRequestStatus.cpp \
    WeigherRequestWakeup.cpp \
    WeigherRequestWeigherKind.cpp \
    WeigherReset.cpp \
    WeigherRezero.cpp \
    WeigherSystemCalibrationSwitch.cpp \
    WeigherSystemConfiguration.cpp \
    WeigherSystemInquire.cpp \
    WeigherSystemRequestInquire.cpp \
    WeigherSystemRequestStatus.cpp \
    WeigherSystemRequestValueMax.cpp \
    WeigherSystemRequestWakeup.cpp \
    WeigherSystemReset.cpp \
    WeigherSystemStatus.cpp \
    WeigherSystemWakeup.cpp \
    WeigherWakeup.cpp \
    WeigherWeigherKind.cpp \
    WeigherRequestVersion.cpp \
    WeigherVersion.cpp \
    WeigherRequestSerialNum.cpp \
    WeigherSerialNumber.cpp \
    WeigherCat3PageRequest.cpp \
    WeigherCat3DateTime.cpp \
    WeigherCat3RecordStatus.cpp \
    WeigherWriteCat3Record.cpp  \
    WeigherCat3Statistics.cpp \
    WeigherCat3AuditRecords.cpp \
    WeigherFactoryDefaultsComplete.cpp \
    WeigherCat3Statistics.cpp \
    WeigherCat3AuditRecords.cpp \
    WeigherReqValueMax.cpp \
    WeigherValueMaxFitParams.cpp \
    WeigherValueMaxAssembly.cpp \
    WeigherValueMaxWriteAssembly.cpp \
    WeigherValueMaxHwVersion.cpp \
    WeigherValueMaxWriteHwVersion.cpp \
    WeigherValueMaxDateOfMan.cpp \
    WeigherValueMaxWriteDateOfMan.cpp \
    WeigherValueMaxSerialNumber.cpp \
    WeigherValueMaxWriteSerialNumber.cpp \
    WeigherValueMaxXGainFactor.cpp \
    WeigherValueMaxWriteXGainFactor.cpp \
    WeigherValueMaxYGainFactor.cpp \
    WeigherValueMaxWriteYGainFactor.cpp \
    WeigherValueMaxZGainFactor.cpp \
    WeigherValueMaxWriteZGainFactor.cpp \
    WeigherValueMaxAllGains.cpp \
    WeigherSystemInfo.cpp \
    WeigherKpcError.cpp


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
