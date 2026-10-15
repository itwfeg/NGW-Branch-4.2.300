##############################################################
#
# File : makefile
#
# Make file used for building HFC/
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
    devices \
    drivers \
    messages \
    reports \
    tags

C_SRCS :=
CPP_SRCS := \
    AbstractGraphicManager.cpp \
    AccessKeys.cpp \
    AllBarcodeScanners.cpp \
    AllCat3AuditTrailRecords.cpp \
    AllCategories.cpp \
    AllCoolCategories.cpp \
    AllCoolCountries.cpp \
    AllCoolPretexts.cpp \
    AllCoolProductionSteps.cpp \
    AllCoolTexts.cpp \
	AllDepts.cpp \
    AllEmailAlerts.cpp \
    AllExpirationLabels.cpp \
    AllExternalPrinters.cpp \
    AllGenericLabelTexts.cpp \
    AllOperators.cpp \
    AllProductTextCfgs.cpp \
    AllRecalls.cpp \
    AllSpecials.cpp \
    BackupRestoreErrors.cpp \
    BackupRestoreInitiateReadAllMsg.cpp \
    BackupRestoreManager.cpp \
    BackupRestoreProgress.cpp \
    BackupRestoreSelections.cpp \
    BarCodeNumber.cpp \
    BarcodeScannerManager.cpp \
    Batch.cpp \
    BatchManager.cpp \
    Cat3AuditTrailRecord.cpp \
    Category.cpp \
    CategoryGraphicManager.cpp \
    CategoryManager.cpp \
    CfgAccumulateItems.cpp \
    CfgAll.cpp \
    CfgBase.cpp \
    CfgCommInterface.cpp \
    CfgCommScheduled.cpp \
    CfgCurrency.cpp \
    CfgEmail.cpp \
    CfgFrequentShopper.cpp \
    CfgFTP.cpp \
    CfgGenericLabelText.cpp \
    CfgInternal.cpp \
    CfgItem.cpp \
    CfgMarquee.cpp \
    CfgNetwork.cpp \
    CfgOperatorAccess.cpp \
    CfgPrepack.cpp \
    CfgPrintedFields.cpp \
    CfgScale.cpp \
    CfgSelfService.cpp \
    CfgStoreInfo.cpp \
    CfgSupportedLabelLengths.cpp \
    CfgSynchronization.cpp \
    CfgGCD.cpp \
    Class.cpp \
    ClassManager.cpp \
    ClassTotal.cpp \
    ClassTotalManager.cpp \
    ClassTree.cpp \
    ClassTreeBranch.cpp \
    CommHistory.cpp \
    Configuration.cpp \
    ConfigurationAccess.cpp \
    ConfigurationManager.cpp \
    COOLCategory.cpp \
    COOLCategoryCountry.cpp \
    COOLCategoryCountryManager.cpp \
    COOLCategoryManager.cpp \
    COOLCategoryPretext.cpp \
    COOLCategoryPretextManager.cpp \
    COOLCategoryProdStep.cpp \
    COOLCategoryProdStepManager.cpp \
    COOLCountry.cpp \
    COOLCountryManager.cpp \
    COOLList.cpp \
    COOLListText.cpp \
    COOLListTextManager.cpp \
    COOLManager.cpp \
    COOLManagerCategory.cpp \
    COOLManagerCategoryCountry.cpp \
    COOLManagerCategoryPretext.cpp \
    COOLManagerCategoryProductionStep.cpp \
    COOLManagerCountry.cpp \
    COOLManagerPretext.cpp \
    COOLManagerProductionStep.cpp \
    COOLManagerShortlist.cpp \
    COOLManagerText.cpp \
    COOLPretext.cpp \
    COOLPretextManager.cpp \
    COOLProductionStep.cpp \
    COOLProductionStepManager.cpp \
    COOLShortlist.cpp \
    COOLShortlistManager.cpp \
    COOLText.cpp \
    COOLTextManager.cpp \
    CoolTotal.cpp \
    COOLTotalManager.cpp \
    CutTestManager.cpp \
    CutTestTotal.cpp \
    CutTestTotalManager.cpp \
    CuttingTest.cpp \
    CuttingTestReport.cpp \
    CutTestHistoryListing.cpp \
    CutTestDetailsOption1.cpp\
    DataManager.cpp \
    DBAccess.cpp \
	Dept.cpp \
	DeptManager.cpp \
    Device.cpp \
    DeviceManager.cpp \
    DevicesList.cpp \
    DirectoryList.cpp \
    EmailAlertsManager.cpp \
    EvolWrapperErrorHistoryManager.cpp \
    EvolWrapperKnifeHistory.cpp \
    EvolWrapperKnifeHistoryManager.cpp \
    EvolWrapperTimingsManager.cpp \
    ExpirationLabel.cpp \
    ExpirationLabelManager.cpp \
    ExternalCommSupport.cpp \
    FeaturedCategory.cpp \
    FlashFile.cpp \
    FlashFileManager.cpp \
    Font.cpp \
    FooterRecord.cpp \
    FrequentShopper.cpp \
    GenericHfcObject.cpp \
    GenericLabelText.cpp \
    GenericLabelTextManager.cpp \
    Graphic.cpp \
    GraphicFileNames.cpp \
    GraphicManager.cpp \
    HeaderRecord.cpp \
    HfcError.cpp \
    HfcErrorClass.cpp \
    HfcErrorFont.cpp \
    HfcErrorGraphic.cpp \
    HfcErrorIndexNum.cpp \
    HfcHidDeviceEvent.cpp \
    HFCManagerContainer.cpp \
    HfcObject.cpp \
    HfcPeripheralList.cpp \
    HfcPingResponse.cpp \
    HfcProductDescLookup.cpp \
    HfcProductLabelFormSizeMsg.cpp \
    HfcRequestPeripheralList.cpp \
    HfcRequestPing.cpp \
    HfcResponse.cpp \
    HfcStartReport.cpp \
    HfcUpdateStatus.cpp \
    HobAllAccumulatedItems.cpp \
    HobDateTime.cpp \
    HobEmailAlert.cpp \
    HobIpAddress.cpp \
    HobList.cpp \
    HobOperator.cpp \
    HobString.cpp \
    HobToken.cpp \
    HobTokenizer.cpp \
    HobTokenizerBar.cpp \
    HobTokenizerCSV.cpp \
    HobTokenizerUS.cpp \
    HobTokenTagged.cpp \
    HourlyTotal.cpp \
    HourlyTotalManager.cpp \
    ImportManager.cpp \
    ImportSelections.cpp \
    KnifeTimeHistory.cpp \
    LabelGraphicManager.cpp \
    LabelStockDescriptor.cpp \
    LabelType.cpp \
    LabelTypeManager.cpp \
    MultiLanguageText.cpp \
    NotifyLocalBatches.cpp \
    NutriDataColumn.cpp \
    Nutrifacts.cpp \
    NutrifactsManager.cpp \
    OnlineOrderProduct.cpp \
    OperatorChange.cpp \
    OperatorChangeManager.cpp \
    OperatorChangeSummary.cpp \
    OperatorManager.cpp \
    OperatorNote.cpp \
    OperatorNoteManager.cpp \
    OperatorTotal.cpp \
    OperatorTotalManager.cpp \
    Peripheral.cpp \
    PeripheralManager.cpp \
    PresentationMethod.cpp \
    PresentationMethodManager.cpp \
    PlanogramGraphicManager.cpp \
    PrinterInfo.cpp \
    Product.cpp \
    ProductDefault.cpp \
    ProductDefaultManager.cpp \
    ProductGraphicManager.cpp \
    ProductManager.cpp \
    ProductRunTotal.cpp \
    ProductStatistic.cpp \
    ProductText.cpp \
    ProductTextCfg.cpp \
    ProductTextCfgManager.cpp \
    ProductTextManager.cpp \
    ProductTotal.cpp \
    ProductTotalManager.cpp \
    ProductTransaction.cpp \
    ProductTransactionMaster.cpp \
    ProductTree.cpp \
    ProductTreeBranch.cpp \
    ReadBatchesRecord.cpp \
    ReadSoftKeyParams.cpp \
    Recall.cpp \
    RecallManager.cpp \
    RecordTreeBranch.cpp \
    RegisterSecondaryScale.cpp \
    RemotePrintRequest.cpp \
    RemotePrintTemplateRequest.cpp \
    ResponseMessageManager.cpp \
    Scale.cpp \
    ScaleManager.cpp \
    ScaleStatus.cpp \
    Softkey.cpp \
    SoftkeyFeature.cpp \
    SoftkeyFeatureManager.cpp \
    SoftkeyGraphicManager.cpp \
    SoftkeyGroup.cpp \
    SoftkeyGroupManager.cpp \
    SoftkeyManager.cpp \
    SoftkeyScriptManager.cpp \
    SoftkeyStyle.cpp \
    SoftkeyStyleManager.cpp \
    Special.cpp \
    SpecialsGraphicManager.cpp \
    SpecialsManager.cpp \
    Statistics.cpp \
    StreamingGraphic.cpp \
    StreamingGraphicManager.cpp \
    StreamingGraphicsList.cpp \
    SynchronizationChange.cpp \
    SynchronizationChangeManager.cpp \
    SynchronizationRecord.cpp \
    SynchronizationStatus.cpp \
    SystemFontConversionTable.cpp \
    SystemPrinters.cpp \
    Tare.cpp \
    TerminateMessage.cpp \
    TextLinks.cpp \
    Total.cpp \
    TouchScreenCalibrationStep.cpp \
    Transaction.cpp \
    TransactionManager.cpp \
    TransactionType.cpp \
    TransactionTypeManager.cpp \
    VersionInfo.cpp \
    WeigherInfo.cpp \
    WeighModeTotalType.cpp \
    WindowsConfigurationManager.cpp \
    WriteSingleField.cpp

