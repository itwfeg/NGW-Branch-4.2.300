##############################################################
#
# File : makefile
#
# Make file used for building HFC/devices/
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
    EVMMemoryStatistics.cpp \
    HfcAccessKeyActivation.cpp \
    HfcAccessKeyDeactivation.cpp \
    HfcBoardSiteTestCompleteUI.cpp \
    HfcBoardSitePercentComplete.cpp \
    HfcBoardSiteTestStatus.cpp \
    HfcCUEvolWrapperStatus.cpp \
    HfcEVMMemoryStatistics.cpp \
    HfcEvolLabelApplierInquire.cpp \
    HfcEvolLabelApplierConfig.cpp \
    HfcEvolLabelApplierPositionInfo.cpp \
    HfcEvolWrapperAllHtBoxesOneParam.cpp \
    HfcEvolWrapperAllTrayTypes.cpp \
    HfcEvolWrapperCameraCalibration.cpp \
    HfcEvolWrapperCameraHeightScan.cpp \
    HfcEvolWrapperCameraScan.cpp \
    HfcEvolWrapperCamerasTracking.cpp \
    HfcEvolWrapperCameraThreshold.cpp \
    HfcEvolWrapperChangeTrayTypePrecedence.cpp \
    HfcEvolWrapperConfigLabelWidthUpdate.cpp \
    HfcEvolWrapperConfig.cpp \
    HfcEvolWrapperControl.cpp \
    HfcEvolWrapperCycleCount.cpp \
    HfcEvolWrapperErrorHistory.cpp \
    HfcEvolWrapperErrorHistoryList.cpp \
    HfcEvolWrapperEventCapture.cpp \
    HfcEvolWrapperHeightBox.cpp \
    HfcEvolWrapperHeightBoxParam.cpp \
    HfcEvolWrapperKnifeTimeHist.cpp \
    HfcEvolWrapperKnifeTimeHistory.cpp \
    HfcEvolWrapperInquire.cpp \
    HfcEvolWrapperLoad.cpp \
    HfcEvolWrapperMotorProfile.cpp \
    HfcEvolWrapperPackageDesc.cpp \
    HfcEvolWrapperPackageHistory.cpp \
    HfcEvolWrapperPackageTimings.cpp \
    HfcEvolWrapperPackageTimingsPointer.cpp \
    HfcEvolWrapperPkgDetectorStatus.cpp \
    HfcEvolWrapperRemoveUpgradeFiles.cpp \
    HfcEvolWrapperRenameHeightBox.cpp \
    HfcEvolWrapperSaveCycleCount.cpp \
    HfcEvolWrapperSensors.cpp \
    HfcEvolWrapperSettings.cpp \
    HfcEvolWrapperStatus.cpp \
    HfcEvolWrapperSystemInfo.cpp \
    HfcEvolWrapperSystemInfoWrite.cpp \
    HfcEvolWrapperSystemStatus.cpp \
    HfcEvolWrapperTest.cpp \
    HfcEvolWrapperTestComplete.cpp \
    HfcEvolWrapperTrayNames.cpp \
    HfcEvolWrapperTrayType.cpp \
    HfcEvolWrapperWrapBox.cpp \
    HfcEvolWrapperReqUIModel.cpp \
    HfcEvolWrapperUIModel.cpp \
    HfcGCDDevError.cpp \
    HfcGCDDevFileInfo.cpp \
    HfcGCDDevRequestFileUpload.cpp \
    HfcGCDDevStatus.cpp \
    HfcGCDSystemStatus.cpp \
    HfcILARequestSpeed.cpp \
    HfcILASpeed.cpp \
	HfcKPCBootLoaderStatus.cpp \
	HfcKPCLoaderInfo.cpp \
	HfcKPCLoaderProgress.cpp \
	HfcKPCBootUpgraderStatus.cpp \
	HfcKPCUpgradeInfo.cpp \
	HfcKPCUpgradeProgress.cpp \
    HfcLabelApplierConfig.cpp \
    HfcLabelApplierInquire.cpp \
    HfcLabelApplierMode.cpp \
    HfcLabelApplierSensors.cpp \
    HfcLabelApplierSensorsTracking.cpp \
    HfcLabelApplierSetup.cpp \
    HfcLabelApplierStatus.cpp \
    HfcLabelApplierSystemRequestStatus.cpp \
    HfcLabelApplierSystemStatus.cpp \
    HfcLabelApplierTest.cpp \
    HfcLabelPrintingCounts.cpp \
    HfcMarqueeSystemStatus.cpp \
    HfcMarqueeTest.cpp \
    HfcNetworkStatus.cpp \
    HfcNumericSystemStatus.cpp \
    HfcNumericTest.cpp \
    HfcNGWStartUpgrade.cpp \
    HfcNGWUpgradeInfo.cpp \
    HfcNGWUpgradeStatus.cpp \
    HfcNGWUpgradeProgress.cpp \
    HfcNGWUpgradeFinished.cpp \
    HfcNGWUpgradeRequired.cpp \
    HfcOperationModeStatus.cpp \
    HfcPrinterAdvanceLabelStock.cpp \
    HfcPrinterConfig.cpp \
    HfcPrinterCutterStatus.cpp\
    HfcPrinterHeadDotStatus.cpp\
    HfcPrinterHeadDotWear.cpp\
    HfcPrinterHeadType.cpp\
    HfcPrinterInquire.cpp \
    HfcPrinterNewAddress.cpp \
    HfcPrinterPrinterType.cpp \
    HfcPrinterPrinterTypeTracking.cpp \
    HfcPrinterPrintTestLabel.cpp \
    HfcPrinterRequestCalibrate.cpp \
    HfcPrinterRequestInquire.cpp \
    HfcPrinterRequestLabelSizing.cpp \
    HfcPrinterRequestPrintheadPower.cpp \
    HfcPrinterRequestStockMetrics.cpp \
    HfcPrinterSensors.cpp \
    HfcPrinterSensorsTracking.cpp \
    HfcPrinterSetPrinters.cpp \
    HfcPrinterStatus.cpp \
    HfcPrinterStockMetrics.cpp \
    HfcPrinterSystemConfig.cpp \
    HfcPrinterSystemInquire.cpp \
    HfcPrinterSystemRequestStatus.cpp \
    HfcPrinterSystemStatus.cpp \
    HfcPrinterUseContinuousStock.cpp \
    HfcPrintheadPower.cpp \
    HfcPrinterDefaultsComplete.cpp \
    HfcPrinterHeadWear.cpp \
    HfcProductChangeMsg.cpp \
    HfcProductCompleteMsg.cpp \
    HfcProductLookupMsg.cpp \
    HfcProductPackageCompleteMsg.cpp \
    HfcProductStartPrintForTransactionMsg.cpp \
    HfcProductSupportDataMsg.cpp \
    HfcProductUpdateMsg.cpp \
    HfcProductUpdateValuesMsg.cpp \
    HfcRFIDStatus.cpp \
    HfcRFIDTIRISReadOnlyId.cpp \
    HfcScannerProductRead.cpp \
    HfcSystemCalibrationSwitch.cpp \
    HfcSystemConfig.cpp \
    HfcSystemInquire.cpp \
    HfcTimeout.cpp \
    HfcUpgradeList.cpp \
    HfcUpgradePeripheral.cpp \
    HfcUpgradeTransferStatus.cpp \
    HfcUsbWeigherSystemStatus.cpp \
    HfcVmStatusMsg.cpp \
    HfcWeigherCat3AuditRecords.cpp \
    HfcWeigherCat3RecordStatus.cpp \
    HfcWeigherCat3Statistics.cpp \
    HfcWeigherInquire.cpp \
    HfcWeigherMode.cpp \
    HfcWeigherSystemCalibrationSwitch.cpp \
    HfcWeigherSystemConfig.cpp \
    HfcWeigherSystemInquire.cpp \
    HfcWeigherSystemStatus.cpp \
    HfcWeigherSystemValueMax.cpp \
    HfcWeigherDefaultsComplete.cpp \
    HfcWeigherValueMaxFitParams.cpp \
    HfcWrapperConfig.cpp \
    HfcWrapperControl.cpp \
    HfcWrapperCycle.cpp \
    HfcWrapperCycleMode.cpp \
    HfcWrapperErrorHistory.cpp \
    HfcWrapperErrorHistoryTracking.cpp \
    HfcWrapperErrorTotals.cpp \
    HfcWrapperInquire.cpp \
    HfcWrapperIntakeMode.cpp \
    HfcWrapperIntakePkgSize.cpp \
    HfcWrapperLoad.cpp \
    HfcWrapperPackageHistory.cpp \
    HfcWrapperPackageTotals.cpp \
    HfcWrapperPackCodeInfo.cpp \
    HfcWrapperPackCodeNames.cpp \
    HfcWrapperSensors.cpp \
    HfcWrapperSensorsTracking.cpp \
    HfcWrapperSettings.cpp \
    HfcWrapperStatus.cpp \
    HfcWrapperStatusInfo.cpp \
    HfcWrapperStatusTracking.cpp \
    HfcWrapperSystemRequestStatus.cpp \
    HfcWrapperSystemStatus.cpp \
    HfcWrapperTest.cpp \
    HfcWrapperTestTracking.cpp \
    HfcWriteFpmManData.cpp \
    HfcCLARemoveUpgradeFiles.cpp \
    HfcCLASysInfo.cpp \
    HfcCLASysInfoWrite.cpp \
    HfcCLASysStatus.cpp \
    HfcCLATestComplete.cpp \
    HfcWriteFpmMetroVersion.cpp \
    HfcWriteFpmPoundsFor.cpp \
    HfcRequestPrinterLabelCount.cpp \
    HfcPrinterTestCassette.cpp \
    HfcWrPPPDistance.cpp \
    HfcWrPPPCalibration.cpp \
    HfcWrPPPConfig.cpp \
    HfcWrPPPRequest.cpp \
    HfcWrPPPVersion.cpp \
    HfcWrPPPStatus.cpp \
    HfcNGWWrapperPause.cpp \
    HfcWrPPPTest.cpp \
    HfcNGWWrapperResume.cpp \
    HfcNGWDimensionOverrides.cpp \
    HfcPrinterGapCalStatus.cpp \
    HfcPrinterTest.cpp
  
    

