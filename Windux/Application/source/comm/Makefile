##############################################################
#
# File : makefile
#
# Make file used for building db/
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
    cgi \
    email \

#
#Source and object files etc
#
C_SRCS := 
CPP_SRCS := \
    CommSession.cpp \
    CMFile.cpp \
    CMReaderWriter.cpp \
    CMReaderWriterEvolUI.cpp \
    CMReaderWriterEvolTag.cpp \
    CMReaderWriterHandsFreeDevice.cpp \
    CMReaderWriterSyncTag.cpp \
    CMReaderWriterUltimaDisk.cpp \
    CMReaderWriterDBIndependent.cpp \
    CMReaderWriterGCD.cpp \
    InitializerComm.cpp \
    Messenger.cpp \
    NetworkConnectivityManager.cpp \
    NetworkStatus.cpp \
    Ping.cpp \
    ServerSocket.cpp \
    SessionMessage.cpp \
    Socket.cpp

# These source files exist but the MSVC project does not
# include them in the build
#MessageQueue.cpp \
#MessengerQueue.cpp 

INC_DIRS := \
    -I$(HOBART_APP)/PAL \
    -I$(HOBART_APP)/PAL/PAL_linux \
    -I$(HOBART_APP)/inc/linux \
    -I$(HOBART_APP)/source/app \
    -I$(HOBART_APP)/source/db \
    -I$(HOBART_APP)/source/db/util \
    -I$(HOBART_APP)/source/db/util/filesys \
    -I$(HOBART_APP)/source/db/util/thread \
    -I$(HOBART_APP)/source/core/Synchronization \
    -I$(HOBART_APP)/source/core/ \
    -I$(HOBART_APP)/source/core/VersionManager/include \
    -I$(HOBART_APP)/source/comm/ \
    -I$(HOBART_APP)/HobartHandsFree/shared \
    -I$(HOBART_APP)/source/EvolutionRTC/HandsFreeDevice/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PrinterManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/MessageCenter \
    -I$(HOBART_APP)/source/EvolutionRTC/RTCManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/OperatingModes/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/RendererManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/ScannerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WrapperManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/WeigherManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/PngGraphics/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/TimerManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/GCDManager/Include \
    -I$(HOBART_APP)/source/util \
    -I$(HOBART_APP)/source/util/Inc \
    -I$(HOBART_APP)/source/HFC \
    -I$(HOBART_APP)/source/HFC/messages \
    -I$(HOBART_APP)/source/HFC/tags \
    -I$(HOBART_APP)/source/MetrologicallySignificant \

export INC_DIRS

ARCHIVES := \
    $(HOBART_OUT_DIR)/PAL_PAL_linux.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_LabelApplierManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_LabelApplierManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_MarqueeManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_MarqueeManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_MessageCenter.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_NumericManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_NumericManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_OperatingModes_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_OperatingModes_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_PngGraphics_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_PrinterManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_PrinterManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_ProductDev_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RFIDManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RFIDManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RTCManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RTCManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RendererManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RendererManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_ScannerManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_ScannerManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_TimerManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_TimerManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_USBCommManagers_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_USBCommManagers_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_USBManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_WeigherManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_WeigherManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_WrapperManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_WrapperManager_Source.a \
    $(HOBART_OUT_DIR)/source_HFC.a \
    $(HOBART_OUT_DIR)/source_HFC_devices.a \
    $(HOBART_OUT_DIR)/source_HFC_messages.a \
    $(HOBART_OUT_DIR)/source_HFC_reports.a \
    $(HOBART_OUT_DIR)/source_HFC_tags.a \
    $(HOBART_OUT_DIR)/source_app.a \
    $(HOBART_OUT_DIR)/source_app_Messages.a \
    $(HOBART_OUT_DIR)/source_comm_cgi.a \
    $(HOBART_OUT_DIR)/source_comm_cgi_http.a \
    $(HOBART_OUT_DIR)/source_comm_email.a \
    $(HOBART_OUT_DIR)/source_comm_email_SMTP.a \
    $(HOBART_OUT_DIR)/source_core.a \
    $(HOBART_OUT_DIR)/source_core_StateMachine.a \
    $(HOBART_OUT_DIR)/source_core_Synchronization.a \
    $(HOBART_OUT_DIR)/source_core_Synchronization_Commands.a \
    $(HOBART_OUT_DIR)/source_core_Synchronization_Messages.a \
    $(HOBART_OUT_DIR)/source_core_VersionManager_messages.a \
    $(HOBART_OUT_DIR)/source_core_VersionManager_source.a \
    $(HOBART_OUT_DIR)/source_db_dbd.a \
    $(HOBART_OUT_DIR)/source_db_dbm.a \
    $(HOBART_OUT_DIR)/source_db_util_accountant.a \
    $(HOBART_OUT_DIR)/source_db_util_checksums.a \
    $(HOBART_OUT_DIR)/source_db_util_e_exception.a \
    $(HOBART_OUT_DIR)/source_db_util_e_string.a \
    $(HOBART_OUT_DIR)/source_db_util_filesys.a \
    $(HOBART_OUT_DIR)/source_db_util_nibble.a \
    $(HOBART_OUT_DIR)/source_db_util_platform.a \
    $(HOBART_OUT_DIR)/source_db_util_properties.a \
    $(HOBART_OUT_DIR)/source_db_util_time.a \
    $(HOBART_OUT_DIR)/source_util.a \
    $(HOBART_OUT_DIR)/source_util_Src.a \
    $(HOBART_OUT_DIR)/source_MetrologicallySignificant.a \

##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