INC_DIRS := \
    -I$(HOBART_APP)/PAL \
    -I$(HOBART_APP)/PAL/PAL_linux \
    -I$(HOBART_APP)/inc/linux \
    -I$(HOBART_APP)/source/app \
    -I$(HOBART_APP)/source/comm \
    -I$(HOBART_APP)/source/comm/email \
    -I$(HOBART_APP)/source/core \
    -I$(HOBART_APP)/source/core/Synchronization \
    -I$(HOBART_APP)/source/core/VersionManager/include \
    -I$(HOBART_APP)/source/db \
    -I$(HOBART_APP)/source/db/util \
    -I$(HOBART_APP)/source/db/util/e_string \
    -I$(HOBART_APP)/source/db/util/filesys \
    -I$(HOBART_APP)/source/db/util/thread \
    -I$(HOBART_APP)/source/EvolutionRTC/MessageCenter \
    -I$(HOBART_APP)/source/EvolutionRTC/BoardSite/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/CodeUpdater/Wrapper/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/DiagnosticsManager/Messages \
    -I$(HOBART_APP)/source/EvolutionRTC/HandsFreeDevice/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/LabelApplierManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/MarqueeManager/Include \
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
    -I$(HOBART_APP)/source/EvolutionRTC/GCDManager/Include \
    -I$(HOBART_APP)/source/HFC \
    -I$(HOBART_APP)/source/HFC/devices \
    -I$(HOBART_APP)/source/HFC/messages \
    -I$(HOBART_APP)/source/HFC/reports \
    -I$(HOBART_APP)/source/HFC/tags \
    -I$(HOBART_APP)/source/util \
    -I$(HOBART_APP)/source/util/Inc \
    -I$(HOBART_APP)/source/MetrologicallySignificant \
    -I$(HOBART_APP)/HobartCustomerHook \
    -I$(HOBART_APP)/../Smbus/driver \
    -I$(HOBART_APP)/source/EvolutionRTC/CanManager/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/CanManager/Messages/Include \
    -I$(HOBART_APP)/source/EvolutionRTC/KPCBootManager/Include/ \
    -I$(HOBART_APP)/source/EvolutionRTC/KPCBootManager/Messages/Include/ \
    -I$(HOBART_APP)/source/EvolutionRTC/KPCBootUpgrader/Include/ \
    -I$(HOBART_APP)/source/EvolutionRTC/KPCBootUpgrader/Messages/Include/ \
    