#
# define ARCHIVES for 'make testbuild'
#
ARCHIVES := \
    $(HOBART_OUT_DIR)/source_HFC.a \
    $(HOBART_OUT_DIR)/source_HFC_messages.a \
    $(HOBART_OUT_DIR)/source_HFC_reports.a \
    $(HOBART_OUT_DIR)/source_HFC_tags.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_MessageCenter.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_LabelApplierManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_LabelApplierManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_MarqueeManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_MarqueeManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_NumericManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_NumericManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_OperatingModes_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_OperatingModes_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_PngGraphics_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_PrinterManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_PrinterManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_ProductDev_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RendererManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RendererManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RFIDManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RFIDManager_Source.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RTCManager_Messages.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_RTCManager_Source.a \
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
    $(HOBART_OUT_DIR)/source_comm.a \
    $(HOBART_OUT_DIR)/source_comm_cgi.a \
    $(HOBART_OUT_DIR)/source_comm_cgi_http.a \
    $(HOBART_OUT_DIR)/source_comm_email.a \
    $(HOBART_OUT_DIR)/source_comm_email_SMTP.a \
    $(HOBART_OUT_DIR)/source_db_dbd.a \
    $(HOBART_OUT_DIR)/source_db_dbm.a \
    $(HOBART_OUT_DIR)/source_db_util_accountant.a \
    $(HOBART_OUT_DIR)/source_db_util_platform.a \
    $(HOBART_OUT_DIR)/source_db_util_checksums.a \
    $(HOBART_OUT_DIR)/source_db_util_properties.a \
    $(HOBART_OUT_DIR)/source_db_util_filesys.a \
    $(HOBART_OUT_DIR)/source_db_util_e_string.a \
    $(HOBART_OUT_DIR)/source_db_util_e_exception.a \
    $(HOBART_OUT_DIR)/source_db_util_nibble.a \
    $(HOBART_OUT_DIR)/source_db_util_time.a \
    $(HOBART_OUT_DIR)/source_util.a \
    $(HOBART_OUT_DIR)/source_util_Src.a \
    $(HOBART_OUT_DIR)/source_app.a \
    $(HOBART_OUT_DIR)/source_app_Messages.a \
    $(HOBART_OUT_DIR)/source_core.a \
    $(HOBART_OUT_DIR)/source_core_StateMachine.a \
    $(HOBART_OUT_DIR)/source_core_Synchronization.a \
    $(HOBART_OUT_DIR)/source_core_Synchronization_Commands.a \
    $(HOBART_OUT_DIR)/source_core_Synchronization_Messages.a \
    $(HOBART_OUT_DIR)/source_core_VersionManager_messages.a \
    $(HOBART_OUT_DIR)/source_core_VersionManager_source.a \
    $(HOBART_OUT_DIR)/PAL_PAL_linux.a \