export INC_DIRS

#
# define ARCHIVES for 'make testbuild'
#
ARCHIVES := \
    $(HOBART_OUT_DIR)/source_app.a \
    $(HOBART_OUT_DIR)/source_app_Messages.a \
    $(HOBART_OUT_DIR)/source_comm.a \
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
    $(HOBART_OUT_DIR)/source_HFC.a \
    $(HOBART_OUT_DIR)/source_HFC_devices.a \
    $(HOBART_OUT_DIR)/source_HFC_messages.a \
    $(HOBART_OUT_DIR)/source_HFC_reports.a \
    $(HOBART_OUT_DIR)/source_HFC_tags.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_MessageCenter.a \
    $(HOBART_OUT_DIR)/source_EvolutionRTC_DiagnosticsManager_Messages.a \
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
    $(HOBART_OUT_DIR)/source_db_dbd.a \
    $(HOBART_OUT_DIR)/source_db_dbm.a \
    $(HOBART_OUT_DIR)/source_db_util_accountant.a \
    $(HOBART_OUT_DIR)/source_db_util_checksums.a \
    $(HOBART_OUT_DIR)/source_db_util_e_string.a \
    $(HOBART_OUT_DIR)/source_db_util_e_exception.a \
    $(HOBART_OUT_DIR)/source_db_util_filesys.a \
    $(HOBART_OUT_DIR)/source_db_util_nibble.a \
    $(HOBART_OUT_DIR)/source_db_util_platform.a \
    $(HOBART_OUT_DIR)/source_db_util_properties.a \
    $(HOBART_OUT_DIR)/source_db_util_time.a \
    $(HOBART_OUT_DIR)/source_util.a \
    $(HOBART_OUT_DIR)/source_util_Src.a \
    $(HOBART_OUT_DIR)/source_MetrologicallySignificant.a \
    $(HOBART_OUT_DIR)/PAL_PAL_linux.a


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk

exe: $(OUT_TARGET)
	@echo Linking $(OUTPUT_DIR)/$(OUTPUT)
	g++ -lrt $(OUT_OBJS) \
	    $(OUT_DIR)/source_EvolutionRTC_MessageCenter.a \
	    $(OUT_DIR)/PAL_PAL_linux.a \
	    $(OUT_DIR)/source_util.a \
	    $(OUT_DIR)/source_util_Src.a \
	    $(OUT_DIR)/source_db_util_e_string.a \
	    $(OUT_DIR)/source_db_util_time.a \
	    $(OUT_DIR)/source_db_util_e_exception.a \
	    $(OUT_DIR)/source_db_util_platform.a \
	    $(OUT_DIR)/source_db_util_checksums.a \
	    $(OUT_DIR)/source_db_util_properties.a \
	    -o main.exe

mytestbuild:
	g++ $(CXX_LFLAGS) \
	    $(OUT_OBJS) \
	    -Wl,--start-group \
	    $(OUT_DIR)/PAL_PAL_main.a \
	    $(ARCHIVES) \
	    -Wl,--end-group \
	    -o main.exe