#    $(HOBART_OUT_DIR)/source_comm_cgi_http.a \


#    $(HOBART_OUT_DIR)/source_db_util_properties.a \
#    $(HOBART_OUT_DIR)/source_db_util_cloptions.a \
#    $(HOBART_OUT_DIR)/source_util.a \
#    $(HOBART_OUT_DIR)/source_EvolutionRTC_MessageCenter.a \
#    $(HOBART_OUT_DIR)/source_HFC.a \
#    $(HOBART_OUT_DIR)/source_db_dbm.a \
#    $(HOBART_OUT_DIR)/source_db_std_access.a \
#    $(HOBART_OUT_DIR)/source_db_dbd.a \
#    $(HOBART_OUT_DIR)/source_db_util_accountant.a \
#    $(HOBART_OUT_DIR)/source_db_util_platform.a \
#    $(HOBART_OUT_DIR)/source_db_util_checksums.a \
#    $(HOBART_OUT_DIR)/source_db_util_filesys.a \
#    $(HOBART_OUT_DIR)/source_db_util_e_string.a \
#    $(HOBART_OUT_DIR)/source_db_util_e_exception.a \
#    $(HOBART_OUT_DIR)/source_db_util_time.a \
#    $(HOBART_OUT_DIR)/source_util.a \
#    $(HOBART_OUT_DIR)/source_util_Src.a \
#    $(HOBART_OUT_DIR)/PAL.a \
#    $(HOBART_OUT_DIR)/PAL_PAL_linux.a \


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
