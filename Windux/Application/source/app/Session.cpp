#if 0
#define SHOW_BYTE_DETAIL
#endif
/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "MemShow.h"
#include "Session.h"
#include "SessionManager.h"
#include "SessionManagerHelper.h"
#include "SessionMessage.h"
#include "Messages/RebootMessage.h"
#include "Messenger.h"
#include "Ping.h"
#if defined ( _WIN32 )
#include <shellapi.h>
#endif

#include "PrinterStatus.h"
#include "PrinterGeneric.h"

#include "Messages/SynchronizationDevEvent.h"
#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"
#include "SynchronizationStatus.h"
#include "Messages/ProcessHQBatchMessage.h"

#include "Configuration.h"
#include "HobTokenizerCSV.h"
#include "HobDateTime.h"
#include "GenericHfcObject.h"
#include "HfcResponse.h"
#include "HfcEVMMemoryStatistics.h"

#include "MessageFormat.h"

#include "BackupRestoreManager.h"
#include "BackupRestoreSelections.h"
#include "BackupRestoreInitiateReadAllMsg.h"
#include "ImportManager.h"
#include "ImportSelections.h"

#include "ResponseMessageManager.h"
#include "ConfigurationManager.h"
#include "OperatorManager.h"
#include "ProductManager.h"
#include "ClassManager.h"
#include "NutrifactsManager.h"
#include "ProductTextManager.h"
#include "LabelTypeManager.h"
#include "GraphicManager.h"
// This include file is in HFC
#include "messages/TransactionDevEvent.h"
#include "messages/TransactionTypeEvent.h"
#include "TransactionManager.h"
#include "TransactionTypeManager.h"
#include "HourlyTotalManager.h"
#include "OperatorTotalManager.h"
#include "ClassTotalManager.h"
#include "ProductTotalManager.h"
#include "OperatorChangeManager.h"
#include "CutTestDevEvent.h"
#include "CutTestManager.h"
#include "CuttingTest.h"
#include "CutTestTotalManager.h"
#include "ExpirationLabelManager.h"

#include "KpcSystemErrors.h"
#include "KpcBootEvent.h"
#include "KpcBootUpgraderEvent.h"
// Loader
#include "HfcKPCBootLoaderStatus.h"
#include "HfcKPCLoaderInfo.h"
#include "HfcKPCLoaderProgress.h"
#include "KPCBootGeneric.h"
// Upgrader
#include "HfcKPCBootUpgraderStatus.h"
#include "HfcKPCUpgradeInfo.h"
#include "HfcKPCUpgradeProgress.h"
#include "KPCBootUpgraderGeneric.h"

#include "TerminateMessage.h"
#include "UpdateSoftwareAutomatically.h"

#include "PresentationMethod.h"
#include "CategoryManager.h"
#include "Category.h"
#include "SpecialsManager.h"
#include "Special.h"
#include "StreamingGraphic.h"
#include "StreamingGraphicManager.h"

#include "LabelGraphicManager.h"
#include "CategoryGraphicManager.h"
#include "ProductGraphicManager.h"
#include "SoftkeyGraphicManager.h"
#include "SpecialsGraphicManager.h"

#include "Product.h"
#include "ProductTransaction.h"
#include "ProductDefault.h"
#include "LabelType.h"
#include "CfgScale.h"
#include "WeighModeTotalType.h"
#include "Statistics.h"
#include "DirectoryList.h"
#include "DevicesList.h"
#include "NGWGeneric.h"

#include "CommHistory.h"

#include "ReportManager.h"

#include "OperationModeGeneric.h"
#include "OperationModeIdle.h"
#include "OperationModeOperate.h"
#include "OperationModeOperate.h"
#include "OperationModeAutoWeigh.h"
#include "OperationModeSemiWeigh.h"
#include "OperationModeManualWeigh.h"
#include "OperationModeWrapOnly.h"
#include "OperationModeAutoManualApply.h"
#include "OperationModeSupervisor.h"
#include "OperationModeService.h"
#include "OperationModePowerDown.h"
#include "OperationModeProductionEntry.h"
#include "OperationModeAccumulateItems.h"
#include "OperationModeBuildAMeal.h"
#include "HfcOperationModeStatus.h"
#include "LabelApplierReset.h"
#include "LabelApplierRequestInquire.h"
#include "LabelApplierPositionInfo.h"

#include "WeigherEvent.h"
#include "WeigherRequestInquire.h"
#include "WeigherRequestStatus.h"
#include "WeigherCalibration.h"
#include "HfcWeigherStatus.h"
#include "HfcSystemCalibrationSwitch.h"
#include "WeigherCat3PageRequest.h"
#include "WeigherCat3AuditRecords.h"
#include "HfcWeigherCat3Statistics.h"
#include "HfcWeigherCat3AuditRecords.h"
#include "Cat3AuditTrailRecord.h"
#include "AllCat3AuditTrailRecords.h"
#include "HfcWeigherCat3RecordStatus.h"
#include "HfcWeigherConfig.h"
#include "HfcWeigherDefaultsComplete.h"
#include "WeigherSystemRequestStatus.h"

#include "HfcRequestPeripheralList.h"
#include "HfcPeripheralList.h"

#include "HfcPrinterSystemRequestStatus.h"
#include "HfcPrinterStatus.h"
#include "HfcPrinterRequestStockMetrics.h"
#include "HfcPrinterStockMetrics.h"
#include "PrinterRequestStatus.h"
#include "PrinterSystemRequestStatus.h"
#include "HfcStartReport.h"
#include "HfcPrinterPrintTestLabel.h"


#include "HfcILASpeed.h"
#include "NowServingQueueManagementMessage.h"

#include "GenericLabelTextManager.h"

#include "VersionInfo.h"

#include "TimerManager.h"
#include "EmailManager.h"
#include "EmailAlertsManager.h"
#include "NetworkConnectivityEvents.h"
#include "HfcNetworkStatus.h"
#include "cgi/CGICallManagerEvents.h"
#include "Timer.h"
#include "Timeout.h"
#include "HfcTimeout.h"

#include "HfcVmStatusMsg.h"

#include "MarqueeEvent.h"
#include "MarqueeGeneric.h"
#include "HfcMarqueeTest.h"
#include "MarqueeTest.h"
#include "MarqueeOpMode.h"
#include "MarqueeMode.h"
#include "MarqueeUpdate.h"

#include "NumericEvent.h"
#include "HfcNumericTest.h"
#include "NumericTest.h"
#include "NumericOpMode.h"

#include "PeripheralManager.h"

#include "NetworkConnectivityManager.h"

#include "GCDSession.h"
#include "GCDSystemMsg.h"
#include "GCDFtpUpdateMsg.h"

#include "HobList.h"
#include "HobComplexVersion.h"

#include "EvolWrapperTimingsManager.h"

#include "DeviceManager.h"
#include "ProductTextCfgManager.h"
#include "ProductTextCfg.h"
#include "BarcodeScannerManager.h"
#include "HfcScannerProductRead.h"

#include "DeptManager.h"

#include "BuildNumber.h"

#include "Integer.h"
#include "UInteger.h"
#include "Long.h"
#include "Hex.h"
#include "ULong.h"
#include "Bool.h"

#include "DBException.h"
#include "DBManager.h"

#include "WeigherMode.h"
#include "HfcWeigherMode.h"

#include "WrapperEvent.h"

#include "NetworkEvent.h"
#include "HfcRequestPing.h"
#include "HfcPingResponse.h"

#include "CMFile.h"
#include "CMReaderWriterEvolUI.h"
#include "CMReaderWriterEvolTag.h"

#include "Trace.h"

#include "VersionManager.h"
// for testing only, should be removed
#include "ShutdownSystemRequest.h"

#include "file.h"
#include "directory.h"
//#include "dblocale.h"

#include "time.h"

#include "HfcAccessKeyActivation.h"
#include "HfcAccessKeyDeactivation.h"

#include "HobEmailAlert.h"

#include "DBFlashFilesRM.h"
#include "DBFlashFile.h"
#include "FlashFileManager.h"

#include "ScaleStatus.h"

#include "ScaleModel.h"

#include "COOLManager.h"
#include "COOLDevEvent.h"
#include "COOLTotalManager.h"
// This include file is in HFC
#include "messages/COOLTotalEvent.h"
#include "SoftkeyManager.h"
#include "SoftkeyDevEvent.h"
#include "SoftkeyStyle.h"
#include "SoftkeyFeature.h"
#include "SoftkeyGroup.h"

#include "FpmEvent.h"

#include "NumericGeneric.h"
#include "NumericField.h"
#include "NumericAnnunciator.h"
#include "ServiceSpeaker.h"

#include "RFIDEvent.h"
#include "HfcRFIDTIRISReadOnlyId.h"
#include "EvolWrapperKnifeHistory.h"

#include "DBCategoriesRM.h"
#include "DBClassesRM.h"
#include "DBCoolCategoriesRM.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolTextsRM.h"
#include "DBDevicesRM.h"
#include "DBLabelTextsRM.h"
#include "DBNutritionsRM.h"
#include "DBProductsRM.h"
#include "DBProductTextsRM.h"
#include "DBSoftKeysRM.h"
#include "DBSoftKeyFeaturesRM.h"
#include "DBSoftKeyGroupsRM.h"
#include "DBSoftKeyStylesRM.h"
#include "DBSpecialProgramsRM.h"
#include "DBStreamingGraphicsRM.h"
#include "DBEmailAlertsRM.h"
#include "DBLabelsRM.h"
#include "DBProductStatisticsRM.h"
#include "DBROMLabelsRM.h"
#include "DBKnifeTimeHistoryRM.h"

#include "RecallManager.h"
#include "DBRecallsRM.h"

#include "UsbManager.h"

#include "WriteSingleField.h"

#include "WindowsConfigurationEvents.h"
#include "WindowsConfigurationManager.h"

#include "RealTimeController.h"
#include "MarqueeSystemStatus.h"
#include "NumericSystemStatus.h"

#include "HfcProductUpdateMsg.h"
#include "Messages/UIDialogMsg.h"

#include "HfcEvolWrapperControl.h"

// GCD
#include "ProductChangeMsg.h"
#include "APCCommManager.h"

#include "SystemStatusInfo.h"
#include "GCDSystemStatus.h"

#include "SendErrorEmailEvent.h"
#include "SendGcdPlaylistErrorEmailEvent.h"
#include "SendGcdFtpErrorEmailEvent.h"

#include "ProductStatusMsg.h"
#include "ScaleManager.h"

#include <DiagnosticsReport.h>

#include "NGWEvent.h"
#include "CanManager.h"
#include "NGWUpdateManager.h"
#include "GpioDeviceReset.h"
#include "AtmelDeviceHandler.h"

#include "ListCameraSystemPredictions.h"

#include "UsbWeigherSystemStatus.h"

#ifndef returnToLater
#include "LocaleSettings.h"
#endif
#include "ImportEvent.h"
extern "C" STATUS mkbootUSBMSD (char *pDestDev, char *in, BOOL vxCopyApp);

#ifndef NOFBWF
#if defined ( _WIN32 )
#include "fbwfapi.h"
#endif
#endif

#include "HfcHidDeviceEvent.h"

#include "FpmManager.h"
#include "FpmEvent.h"
#include "MtdManufacturingData.h"
#include "HfcWriteFpmManData.h"
#include "HfcRequestPrinterLabelCount.h"
#include "UpdateSoftwareManually.h"

#include "MultiLanguageText.h"
#include "MessageLogger.h"

#include "PythonScripter.h"

#include "PrinterManager.h"
#include "DiagnosticsEvent.h"

#include "ReportEvent.h"


#include "messagefactory.h"
#include "pong.h"
#include "devicelist.h"
#include "devicelistreq.h"
#include "deviceopnamelist.h"
#include "devicecmdresult.h"
#include "deviceconnect.h"
#include "devicedisconnect.h"
#include "devicesendmessage.h"
#include "devicestate.h"
#include "HandsFreeDeviceEvent.h"

#include "KPCBootManager.h"
#include "HfcBoardSiteTestCompleteUI.h"
#include "HfcBoardSitePercentComplete.h"
#include "HfcBoardSiteTestStatus.h"
#include "BoardSiteGeneric.h"
#include "BoardSiteTest.h"

#include "HfcUsbWeigherSystemStatus.h"

#include <HobartCustomerHook.h>

#include "WrapperControl.h"
#include "WrapperConfiguration.h"


using namespace std;

#define MAX_WEBPAGE_FRAME_NUMBER 27

//#define FOCUSFIX_XCOORDINATE 5
//#define FOCUSFIX_YCOORDINATE 595


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Session::_dataType_ = 0;
std::string Session::_dataTypeName_("Session");
#endif
const std::string Session::boardSiteFile_("/etc/hobart/boardsite");
const std::string Session::boardSiteLvdsFile_("/etc/hobart/boardsitelvds");
std::vector<int> Session::remoteReturnNums;
std::vector<int> Session::remoteResubmitNums;


Session::Session(CommMechanism *uiClient, bool /*enableInput*/) : DBSession(uiClient, "Session")
{
    requestedWeigherStatus = false;
    isTryingToDismissKeyboard = false;
    isTryingToCreateKeyboard = false;

#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

	Debug::trace("Session.construct", util_string::ptrtostring(this) + " id = " + getName() + " - Session(CommMechanism *uiClient)");
    init(uiClient);
}

Session::~Session()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif

    shutdownTimerThreads();
    
    Debug::trace("Session.construct", util_string::ptrtostring(this) + " id = " + getName() + " - ~Session destructor");

    if ( registeredForContinuousStatistics_ )
    {
        SessionMessage stopStats(SessionDev, SnStopStatistics);
        stopStats.setFormatType(InternalFormat);
        send(&stopStats, Messenger::DEST_RTC);

        registeredForContinuousStatistics_ = false;
    }

    if (isCommSession())
    {
	    backupDatabase("CommSessionDestructor");


		// SOF-4769: Scheduled Merge - Execute the merge after 'Comm' is complete
		SessionMessage execMerge(SessionDev, snMergeNowAfterComm);
		execMerge.setFormatType(InternalFormat);
		send(&execMerge, Messenger::DEST_RTC);

	}

    if ( checkForExpiredRecords_.joinable() )
    	checkForExpiredRecords_.join();

	ShutdownNicely();

	deregisterAllEvents();

	/* tell the thread to exit */
	m_ExitEvent.EventSet();

    if (currentDateTime)
    {
        delete currentDateTime;
        currentDateTime = NULL;
    }

    if (commHistory)
    {
        delete commHistory;
        commHistory = NULL;
    }

    if (currentStreamingGraphic)
    {
        delete currentStreamingGraphic;
        currentStreamingGraphic = NULL;
    }

    if (responseMessageManager)
    {
        delete responseMessageManager;
        responseMessageManager = NULL;
    }

    if (backupRestoreManager)
    {
        delete backupRestoreManager;
        backupRestoreManager = NULL;
    }

//	DO NOT delete the ImportManager here because we delete it upon receiving an (ImportDev,ImportTerminate) msg.
//	In Linux, we can be in the Session Destructor before we receive the ImportTerminate msg, which is not pretty.
//    if (importManager)
//    {
//        delete importManager;
//        importManager = NULL;
//	    Debug::trace("Session.construct", util_string::ptrtostring(this) + " id = " + getName() + " - ~Session destructor:  importManager = NULL");
//    }

    if (reportManager)
    {
        delete reportManager;
        reportManager = NULL;
    }

    if (currentWeight)
    {
        delete currentWeight;
        currentWeight = NULL;
    }
	if (currentWeigherCat3Statistics)
	{
		delete currentWeigherCat3Statistics;
		currentWeigherCat3Statistics = NULL;
	}
	if (currentAllCat3AuditTrailRecords.notNull())
	{
		//delete currentAllCat3AuditTrailRecords;
		currentAllCat3AuditTrailRecords = 0;
	}

    if (weigherCalibration)
    {
        delete weigherCalibration;
        weigherCalibration = NULL;
    }

	if (currentPrinterStatus.size())
	{
		for (int i = 0; i < (int)currentPrinterStatus.size(); i++)
		{
			if (currentPrinterStatus[i])
			{
				delete currentPrinterStatus[i];
				currentPrinterStatus[i] = NULL;
			}
		}
	}

    if (!printerStockMetrics.empty())
    {
        for (short printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
        {
            if (printerStockMetrics[printerNum])
            {
                delete printerStockMetrics[printerNum];
                printerStockMetrics[printerNum] = NULL;
            }
        }
        printerStockMetrics.clear();
    }

    if ( evolWrapperTimingsManager_ )
    {
        delete evolWrapperTimingsManager_;
        evolWrapperTimingsManager_ = NULL;
    }

//	Not used - old ILA.    if (currentILASpeed)
//    {
//        delete currentILASpeed;
//        currentILASpeed = NULL;
//    }

    if (savedModeChangeMessage)
    {
        delete savedModeChangeMessage;
        savedModeChangeMessage = NULL;
    }

	while ( currentWeightDoomed.size() )
	{
		delete currentWeightDoomed.front();
		currentWeightDoomed.pop_front();
	}
}


void Session::init(CommMechanism *UNUSED(uiClient))
{
    //Debug::setTraceLevel(60);
    Debug::trace("Session", HobString("init() - Session"));

    currentLanguageSelection = PrimaryLanguage;

    currentWebPageFrame = 0;
    currentWebPages.assign(MAX_WEBPAGE_FRAME_NUMBER + 1, "");

    firstLabelLength = 0;
    secondLabelLength = 0;
    secondLabelRotated = 0;

    currentMode = OpModeIdle;
    currentModeOp = OP_UNKNOWN;
    currentModeOpState = OPSTATE_UNKNOWN;
    mostRecentRunMode = OpModeManualWeigh;
	toggleReturnToRunMode = OpModeManualWeigh;

    restoreVersionIndependentData = false;

    evolWrapperTimingsManager_ = 0;

    // The attribute, startPrintKeyPressedTime, does not need explicit initialization


    registeredForContinuousStatistics_ = false;
    statisticsListeners_ = 0;


    accessKeys.setKeys(0);

    constantlyUpdatingDateTime = false;
    currentDateTime = 0;

	constantlyUpdatingNetworkStatus = false;

    commRequestedShutdown_ = false;
    commHistory = 0;

    currentStreamingGraphic = 0;

    responseMessageManager = 0;
    backupRestoreManager = 0;
    importManager = 0;
    reportManager = 0;

    onSelfServiceStartupScreen = false;
    onStartUpScreenInSecondLanguage = false;

    // This Session won't actually own the data this pointer points at,
    // so don't delete it in the destructor.
    autoLogoutTimerId = 0;

    touchScreenSettingType = 1;         // up = 5, 1 = normal

    // The attribute, expiredTimer object, needs no additional initialization beyond its constructor

    mergeFlashFileData = false;


    compactFlashTaskId = 0;
    mkbootStatus = 0;


    currentWeight = 0;
    currentWeigherState = WeigherStateUnknown;

    dualRangeEnabled = false;
	MetSig::setDualRangeDirection(DualRangeUp);

    weigherSwitchPressed = false;
    weigherCalibration = 0;

	currentWeigherCat3Statistics = 0;
	currentAllCat3AuditTrailRecords = 0;
	currentCat3AuditRecordPage = 0;
	currentCat3NextPageRequests = 0;
	currentNonWeigherCat3Records = 0;


	currentPrinterStatus.resize(MaxPrinterTypes);
	for (int i = 0; i < MaxPrinterTypes; i++)
		currentPrinterStatus[i] = 0;

    printerStockMetrics.insert(printerStockMetrics.begin(), MaxPrinterTypes, (HfcPrinterStockMetrics *)NULL);
    prCalibStep = PrNoCalib;
    currentPrinter = FirstPrinter;
//    currentILASpeed = 0;

//    currentPackCode = UndefinedPackCode;
    //The attribute, autoConfigSize, is never used in this class!!
    wrapperSealerBeltOn = false;
    wrapperSealerBeltOff = false;

    virtualKeyboardParams_.validateFunction  = &validateVirtualKeyboard;
    strcpy(virtualKeyboardParams_.myClassName, "ThunderRT6FormDC");
#if !defined ( __linux__ )
    virtualKeyboardParams_.windowInfo.cbSize = sizeof(WINDOWINFO);
#endif      // !__linux__
    virtualKeyboardParams_.windowHandle      = NULL;

    primaryScaleParams_.validateFunction  = &validatePrimaryScaleTitle;
    strcpy(primaryScaleParams_.myClassName, "IEFrame");
    wcscpy(primaryScaleParams_.myScaleTitle, L"HLX Scale Mode");
#if !defined ( __linux__ )
    primaryScaleParams_.windowInfo.cbSize = sizeof(WINDOWINFO);
#endif      // !__linux__
    primaryScaleParams_.windowHandle      = NULL;

    browserScaleParams_.validateFunction  = &validateBrowserScaleTitle;
    strcpy(browserScaleParams_.myClassName, "IEFrame");
    wcscpy(browserScaleParams_.myBrowserTitle, L"Intranet Mode");
#if !defined ( __linux__ )
    browserScaleParams_.windowInfo.cbSize = sizeof(WINDOWINFO);
#endif      // !__linux__
    browserScaleParams_.windowHandle      = NULL;

    startingIntranetBrowser = false;

    skipRFIDLoginAndNotifyUIofRFIDInput = false;
    savedIntranetAddress = "";

    savedModeChangeMessage = 0;

    wrapperSealerBeltPower = false;

	accumulateItemsAddedMasterProduct = false;

	//Sessions want to sleep in Run() (used to sleep in noMessageFromMessengerProcessing())
	sleepInRunFunction = true;


	accumulateItemsAddedMasterProduct = false;		// they must of really wanted this to be false

	receivedCommHeader_ = false;

	previousWirelessStrength_ = -1;

//	HW-196: Wait until the Operator acknowledges that Initialization is complete, instead of using a timer.
	browserBootupDone_ = false;
	peripheralCheckCounter_ = 0;

	waitingForImportTerminate_ = false;

	restoredProducts = false;

	currentAutoBackupDatabaseGroup_ = 0;

	readingCat3AuditTrailForUI = false;
	readingCat3AuditTrailForPrinting = false;
	readingCat3AuditTrailForSavingToDisk = false;
	readingCat3AuditTrailForComm = false;
	readingCat3AuditTrailForHTe = false;
	ipAddressForSavingAuditTrailResponse = "";
	readingCat3WeigherConfigForComm = false;

	currentHandsFreeDevicesInRangeList = 0;
	currentHandsFreeDevicesInRangeOffset = 0;

	hfpdLoggedInOutOfRangeCount = 0;
	hfpdStrongestSignalCount = 0;

    cameraSystemProductRecognitionLoopCount = 0;
    cameraSystemReady = false;

	// SOF-3699
	fakeWeightMode = false;

    cutterInstalled = false;
    //SOF-4475
    wghtDialogSet = false;
    
	previousModeForStartup = (int)OpModeManualWeigh;

	selfServeIgnoreWeightChangeForPrintToScreen = false;
	
    hasProcessedWrStatusMsg = false;
    hasProcessedPrSystemMsg = false;
	
	setStatus(STATUS_ACTIVE);

    #if defined ( __linux__ )
	util_filesystem::File  boardSiteFile( boardSiteFile_ );
	//do we to register for board site msg?
	if ( boardSiteFile.exists() )  {
		Debug::trace("Session.BoardSite", "Register for board site test message.");
		//Phase 0 of ARK board site project.		
		registerEvent( BoardSite, BrdSiteBoardTestCompleteUI );
        registerEvent( BoardSite, BrdSiteReqCompleteResults );
        registerEvent( BoardSite, BrdSitePercentCompete );
	} else {
        util_filesystem::File  boardSiteFileLvds( boardSiteLvdsFile_ );
        if( boardSiteFileLvds.exists( ) ) {
    		Debug::trace("Session.BoardSite", "Register for board site lvds test message.");
            registerEvent( BoardSite, BrdSiteBoardTestCompleteUI );
            registerEvent( BoardSite, BrdSitePercentCompete );
            registerEvent( BoardSite, BrdSiteTestStatus );
        }

    }
    #endif

}


void Session::timeoutProcess( __int32 timer_id )
{
    if ( VirtServiceJumperSet == timer_id )
    {
        util_filesystem::File serviceKeyFile(Configuration::getInstance()->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY), Configuration::getInstance()->getServiceKeyFile());

        Debug::trace("Session.VirtualJumper", HobString("timer timed out removing key path = ") + serviceKeyFile.getFullPath());

        serviceKeyFile.remove();
    }

    // CAMERA
    else if ( CameraSystemProductRecognition == timer_id )
    {
        if (( productManager ) && ( productManager->getCurrentProduct() ))
        {
            Debug::trace( "Session.Camera", "stopping camera due to product" );
            enableCameraSystemProductRecognition( false );
        }
        else
        {
            const int maxLoops( Configuration::getInstance()->getCameraSystemPredictLoops() );

            Debug::trace( "Session.Camera", "read camera system (product detection)" );

            // read camera (predict)
            performHookAction( "Predict", false );

            if ( maxLoops <= ++cameraSystemProductRecognitionLoopCount )
            {
                Debug::trace( "Session.Camera", "stopping camera due to max loops reached" );
                enableCameraSystemProductRecognition( false );
            }
        }
    }
}

bool Session::isVirtualServiceJumperSet()
{
    util_filesystem::Directory evolutionDir("");
    evolutionDir.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY));

    util_filesystem::File serviceKeyFile(evolutionDir.getFullPath(), Configuration::getInstance()->getServiceKeyFile());

    if ( serviceKeyFile.exists() )
    {
        Debug::trace("Session.VirtualJumper", HobString("service Key found") );

        int minsTimeout = Configuration::getInstance()->getServiceKeyRemoveTimeout();
        if (minsTimeout > 0 && !isTimerActive(VirtServiceJumperSet))
        {
            Debug::trace("Session.VirtualJumper", HobString("timeout = ") + Integer::toString(minsTimeout));
            createTimer( minsTimeout * 60 * 1000, VirtServiceJumperSet );
        }
		else if (minsTimeout == 0 && isTimerActive(VirtServiceJumperSet))
        {
            Debug::trace("Session.VirtualJumper", HobString("timeout = ") + Integer::toString(minsTimeout));
            cancelTimer( VirtServiceJumperSet );
        }

        return true;
    }

    return false;
}

bool Session::isVirtualQAServiceJumperSet()
{
    util_filesystem::Directory evolutionDir("");
    evolutionDir.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY));

    util_filesystem::File serviceKeyFile(evolutionDir.getFullPath(), Configuration::getInstance()->getServiceKeyFile());

    if ( serviceKeyFile.exists() && serviceKeyFile.size() == 42 )
    {
        Debug::trace("Session.VirtualJumper", HobString("QA service Key found") );
        return true;
    }
    return false;
}

// CAMERA
void Session::enableCameraSystemProductRecognition( bool enabled )
{
    // check enabled
    if ( CameraDisabled == Configuration::getInstance()->toIntCfg( fieldOffsetCfgCameraSystemTrustLevel ) )
    {
        Debug::trace( "Session.Camera", "camera not enabled" );
        return;
    }
    // camera enabled is primary session only
    else if (( enabled ) && ( !isPrimarySession() ))
    {
        Debug::trace( "Session.Camera", "cannot enable camera on non-primary session" );
        return;
    }
    // only valid for self service scale
    else if (( !Configuration::getInstance()->isSelfServiceScale() ) && ( !Configuration::getInstance()->getCameraSystemAllowedInOperate() ))
    {
        Debug::trace( "Session.Camera", "not self service scale" );
        return;
    }

    const int waitTime( Configuration::getInstance()->getCameraSystemPredictWaitTime() );
    const bool active( isTimerActive( CameraSystemProductRecognition ) );

    Debug::trace( "Session.Camera", "active = " + Integer::toString( active ) + " enabled = " + Integer::toString( enabled ) + " timeout = " + Integer::toString( waitTime ) );

    // enable
    if (( enabled ) && ( !active ) && ( 0 < waitTime ))
    {
        Debug::trace( "Session.Camera", "starting camera system" );
        createTimer( waitTime, CameraSystemProductRecognition, true );

        cameraSystemProductRecognitionLoopCount = 0;

        // read camera (predict)
        performHookAction( "Predict", false );
    }
    // disable
    else if (( !enabled ) && ( active ))
    {
        Debug::trace( "Session.Camera", "stopping camera system" );
        cancelTimer( CameraSystemProductRecognition );
    }
}

// CAMERA
void Session::handleCameraPrediction( const std::vector<std::string>& request )
{
    Debug::trace( "Session.Camera", "camera request size " + Integer::toString( request.size() ) );

    // request structure:
    // 0 - always "PredictResults"
    // 1 - result code
    // 2 - image capture filename
    // 3 - number of predictions
    // 4 - list of predictions (pipe delimited)
    // 5 - number of probabilities
    // 6 - list of probabilities (pipe delimited)

    if ( request.size() < 2 )
        return;

    const int result( std::stoi( request[1] ) );

    Debug::trace( "Session.Camera", "camera result " + Integer::toString( result ) );

    if (( 0 == result ) && ( 4 <= request.size() ))
    {
        static const char delim( '|' );

        const HobString image( request[2] );

        // parse out results
        size_t numResults( std::stoi( request[3] ) );
        HobList results;

        if (( 0 < numResults ) && ( 5 <= request.size() ))
            splitString( request[4], results, delim );

        // parse out probabilities
        size_t numProbs( 0 );
        HobList probs;

        if ( 6 <= request.size() )
        {
            numProbs = std::stoi( request[5] );

            if (( 0 < numProbs ) && ( 7 <= request.size() ))
                splitString( request[6], probs, delim );
        }

        Debug::trace( "Session.Camera", "num predictions " + Integer::toString( numResults ) + " [" + image + "]"  );

        if ( numResults != results.size() )
            Debug::trace( "Session.Camera", "results size mismatch!" );
        else if ( numProbs != probs.size() )
            Debug::trace( "Session.Camera", "probs size mismatch!" );
        else if ( 0 < numResults )
        {
            // log out predictions
            for ( size_t i( 0 ); i < numResults; ++i )
            {
                if ( i < probs.size() )
                    Debug::trace( "Session.Camera", "prediction " + results.getElementAtPosition(i) + " " + probs.getElementAtPosition(i) );
                else
                    Debug::trace( "Session.Camera", "prediction " + results.getElementAtPosition(i) );
            }

            // handle event
            ListCameraSystemPredictionsMessage obj;
            obj.setResult( result );
            obj.setImage( image );
            obj.setProducts( results );
            obj.setConfidenceLevels( probs );

            SessionMessage m( SoftkeyDev, ListCameraSystemPredictions );
            m.setFormatType( InternalFormat );
            m.setHfcObject( &obj );

            dispatchEvent( &m );
        }
    }
}

int Session::send(message *msg, int destination)
{
	int bytesSent = -1;
	if (msg)
	{
		Debug::trace("Session.send", getName() + "Sending message to socket: ");
		const std::string buffer(msg->serialize());
		const uint32_t bufferLen(htonl((uint32_t) buffer.size()));

		//QByteArray a;
		string a;
		a.append(reinterpret_cast<const char*>(&bufferLen), sizeof(bufferLen));
		a.append(buffer.c_str(), buffer.size());

		//const char *serializedData = message->serialize();
		//int serializedLength = message->getSerializedLength();
		//int bytesSent = send(serializedData, serializedLength, destination, shouldDelayAfterSending);
		bytesSent = myMessenger.send(a.c_str(), (int)a.length(), destination);


		//sock_->write(a);
	}
	return bytesSent;
}

/*****************************************************************************
 *
 * This is the method that allows other classes to send messages.
 * This allows the Session to have some control over whether messages
 * should be sent or not.  No one should ask the Session's Messenger directly
 * to send a message; they should go through the Session (via this method).
 *
 * This is an override of the virtual method in AbstractSession class.
 *
 ****************************************************************************/
int Session::send(EvolMessage *message, int destination, bool shouldDelayAfterSending)
{
    HobString debugHeader = HobString("send(): ");
    int bytesSent = -1;

    if (message)
    {
        bool shouldSendMessage = true;
        if (destination == Messenger::DEST_RTC)
        {
            /*
             * Even though any Session, or its managers (ProductManager, OperatorManager, ...)
             * can send a message directly to the RTC, since the creation of the
             * IEventManager interface came about, we still want everyone else to send messages to the RTC
             * via this method, so that we can centrally distinguish remote Sessions from the primary Session;
             *
             * Normally, only the primary Session should be able to send messages to the RTC
             * that actually control the devices.  However, we do allow remote users to register
             * for messages, read device status and config, and other control messages.
             *
             * Any remote session that wants to register for device statuses, or other device messages,
             * can still send 'register' messages to the EventManager via the IEventManager interface.
             *
             * Could still log an info or warning message???
             */

            if (!rtcMessageShouldBeSent(message))
                shouldSendMessage = false;
		}
		if (shouldSendMessage) {
            if(destination == Messenger::DEST_UI) {
                Debug::trace("Session.sendToUI", debugHeader+"Sending UI message to AbstractSession");
            }
            bytesSent = AbstractSession::send(message, destination, shouldDelayAfterSending);
        }
    }
    else
    {
        Debug::trace("Session.send", debugHeader+"ERROR: message is NULL, ignoring");
        // todo - log warning message, trying to send a message without passing in a Message to send.
    }
    return bytesSent;
}


/*
 * This method determines which messages a remote browser can send to the Real-time Controller.
 */
bool Session::rtcMessageShouldBeSent(EvolMessage *msg)
{
    if (!msg)
        return false;

    bool shouldSend = true;
    if (!isPrimarySession())
    {
        EvolDeviceType dev = msg->getDeviceType();
        int ev = msg->getEventType();

        if (getCurrentMode() == OpModeMonitor && ev != SnMonitorModeOff)
            return false;

        // Allow any register or deregister events.
        if (dev == Wrapper)
        {
            if (ev == WrTest || ev == WrControl || ev == WrCycle || ev == WrJog ||
                    ev == WrLoad || ev == WrEvolControl || ev == WrEvolLoad)
                shouldSend = false;
        }
        else if (dev == LabelApplier)
        {
            if (ev == LaTest)
                shouldSend = false;
        }
    }
    return shouldSend;
}


void Session::sendDialogToMonitoringSessions(bool addDialog, ErrorEvent errorDevice, int errorCode, short responseStyle)
{
    if (!monitoringSessions.empty())
    {
        /*
           vector<AbstractSession *>::iterator pos;
           pos = monitoringSessions.begin();
           while (pos != monitoringSessions.end())
           {
           Session *ssn = dynamic_cast<Session *> (*pos);
           if (ssn)
           {
           / * Instead of executing the following directly, we should send a message
         * to that session, via the 'direct' method of sending messages provided
         * by the MessageCenter.
         * /
         ResponseMessageManager *rmm = ssn->getResponseMessageManager();
         if (rmm)
         {
         if (addDialog)
         rmm->addMessage(errorDevice, errorCode, responseStyle);
         else
         rmm->removeMessage(errorDevice, errorCode, responseStyle);
         }
         pos++;
         }
         }
         */

        HobString data(Bool::toString(addDialog));
        data += BrowserFormatSeparator;
        data += Integer::toString(errorDevice);
        data += BrowserFormatSeparator;
        data += Integer::toString(errorCode);
        data += BrowserFormatSeparator;
        data += Integer::toString(responseStyle);

        SessionMessage sendToMonitors(SessionDev, SnPrimaryToMonitoringSessionPopDialog);
        sendToMonitors.setFormatType(InternalFormat);
        GenericHfcObject gho(data);
        sendToMonitors.setHfcObject(&gho);
        send(&sendToMonitors, Messenger::DEST_RTC);
    }
}


void Session::sendMonitorModeMessage(SessionEvent mode)
{
    SessionMessage msg(SessionDev, mode);
    msg.setFormatType(InternalFormat);
    int bytesSent = send(&msg, Messenger::DEST_RTC);
    if (bytesSent < 0)
    {
        // todo - error handling?
        Debug::trace("Session.monitor", "monitor mode MESSAGE COULD NOT BE SENT to RTC");
    }
}

#ifndef NOFBWF
ULONG Session::disableFBWF()
{
	ULONG result = NO_ERROR;
	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::FBWFACTIVATED))
	{
		ULONG current = 0, next = 0;
		result = FbwfIsFilterEnabled(&current, &next);

		if (result == NO_ERROR)
		{
			if (current)
			{
				//disable fbwf upon reboot.
				result = FbwfDisableFilter();
			}
		}
	}
	return result;
}
#endif

void Session::noMessageFromMessengerProcessing()
{
    Debug::trace("Session.nomsg", getName() + HobString(" - No messages received from my messenger."));

#ifndef NOFBWF
	if (!Configuration::getInstance()->toBoolCfgI(CfgInternal::FBWFACTIVATED))
	{
		// Safety check.  If the FBWF is not supposed to be on, and it is, then turn it off and reboot.
		ULONG current = 0, next = 0, result = NO_ERROR;
		result = FbwfIsFilterEnabled(&current, &next);
		if (result == NO_ERROR)
		{
			if (current)
			{
				result = FbwfDisableFilter();

				// reboot the scale
				RebootMessage rebootMsg(false);
				send(&rebootMsg, Messenger::DEST_RTC, false);
			}
		}
	}
#endif

	if (currentModeOp == OP_WAITINGTOFINISHREGISTRATION)
	{
		// We are waiting for the database to be restored after an upgrade.
		Debug::trace("Session", "currentModeOp == OP_WAITINGTOFINISHREGISTRATION");
		if ( SessionManager::getInstance()->isRestoringDB() )
		{
			Debug::trace("Session", "Still waiting for db to be restored ...");
			PAL_absclass::PAL_sleep(2000);
		}
		else
		{
			Debug::trace("Session", "SessionManager indicates that db has been restored");
			currentModeOp = OP_UNKNOWN;

            if (responseMessageManager)
	            responseMessageManager->removeMessage(ErrVersionManagerDev, VersionManagerRestoreIndependentFormat, HfcResponse::RESPONSESTYLE_NOINPUT);


            // FT-804: Let UI know we are done with the restore
			SessionMessage allDoneNow(SessionDev, DBRestoreFinished);
			allDoneNow.setFormatType(BrowserFormat);
			send(&allDoneNow, Messenger::DEST_UI);

			// Finish the registration process
			handleRegistrationMessage();
		}
		return;
	}

	// don't do anything if the scale is shutdown, we are waiting only for the user to press the "Reboot" button on the screen
	if (currentModeOp != OP_SCALESHUTDOWN && currentModeOp != OP_WAITFORDBCLOSEDTEMPORARILY) // && currentMode != OpModeMonitor)
	{
		if (constantlyUpdatingDateTime)
			sendDateTimeMessage();

		if (currentModeOp == OP_WAITINGFORCOMMBEFORESHUTDOWN || currentModeOp == OP_WAITINGFORCOMMBEFORESHUTDOWNANDREBOOT || currentModeOp == OP_WAITINGFORCOMMBEFOREFBWFREBOOT)
		{
			if (!SessionManager::getInstance()->commSessionExists())
			{
				SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
				shutdownMsg.setFormatType(BrowserFormat);
				GenericHfcObject gho;
				if (currentModeOp == OP_WAITINGFORCOMMBEFORESHUTDOWN)  // waiting to shutdown only
					gho.setData(inttostring(ShutdownRequest_TurnOff));
				else if (currentModeOp == OP_WAITINGFORCOMMBEFOREFBWFREBOOT)  // waiting to turn off FBWF and reboot
					gho.setData(inttostring(ShutdownRequest_ConfigureOS));
				else  // waiting to shutdown and reboot
					gho.setData(inttostring(ShutdownRequest_Reboot));
				shutdownMsg.setHfcObject(&gho);
				handleMessage(&shutdownMsg);
			}
		}

		else if (currentModeOp == OP_HANDLETIMEOUT)
			handleTimeout(&expiredTimer, true);

		else if (currentModeOp == OP_PRINTKEYPRESSED)
		{
			/* The print key was pressed, and now I'm waiting for the browser to send back a message
			 * indicating whether the current field was updated, it's new value, and what field was modified.
			 * This data should be returned to the application in a WriteSingleField message.
			 * This functionality allows the user to enter 199 into the unit price field, for example,
			 * and press the PRINT key, instead of the ENTER key.  Since the browser does not receive the PRINT
			 * key (the keyboard driver sends it directly to the application), the field update would never be
			 * completed before printing the label.
			 * Just in case the browser never responds with a WriteSingleField message, this "timer" will
			 * allow me to go ahead and print the label, with the assumption that no data was changed.
			 */
			if ((time((time_t *)NULL) - startPrintKeyPressedTime) > ((time_t)1 /*PRINTKEY_PRESSED_NORESPONSE_TIME*/))
			{
				Debug::trace("Session", getName() + HobString(" - timed out waiting for WriteSingleField from browser."));
				currentModeOp = OP_UNKNOWN;
				SessionMessage msg(ProductDev, ProductPrint);
				handleMessage(&msg);
			}
			else
				Debug::trace("Session", getName() + HobString(" - waiting for WriteSingleField and WeigherStatus from browser."));
		}

		else if (currentModeOp == OP_CHECKFOREXPIREDDBRECORDS)
		{
			Debug::trace("PurgeOldRecords", getName() + HobString(" - Session begin of OP_CHECKFOREXPIREDDBRECORDS"));
			currentModeOp = OP_UNKNOWN;
			SessionMessage msg(SessionDev, SnCheckForExpiredRecords);
			handleMessage(&msg);

			if (!isPrimarySession())
			{
				// Make this session go away.  Checking for expired transactions, product statistics, and expiration labels was all it was needed for.
				PAL_absclass::PAL_sleep(1000);
				//terminate(false);
				//quitGracefully();
				setStatus(STATUS_TERMINATED);
			}
		}

		else if (currentModeOp == OP_PLANOGRAMPROCESSING)
		{
			if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PLANOGRAMSENABLED))
			{
				Debug::trace("Planogram", getName() + HobString(" - Session begin of OP_PLANOGRAMPROCESSING"));
				currentModeOp = OP_UNKNOWN;
				SessionMessage msg(GraphicDev, CheckForNewPlanogramReceived);
				handleMessage(&msg);
			}

			if (!isPrimarySession())
			{
				// Make this session go away.
				PAL_absclass::PAL_sleep(1000);
				//terminate(false);
				//quitGracefully();
				setStatus(STATUS_TERMINATED);
			}
		}

		else if (currentModeOp == OP_PROCESSDOWNLOADEDFILES)
		{
			Debug::trace("ProcessDownloadedFiles", getName() + HobString(" - Session begin of OP_PROCESSDOWNLOADEDFILES."));
//			currentModeOp = OP_UNKNOWN;		//	We already do this for 'case SnProcessDownloadedFiles'
			SessionMessage msg(SessionDev, SnProcessDownloadedFiles);
			handleMessage(&msg);
		}

		else if (currentModeOp == OP_PROCESSINGDOWNLOADEDFILES)
		{
			// We've created a BackupRestoreManager to process a downloaded file.
			// Now we're just waiting for it to finish before we go process the next file.
			// The currentModeOp will get reset to OP_PROCESSDOWNLOADEDFILES in handleBackupRestoreMessage()
			// when we get the Terminate event from the BackupRestoreManager.
			// So just sleep a little until that happens.

			Debug::trace("ProcessDownloadedFiles", getName() + HobString(" - in OP_PROCESSINGDOWNLOADEDFILES"));
			PAL_absclass::PAL_sleep(1000);
		}

		else if ( currentModeOp == OP_RESTORINGDBINDEPENDENTDATA && !restoreVersionIndependentData && SessionManager::getInstance()->isRestoringDB() )
		{
			Debug::trace("ProcessDownloadedFiles", getName() + HobString(" - Session begin of OP_RESTORINGDBINDEPENDENTDATA"));
			SessionMessage msg(SessionDev, SnRestoreIndependentData);
			handleMessage(&msg);
		}

		else if (currentModeOp == OP_AUTOBACKUPDATABASE)
		{
			bool isTracing = Debug::isTracing("AutoBackupDatabase");
			if (isTracing)
				Debug::trace("AutoBackupDatabase", getName() + HobString(" - Session begin of OP_AUTOBACKUPDATABASE"));

			Configuration *cfg = Configuration::getInstance();
			if (!cfg)
				return;

			/*  This is the original implementation of having chobart_d periodically back-up the database.  But then it was determined that HTe and chobart_d backup the 
			 *  database differently, so if you compare CRC's from each backup, which HTe does, the CRC's will never be the same, and so HTe will arrange
			 *  to always update the scale, even when it really doesn't have to.  This was originally implemented for Hyvee.
			 *  So now I just have to send a message to smBackend to backup the database, since that is the same procedure as to how HTe backs it up.
			 *  Leaving this code here in case we ever need to go back to it.
			 *

			Configuration *cfg = Configuration::getInstance();
			if (cfg)
			{
				util_filesystem::Directory autoBackupDir(cfg->toStringCfgI(CfgInternal::AUTOBACKUPDATABASELOCATION));
				if (!autoBackupDir.exists())
					autoBackupDir.create();

				File f(cfg->toStringCfgI(CfgInternal::AUTOBACKUPDATABASELOCATION), "");

				currentModeOp = OP_PROCESSINGAUTOBACKUPDATABASE;

				SessionMessage msg(BackupRestore, Backup);
				BackupRestoreSelections brs;
				brs.setMode(BRMerge);
				brs.setFormat(BREvolution);
				brs.setDisplayStats(false);

				switch ( currentAutoBackupDatabaseGroup_ )
				{
					case 0:
					{
						Debug::trace("AutoBackupDatabase", getName() + string(" - OP_AUTOBACKUPDATABASE - starting to backup CONFIG data") );
						currentAutoBackupDatabaseGroup_++;

						f.setFileName( cfg->toStringCfgI(CfgInternal::AUTOBACKUPCONFIGFILE) );
						brs.setFileName( f.getFullPath() );
						brs.setScaleConfigSelected(true);
						msg.setHfcObject(&brs);
						handleMessage(&msg);
						break;
					}

					case 1:
					{
						Debug::trace("AutoBackupDatabase", getName() + string(" - OP_AUTOBACKUPDATABASE - starting to backup SYNC-CONFIG data") );
						currentAutoBackupDatabaseGroup_++;

						f.setFileName( cfg->toStringCfgI(CfgInternal::AUTOBACKUPSYNCHCONFIGFILE) );
						brs.setFileName( f.getFullPath() );
						brs.setSynchronizeConfigSelected(true);
						msg.setHfcObject(&brs);
						handleMessage(&msg);
						break;
					}

					case 2:
					{
						Debug::trace("AutoBackupDatabase", getName() + string(" - OP_AUTOBACKUPDATABASE - starting to backup STORE-CONFIG data") );
						currentAutoBackupDatabaseGroup_++;

						f.setFileName( cfg->toStringCfgI(CfgInternal::AUTOBACKUPSTORENAMENUMBERFILE) );
						brs.setFileName( f.getFullPath() );
						brs.setStoreDataSelected(true);
						msg.setHfcObject(&brs);
						handleMessage(&msg);
						break;
					}

					case 3:
					{
						Debug::trace("AutoBackupDatabase", getName() + string(" - OP_AUTOBACKUPDATABASE - starting to backup OPERATOR data") );
						currentAutoBackupDatabaseGroup_++;

						f.setFileName( cfg->toStringCfgI(CfgInternal::AUTOBACKUPOPERATORDATAFILE) );
						brs.setFileName( f.getFullPath() );
						brs.setOperatorDataSelected(true);
						msg.setHfcObject(&brs);
						handleMessage(&msg);
						break;
					}

					case 4:
					{
						Debug::trace("AutoBackupDatabase", getName() + string(" - OP_AUTOBACKUPDATABASE - starting to backup LABEL data") );
						currentAutoBackupDatabaseGroup_++;

						f.setFileName( cfg->toStringCfgI(CfgInternal::AUTOBACKUPLABELDATAFILE) );
						brs.setFileName( f.getFullPath() );
						brs.setResourcesSelected(true);
						msg.setHfcObject(&brs);
						handleMessage(&msg);
						break;
					}

					case 5:
					{
						Debug::trace("AutoBackupDatabase", getName() + string(" - OP_AUTOBACKUPDATABASE - starting to backup FLASHKEY data") );
						currentAutoBackupDatabaseGroup_++;

						f.setFileName( cfg->toStringCfgI(CfgInternal::AUTOBACKUPFLASHKEYDATAFILE) );
						brs.setFileName( f.getFullPath() );
						brs.setSoftkeyDataSelected(true);
						msg.setHfcObject(&brs);
						handleMessage(&msg);
						break;
					}

					case 6:
					{
						Debug::trace("AutoBackupDatabase", getName() + string(" - OP_AUTOBACKUPDATABASE - starting to backup PLU data") );
						currentAutoBackupDatabaseGroup_++;

						f.setFileName( cfg->toStringCfgI(CfgInternal::AUTOBACKUPPLUDATAFILE) );
						brs.setFileName( f.getFullPath() );
						brs.setPluDataSelected(true);
						msg.setHfcObject(&brs);
						handleMessage(&msg);
						break;
					}

					default:
					{
						// Tell smBackend to recalculate the checksums for the auto-backup files.
						const string command( PythonScripter::buildPythonCall( Configuration::getInstance()->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), Configuration::getInstance()->toIntCfgI(CfgInternal::SMBACKENDPORT), string("\"makeChecksums|1\"") ) );

						if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED))
						{
							Debug::trace("AutoBackupDatabase", getName() + string(" backup finished, notifying smbackend to update checksums for these files, command: ") + command);
							string connectionStatusOutput;
							PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
							// We don't care about any output from the command.
						}
						else
							Debug::trace("AutoBackupDatabase", getName() + string("python disabled, python script not called, command: ") + command);


						if ( !isPrimarySession() )
						{
							currentModeOp = OP_UNKNOWN;

							Debug::trace("AutoBackupDatabase", getName() + " - Finished processing AutoBackupDatabase.");

							// Make this session go away.
							PAL_absclass::PAL_sleep(1000);
							//terminate(false);
							//quitGracefully();
							Debug::trace("Session.terminate", getName() + " - SnPerformAutoBackupDB:  setStatus(STATUS_TERMINATED).");
							setStatus(STATUS_TERMINATED);
						}
					}
				}
			}

			*  end of the original implementation of auto-backup of the database.
			*/

			// The new way of telling smBackend to process the auto-backup.
			
			// Backgrounded (10-16-23 dont think this is working all too well, with a 3300 PLU scale, the CPU is 88-94% with peaks at 100) )
			const string command(PythonScripter::buildPythonCall(cfg->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), cfg->toIntCfgI(CfgInternal::SMBACKENDPORT), string("\"backupDatabase|1\n\"")));
			
			// Blocking		
			//string command = PythonScripter::buildPythonCall(cfg->toStringCfgI(CfgInternal::PYTHONSENDSMBACKENDMESSAGESCRIPTNAME), cfg->toIntCfgI(CfgInternal::SMBACKENDPORT), string("\"backupDatabase|1\"") );

			if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED))
			{
				if (isTracing)
					Debug::trace("AutoBackupDatabase", getName() + string(" notifying smbackend to backup the database and generate the checksums for the backup files, command: ") + command);
				
				// Talking to Jim in System Test, it didnt seem logical to backup the DB if the frequency is set to None. 
				if (0 == Configuration::getInstance()->toIntCfg(fieldOffsetCfgAutoBackupDBFrequency))
				{
					Debug::trace("AutoBackupDatabase", string("MessageFromMessengerProcessing(): AutoBackupDBFrequency == NONE so EXIT - not now. See comment LAP - 8/10/18 in SessionManager.cpp") );
				//	return;				// if auto updates not turned on - then bail
				}
				else
					Debug::trace("AutoBackupDatabase", string("MessageFromMessengerProcessing(): AutoBackupDBFrequency > 0 so Backup: " + Configuration::getInstance()->toStringCfg(fieldOffsetCfgAutoBackupDBFrequency) ));

				string connectionStatusOutput;
				PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
				if (isTracing)
					Debug::trace("AutoBackupDatabase", getName() + HobString(" - returned from python call to smbackend to backup the database"));

				// We don't care about any output from the command.
			}
			else if (isTracing)
				Debug::trace("AutoBackupDatabase", getName() + string("python disabled, python script not called, command: ") + command);

			if ( !isPrimarySession() )
			{
				currentModeOp = OP_UNKNOWN;

				if (isTracing)
					Debug::trace("AutoBackupDatabase", getName() + " - Finished processing AutoBackupDatabase.");

				// Make this session go away.
				PAL_absclass::PAL_sleep(1000);
				//terminate(false);
				//quitGracefully();
				Debug::trace("Session.terminate", getName() + " - SnPerformAutoBackupDB:  setStatus(STATUS_TERMINATED).");
				setStatus(STATUS_TERMINATED);
			}

		}

		else if (currentModeOp == OP_PROCESSINGAUTOBACKUPDATABASE)
		{
			// We've created a BackupRestoreManager to auto-backup the database.
			// Now we're just waiting for it to finish before we go process the next group of data.
			// The currentModeOp will get reset to OP_AUTOBACKUPDATABASE in handleBackupRestoreMessage()
			// when we get the Terminate event from the BackupRestoreManager.
			// So just sleep a little until that happens.

			Debug::trace("AutoBackupDatabase", getName() + HobString(" - in OP_PROCESSINGAUTOBACKUPDATABASE"));
			PAL_absclass::PAL_sleep( (currentAutoBackupDatabaseGroup_ > 4) ? 3000 : 1000);
		}

		else if (currentModeOp == OP_INITHANDSFREEDEVICEMGR)
		{
			// Set up a timer to tell us when to go read the devices that are in-range
			if (TimerManager::getInstance())
			{
				int timerVal = Configuration::getInstance()->toIntCfgI(fieldOffsetCfgIGetDevicesInLoginRangeInterval);  // value is in seconds
				if (timerVal > 0)
				{
					TimerManager::getInstance()->addTimer(getId(), timerVal, Timer::RepeatInMinutes, timedFuncGetHandsFreeDevicesInLoginRange);
					Debug::trace("HandsFree", "Created 'get hands-free Devices in range' timer: seconds=" + inttostring(timerVal));
				}
			}

			registerEvent(HandsFreeDevice, HFDSendMessage);
			registerEvent(HandsFreeDevice, HFDSendOpNameList);
			registerEvent(TimerDev, TimerExpired);
			registerEvent(SessionDev, SnTerminate);

			SessionMessage msg(HandsFreeDevice, HFDSendOpNameList);
			handleMessage(&msg);

			currentModeOp = OP_UNKNOWN;
		}

        else if (currentModeOp == OP_SENDWRAPPERDATATOAPPHOOK)
        {
            Debug::trace("Session.sendWrapperDataToHook", "Handling OP_SENDWRAPPERDATATOHOOK session mode");
            
            Session *primarySession = SessionManager::getInstance()->getPrimaryConsoleSession();

            // We'll grab what we can from the Configuration and FPM managers, since that data is readily accessible.
            HfcEvolWrapperControl *wrControl = primarySession->getConfigurationManager()->getCurrentEvolWrapperControl();
			MtdNGWWrapperData *wrapperFlashData = FpmManager::getInstance()->readWrapperData();

			// Off the data goes to be processed and sent to the HTe Enterprise app hook.
			sendWrapperDataToHook(wrControl);
			sendWrapperDataToHook(wrapperFlashData);

            // Get this session listening for WrEvolStatus messages, then send a request for that message to the wrapper.
            // Ideally we'd fetch this directly from some other object, as seen above, but I don't think that's an option at present.
            registerEvent(Wrapper, WrEvolStatus);
            sendWrapperWrReqStatusMsg();
            
            Debug::trace("Session.sendWrapperDataToHook", "Sending messages to printers to get version information.");
            registerEvent(PrinterSystem, SysStatus);
            sendPrinterSysReqStatusMsgs(this);
            // PrinterSystemRequestStatus pr1Msg(this, FirstPrinter);
            // PrinterSystemRequestStatus pr2Msg(this, SecondPrinter);
            // dispatchEvent(&pr1Msg);
            // dispatchEvent(&pr2Msg);

            // See whether the session processed the messages we just sent (or at least *some* messages).
            // If so, great; if not, we'll wait a little bit and see whether it's processed sometime shortly thereafter.
            // We don't want to keep this session running forever though -- hence the relatively short 15-second wait.
            int statusAttempts = 0;
            while ( !hasProcessedWrStatusMsg )
            {
                Debug::trace("Session.sendWrapperDataToHook", "Waiting on WrEvolStatus message to be processed...");
                PAL_absclass::PAL_sleep(5000);
                statusAttempts++;
                if (statusAttempts == 3)
                {
                    Debug::trace("Session.sendWrapperDataToHook", "Tired of waiting on WrEvolStatus message; moving on");
                    break;
                }
            }
            int prSystemAttempts = 0;
            while ( !hasProcessedPrSystemMsg )
            {
                Debug::trace("Session.sendWrapperDataToHook", "Waiting on Printer SysStatus message to be processed...");
                PAL_absclass::PAL_sleep(5000);
                prSystemAttempts++;
                if (prSystemAttempts == 3)
                {
                    Debug::trace("Session.sendWrapperDataToHook", "Tired of waiting on Printer SysStatus message; moving on");
                    break;
                }
            }

            // The primary session should *never* be running this -- we construct a helper session specifically for this mode --
            // but just to be sure we don't accidentally cull the main session:
            if ( !isPrimarySession() )
            {
				setStatus(STATUS_TERMINATED);
                Debug::trace("Session.terminate", getName() + " - SnSendWrapperDataToHook:  setStatus(STATUS_TERMINATED).");
            }

        }
/*

		if (isPrimarySession() && Configuration::getInstance()->isSelfServiceScale() && isInOperateMode() &&
			 currentModeOp != OP_ENTERINGSUPERVISORMODE && restoreVersionIndependentData == false &&
			 accessKeys.isAnyAccessKeyPresent() )
		{
			Debug::trace("Session", getName() + HobString(" - in operate mode, supervisor key found, so switching to Supervisor mode."));
			responseMessageManager->removeAllMessages();

			//SessionMessage super(OperationMode, OpModeSupervisor);
			//super.setFormatType(BrowserFormat);
			//send(&super, Messenger::DEST_UI);

			//Debug::trace("Session", getName() + HobString(" -- 2 -- currentModeOp: ") + HobString(Integer::toString(currentModeOp)));
			//currentModeOp = OP_ENTERINGSUPERVISORMODE;
		}
*/
	}
}

void Session::sessionMessageProcess( SessionMessage * msg )
{
    try
    {
        if (Debug::isTracing("Session.receive"))
        {
            const HobString &m = msg->getOriginalBuff();
#ifdef SHOW_BYTE_DETAIL
            HobString h = getName() + HobString(" - message received: ") + HobString(Hex::toString(m.data(), (int)m.size()));
            Debug::trace("Session.receive", h);
#endif
            if ( msg->getFormatType() == BrowserFormat || isCommSession() )
            {
#ifndef SHOW_BYTE_DETAIL
				if (msg->getEventType() != SnDebugWrite)
				{
#endif
                HobString h2 = getName() + HobString(" - message received: ") + m;
                Debug::trace("Session.receive", h2);
#ifndef SHOW_BYTE_DETAIL
				}
#endif
            } else if (Debug::isTracing("Session.receiveAll")) {
				HobString h = getName() + HobString(" - message received: ") + HobString(Hex::toString(m.data(), (int)m.size()));
				Debug::trace("Session.receiveAll", h);
			}
        } 

        if (currentModeOp == OP_SCALESHUTDOWN)
        {
            Debug::trace("Session", getName() + HobString(" scale is shutdown, we only accept the message that initiates a reboot, or SnDialogResize"));
            if (msg->getEventType() == ErrRemoveAnyModalDialog)
            {
                HfcResponse *response = dynamic_cast<HfcResponse *> (msg->getHfcObject());

                if ( response )
                {
                    bool powerDownOnly = (response->getAnswer() == 0);
                    Debug::trace("Session", string("The user requested to") +
                            (powerDownOnly ? "power down" : "restart") + " the scale");

                    // reboot the scale
                    RebootMessage rebootMsg(response->getAnswer() == 0);
                    send(&rebootMsg, Messenger::DEST_RTC, false);
                }
            }
        }
        else
        {
            handleMessage(msg);

            if (currentModeOp == OP_PRINTKEYPRESSED && Configuration::getInstance()->isSelfServiceScale())
            {
                /* For self serve scales, this will handle not sending the message to the browser to ask
                 * for any field updates for which the user hasn't pressed the Enter key before pressing
                 * the Print key (since this situation can't happen in self-serve mode).
                 * So, when prepareToPrint() is called by the ProductManager, when the current message is
                 * finished processing, the label will now be printed.
                 */
                Debug::trace("Session", getName() + HobString(" - SelfService:  bypassing message to browser to get any field updates."));
                currentModeOp = OP_UNKNOWN;
                SessionMessage pp(ProductDev, ProductPrint);
                handleMessage(&pp);
            }
        }
    }
    catch (DBException &dbe)
    {
        //              Debug::trace("DBAccess.exception", HobString("Session::activeFunctionTask() ********** database exception **********"));
        //              OutputDebugString( L"DBAccess.exception\r\n" );
        showDatabaseException("Session.sessionMessageProcess(" + util_string::inttostring(msg->getDeviceType()) + "," +
                util_string::uinttostring(msg->getEventType()) + ")", dbe);
    }
	catch(std::exception se)
    {
		if (Configuration::getInstance()->toBoolCfgI("exceptionsDisplayDialog"))
			SessionManagerHelper::getInstance()->addDialog(0, 1, se.what());

        LogCatchAndThrow;
    }
    catch(... )
    {
        Debug::trace("Session.error", HobString("sessionMessageProcess():  catch(...)"));
        LogCatchAndThrow;
    }

    Debug::trace("Session", getName() + " - finished processing message (" + util_string::inttostring(msg->getDeviceType()) +
            "," + util_string::uinttostring(msg->getEventType()) + ")");
}


void Session::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

    if (currentMode == OpModeMonitor && msg->getFormatType() == BrowserFormat)
    {
        int dev = msg->getDeviceType();
        int ev = msg->getEventType();
        if (dev == OperationMode ||
                (dev == SessionDev && ev == SnMonitorModeOff) ||
                (dev == SessionDev && ev == SnTerminate))
            ;
        else
        {
            Debug::trace("Session.monitor", "Ignoring message from the UI (Browser) - we are in monitor mode!!");
            return;
        }
    }
	else if (currentModeOp == OP_WAITINGFORREMOTELOGIN)
	{
		bool isTracing = Debug::isTracing("Session");
		if (msg->getFormatType() == BrowserFormat && msg->getDeviceType() == OperatorDev && msg->getEventType() == OpRemoteLogin)
		{
			if (isTracing)
				Debug::trace("Session", "Received remoteLogin message when we were waiting for it ...");
			bool remoteUserLoggedIn = false;

			GenericHfcObject *logindata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
			if (logindata)
			{
				if (Configuration::getInstance()->toIntCfg(fieldOffsetCfgRemoteAccessLoginMethod) == RemoteAccessLoginMethod_OperatorID)
				{
					HobString opID = logindata->getData();
					opID.unQuoteForCSV();

					if (isTracing)
						Debug::trace("Session", "Using OperatorID method to login: " + opID);

					HobOperator oper;
					oper.setOpNum(opID);
					oper.setCurrentMode(HobOperator::CURRENTMODE_OPERATE);

					SessionMessage loginMsg(OperatorDev, OpLogin);
					loginMsg.setFormatType(BrowserFormat);
					loginMsg.setHfcObject(&oper);
					operatorManager->handleMessage(&loginMsg);

					if (operatorManager->getLoggedInOperator())
					{
						remoteUserLoggedIn = true;
						if (isTracing)
							Debug::trace("Session", "Successfully logged in OpID");
					}
				}
				else if (Configuration::getInstance()->toIntCfg(fieldOffsetCfgRemoteAccessLoginMethod) == RemoteAccessLoginMethod_UsernamePassword)
				{
					string userNameAndPassword = logindata->getData();
					HobTokenizerCSV tok(&userNameAndPassword, BrowserFormatSeparator);
					HobString userName, password;
					if (tok.next(&userName) != -1 && tok.next(&password) != -1)
					{
						userName.unQuoteForCSV();
						password.unQuoteForCSV();

						if (isTracing)
							Debug::trace("Session", "Using username/password method to login: " + userName + " / "+ password);

						if (userName == Configuration::getInstance()->toStringCfg(fieldOffsetCfgRemoteAccessUsername) &&
							password == Configuration::getInstance()->toStringCfg(fieldOffsetCfgRemoteAccessPassword))
						{
							remoteUserLoggedIn = true;
							if (isTracing)
								Debug::trace("Session", "Successfully logged in username/passward");
						}
					}
				}
			}

			if (remoteUserLoggedIn)
			{
				currentModeOp = OP_UNKNOWN;

				// Send the Registration confirmation message to the UI to indicate
				// to the UI it can continue on since the remote user logged in.
				SessionMessage registerResponse(SessionDev, SnRegister);
				registerResponse.setFormatType(BrowserFormat);
				send(&registerResponse, Messenger::DEST_UI);
			}
			else
			{
				if (isTracing)
					Debug::trace("Session", "Remote login failed!");

				//If not successful, retell the UI that remote login is required.
				SessionMessage loginRequiredMsg(OperatorDev, OpRemoteLoginRequired);
				loginRequiredMsg.setFormatType(BrowserFormat);
				GenericHfcObject gho(Configuration::getInstance()->toStringCfg(fieldOffsetCfgRemoteAccessLoginMethod));
				loginRequiredMsg.setHfcObject(&gho);
				send(&loginRequiredMsg, Messenger::DEST_UI);

/* If we want to tell the UI if remote login was successful or not, use this code:
			// Let the UI know if the login was successful or not.
			SessionMessage remoteLoginResult(OperatorDev, OpRemoteLoginResult);
			remoteLoginResult.setFormatType(BrowserFormat);
			GenericHfcObject gho((remoteUserLoggedIn) ? "1" : "0");
			remoteLoginResult.setHfcObject(&gho);
			send(&remoteLoginResult, Messenger::DEST_UI);
*/
			}

		}
		else if (isTracing)
			Debug::trace("Session", "Received a message while we were waiting for remoteLogin message, ingoring it - dev:" + inttostring(msg->getDeviceType()) + "  evt:" + inttostring(msg->getEventType()));


		// While we are waiting for the remote user to login, every message except for the login message is consumed (ignored).
		return;
	}

    // This is used to allow a specific handleMessage method to be called to handle the received message.
    // It was originally used for the BackupRestoreManager to have its Session read the file during a Restore, but this is no longer done.
    // It is still here for future use.
    if (msg->getMessageHandler())
    {
        msg->getMessageHandler()->handleMessage(msg);
    }
    else
    {
        Debug::trace("Session", "Handle message " +
			util_string::uinttostring(msg->getDeviceType()) + "," + 
			util_string::uinttostring(msg->getEventType()) );

        switch(msg->getDeviceType())
        {
            case SynchronizationDev:
            {
                if (msg->getEventType() == synchronization::SynchronizationStatusUpdateEvent &&
					( isInSupervisor() || Configuration::getInstance()->toBoolCfgI(CfgInternal::SYNCHRONIZATIONSTATUSMESSAGESENABLED) ))
                {
                    GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
                    if (msgdata)
                    {
                        string errNumString = msgdata->getData();
                        long errNum = Long::valueOf(errNumString);
                        //if (responseMessageManager)
                        //    responseMessageManager->removeAllMessages(ErrUnknown, HfcResponse::RESPONSESTYLE_STATUS);
						if (errNum != synchronization::ClearStatusUpInHere)
						{
							addDialogMessage(ErrSynchronizationDev, errNum, HfcResponse::RESPONSESTYLE_NOINPUT  /*HfcResponse::RESPONSESTYLE_STATUS*/);
			                PAL_absclass::PAL_sleep(1000);
							removeDialogMessage(ErrSynchronizationDev, errNum, HfcResponse::RESPONSESTYLE_NOINPUT  /*HfcResponse::RESPONSESTYLE_STATUS*/);
						}
                    }
                }
                break;
            }

                // cases from EvolDeviceType in event.h
            case Weigher:
            case WeigherSystem:
                handleWeigherMessage(msg);
                break;

            case Printer:
            case PrinterSystem:
			{
				if (msg->getEventType() == PrDonePostImagingSelfServeBarcodeLabelImage)
				{
					GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
					if (msgdata)
						productManager->donePostImagingSelfServeBarcodeLabelImage( stringtoint(msgdata->getData()) );
					break;
				}
				else
					handlePrinterMessage(msg);
				break;
			}

			case KPCSystem:
			case KPCBootloader:
			case KPCBootUpdater:
				handleKPCMessage(msg);
				break;
            case BoardSite:
                 handleBoardSiteMessage(msg);
                 break;
            case SessionDev:
                handleSessionMessage(msg);
                break;

            case OperatorDev:
                Debug::trace("Session", getName() + HobString(" - handling OPERATOR DataType"));
                if ( (msg->getEventType() == DtReadAll || msg->getEventType() == OpReadNote) && isCommSession() )
                    handleReadAll(msg);
                else if (msg->getEventType() == OpNotifyUIofRFIDInputStart)
                    skipRFIDLoginAndNotifyUIofRFIDInput = true;
                else if (msg->getEventType() == OpNotifyUIofRFIDInputEnd)
                    skipRFIDLoginAndNotifyUIofRFIDInput = false;
                else
                    operatorManager->handleMessage(msg);

                if (msg->getEventType() == OpLogout)
                    productManager->clearCurrentProduct(true);
                break;

            case ProductDev:
			{
				Debug::trace("Session", getName() + HobString(" - handling PRODUCT DataType"));
                Debug::trace("Session.ProductDev", getName() + HobString(" - handling PRODUCT DataType"));

				UINT8 ev = msg->getEventType();

                Debug::trace("Session.ProductDev", getName() + " event type = " + std::to_string(ev));

                if(ev == ProductRead)
                {
                    Debug::trace("Session.ProductDev", getName() + HobString(" - handling ProductRead event"));
                }

				if ((ev == ProductReadAll || ev == ProductReadAllExpirationLabels) && isCommSession())
					handleReadAll(msg);
				else
				{
					if (ev == ProductFinishedPrintingAllLabels && isInAnAccumulatingMode())
						accumulateItemsAddedMasterProduct = false;

					else if (ev == ProductFindSoftKeysByDescription && isPrimarySession())
						// Forward the search string on to any monitoring sessions
						sendToMonitoringSessions(msg, Messenger::DEST_UI);

					else if (ev == ProductAddItemToAccumulatedItems)
					{
						if (isInAnAccumulatingMode() && !accumulateItemsAddedMasterProduct)
						{
							Debug::trace("Session", "We're in OpModeAccumulateItems or OpModeBuildAMeal, now sending the master product to the mode.");
							Product *myProd = new Product();
							if (myProd)
							{
								accumulateItemsAddedMasterProduct = true;
								productManager->setHasAccumulateItems(true);

								myProd->setDBAccess(this);
								myProd->setProductNum(999999);
								myProd->setProductType(Product::PRODUCTTYPE_FIXEDWEIGHT);
								myProd->setDescription("Accumulate Items Master Product");
								if (getCurrentMode() == OpModeBuildAMeal || Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableItemStripMode))
								{ // start with receipt label
									myProd->setLabelType(FirstPrinter, LabelType::findRecord(Configuration::getInstance()->toIntCfgI(CfgInternal::ACCUMITEMSLABELFAMILYSTART), myProd->getDeptNum(), this));
								}
								else
								{ // start with Individual totals label
									myProd->setLabelType(FirstPrinter, LabelType::findRecord(Configuration::getInstance()->toIntCfgI(CfgInternal::ACCUMITEMSTOTALSONLYLABELFAMILYSTART), myProd->getDeptNum(), this)); // FT-673
								}

								//bool labelTypeError = productManager->setUpLabelTypes();
								// The following comes from the call made directly above, which I can't use because it uses the ProductManager's currentProduct.
								LabelType *labelType = myProd->getLabelType(FirstPrinter);
								LabelType *pCorrectedLabelType;
								LabelMediaReturn result = productManager->labelTypeErrorTest(labelType, &pCorrectedLabelType, FirstPrinter);
								if (result == LabelCorrected)
								{
									Debug::trace("Session", HobString("checking accumulate items master product label type - Label type - Found a label in the family to fit stock: PrinterNum = ") + HobString(Integer::toString(FirstPrinter)) + HobString("	OrigNum = ") + HobString(Long::toString(labelType->getLabelTypeNum())) + HobString("	NewNum = ") + HobString(Long::toString(pCorrectedLabelType->getLabelTypeNum())));
									myProd->setLabelType(FirstPrinter, pCorrectedLabelType);
									labelType = myProd->getLabelType(FirstPrinter);
								}

								// now that we have the label type set up, we can set up the barcode data
								if (labelType)
								{
									labelType->generateEmbeddedLabels();

									// The setting up of the barcode number object used to be here, but has been moved to AccumulateItemsMode.cpp,
									// when the Operator presses the print button to print the item-strip label, or the totals label (as per the configuration).
								}

								BarCodeNumber bcn = myProd->getBarCodeNum();
								bcn.setFormatString(EAN13, Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsEANFormatString));

								short barcodeType = (labelType) ? labelType->getBarcodeType() : (short) UPCA;  // This should never happen since the master product should always have a label type, but just in case, don't want to crash.
								bcn.setBarcodeType(barcodeType);
								myProd->setBarCodeNum(bcn);

								ProductTransaction *myTrans = myProd->getCurrentTransaction();
								if (myTrans)
								{
									myTrans->setBarCodeNum(bcn);

									Debug::trace("Session", HobString("entered OpModeAccumulateItems|OpModeBuildAMeal  created a Product*=") + HobString(Hex::toString((long long)myProd)));
									HfcProductUpdateMsg hpum(this, myTrans);
									send(&hpum, Messenger::DEST_RTC);
									Debug::trace("Session", HobString("entered OpModeAccumulateItems|OpModeBuildAMeal  sent Product*=") + HobString(Hex::toString((long long)myProd)));
								}
							}
						}
					}

					if (ProductRestore == ev)
						restoredProducts = true;

					////////////////////////////////////////////////////////////////////////////////////////////
					if ( isRemotePrintSession() && ev == ProductRemotePrintStatus )
					{
						Debug::trace("ProductManager.remotePrint", getName() + " - Session handling ProductDev | ProductRemotePrintStatus(92)");

						GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
						if (msgdata)
						{
							HobString status = msgdata->getData();
							Debug::trace("ProductManager.remotePrint", getName() + string(" - data received: ") + status);

							// Forward message on to smBackend caller
							msg->setFormatType(BrowserFormat);
							send(msg, Messenger::DEST_UI);
							Debug::trace("ProductManager.remotePrint", getName() + " - ProductRemotePrintStatus message sent on to smBackend");

							// now see if we terminate based upon the code, the code is the first value in the bar-separated return string
							std::string::size_type lineindex = status.find('|');
							if (lineindex > 0)
							{
								string status_code = status.substr(0, lineindex);
								Debug::trace("ProductManager.remotePrint", getName() + " - ProductRemotePrintStatus message -  calling terminateRemotePrintConnection" + status_code);
								terminateRemotePrintConnection(util_string::stringtoint(status_code));
							}
						}
						break;
					}

					////////////////////////////////////////////////////////////////////////////////////////////

					productManager->handleMessage(msg);

				}

				break;
			}

            case ProductDefaultsDev:
                Debug::trace("Session", getName() + HobString(" - handling PRODUCT DEFAULTS DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    productManager->handleMessage(msg);
                break;

            case ClassDev:
                Debug::trace("Session", getName() + HobString(" - handling CLASS DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    classManager->handleMessage(msg);
                break;

            case NutrifactsDev:
                Debug::trace("Session", getName() + HobString(" - handling NUTRI FACTS DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    nutrifactsManager->handleMessage(msg);
                break;

            case ExpandedTextDev:
                Debug::trace("Session", getName() + HobString(" - handling EXPANDED TEXT DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    getProductTextManager(psttExpandedText)->handleMessage(msg);
                break;

            case SpecialMessageDev:
                Debug::trace("Session", getName() + HobString(" - handling SPECIAL MESSAGE DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    getProductTextManager(psttSpecialMessage)->handleMessage(msg);
                break;

            case ProductNotesDev:
                Debug::trace("Session", getName() + HobString(" - handling PRODUCT NOTES DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    getProductTextManager(psttProductNote)->handleMessage(msg);
                break;

            case MarqueeTextDev:
                Debug::trace("Session", getName() + HobString(" - handling Marquee TEXT DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    getProductTextManager(psttMarqueeText)->handleMessage(msg);
                break;

            case ProductText5Dev:
                Debug::trace("Session", getName() + HobString(" - handling PRODUCT TEXT 5 DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    getProductTextManager(psttProductText5)->handleMessage(msg);
                break;

            case ProductText6Dev:
                Debug::trace("Session", getName() + HobString(" - handling PRODUCT TEXT 6 DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    getProductTextManager(psttProductText6)->handleMessage(msg);
                break;

            case ProductText7Dev:
                Debug::trace("Session", getName() + HobString(" - handling PRODUCT TEXT 7 DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    getProductTextManager(psttProductText7)->handleMessage(msg);
                break;

            case ProductText8Dev:
                Debug::trace("Session", getName() + HobString(" - handling PRODUCT TEXT 8 DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    getProductTextManager(psttProductText8)->handleMessage(msg);
                break;

            case ProductText9Dev:
                Debug::trace("Session", getName() + HobString(" - handling PRODUCT TEXT 9 DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    getProductTextManager(psttProductText9)->handleMessage(msg);
                break;

            case LabelTypeDev:
                Debug::trace("Session", getName() + HobString(" - handling LABEL TYPE DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    labelTypeManager->handleMessage(msg);
                break;

            case GraphicDev:
                Debug::trace("Session", getName() + HobString(" - handling GRAPHIC DataType"));
                if ((msg->getEventType() == DtReadAll || msg->getEventType() == ReadAllProductGraphics || msg->getEventType() == ReadAllCategoryGraphics ||
                            msg->getEventType() == ReadAllSpecialsGraphics || msg->getEventType() == ReadAllStreamingGraphics || msg->getEventType() == ReadAllSoftkeyGraphics)
                        && isCommSession())
                    handleReadAll(msg);
                else
                    graphicManager->handleMessage(msg);
                break;

            case FontDev:
            {
                HobList fList;
                fList.push_back(HobString("Arial"));
                fList.push_back(HobString("Courier"));
                fList.push_back(HobString("Hobart"));
                fList.push_back(HobString("HobartPlain"));
                fList.push_back(HobString("HobartBold"));
                SessionMessage namesMsg(FontDev, 21);
                namesMsg.setFormatType(BrowserFormat);
                namesMsg.setHfcObject(&fList);
                send(&namesMsg, Messenger::DEST_UI);
                break;
            }

            case CategoryDev:
                Debug::trace("Session", getName() + HobString(" - handling CATEGORY DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    categoryManager->handleMessage(msg);
                break;

            case SpecialsDev:
                Debug::trace("Session", getName() + HobString(" - handling SPECIALS DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    specialsManager->handleMessage(msg);
                break;

            case COOLDev:
                Debug::trace("Session", getName() + HobString(" - handling COOL DataType"));
                Debug::trace("Session.Clare", getName() + HobString(" - handling COOL DataType"));
                if (isCommSession() &&
                        (msg->getEventType() == COOLDevReadAllCategories ||
                         msg->getEventType() == COOLDevReadAllCountries ||
                         msg->getEventType() == COOLDevReadAllProductionSteps ||
                         msg->getEventType() == COOLDevReadAllPretexts ||
                         msg->getEventType() == COOLDevReadAllCoolText ||
                         msg->getEventType() == COOLDevReadAllCoolTextForUltima ||
                         msg->getEventType() == COOLDevReadAllCategoryCountries ||
                         msg->getEventType() == COOLDevReadAllCategoryCountriesForUltima ||
                         msg->getEventType() == COOLDevReadAllCategoryProductionSteps ||
                         msg->getEventType() == COOLDevReadAllCategoryProdStepsForUltima ||
                         msg->getEventType() == COOLDevReadAllCategoryPretexts ||
                         msg->getEventType() == COOLDevReadAllShortlists ||
                         msg->getEventType() == COOLDevReadAllShortlistsForUltima ||
                         msg->getEventType() == COOLDevReadAllCOOLTotalsForUltima))
                    handleReadAll(msg);
                else
                    coolManager->handleMessage(msg);
                break;

            case SoftkeyDev:
                Debug::trace("Session", getName() + HobString(" - handling Softkey DataType"));

                if (( isCommSession() ) &&
                    ( msg->getEventType() == ReadAllSoftkeys || msg->getEventType() == ReadAllSoftkeyGroups ||
                      msg->getEventType() == ReadAllSoftkeyFeatures || msg->getEventType() == ReadAllSoftkeyStyles ))
                {
                    handleReadAll(msg);
                }
                else if (( isInOperateMode() ) && ( msg->getEventType() == LoadAllSoftkeyGroups ))
                {
                    msg->setFormatType(BrowserFormat);
                    send(msg, Messenger::DEST_UI);
                }
                else
                {
                    // CAMERA
                    if ( ListCameraSystemPredictions == msg->getEventType() )
                    {
                        // stop looking for camera predictions when we get one
                        enableCameraSystemProductRecognition( false );

                        if ( CameraLearningModeOnly == Configuration::getInstance()->toIntCfg( fieldOffsetCfgCameraSystemTrustLevel ) )
                            break; // nothing to do, learning mode only
                        else if ( CameraEnabledTrusted == Configuration::getInstance()->toIntCfg( fieldOffsetCfgCameraSystemTrustLevel ) )
                        {
                            ListCameraSystemPredictionsMessage *obj( dynamic_cast<hfc::ListCameraSystemPredictionsMessage*>( msg->getHfcObject() ) );

                            if ( !obj )
                                break;

                            HobList products( obj->getProducts() );
                            HobList conf( obj->getConfidenceLevels() );

                            // if only one product lets pull it up
                            if (( 1 == products.size() ) && ( 1 == conf.size() ))
                            {
                                // retrieve first list item (HobList is weird...)
                                HobString conf0;
                                conf.first( conf0 );

                                // they are certain!
                                if ( "CERTAIN" == conf0 )
                                {
                                    Product prod;
                                    prod.setProductNum( util_string::stringtoint( products.getElementAtPosition( 0 ) ) );

                                    SessionMessage cameraItem( ProductDev, ProductRead );
                                    cameraItem.setFormatType( BrowserFormat );
                                    cameraItem.setHfcObject( &prod );

                                    handleMessage(&cameraItem);

                                    break;
                                }
                            }
                        }
                    }

                    softkeyManager->handleMessage(msg);

    				// The following will check if we should print in "label" mode (semi-weigh mode) if we already have a stable weight on the weigher,
	    			// but we needed some input from the user that prevented us from printing when we originally received the stable weight, such as
		    		// if we had forced fields for which the user had to enter the data.
                    if (( ExecuteSoftkey == msg->getEventType() ) &&
                        ( isPrimarySession() ) && ( isInOperateMode() ) && ( productManager && productManager->getCurrentProduct() ))
                    {
                        Debug::trace("Session", getName() + HobString(" - handling ExecuteSoftkey in operate mode"));

	                    Softkey *key( dynamic_cast<Softkey*>( msg->getHfcObject() ) );

                        if ( !key )
                            Debug::trace("Session", getName() + HobString(" - no softkey object attached!"));
                        else
                        {
                            Debug::trace("Session", getName() + HobString(" - softkey action is ") + longtostring(key->getAction()));

                            if (( Action_ForcedCameraOverride == key->getAction() ) ||
                                ( Action_ChangeLabelPlacementAndPosition == key->getAction() ))
                            {
    						    requestAWeigherStatus();
                            }
                        }
                    }
                }
                break;

            case StreamingGraphicDev:
                Debug::trace("Session", getName() + HobString(" - handling STREAMING GRAPHIC DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    streamingGraphicManager->handleMessage(msg);
                break;

            case OperationMode:
            {
                if (OpModeLabelsPrintedCounts == msg->getEventType())
                {
                    Debug::trace("Session", getName() + HobString(" - handling OpModeLabelsPrintedCounts, sending to UI"));
                    msg->setFormatType(BrowserFormat);
                    send(msg, Messenger::DEST_UI);
                }
                else
                    handleModeMessage(msg);
                break;
            }

            case TransactionTypeDev:
                Debug::trace("Session", getName() + HobString(" - handling TRANSACTION TYPE DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    transactionTypeManager->handleMessage(msg);
                break;

            case GenericTextDev:
                Debug::trace("Session", getName() + HobString(" - handling GENERIC TEXT DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    genericLabelTextManager->handleMessage(msg);
                break;

            case ConfigurationDev:
                if ((msg->getEventType() == CfgStoreInfoRead || msg->getEventType() == DtReadAll) && isCommSession())
                    handleReadAll(msg);
                else if (configurationManager)
                    configurationManager->handleMessage(msg);
                break;

            case Error:
            {
				Debug::trace("Session", getName() + " - handling Error | ErrEOF message");
                Debug::trace("Session.Error", "event type = " + util_string::uinttostring(msg->getEventType()));
                Debug::trace("Session.Error", "device type = " + util_string::uinttostring(msg->getDeviceType()));


				if (msg->getEventType() == ErrEOF)
                {
					// Since smBackend can just close the socket connection (for remote print sessions)
					if (isRemotePrintSession())
					{
						if (getStatus() != STATUS_TERMINATED)
						{
							Debug::trace("Session", getName() + " - isRemotePrintSession and status not STATUS_TERMINATED, so dispatching terminate message");
							EvolMessage terminateMsg(SessionDev, SnTerminate);
							dispatchEvent(&terminateMsg, this);
						}
					}
					else
					{
						SessionMessage shutdownMsg(SessionDev, SnCommEnd);
						handleSessionMessage(&shutdownMsg);
					}
					return;
				}

                HfcResponse *response = dynamic_cast<HfcResponse *> (msg->getHfcObject());
                if (!response || !responseMessageManager)
                    return;

                if (msg->getEventType() == ErrRemoveAnyModalDialog)
                {
                    Debug::trace("Session.Error", "ErrRemoveAnyModalDialog received");

                    responseMessageManager->manageResponseAnswer(response);

                    Debug::trace("Session.Error", "response getCode = " + util_string::uinttostring(response->getCode()));

                    if (response->getCode() == OperatorErrEnterIDorRFID)
                    {
                        if (response->getAnswer() == 1)  // ok pressed
                        {
                            if (operatorManager)
                            {
                                HobOperator op;
                                op.setOpNum(response->getUserInput());
                                if (isInOperateMode())
                                    op.setCurrentMode(HobOperator::CURRENTMODE_OPERATE);
                                else if (isInSupervisor())
                                    op.setCurrentMode(HobOperator::CURRENTMODE_SUPERVISOR);
                                else if (isInService() || getCurrentMode() == OpModeServiceLogin)
                                    op.setCurrentMode(HobOperator::CURRENTMODE_SERVICE);

                                if ( cutTestManager )
                                {
									if ( cutTestManager->isWaitingForLoginToStartCutTest() )
										if ( !cutTestManager->checkForSuspendedCutTests( response->getUserInput() ) )
											break;
								}

                                SessionMessage loginMsg(OperatorDev, OpLogin);
                                loginMsg.setHfcObject(&op);
                                operatorManager->handleMessage(&loginMsg);

                                if ( operatorManager->getLoggedInOperator() && cutTestManager )
                                {
									if ( cutTestManager->isWaitingForLoginToStartCutTest() )
										cutTestManager->initiateCuttingTestOption1();
								}

                                if (operatorManager->getLoggedInOperator() && productManager)
                                {
									bool isServiceCounterScale = Configuration::getInstance()->isServiceCounterScale();		//	HA-185: Do this only for Service counter

									// FT-809
									if (productManager->isWaitingForLoginBeforeAddToAccum() && isServiceCounterScale)
									{
										productManager->printLabel();
										if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgAccumItemsReturnToStartScreen))
											productManager->clearCurrentProduct(true, true);
									}
									else if (productManager->isWaitingForLoginBeforeAddToProductionEntry() && isServiceCounterScale)
									{
										SessionMessage lookupPlu(ProductDev, ProductRead);
										productManager->handleMessage(&lookupPlu);
									}
									else if (productManager->isWaitingForLoginBeforePrinting())
									{
									    //removeDialogMessage(ErrOperatorDev, OperatorErrEnterIDorRFID, HfcResponse::RESPONSESTYLE_PROMPTOKCANCEL);
										SessionMessage printIt(SessionDev, SnPrintKeyPressed);
										productManager->handleMessage(&printIt);
									}
									else if (productManager->isWaitingForLoginBeforeWriteSingleField())
									{
										productManager->continueProcessingWriteSingleField();
									}
									else  // cleanup
									{
										productManager->cancelWaitingForLogin();
									}
                                }
                            }
                        }
                        else  // cancel pressed
                        {
                            productManager->cancelWaitingForLogin();
                        }
                        break;
                    }
                    else if (response->getCode() == OperatorErrRenumberWarning)
                    {
                        // We just ignore the cancel button being pressed.  There is nothing to do.
                        //if (response->getAnswer() == 1)  // ok pressed
                        //      addDialogMessage(ErrOperatorDev, OperatorErrRenumberOperator, HfcResponse::RESPONSESTYLE_PROMPTOKCANCEL);
                        break;
                    }
                    else if (response->getCode() == OperatorErrRenumberOperator)
                    {
                        // We just ignore the cancel button being pressed.  There is nothing to do.
                        if (response->getAnswer() == 1)  // ok pressed
                        {
                            // Have to have a current operator in order to renumber them.  The operatorManager will check.
                            //if (operatorManager)
                            //  operatorManager->renumberCurrentOperator(response->getUserInput());
                        }
                        break;
                    }
                    else if (response->getCode() == ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::ProductionEntryProductsEntered)
                    {
                        // user answered the prompt on whether they want to leave Production Entry mode with Products selected.
                        if (response->getAnswer() == 1)  // ok pressed, leave Production Entry mode, changing to the mode the user indicated before the dialog was displayed.
                        {
                            // use the saved mode change message to change to the mode the operator was trying to change to
                            if (savedModeChangeMessage)
                            {
                                currentModeOp = Session::OP_CONTINUELEAVINGPRODUCTIONENTRYMODE;
                                handleModeMessage(savedModeChangeMessage);
                            }
                            currentModeOp = Session::OP_UNKNOWN;
                        }
                        else  // cancel pressed, stay in Production Entry mode.
                        {
                        }

                        if (savedModeChangeMessage)
                        {
                            delete savedModeChangeMessage;
                            savedModeChangeMessage = 0;
                        }
                        break;
                    }

                    if (currentModeOp == Session::OP_WEIGHERCALIBRATE && weigherCalibration)
                    {
                        handleWeigherCalibration(msg);
                        break;
                    }

					if ( currentModeOp == OP_MEAL_WAITINGTORESOLVEPRINTERSTATUS )
					{
						Debug::trace("Session", "error dismissed and in OP_MEAL_WAITINGTORESOLVEPRINTERSTATUS");
						SessionMessage addIt(ProductDev, ProductAddItemToAccumOrMeal);
						productManager->handleMessage(&addIt);
						break;
					}

                    if (( currentModeOp == Session::OP_MEDIASENSORCALIBRATION ) ||
                        ( currentModeOp == Session::OP_LABELTAKENCALIBRATION ))
                    {
                        if ( handlePrCalib( msg ) )
                            break;
                    }
                    /*
                       else if (currentModeOp == Session::OP_WEIGHERCONFIGURATION)
                       {
                    // The user pressed the cancel button while we were waiting for them to press the calibration switch.
                    // So, leave weigher configuration.
                    Debug::trace("Session", getName() + HobString(" - user pressed cancel while entering weigher configuration, instead of pressing calib switch"));
                    deregisterEvent(WeigherSystem, SysCalibSwitch, msgQId);
                    currentModeOp = Session::OP_UNKNOWN;

                    SessionMessage leaveConfig(Weigher, WgConfigurationEnd);
                    leaveConfig.setFormatType(BrowserFormat);
                    send(&leaveConfig, Messenger::DEST_UI);
                    break;
                    }
                    */
                    else if (response->getErrorDevice() == ErrLabelApplier)
                    {
                        // Don't need to send a reset to the label applier for an overrun error.
                        if (response->getCode() != LaErrPackageOverrunError)
                        {
                            LabelApplierReset lar(this);
                            dispatchEvent(&lar);
                            break;
                        }
                    }

                    else if (response->getErrorDevice() == ErrSessionDev)
                    {
                        if (response->getCode() == SessionDevCommExistingBeforeShutdown)
                        {
                            SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
                            shutdownMsg.setFormatType(BrowserFormat);
                            GenericHfcObject gho;
                            if (response->getAnswer() == HfcResponse::RESPONSEANSWER_OK)  // OK was pressed
                                gho.setData(inttostring(ShutdownRequest_Force));
                            else // CANCEL was pressed
                                gho.setData(inttostring(ShutdownRequest_Cancel));
                            shutdownMsg.setHfcObject(&gho);
                            handleSessionMessage(&shutdownMsg);
                        }
                        else if (response->getCode() == SessionDevShutdownAreYouSure)
                        {
                            if (response->getAnswer() == HfcResponse::RESPONSEANSWER_OK)  // OK was pressed
                            {
								if (accessKeys.isAnyAccessKeyPresent())
								{
									currentModeOp = OP_WAITINGFORSHUTDOWNUSBREMOVAL;
		                            responseMessageManager->addMessage(ErrSessionDev, SessionDevRemoveUSBDrive, HfcResponse::RESPONSESTYLE_NOINPUT);
								}
								else
								{
									SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
				                    shutdownMsg.setFormatType(BrowserFormat);
									GenericHfcObject gho(inttostring(ShutdownRequest_TurnOff));
						            shutdownMsg.setHfcObject(&gho);
							        handleSessionMessage(&shutdownMsg);
								}
							}
                        }
                        else if (response->getCode() == SessionDevRebootImmediate || response->getCode() == SessionDevDBIsCorrupt)
                        {
							SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
		                    shutdownMsg.setFormatType(BrowserFormat);
							GenericHfcObject gho(inttostring(ShutdownRequest_ForceReboot));
				            shutdownMsg.setHfcObject(&gho);
					        handleSessionMessage(&shutdownMsg);
                        }
						else if (response->getCode() == SessionDevShutdownWithoutFBWFAreYouSure ||
									response->getCode() == SessionDevConfigWindowsWithRestrictedLogin)
                        {
                            #if defined ( __linux__ )
								int result = system("pkill firefox");
	
								if (result == -1)
								{
									Debug::trace("Session", "Failed to kill firefox!");
								}																	
							#else
								if (response->getAnswer() == HfcResponse::RESPONSEANSWER_OK)  // OK was pressed
								{
									SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
									shutdownMsg.setFormatType(BrowserFormat);
									GenericHfcObject gho(inttostring(ShutdownRequest_ConfigureOS));
									shutdownMsg.setHfcObject(&gho);
									handleSessionMessage(&shutdownMsg);
								}
							#endif
                        }
						else if (response->getCode() == SessionDevShutdownWithoutFBWFLinuxAreYouSure)
						{
							#if defined ( __linux__ )							
								if (response->getAnswer() == HfcResponse::RESPONSEANSWER_OK)
								{
									//Write out config_os_mode.dat to /home/Hobart/AccessScale
									PAL_absclass::PAL_createFile( "/home/Hobart/AccessScale/config_os_mode.dat",
											PAL_FILE_GEN_RD | PAL_FILE_GEN_WR,
											PAL_FILE_SHARE_RD | PAL_FILE_SHARE_WR,
											NULL,
											PAL_FILE_CREATE_FORCE,
											0,
											NULL);

									//change permission of dat file
									int result = system("chmod 777 /home/Hobart/AccessScale/config_os_mode.dat");
									
									if (result == -1)
									{
										Debug::trace("Session", "Failed to change config_os_mode permissions");
									}
									
									//kill firefox
									result = system("pkill firefox");
		
									if (result == -1)
									{
										Debug::trace("Session", "Failed to kill firefox!");
									}
								}
							#else
								//Do what we do today for Windows -> In case we ever want to run firefox with the Windows OS...
								if (response->getAnswer() == HfcResponse::RESPONSEANSWER_OK)  // OK was pressed
								{
									SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
									shutdownMsg.setFormatType(BrowserFormat);
									GenericHfcObject gho(inttostring(ShutdownRequest_ConfigureOS));
									shutdownMsg.setHfcObject(&gho);
									handleSessionMessage(&shutdownMsg);
								}
							#endif
						}
						else if (response->getCode() == SessionDevConfigWindowsWithoutRestrictedLogin)
						{
							if (response->getAnswer() == HfcResponse::RESPONSEANSWER_OK)  // OK was pressed
							{
								Debug::trace("Session", getName() + HobString("user pressed OK from message SessionDevNoUpdateMessageBeforeDesktop, before calling accessSystemDesktop"));
								accessSystemDesktop();
							}
						}
                        else if (response->getCode() == SessionDevBrowserBootupCompleted)
                        {
                            Debug::trace("Session", getName() + HobString("SessionDevBrowserBootupCompleted"));

//	HW-196: Wait until the Operator acknowledges that Initialization is complete, instead of using a timer.
		                    if (peripheralManager)
							{
					        	Debug::trace("Session.initComplete", getName() + HobString("SessionDevBrowserBootupCompleted:  Set the checkForMissingPeripherals flag and request the Peripheral list."));
			                    peripheralManager->setcheckForMissingPeripherals(true);

		                        HfcRequestPeripheralList reqPeripheralList(this);
		                        dispatchEvent(&reqPeripheralList);
							}

                        	if (isPrimarySession())
							{
	                            // Dispatch a message to any thread that wants to know when the primary UI is completely up and running
	                            SessionMessage shutdownMsg(SessionDev, SnBootupCompleted);
	                            shutdownMsg.setFormatType(InternalFormat);
	                            send(&shutdownMsg, Messenger::DEST_RTC, false);
	                            

								if ( previousModeForStartup == OpModeSelfService )
								{
									SessionMessage mode(OperationMode, previousModeForStartup);     // OpModeAutoManualApply, OpModeManualWeigh
									Debug::trace("Session", "Setting up for mode: " + inttostring(previousModeForStartup));
									handleModeMessage(&mode);

									// Have to send this message here, to blank the gcd when in self-service mode,
									// when going to Self-service mode at bootup.  When it gets sent in
									// handleModeMessage, the gcd wasn't fully set up, so the gcd didn't blank out.
									EvolMessage msgGCD( GCDSessionDev, GcdSessionColorTestBlankScreenOn );
									dispatchMessage(&msgGCD);
								}
	                            
#if defined ( _WIN32 )
	                            //Minimize the appLauncher spalsh so that it
	                            //does not cover the User interface.
	                            HWND target = FindWindow(0, _T("winAppLauncher"));
	                            if (target)
	                            {
	                                ShowWindow(target, SW_MINIMIZE);
	                            }
#endif
                            }
                        }
                        else if (response->getCode() == SessionDevNoUpdateMessageBeforeDesktop)
                        {
                            Debug::trace("Session", getName() + HobString("user pressed OK from message SessionDevNoUpdateMessageBeforeDesktop, before calling accessSystemDesktop"));
                            accessSystemDesktop();
                        }

                        break;
                    }

					else if ( response->getErrorDevice() == ErrWeigher )
					{
						if ( response->getCode() == WeigherErrNegativeGrossWeight )
						{
							// answer is Rezero was pressed
							SessionMessage rezero(Weigher, WgRezero);
							handleMessage(&rezero);
							break;
						}
						else if ( response->getCode() == WeigherErrScaleNotZero )
						{
							//if (response->getAnswer() == HfcResponse::RESPONSEANSWER_CANCEL)  // Ignore was pressed
							//{
							//   requestAWeigherStatus();

							if (productManager)
								productManager->handleMessage(msg);
							//}
							//else // answer is Rezero was pressed
							//{
							//    SessionMessage rezero(Weigher, WgRezero);
							//    handleMessage(&rezero);
							//}
							break;
						}
						else if (response->getCode() == WeigherErrValMaxEepCommFailure || response->getCode() == WeigherErrValMaxEepAcclCommFailure)
						{
							// Add reboot
							SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
							shutdownMsg.setFormatType(BrowserFormat);
							GenericHfcObject gho;
							if (response->getAnswer() == HfcResponse::RESPONSEANSWER_OK)  // OK was pressed
								gho.setData(inttostring(ShutdownRequest_Force));
							else // CANCEL was pressed
								gho.setData(inttostring(ShutdownRequest_Cancel));
							shutdownMsg.setHfcObject(&gho);
							handleSessionMessage(&shutdownMsg);
						}
					}

                    else if (response->getCode() == GCDSessionLostConnectionToGCD)
					{
						if (response->getAnswer() == HfcResponse::RESPONSEANSWER_CANCEL)  // Ignore was pressed
						{
							// Remove the message from the queue - allow operations to continue (except printing)
							// **************************************************************************************************
							// **************************************************************************************************
							// **************************************************************************************************
							// **************************************************************************************************
							// **************************************************************************************************
							// This removeDialogMessage should not be required here, because the message would have been removed 
							// above right after the getEventType() call deterined it was ErrRemoveAnyModalDialog.
							// However, this error message was incorrectly numbered.  The GCDSessionLostConnectionToGCD error
							// starts with 145, which is for the NetworkConnectivityDevError messages, not for ErrGCDSessionDev
							// error messages, so the detection of the "device" incorrectly identified this error to be for the
							// wrong device, and so the ResponseMessageManager would not find the error in the message to remove
							// it.  In the future, any ErrGCDSessionDev message needs to be renumbered to be in the 159000-159999
							// range, and then the following removeDialogMessage() call can be removed.
							// Refer to files: EvolutionRTC\GCDManager\Include\GCDEvent.h, and webPages\MessagesBundle_*.*.
							// **************************************************************************************************
							// **************************************************************************************************
							// **************************************************************************************************
							// **************************************************************************************************
							// **************************************************************************************************
							
							//removeDialogMessage(ErrGCDSessionDev, GCDSessionLostConnectionToGCD, HfcResponse::RESPONSESTYLE_CANCEL);

							/*  No longer anything to do here.  LAP
							// Set the GCD as detached so that we stop attempting to reset the GCD
							if (Configuration::getInstance()->isRealTimeControllerEnabled())
							{
								// Set GCD status in the peripheral list in RealTimeController
								RealTimeController::getInstance()->GCDDetached();
							}
							*/
						}

						// I found that the scale wasn't really locked up, but just unresponsive
						// I asked the QA tester to bring up taskmanager, and just kill the
						// GCDWandMWriter.exe process. Once he killed GCDWandMWriter.exe
						// the scale was 100% responsive and working again just fine.

						// So I had the idea to just programatically kill the exe right here
						 
						// put in a task killing command here for the GCDWandMWriter.exe
						// maybe using a taskkill.exe or something combination. If killing it is valid.

						break;
					}

                    else if (productManager &&  response->getCode() >= ProductErrUnknownError && response->getCode() < 101999 /*ProductErrMaxError*/)
                    {
                        productManager->handleMessage(msg);
                        break;
                    }

                    else if (categoryManager && response->getCode() >= 121000 && response->getCode() < 121999)
                    {
                        if (msg->getFormatType() == BrowserFormat)
                            send(msg, Messenger::DEST_UI);              //      So the Menus will be enabled again.
                        categoryManager->handleMessage(msg);
                        break;
                    }

                    else if (specialsManager && response->getCode() >= 124000 && response->getCode() < 124999)
                    {
                        if (msg->getFormatType() == BrowserFormat)
                            send(msg, Messenger::DEST_UI);              //      So the Menus will be enabled again.
                        specialsManager->handleMessage(msg);
                        break;
                    }

					else if ( response->getCode() == PrinterErrSizedContinuousOK )
					{
						Debug::trace("Session", getName() + " - PrinterErrSizedContinuousOK dialog, user pressed: " + inttostring(response->getAnswer()) );

                        if (response->getAnswer() == HfcResponse::RESPONSEANSWER_CANCEL)  // Cancel pressed
                        {
							// Printer sized for continuous stock, but the user pressed Cancel to the question to verify they are
							// actually using continuous stock, to indicate it's not continuous stock.  This could happen if the labels
							// in the casette are beyond the media sensor, ut not up to the print roller, so the stock never advances
							// and the FPC never sees the bar on the label backing.  So send a message to the PrinterManager telling
							// it to re-setup the teach table for sizing, since it already setup not to size, due to detecting the continuous stock.
							Debug::trace("Session", getName() + " - PrinterErrSizedContinuousOK dialog, user pressed Cancel.");

							PrinterGeneric prNormalSizing(PrSetupForNormalSizing, this, FirstPrinter);
							dispatchEvent(&prNormalSizing);

							// Need to re-save the label length back to 0, since it was already saved as LengthContinuous_Lines in setPrinterStockMetrics()
							Configuration::getInstance()->setCfg( fieldOffsetCfgPrinter1LabelLength, "0" );
							Configuration::getInstance()->saveField( fieldOffsetCfgPrinter1LabelLength, this );
						}
					}

                    //else if (msg->getFormatType() == BrowserFormat)
                    //  send(msg, Messenger::DEST_UI);
                }
                else  // not ErrRemoveAnyModalDialog
                {
                    if (msg->getFormatType() == BrowserFormat)
					{
						bool rmmSend = true;	//	HW-196
						if (response->getCode() == SessionDevShutdownWithoutFBWFAreYouSure)
						{
							if (!Configuration::getInstance()->toBoolCfgI(CfgInternal::FBWFACTIVATED))
							{
								if (Configuration::getInstance()->toBoolCfgI(CfgInternal::USINGRESTRICTEDLOGINS))
									response->setCode(SessionDevConfigWindowsWithRestrictedLogin);
								else
									response->setCode(SessionDevConfigWindowsWithoutRestrictedLogin);
							}
						}
                        else if ( ( response->getCode() == SessionDevShuttingdown) )
                        {
                            if (Configuration::getInstance()->isSelfServiceScale())
                                return;         // if we are self serv and we get a shutdown message - bail
                        }
						else if (response->getCode() == VersionManagerUsbDriveStillPresent)
						{
							if (accessKeys.isAnyAccessKeyPresent())
							{
								Debug::trace("Session", getName() + HobString(" - UI asked for dialog for VersionManagerUsbDriveStillPresent, USB key is present, so display message."));
								currentModeOp = OP_WAITINGFORUSBKEYREMOVAL;
							}
							else
							{
								Debug::trace("Session", getName() + HobString(" - UI asked for dialog for VersionManagerUsbDriveStillPresent, but USB key is NOT present, so don't display message."));
								msg->setEventType(ErrRemoveAnyModalDialog);
								send(msg, Messenger::DEST_UI);
								break;
							}
						}
//	HW-196: Currently Windows is very slow to boot up and messages and peripherals lag behind.
//			We are trying to wait until we see that the weigher has attached
//			in order to send the SessionDevBrowserBootupCompleted to the UI.
						else if (response->getCode() == SessionDevBrowserBootupCompleted)
						{
							rmmSend = false;				//	Don't respond with the message here.
		                    Debug::trace("Session.initComplete", getName() + HobString(" - BrowserBootupCompleted:  browserBootupDone_ = ") + booltostring(browserBootupDone_));
							if (!browserBootupDone_)		//	The UI appears to be sending the message twice, so only do this the first time.
							{
								browserBootupDone_ = true;
								//	Put up the "Initialization and and loading sequence... Please wait" message. We will remove it when we know we are done.
								//	This way we know the user can't do anything on the console until everything is up - or we have timed out.
								responseMessageManager->addMessage(ErrSessionDev, SessionDevBrowserBootupStarted, HfcResponse::RESPONSESTYLE_NOINPUT);
								checkInitializationIsComplete();
							}
						}

                        if (rmmSend)
						{
							responseMessageManager->addMessage(response->getErrorDevice(), (int)response->getCode(), response->getStyle());
						}
					}
                    if (isPrimarySession() && (response->getCode() == SessionDevBrowserBootupCompleted))
                    {
						if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::SYNCHRONIZATIONENABLED) )
						{
							EvolMessage message(SessionDev, SnInitializeSynchronization);
							dispatchEvent(&message);
							
							EvolMessage message1(NetworkConnectivityDev, NetworkSyncStarted);
							dispatchEvent(&message1);
							break;
						}
#if defined ( _WIN32 )
                        // 	Is this left over from WINDUX?  Is it still needed?
                        //Minimize the appLauncher splash so that it
                        //does not cover the User interface.
                        HWND target = FindWindow(0, _T("winAppLauncher"));
                        if (target)
                        {
                            ShowWindow(target, SW_MINIMIZE);
                        }
#endif
                    }
                }
                break;
            }

            case TransactionDev:
            {
                Debug::trace("Session", getName() + " - handling TRANSACTIONS DataType");

                if ( (msg->getEventType() == DtReadAll || msg->getEventType() == TransactionReadAndClear) && isCommSession() )
                    handleReadAll(msg);
                else
                    transactionManager->handleMessage(msg);

                break;
            }

            case HourlyTotalDev:
            {
                Debug::trace("Session", getName() + " - handling HOURLYTOTAL DataType");

                if ( msg->getEventType() == DtReadAll && isCommSession() )
                    handleReadAll(msg);
                else
                    hourlyTotalManager->handleMessage(msg);

                break;
            }


            case OperatorTotalDev:
            {
                Debug::trace("Session", getName() + " - handling OPERATORTOTAL DataType");

                if ( msg->getEventType() == DtReadAll && isCommSession() )
                    handleReadAll(msg);
                else
                    operatorTotalManager->handleMessage(msg);

                break;
            }

            case ClassTotalDev:
            {
                Debug::trace("Session", getName() + " - handling CLASSTOTAL DataType");

                if ( msg->getEventType() == DtReadAll && isCommSession() )
                    handleReadAll(msg);
                else
                    classTotalManager->handleMessage(msg);

                break;
            }

            case ProductTotalDev:
            {
                Debug::trace("Session", getName() + " - handling PRODUCTTOTAL DataType");

                if ( msg->getEventType() == DtReadAll && isCommSession() )
                    handleReadAll(msg);
                else
                    productTotalManager->handleMessage(msg);

                break;
            }

            case COOLTotalDev:
            {
                Debug::trace("Session", getName() + " - handling COOLTotalDev DataType");

                if ( msg->getEventType() == hfc::ReadAllCOOLTotalsForUltima && isCommSession() )
                    handleReadAll(msg);
                else
                    coolTotalManager->handleMessage(msg);

                break;
            }

            case OperatorChangesDev:
            {
                Debug::trace("Session", getName() + " - handling OPERATORCHANGE DataType");

                if ( msg->getEventType() == DtReadAll && isCommSession() )
                    handleReadAll(msg);
                else
                    operatorChangeManager->handleMessage(msg);

                break;
            }

            case EmailAlertsDev:
			{
                Debug::trace("Session", getName() + HobString(" - handling EMAIL ALERTS DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    emailAlertsManager->handleMessage(msg);
				
                break;
			}
            case EmailManagerDev:
			{
                Debug::trace("Session", getName() + HobString(" - handling Email DataType"));
                EmailManager::getInstance()->handleMessage(msg, this);
                break;
			}
            case CGICallManagerDev:
            {
                Debug::trace("Session", getName() + HobString(" - handling CGICallManager DataType"));

                handleCGICallManagerMessage(msg);

                break;
            }

            case PeripheralDev:
                Debug::trace("Session", getName() + HobString(" - handling PERIPHERALDEV DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    peripheralManager->handleMessage(msg);
                break;

            case BackupRestore:
                handleBackupRestoreMessage(msg);
                break;

            case ImportDev:
                handleImportMessage(msg);
                break;

            case ReportManagerDev:
			{
				Debug::trace("Session", 20, "ReportManagerDev call handleReportMessage()");
				handleReportManagerMessage(msg);
                
				break;
			}
            case NetworkConnectivityDev:
                handleNetworkConnectivityManagerMessage(msg);
                break;

			case CutTestDev:
			{
				Debug::trace("Session", getName() + " - handling CutTest DataType");

				if ( msg->getEventType() == DtReadAll && isCommSession() )
					handleReadAll(msg);
				else
					cutTestManager->handleMessage(msg);
				break;
			}

			case CutTestTotalDev:
			{
				Debug::trace("Session", getName() + " - handling CutTestTotal DataType");

				if ( msg->getEventType() == DtReadAll && isCommSession() )
					handleReadAll(msg);
				else
					cutTestTotalManager->handleMessage(msg);
				break;
			}

            case TimerDev:
            {
                Debug::trace("Session", getName() + HobString(" - handling TimerDev DataType"));

                if (msg->getEventType() == TimerExpired)
                {
                    Debug::trace("Session", getName() + HobString(" - handling TimerExpired Event"));

                    // Get the SessionID from the expired timer to determine if this session
                    // was really the one who submitted this timer to begin with.
                    // If this timer is really mine (this session's), then this session is the one
                    // that should handle it.  If the timer came from another session, then just
                    // ignore it.

                    HfcTimeout *timeout = dynamic_cast<HfcTimeout *> (msg->getHfcObject());
                    if (timeout && timeout->isSessionIdSet() && timeout->getMySessionId() == getId())
                    {
                        Debug::trace("Session", getName() + HobString(" - this is my timer that expired; I will handle it."));
                        handleTimeout(timeout);
                    }
                }
                break;
            }

            case PresentationMethodDev:
                Debug::trace("Session", getName() + HobString(" - handling PRESENTATIONMETHOD DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    // todo - don't do anything with it, yet; need to create a PresentationMethodManager to handle messages coming in via DataGateweigh
                    break;

            case ProductStatisticsDev:
                Debug::trace("Session", getName() + HobString(" - handling PRODUCT STATISTICS DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    productManager->handleMessage(msg);
                break;

            case VersionManagerDev:
                handleVersionManagerMessage(msg);
                break;

            case WindowsConfigurationDev:
                handleWindowsConfigurationManagerMessage(msg);
                break;

            case NetworkDev:
                handleNetworkDevMessage(msg);
                break;


            case System:
                handleSystemMessage(msg);
                break;

            case Marquee:
            case MarqueeSystem:
                handleMarqueeMessage(msg);
                break;

            case Numeric:
            case NumericSystem:
                handleNumericMessage(msg);
                break;

            case GCDSessionDev:
				handleGraphicalCustomerDisplayMessage(msg);
				break;
				
			case LabelApplier:
            case LabelApplierSystem:
                handleLabelApplierMessage(msg);
                break;

            case Wrapper:
            case WrapperSystem:
                handleWrapperMessage(msg);
                break;
            case NGWrapper:
            	handleNGWrapperMessage(msg);
            	break;

            case RFIDReader:
                Debug::trace("Session.rfid", getName() + HobString(" - received RFIDReader message"));
                handleRFIDMessage(msg);
                break;

			case BarcodeScannerDev:
                Debug::trace("Session", getName() + HobString(" - handling BarcodeScannerDev DataType"));
                
				if (( msg->getEventType() == DtReadAll ) && ( isCommSession() ))
					handleReadAll(msg);
				else
				{
					switch ( msg->getEventType() )
					{
					case BarcodeScannerEnable:
					case BarcodeScannerDisable:
						Debug::trace("Session.BarcodeScannerDev", getName() + HobString(" - handling BarcodeScannerDev DataType"));

						// HT-440
						// Made self service checks less restrictive... we need to enable/disable scanner outside of self service mode
						if ( !isPrimarySession() )
						{
							Debug::trace("Session.BarcodeScannerDev", getName() + HobString(" - ignore barcode enable/disable message"));
							break;
						}

						// go the RTC route
						Debug::trace("ScannerManager", "Go RTC Route");
						msg->setFormatType(InternalFormat);
						send(msg, Messenger::DEST_RTC);
						break;

					default:
						// go the HFC route
						Debug::trace("ScannerManager", "Go HFC Route");
						barcodeScannerManager->handleMessage(msg);
						break;
					}
				}

				break;

            case HIDDev:
            {
            	Debug::trace("Session.hid", " - received HID device message from USBManager");
				SessionMessage uiMsg(HIDDev, msg->getEventType());
				uiMsg.setFormatType(BrowserFormat);
				send(&uiMsg, Messenger::DEST_UI);
                break;
            }

            case MfgTestDev:
            	Debug::trace("Session.mfg", " - received manufacturing test message from UI");
            	handleMFGTestMessage(msg);                	
            	break;


            case RecallDev:
                Debug::trace("Session", getName() + HobString(" - handling RECALL DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    recallManager->handleMessage(msg);
                break;

            case DeviceDev:
                Debug::trace("Session", getName() + HobString(" - handling DEVICE DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    deviceManager->handleMessage(msg);
                break;

            case ProductTextCfgDev:
                Debug::trace("Session", getName() + HobString(" - handling PRODUCTTEXTCFG DataType"));
                if (msg->getEventType() == DtReadAll && isCommSession())
                    handleReadAll(msg);
                else
                    productTextCfgManager->handleMessage(msg);
                break;

			case DeptDev:
				Debug::trace("Session", getName() + HobString(" - handling DEPT DataType"));
				if (msg->getEventType() == DtReadAll && isCommSession())
					handleReadAll(msg);
				else
					deptManager->handleMessage(msg);
				break;

			case Diagnostics:
            	switch ( diagnosticsEventAction( msg, isPrimarySession() ) )
            	{
            	case Ignore:
            		Debug::trace("Session", getName() + HobString(" - ignore Diagnostics message"));
            		break;
            	case ProcessToRTC:
            		Debug::trace("Session", getName() + HobString(" - forward Diagnostics message to RTC"));
                    msg->setFormatType( InternalFormat );
            		msg->setSourceObject( this );
                    send( msg, Messenger::DEST_RTC );
                    break;
            	case ProcessToUI:
            		Debug::trace("Session", getName() + HobString(" - forward Diagnostics message to UI"));
            		msg->setFormatType( BrowserFormat );
            		send( msg, Messenger::DEST_UI );
            		break;
            	case PrintDiagReport:
            		Debug::trace("Session", getName() + HobString(" - printing Diagnostics report"));
            		diagnosticsPrintDiagReport( msg );
            		break;
            	default:
            		break;
            	}

				break;

            case JoesDevice:
                handleJoeMessage(msg);
                break;

			case HandsFreeDevice:
			{
				bool isTracing = Debug::isTracing("HandsFree");
				string msgData = msg->getOriginalBuff();
				if (isTracing)
				{
					Debug::trace("HandsFree", getName() + " - handling HandsFreeDevice message");
					Debug::trace("HandsFree", getName() + " - message data: " + Hex::toString(msgData.c_str(), msgData.length()));
				}

				if (msg->getEventType() == HFDSendMessage)
				{
					// NOTE:  If, while we are currently sending an email message to hands-free devices in-range,
					// a second message to send some email message to hands-free devices in-range comes in,
					// we will end up quitting sending the first message to all hands-free devices!!!
					// May need to consider a better way of handling this situation.
					if (isTracing)
						Debug::trace("HandsFree", getName() + " - Received HFDSendMessage, request to send a message to all devices in range");

					GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
					if (gho)
					{
						// Save the message.  It will be sent after we receive the next list of devices that are in range (high power).
						handsFreeDeviceMessage = gho->getData();
						currentHandsFreeDevicesInRangeOffset = 0;

						deviceListRequest request(deviceListRequest::HFPD_HIGH_POWER_ADVERT, Configuration::getInstance()->toIntCfgI(fieldOffsetCfgIGetDevicesInRangeAge));
						send(&request, Messenger::DEST_UI);
					}
				}
				else if ( msg->getEventType() == HFDSendOpNameList && operatorManager )
				{
					// To send the opName list back to the hfpdd daemon without it requesting the list (i.e. just send operator names in the database that have the hands-free ID set)
					if (isTracing)
						Debug::trace("HandsFree", getName() + " - Received HFDSendOpNameList, request to send the operator names and associated hands-free device ids in operator table");
					deviceOpNameList dopl;

					HobOperator *oper = operatorManager->firstOperator();
					while (oper)
					{
						if ( oper->getErrorCode() == RECORD_OK && !oper->getOpRFID().empty() )
						{
							dopl.opName_.push_back(oper->getOpName());
							dopl.address_.push_back(oper->getOpRFID());
							if (isTracing)
								Debug::trace("HandsFree", "Adding operator name " + oper->getOpName() + " with id " + oper->getOpRFID() + " to list");
						}
						oper = operatorManager->nextOperator();
					}

					send(&dopl, Messenger::DEST_UI);
					if (isTracing)
						Debug::trace("HandsFree", "Sent device oper names list to hfpdd");
				}
				else
				{
					// All of the rest of hands-free device SessionMessages will be received with device = HandsFreeDevice and event = HFDGenericEvent.
					// The message's data will be read from the SessionMessage::originalBuff.  We will then create the appropriate
					// hands-free device message from that, using Randy B.'s messagefactory.
					// Note:  We don't need to remove the 4-byte recordLength indicator from the received data,
					//        that has was already done by CMReaderWriterHandsFreeDevice::formatFromReceive().
					size_t l = 0;
					message *m = messageFactory::create(msgData.c_str(), msgData.length(), &l);
					if (m)
					{
						if (isTracing)
							Debug::trace("HandsFree", getName() + " - handling HandsFreeDevice message: resulting message*=" + HobString(Hex::toString((long long)m)));

						switch (m->type_)
						{
							case message::HFPD_PING:
							{
								if (isTracing)
									Debug::trace("HandsFree", "Received a ping, sending a pong");
								pong p;
								send(&p, Messenger::DEST_UI);
								if (isTracing)
									Debug::trace("HandsFree", "sent pong");

								delete m;
								m = 0;
								break;
							}

							case message::HFPD_DEV_LIST:
							{
								if (isTracing)
									Debug::trace("HandsFree", getName() + " - Received HFPD_DEV_LIST, a device list");
								deviceList *dl = reinterpret_cast<deviceList*>(m);
								if (dl)
								{
									if (dl->strength_ == message::HFPD_HIGH_POWER_ADVERT)
									{
										if (isTracing)
											Debug::trace("HandsFree", getName() + " - HFPD_HIGH_POWER_ADVERT device list");
										if (dl->address_.size() == 0 || handsFreeDeviceMessage.empty())
										{
											// No devices in the list to which a message could be sent, so if we have a message to send, get rid of it since there are no devices.
											if (isTracing)
												Debug::trace("HandsFree", getName() + " - No devices in the list OR || handsFreeDeviceMessage is empty");
											handsFreeDeviceMessage = "";
											delete m;
											m = 0;
										}
										else
										{
											// Save the list.  Will send the message to each device individually; through the series of messages:
											// connect, send message, disconnect
											if (currentHandsFreeDevicesInRangeList)
											{
												delete currentHandsFreeDevicesInRangeList;
												currentHandsFreeDevicesInRangeList = 0;
											}
											currentHandsFreeDevicesInRangeList = dl;
											currentHandsFreeDevicesInRangeOffset = 0;

											// Start the series of messages needed to send a message to a device (Connect, Send message, Disconnect).
											if (isTracing)
												Debug::trace("HandsFree", getName() + " - sending deviceConnect to " + dl->address_[0]);
											deviceConnect dc(dl->address_[0]);
											send(&dc, Messenger::DEST_UI);
										}
									}
									else
									{
										if (isTracing)
											Debug::trace("HandsFree", getName() + " - HFPD_LOW_POWER_ADVERT device list");
										string closestMAC = determineClosestHandsFreeDevice(dl);

										// If no devices in the list, then send a message to logout any user that may have been logged in
										TIRISReadOnlyId hfTag(this, (!closestMAC.empty()) ? closestMAC : "0000000000000000");
										dispatchMessage(&hfTag);

										delete m;
										m = 0;
									}
								}
								break;
							}

							case message::HFPD_DEV_OPNAME_LIST:
							{
								if (isTracing)
									Debug::trace("HandsFree", getName() + " - Received HFPD_DEV_OPNAME_LIST, request to get the operator names for the device list passed in");
								deviceOpNameList *dopl = reinterpret_cast<deviceOpNameList*>(m);

								size_t numElements = dopl->address_.size();
								if (numElements)
								{
									for (size_t i = 0; i < numElements; i++)
									{
										dopl->opName_[i] = "?";
										if (isTracing)
											Debug::trace("HandsFree", "looking up name for " + dopl->address_[i]);
										HobOperator *oper = HobOperator::findRFID(dopl->address_[i], this);
										if (oper && oper->getErrorCode() == RECORD_OK)
										{
											dopl->opName_[i] = oper->getOpName();
											delete oper;
											oper = 0;
										}
									}
								}

								send(dopl, Messenger::DEST_UI);
								if (isTracing)
									Debug::trace("HandsFree", "Sent device oper names list back to hfpdd");

								delete m;
								m = 0;
								break;
							}

							case message::HFPD_DEV_STATE:
							{
								deviceState *ds = reinterpret_cast<deviceState*>(m);
								if (ds)
								{
									if (isTracing)
										Debug::trace("HandsFree", getName() + " - Received HFPD_DEV_STATE, address = " + ds->address_ + "  state = " + inttostring(ds->state_));

									if (currentHandsFreeDevicesInRangeList && ds->address_ == currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset])
									{
										if (ds->state_ == deviceState::STATE_READY)
										{
											// we've connected to the device that we're trying to send the message to
											// so now give it the message
											if (isTracing)
												Debug::trace("HandsFree", getName() + " - sending deviceSendMessage to " + currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset] + ", msg = " + handsFreeDeviceMessage);
											HobDateTime now;
											deviceSendMessage dsm(currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset], now.getHour(), now.getMinute(), now.getSecond(), Configuration::getInstance()->toIntCfg(fieldOffsetCfgAccumItemsScaleId), handsFreeDeviceMessage);
											send(&dsm, Messenger::DEST_UI);
										}
										else if (ds->state_ == deviceState::STATE_DISCONNECTED)
										{
											// couldn't connect to the device, so skip it and move on to the next one
											if (currentHandsFreeDevicesInRangeList->address_.size() > currentHandsFreeDevicesInRangeOffset + 1)
											{
												// we have another in the list to send the message to, so connect to it
												currentHandsFreeDevicesInRangeOffset++;

												if (isTracing)
													Debug::trace("HandsFree", getName() + " - sending deviceConnect to next device " + currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset]);
												deviceConnect dc(currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset]);
												send(&dc, Messenger::DEST_UI);
											}
											else
											{
												// no more devices to send the message to
												if (isTracing)
													Debug::trace("HandsFree", getName() + " - no more devices to send message to,  " + handsFreeDeviceMessage);
												handsFreeDeviceMessage = "";
												currentHandsFreeDevicesInRangeOffset = 0;
											}
										}
									}
								}
								delete m;
								m = 0;
								break;
							}

							case message::HFPD_DEV_CMD_RESULT:
							{
								deviceCommandResult *dcr = reinterpret_cast<deviceCommandResult*>(m);
								if (dcr)
								{
									if (isTracing)
										Debug::trace("HandsFree", getName() + " - Received HFPD_DEV_CMD_RESULT, address = " + dcr->address_ + "  command = " + inttostring(dcr->command_) + "  result = " + inttostring(dcr->result_));

									//if the result is success and is for a connect command and we have a message to send, then continue looking for the series of responses in order to send the message to the device
									if ( !handsFreeDeviceMessage.empty() && currentHandsFreeDevicesInRangeList && dcr->address_ == currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset] )
									{
										if (dcr->command_ == message::HFPD_DEV_CONNECT)
										{
											if (dcr->result_ != message::HFPD_ERROR_SUCCESS)
											{
												// couldn't connect to the device, so skip it and move on to the next one
												if ( currentHandsFreeDevicesInRangeList->address_.size() > currentHandsFreeDevicesInRangeOffset + 1 )
												{
													// we have another in the list to send the message to, so connect to it
													currentHandsFreeDevicesInRangeOffset++;

													if (isTracing)
														Debug::trace("HandsFree", getName() + " - sending deviceConnect to next device " + currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset]);
													deviceConnect dc(currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset]);
													send(&dc, Messenger::DEST_UI);
												}
												else
												{
													// no more devices to send the message to
													if (isTracing)
														Debug::trace("HandsFree", getName() + " - no more devices to send message to,  " + handsFreeDeviceMessage);
													handsFreeDeviceMessage = "";
													currentHandsFreeDevicesInRangeOffset = 0;
												}
											}
										}

										//else if (the result is for a send command, then disconnect)
										else if (dcr->command_ == message::HFPD_DEV_SEND_MESSAGE)
										{
											if (dcr->result_ != message::HFPD_ERROR_SUCCESS)
											{
												// Do we want to do anything to log that we couldn't send a message to a device that was in range?
											}

											// Send disconnect message
											if (isTracing)
												Debug::trace("HandsFree", getName() + " - sending deviceDisconnect to " + currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset]);
											deviceDisconnect dd(currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset]);
											send(&dd, Messenger::DEST_UI);
										}

										//else if (the result is for a disconnect command and we have a message to send, then move on to the next device in the saved list and start the Connect / Send message / Disconnect series of messages)
										else if (dcr->command_ == message::HFPD_DEV_DISCONNECT)
										{
											if (dcr->result_ != message::HFPD_ERROR_SUCCESS)
											{
												// Do we want to do anything to log that we couldn't disconnect from a device that we were connected to)
											}

											// and we have a message to send, then move on to the next device in the saved list and start the Connect / Send message / Disconnect series of messages)
											// couldn't connect to the device, so skip it and move on to the next one
											if (currentHandsFreeDevicesInRangeList->address_.size() > currentHandsFreeDevicesInRangeOffset + 1)
											{
												// we have another in the list to send the message to, so connect to it
												currentHandsFreeDevicesInRangeOffset++;

												if (isTracing)
													Debug::trace("HandsFree", getName() + " - sending deviceConnect to next device " + currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset]);
												deviceConnect dc(currentHandsFreeDevicesInRangeList->address_[currentHandsFreeDevicesInRangeOffset]);
												send(&dc, Messenger::DEST_UI);
											}
											else
											{
												// no more devices to send the message to
												if (isTracing)
													Debug::trace("HandsFree", getName() + " - no more devices to send message to,  " + handsFreeDeviceMessage);
												handsFreeDeviceMessage = "";
												currentHandsFreeDevicesInRangeOffset = 0;
											}
										}
									}
								}

								delete m;
								m = 0;
								break;
							}

							default:
							{
								// Don't need this message; at least I wasn't expecting it
								if (isTracing)
									Debug::trace("HandsFree", getName() + " - Received HandsFree message I wasn't expecting: " + inttostring(m->type_));
								delete m;
								m = 0;
								break;
							}
						}
					}
				}
				break;
			}

            default:
                {
                    //todo - write warning log message indicating received bad message.
                    Debug::trace("Session", HobString("WARNING - handleMessage does not recognize this Device type: ") + HobString(Integer::toString(msg->getDeviceType())));
                }
                break;
        }

         Debug::trace("Session", "Handle message done!" );
   }

    // This must be done after handling the message above because the SessionType indicating this session is a
    // comm session is set during handling a SessionDev|SnCommStart message.
    if (isCommSession() && commHistory)
    {
        commHistory->recordComm(msg->getDeviceType(), msg->getEventType());
    }
}


string Session::determineClosestHandsFreeDevice( const deviceList *dl )
{
	const bool isTracing( Debug::isTracing("HandsFree") );
	const bool isTracingUI( Debug::isTracing("HandsFreeUI") );

	// when somebody logged in assume they are out of range... if they are in the list this
	// counter will be reset
	if ( hfpdLoggedIn.length() )
		++hfpdLoggedInOutOfRangeCount;

	// determine closest tag
	std::string closest;

	// process list
	if ( dl )
	{
		// log list
		if ( isTracing )
		{
			Debug::trace("HandsFree", "determineClosestHandsFreeDevice:");
			Debug::trace("HandsFree", "Dev#  Address             Signal   Battery   Firmware   Msgs");

			for ( size_t i( dl->address_.size() ); i--; )
				Debug::trace("HandsFree", inttostring(i) + "     " + dl->address_[i] + "     " + inttostring(dl->rssi_[i]) + "     " + inttostring(dl->batteryLevel_[i]) + "       " + inttostring(dl->firmwareVersion_[i]) + "     " + inttostring(dl->messages_[i]));

			Debug::trace("HandsFree", " ");
		}

		// send list to user interface
		if (( isTracingUI ) && ( SessionManager::getInstance()->getPrimaryConsoleSession() ))
		{
			for ( size_t i( dl->address_.size() ); i--; )
			{
				GenericHfcObject gho("HandsFreeTag: " + inttostring(i) + " - " + dl->address_[i] + " - " + inttostring(dl->rssi_[i]) + " - " + inttostring(dl->batteryLevel_[i]) + " - " + inttostring(dl->firmwareVersion_[i]) + " - " + inttostring(dl->messages_[i]));
				SessionMessage m(SessionDev, SnTestMessage);
				m.setHfcObject(&gho);
				m.setFormatType(BrowserFormat);
				SessionManager::getInstance()->getPrimaryConsoleSession()->send(&m, Messenger::DEST_UI);
			}
		}

		// ---- //

		// thou must be this strong to use our scale
		const int minStrength( Configuration::getInstance()->toIntCfg( fieldOffsetCfgHandsFreeDeviceMinLoginPowerLevel ) );

		int closestStrength( minStrength );

		// iterate devices
		for ( size_t i( dl->address_.size() ); i--; )
			if ( minStrength < dl->rssi_[i] )
			{
				// have new closest
				if ( closestStrength < dl->rssi_[i] )
				{
					closest = dl->address_[i];
					closestStrength = dl->rssi_[i];
				}

				// whoever is logged in is still there (sucker! they should be on break eating
				// donuts)... reset logout counter
				if ( hfpdLoggedIn == dl->address_[i] )
					hfpdLoggedInOutOfRangeCount = 0;
			}

	} // valid list

	if ( isTracing )
		Debug::trace("HandsFree", getName() + " determineClosestHandsFreeDevice - closest: " + closest);

	// ---- //

	// track stongest signal

	// nobody here!
	if ( closest.empty() )
	{
		hfpdStrongestSignal.clear();
		hfpdStrongestSignalCount = 0;
	}

	// they are still closest
	else if ( hfpdStrongestSignal == closest )
		++hfpdStrongestSignalCount;

	// new winner
	else
	{
		hfpdStrongestSignal = closest;
		hfpdStrongestSignalCount = 1;
	}

	if ( isTracing )
	{
		Debug::trace("HandsFree", getName() + " determineClosestHandsFreeDevice - logged in: " + hfpdLoggedIn + " out of range: " + inttostring(hfpdLoggedInOutOfRangeCount));
		Debug::trace("HandsFree", getName() + " determineClosestHandsFreeDevice - strongest: " + hfpdStrongestSignal + " count: " + inttostring(hfpdStrongestSignalCount));
	}

	// ---- //

	const int consecutive( Configuration::getInstance()->toIntCfgI( fieldOffsetCfgIConsecutiveTimesClosestToBeLoggedIn ) );

	// somebody logged in right now
	if ( hfpdLoggedIn.length() )
	{
		// they left (probably went to eat donuts)... log them out
		if ( consecutive <= hfpdLoggedInOutOfRangeCount )
			hfpdLoggedIn.clear();

		// somebody standing closer than our logged in person... and they been there a while it seems...
		else if (( hfpdLoggedIn != hfpdStrongestSignal ) && ( consecutive <= hfpdStrongestSignalCount ))
		{
			// they must really want to use the scale (probably to sell donuts)... log them in
			hfpdLoggedIn = hfpdStrongestSignal;
		}
	}

	// nobody is logged in
	if ( hfpdLoggedIn.empty() )
	{
		// who is closest... tag your it
		hfpdLoggedIn = hfpdStrongestSignal;
	}

	if ( isTracing )
		Debug::trace("HandsFree", getName() + " determineClosestHandsFreeDevice - returning closest device: " + hfpdLoggedIn);

	return hfpdLoggedIn;
}


const HobString &Session::getCurrentWebPage() const
{
    return currentWebPages[1];
}
const HobString &Session::getCurrentWebPageDOM() const
{
    return currentWebPages[3];
}
const HobString &Session::getCurrentWebPageMenu() const
{
    return currentWebPages[2];
}
const HobString &Session::getCurrentWebPageWeightFrame() const
{
    return currentWebPages[4];
}
const HobString &Session::getCurrentWebPageTarget() const
{
    return currentWebPages[5];
}


bool Session::isInOperateMode() const
{
    return (currentMode == OpModeAutoWeigh ||
            currentMode == OpModeSemiWeigh ||
            currentMode == OpModeManualWeigh ||
            currentMode == OpModeSelfService ||
            currentMode == OpModeWrapOnly ||
            currentMode == OpModeAutoManualApply ||
				currentMode == OpModeProductionEntry ||
				currentMode == OpModeAccumulateItems ||
				currentMode == OpModeBuildAMeal ||
				currentMode == OpModePrintMultipleLabels) ? true : false;
}

bool Session::isInSupervisor() const
{
    return (currentMode == OpModeSupervisor) ? true : false;
}

bool Session::isInService() const
{
    return (currentMode == OpModeService) ? true : false;
}

bool Session::isInAnAccumulatingMode() const
{
	return ( currentMode == OpModeAccumulateItems || currentMode == OpModeBuildAMeal ) ? true : false;
}

bool Session::isProductionEntryMode() const
{
	// bool isProdEntry = ((currentMode == OpModeProductionEntry ) ? true : false);
	return ( currentMode == OpModeProductionEntry ) ? true : false;
}

void Session::leaveBuildAMealMode()
{
	// If in BuildAMeal mode, switch back to Manual Weigh mode
	if ( currentMode == OpModeBuildAMeal )
	{
		if (productManager)
			productManager->clearCurrentProduct();

		SessionMessage mode(OperationMode, OpModeManualWeigh);
		handleMessage(&mode);

		PAL_absclass::PAL_sleep(250);

		//	Request the new mode to be sent to the UI.
		SessionMessage newMode(SessionDev, SnReadWeighModeTransactionType);
		handleMessage(&newMode);
	}
}

void Session::leaveAccumulateItemsMode()
{
	// If in BuildAMeal mode, switch back to Manual Weigh mode

	if ( currentMode == OpModeAccumulateItems )
	{
		//if (productManager)
		//	productManager->clearCurrentProduct();

		SessionMessage mode(OperationMode, OpModeManualWeigh);
		handleMessage(&mode);

		PAL_absclass::PAL_sleep(250);

		//	Request the new mode to be sent to the UI.
		//SessionMessage newMode(SessionDev, SnReadWeighModeTransactionType);
		//handleMessage(&newMode);
	}
}

void Session::leaveProductionEntryMode()
{
	// If in BuildAMeal mode, switch back to Manual Weigh mode
	if ( currentMode == OpModeProductionEntry )
	{
		//if (productManager)		// dont need to do this bc the list is already clear
		//	productManager->clearCurrentProduct();

		SessionMessage mode(OperationMode, OpModeManualWeigh);
		handleMessage(&mode);

		PAL_absclass::PAL_sleep(250);

		//	Request the new mode to be sent to the UI.
		//SessionMessage newMode(SessionDev, SnReadWeighModeTransactionType);
		//handleMessage(&newMode);
	}	
}

void Session::returnToAccumulateItems(bool sendToUI)
{
	// If in Accumulate Items mode, switch back to Manual Weigh mode

	//if (productManager)
	//	productManager->clearCurrentProduct();

	SessionMessage mode(OperationMode, OpModeAccumulateItems);
	handleMessage(&mode);

	PAL_absclass::PAL_sleep(250);

	//	Request the new mode to be sent to the UI.
	if( sendToUI )
	{
		SessionMessage newMode(SessionDev, SnReadWeighModeTransactionType);
		handleMessage(&newMode);
	}
}

void Session::returnToProductionEntry(bool sendToUI)
{
	// If in Production Entry mode, switch back to Manual Weigh mode

	SessionMessage mode(OperationMode, OpModeProductionEntry);
	handleMessage(&mode);

	PAL_absclass::PAL_sleep(250);

	//	Request the new mode to be sent to the UI.
	if( sendToUI )
	{
		SessionMessage newMode(SessionDev, SnReadWeighModeTransactionType);
		handleMessage(&newMode);
	}
}

void Session::returnToManualWeighMode()
{
	//if (productManager)
	//	productManager->clearCurrentProduct();

	SessionMessage mode(OperationMode, OpModeManualWeigh);
	handleMessage(&mode);

	PAL_absclass::PAL_sleep(500);

	//	Request the new mode to be sent to the UI.
	SessionMessage newMode(SessionDev, SnReadWeighModeTransactionType);
	handleMessage(&newMode);
}

OpModeEvent Session::getCurrentMode() const
{
    return currentMode;
}

void Session::setCurrentMode(OpModeEvent new_mode)
{
	currentMode = new_mode;
}

int Session::getCurrentModeOp() const
{
	const bool logging( Debug::isTracing("Session.currentModeOp") );

	if ( logging )
		Debug::trace("Session.currentModeOp", getName() + HobString(" getCurrentModeOp() = ") + HobString(Integer::toString(currentModeOp)));

    return currentModeOp;
}
void Session::setCurrentModeOp(int modeOp)
{
	const bool logging( Debug::isTracing("Session.currentModeOp") );

    currentModeOp = modeOp;

    if ( logging )
    	Debug::trace("Session.currentModeOp", getName() + HobString(" setCurrentModeOp(") + HobString(Integer::toString(currentModeOp)) + HobString(")"));
}

int Session::getCurrentModeOpState() const
{
    return currentModeOpState;
}
void Session::setCurrentModeOpState(int modeOpState)
{
    currentModeOpState = modeOpState;
}


OpModeEvent Session::getMostRecentRunMode() const
{
    return mostRecentRunMode;
}


ResponseMessageManager *Session::getResponseMessageManager() const
{
    return responseMessageManager;
}


void Session::turnPlatterTareOn()
{
	Debug::trace("Session.PlatterTare", HobString(" turnPlatterTareOn():  "));

    if (productManager && productManager->validateProductForPlatterTare())
    {
        currentModeOp = Session::OP_PLATTERTAREMODE;

        //      This msg will tell the UI to clear the Weight & total price fields on the display.
        SessionMessage ptMsg(SessionDev, SnPlatterTareOn);
        ptMsg.setFormatType(BrowserFormat);
        send(&ptMsg, Messenger::DEST_UI);

        //      This msg will allow them to escape Platter tare mode or allow them to accept the platter tare value.
        if (responseMessageManager)
            responseMessageManager->addMessage(ErrProductDev, ProductErrPlatterTareMode, HfcResponse::RESPONSESTYLE_OK);

        // This requestWeigherStatus() call was originally located in the Softkey Action PlatterTare action. When in Operate Mode, it worked fine, but
		// when we go to Self Service Mode, the Tare field would be blank when we pressed platter tare and their was a weight already located on the
		// platter.  In Operate Mode, if there was a weight on the platter, that weight would shift to the tare field upon pressing Platter Tare. It
		// turns out that the bit in the weigherStatus, largeMotion, was being set to true.  There is no motion occurring and we could not for
		// sure determine at this time, why the bit was set to true when it should not be.  Self Serv and Operate appears to be executing the same code.
		// This block of code below, also fixes the problem.  We made the scale update it status and the flag was reset back to false. Also performing 
		// a requestWeigherStatus() below also fixed the problem, but for some reason, from the Softkey itself, it failed for SS.
		//if (currentWeight)
		//{
		//	Debug::trace("Session.PlatterTare", HobString("turnPlatterTareOn(): I had a currentWeight so I deleted it. Also set currentWeight = 0  "));
		//	delete currentWeight;
		//	currentWeight = 0;
		//}
		
		// See comment just above why we added this and removed this call from the SoftkeyManager.
		Debug::trace("Session.PlatterTare", HobString("turnPlatterTareOn(): now request a new weigherStatus  "));
		requestAWeigherStatus();

    }
    else
    {
        turnPlatterTareOff();
    }
}

void Session::turnPlatterTareOff()
{
    if (currentModeOp == Session::OP_PLATTERTAREMODE)
        currentModeOp = OP_UNKNOWN;

    //  Update the weigher status so the new weight will update the net weight.
    requestAWeigherStatus();
}

void Session::requestAWeigherStatus(bool shouldDelayAfterSending)
{
    Debug::trace("Session", getName() + HobString(" - requestAWeigherStatus() called"));

    //  Update the weigher status so the new weight will update the net weight.
    WeigherRequestStatus wrs(this);
    send(&wrs, Messenger::DEST_RTC, shouldDelayAfterSending);

    requestedWeigherStatus = true;

}


// public so ProductManager can call it.
void Session::prepareToPrint(bool fppTestPrint)
{
    // Store whether this is triggered by FPP Test Print
    setFPPTestPrint(fppTestPrint);

	if (!Configuration::getInstance()->isSelfServiceScale() ||
		Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintOnStableWeight))
	{
        Debug::trace("Session", getName() + HobString(" - prepareToPrint - sending message to Browser to get any updated field ..."));

        //If we have a label rotation change notification popped up here, remove it
        removeDialogMessage(ErrProductDev, ProductErrRotationPlacementChanged, HfcResponse::RESPONSESTYLE_OK);
        SessionMessage msg(SessionDev, SnPrintKeyPressed);
        msg.setFormatType(BrowserFormat);
        send(&msg, Messenger::DEST_UI);
    }

    currentModeOp = OP_PRINTKEYPRESSED;
    startPrintKeyPressedTime = time((time_t *)NULL);
}

bool Session::currentPageCanPrintLabel() const
{
    if (currentWebPageFrame < 0 || currentWebPageFrame > MAX_WEBPAGE_FRAME_NUMBER)
    {
        Debug::trace("Session", getName() + HobString(" - currentPageCanPrintLabel() - currentWebPageFrame is out of range: ") + HobString(Integer::toString(currentWebPageFrame)));
        return false;
    }

    HobString pageToCheck = currentWebPages[currentWebPageFrame];

    // Look for a '/' indicating the beginning of the web page filename.
    // Don't need to, nor want to, look for './', since sometimes we get the name without the '.'
    std::string::size_type begPos = pageToCheck.find("/");
    if (begPos == std::string::npos)
        begPos = 0;
    else
        begPos++;

    // Find the .asp extension on the web page filename
    std::string::size_type endPos = pageToCheck.find(".asp");
    if (endPos == std::string::npos)
    {
        // If we didn't find .asp, look for .htm in case we have any html pages.
        std::string::size_type endPos = pageToCheck.find(".htm");
        if (endPos == std::string::npos)
        {
            // No 'expected' extension found on the web page filename.
            // Assume we can't print on this page.
            return false;
        }
    }

    // If the '/' is after the extension, the web page filename didn't contain the '/'
    // (it was part of the asp parameters).
    if (begPos >= endPos)
        begPos = 0;

    pageToCheck = pageToCheck.substr(begPos, endPos-begPos);
    Debug::trace("Session", getName() + HobString(" - currentPageCanPrintLabel() comparing frame ") + HobString(Integer::toString(currentWebPageFrame)) + HobString("  pageName=") + pageToCheck);

    bool ret = false;
    if (pageToCheck.compare("operateRunScreenRandom") == 0 ||
            pageToCheck.compare("operateRunScreenByCount") == 0 ||
            pageToCheck.compare("operateRunScreenFixed") == 0 ||
            pageToCheck.compare("operateRunScreenFPPRW") == 0 ||
            pageToCheck.compare("operateRunScreenFPPBC") == 0 ||
            pageToCheck.compare("operateRunScreenFPPFW") == 0 ||
            pageToCheck.compare("operateRunScreenRandomEPP") == 0 ||
            pageToCheck.compare("operateRunScreenByCountEPP") == 0 ||
            pageToCheck.compare("operateRunScreenFixedEPP") == 0 ||
            pageToCheck.compare("selfservRunScreenBC") == 0 ||
            pageToCheck.compare("selfservRunScreenRW") == 0 ||
            pageToCheck.compare("operateLabelData") == 0 ||
            pageToCheck.compare("operateShelfLife") == 0 ||
            pageToCheck.compare("operatePrinterSettings") == 0 ||
            pageToCheck.compare("operateLabelPlacement") == 0 ||
            pageToCheck.compare("operateDateTime") == 0 ||
            pageToCheck.compare("operateWrapperSettings") == 0 ||
            pageToCheck.compare("operateWrapperSettingsAdvanced") == 0 ||
				pageToCheck.compare("operateWeighModeTotalType") == 0 ||
				pageToCheck.compare("operateAccumItemsList") == 0)
    {
        ret = true;
    }
    Debug::trace("Session", getName() + HobString(" - currentPageCanPrintLabel() returning ") + HobString(Bool::toString(ret)));
    return ret;
}


LanguageSelection Session::getCurrentLanguageSelection() const
{
    return currentLanguageSelection;
}

void Session::modifiedLanguageNumber(LanguageSelection changedLanguageNumber)
{
    if (currentLanguageSelection == changedLanguageNumber)

        setCurrentLanguageSelection (currentLanguageSelection);
}

void Session::restoreOperateLanguageSelection()
{
    setCurrentLanguageSelection (operateLanguageSelection);
}

void Session::changeOperateLanguageSelection(LanguageSelection cls)
{
    setCurrentLanguageSelection(cls);

    operateLanguageSelection = currentLanguageSelection;

}

// FT-536: commenting out for Fastrack
//static bool firstTimeSetLanguage = true;

void Session::setCurrentLanguageSelection(LanguageSelection cls)
{
#if defined ( __linux__ )
	// FT-536: commenting out for Fastrack
    //Debug::trace("Session.virtualKeyboard", "setCurrentLanguageSelection");
    //if ( PAL_absclass::PAL_findWindow("xvkbd") )
    //{
    //    Debug::trace("Session.virtualKeyboard", "setCurrentLanguageSelection - killing existing virutal keyboard");
    //    PAL_absclass::PAL_closeWindow("xvkbd");
    //}

    Configuration * config = Configuration::getInstance();

    if (cls == SecondaryLanguage)
        Platform::getInstance().setupLocale(config->getSecondaryLanguageCode(), config->getSecondaryCountryCode());
    else
        Platform::getInstance().setupLocale(config->getPrimaryLanguageCode(), config->getPrimaryCountryCode());

    currentLanguageSelection = cls;
    
	// FT-536: commenting out for Fastrack
	//if ( !firstTimeSetLanguage )
	//{
	//	popupVirtualKeyboard(0);
    //	PAL_absclass::PAL_hideWindow("xvkbd");
	//}
	//else
	//	firstTimeSetLanguage = false;
#else
    // jhc: Look in version 95 for other things I tried to kill an applicaiton.  PostMessage, SendMessage, etc.
    bool isTracing = Debug::isTracing("Session.Keyboard");

    Configuration * config = Configuration::getInstance();

    std::string pathstr("");

    // kill the existing keyboard
    HWND hwnd = NULL;
    hwnd = Util::findWindowHandle(&virtualKeyboardParams_);

    if (hwnd != NULL)
    {
        if (isTracing)
            Debug::trace("Session.Keyboard", "setCurrentLanguageSelection - Before calling taskkill for keyboard");

        std::string argyList("/f /im Click-N-Type.exe");
        HINSTANCE res1 = ShellExecuteA (NULL,"open","taskkill.exe",argyList.c_str(),"",SW_SHOWMINNOACTIVE);

        if (isTracing)
            Debug::trace("Session.Keyboard", "setCurrentLanguageSelection - after calling taskkill for keyboard");

        //PAL_absclass::PAL_sleep(1000); // give it time to kill the keyboard, or the next one wont launch.
        bool stillTaskkillin = true;
        int taskkillCtr = 0;
        hwnd = NULL;

        while(stillTaskkillin)
        {
            hwnd = Util::findWindowHandle(&virtualKeyboardParams_);

            if (hwnd != NULL)
            {
                taskkillCtr++;
                if(taskkillCtr == 20)
                {
                    stillTaskkillin = false;            // not going to let it go past 10 secs
                    if (isTracing)
                        Debug::trace("Session.Keyboard", HobString("Timed out trying to kill keyboard.") );

                }
                PAL_absclass::PAL_sleep(500);
            }
            else
            {
                if (isTracing)
                    Debug::trace("Session.Keyboard", HobString("     stillTaskkillin:") + HobString(Integer::toString(taskkillCtr)) );
                stillTaskkillin = false;
            }
        }
    }

    // find out the current language, so we can launch that keyboard, at this given path determined below
    pathstr.assign("c:/program files/Click-N-Type/");

    if (cls == SecondaryLanguage)
    {
        Platform::getInstance().setupLocale(config->getSecondaryLanguageCode(), config->getSecondaryCountryCode());

        pathstr.append(Configuration::getInstance()->getSecondaryLanguageCode()).append(1, '_');
        pathstr.append(Configuration::getInstance()->getSecondaryCountryCode());
    }
    else
    {
        Platform::getInstance().setupLocale(config->getPrimaryLanguageCode(), config->getPrimaryCountryCode());

        pathstr.append(Configuration::getInstance()->getPrimaryLanguageCode()).append(1, '_');
        pathstr.append(Configuration::getInstance()->getPrimaryCountryCode());
    }

    pathstr.append("/Click-N-Type.exe");

    if (isTracing)
        Debug::trace("Session.Keyboard", HobString("setCurrentLanguageSelection -     pathstr:") + pathstr );

    // launch the new keyboard
    //HINSTANCE res2 = ShellExecuteA (NULL,"open",pathstr.c_str(),"","",SW_SHOWNORMAL);
    HINSTANCE res2 = ShellExecuteA (NULL,"open",pathstr.c_str(),"","",SW_SHOWMINNOACTIVE);

    if (isTracing)
        Debug::trace("Session.Keyboard", HobString("setCurrentLanguageSelection -     res2:") + HobString(Hex::toString((long long)res2)) );

    currentLanguageSelection = cls;
#endif
}


bool Session::isOnSelfServiceStartupScreen() const
{
    return onSelfServiceStartupScreen;
}

void Session::setOnSelfServiceStartupScreen(bool onStartupScreen)
{
    onSelfServiceStartupScreen = onStartupScreen;
    manageAutoLogoutTimer();
}

void Session::backupDatabase(HobString functionName)
{
    // The LX scale will not need to write the database back to flash!!
    // So to skip this procedure, set the dbBackupLocation (in Internal Config - config.dat) to be empty

    bool isTracing = Debug::isTracing("Session.backupDb");
    if (isTracing)
    {
        Debug::trace("Session.backupDb", HobString("  backupDatabase - Begin."));
        if (Debug::isTracing("Memory"))
            memShow(0);
    }
    if (dbMgr)
    {
        HobString path = Configuration::getInstance()->getDbBackupLocation();
        if (path == "")
        {
            if (isTracing)
                Debug::trace("Session.backupDb", functionName + " - No path to backup the database to.");
        }
        else
        {
            if (isTracing)
            {
                Debug::trace("Session.backupDb", HobString("Before dbMgr->backupDatabase()"));
                if (Debug::isTracing("Memory"))
                    memShow(0);
            }

            try
            {
                dbMgr->backupDatabase(path);
            }
            catch (DBException &dbe)
            {
                if (Debug::isTracing("DBAccess.exception"))
                {
                    Debug::trace("DBAccess.exception", HobString("Session::backupDatabase() ********** database exception **********"));
                    DBException::Type t = dbe.getType();
                    Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
                }

                // If we have any problem writing the db back to flash, shutdown the scale immediately!!
                SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
                shutdownMsg.setFormatType(InternalFormat);
                send(&shutdownMsg, Messenger::DEST_RTC, false);
            }

            if (isTracing)
            {
                Debug::trace("Session.backupDb", HobString("After dbMgr->backupDatabase()"));
                if (Debug::isTracing("Memory"))
                    memShow(0);
                Debug::trace("Session.backupDb", functionName + " - Backup database at: " + path);
            }
        }
    }
    else
    {
        if (isTracing)
            Debug::trace("Session.backupDb", functionName + " - No database to Backup to!!!");
    }
    if (isTracing)
    {
        Debug::trace("Session.backupDb", HobString("  backupDatabase - End."));
        if (Debug::isTracing("Memory"))
            memShow(0);
    }
}


void Session::createNewDirectory(SessionMessage *msg)
{
    GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
    if (msgdata)
    {
        HobString myDirectory = msgdata->getData();
        Debug::trace("Session", getName() + HobString(" - createNewDirectory: ") + myDirectory + HobString("*"));
        if (!myDirectory.empty())
        {
            util_filesystem::Directory di( myDirectory );

            if ( di.exists() )
            {
                //      The directory already exists. This is an error - report it.
                PAL_absclass::PAL_sleep(55);                    //      Delay because of the POS Window manager we have (causes duplicate actionPerformed events).
                if (responseMessageManager)
                    responseMessageManager->addMessage(ErrSessionDev, SessionDevErrDuplicateDirectory, HfcResponse::RESPONSESTYLE_PROMPTOKCANCEL);
            }
            else
            {
                if ( di.create() )
                {
                    //  Get the new directory name only. We don't want the full path included.
                    HobString dirSeparator = Configuration::getInstance()->getDirectorySeparator();
                    std::string::size_type lastSlash = myDirectory.rfind(dirSeparator);
                    std::string::size_type indexA = lastSlash, indexB = myDirectory.length();
                    if ( myDirectory.size() == lastSlash + 1 )
                    {
                        indexB = lastSlash;
                        lastSlash = myDirectory.rfind(dirSeparator, (lastSlash - 1));
                        indexA = lastSlash;
                    }
                    ++indexA;
                    HobString currentDir = myDirectory.substr(indexA, indexB - indexA);
                    currentDir.append(dirSeparator);                    //      Make sure we have the dirSeparator at the end.

                    GenericHfcObject newDir(currentDir);
                    SessionMessage newDirMsg(SessionDev, SnInsertNewDirectory);
                    newDirMsg.setFormatType(BrowserFormat);
                    newDirMsg.setHfcObject(&newDir);
                    send(&newDirMsg, Messenger::DEST_UI);
                }
            }
        }
    }
}


/**
 * Send the current date and time to the Browser, but only if it is different from the last one sent,
 * or if none has been previously sent.
 */
void Session::sendDateTimeMessage()
{
    HobDateTime *hdt = new HobDateTime();
    /*
       if (hdt->getYear4() < 1900)
       {
       if (responseMessageManager)
       responseMessageManager->addMessage(ErrSessionDev, SessionDevDateTimeReadError, HfcResponse::RESPONSESTYLE_OK);
       }
       */
    if (!currentDateTime || *currentDateTime != *hdt)
    {
        if (Debug::isTracing("HobDateTime"))
        {
            Debug::trace("HobDateTime", "new date/time is different from current date/time - current date/time:");
            if (currentDateTime)
                currentDateTime->show();
            else
                Debug::trace("HobDateTime", "current date/time is NULL");

            Debug::trace("HobDateTime", "new date/time is different from current date/time - new date/time:");
            hdt->show();
        }

        SessionMessage timeMsg(SessionDev, SnWriteDateTime);
        timeMsg.setFormatType(BrowserFormat);
        timeMsg.setHfcObject(hdt);
        send(&timeMsg, Messenger::DEST_UI, false);

        if (currentDateTime)
        {
            Debug::trace("HobDateTime", "deleting current date time within sendDateTimeMessage method");
            delete currentDateTime;
        }
        currentDateTime = hdt;
    }
    else
        delete hdt;
}


// Private method
void Session::handleModeMessage(SessionMessage *msg)
{
    if (!msg)
        return;

    int newMode = msg->getEventType();

	Configuration *cfg = Configuration::getInstance();
	if (!cfg)
		return;

    bool tracing = Debug::isTracing("Session");
    if (tracing)
    {
        Debug::trace("Session", getName() + HobString(" - ****************************************"));
        Debug::trace("Session", getName() + HobString(" - handling MODE DataType"));
        Debug::trace("Session", getName() + HobString(" - current mode: ") + HobString(Integer::toString(currentMode)) + HobString("  new mode requested: ") + HobString(Integer::toString(newMode)));
        Debug::trace("Session", getName() + HobString(" - ****************************************"));
    }

    // Don't do anything if the current mode is the same as the mode the user is trying to change to.
    // unless going to OpModeServiceLogin, used for using the Service key to log into Service
    if (currentMode == newMode && currentMode != OpModeServiceLogin)
        return;

    // if switching to the generic OperateMode mode and we're already in one of the operate modes, just return.
    if (newMode == OpModeOperate && isInOperateMode())
        return;

	// Clear any active plu when leaving self-serve mode.
	if (currentMode == OpModeSelfService && productManager)
		productManager->clearCurrentProduct(true);

	if (productManager && productManager->isPrintingRemoteLabel() && newMode != OpModePrintMultipleLabels)
	{
		addDialogMessage(ErrProductDev, ProductErrPrintPrintingRemoteLabel, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

/* This is commented out because we currently don't inform the operator that they have added items to the Production Entry,
   and now they are asking to get out of Production Entry mode, thus we would remove all the items currently in the Production Entry,
	and not print the labels for them.  We don't ask them if that is really what they want to do.
       if (currentMode == OpModeProductionEntry && productManager && productManager->hasPrintingProduct())
       {
       if (currentModeOp != Session::OP_CONTINUELEAVINGPRODUCTIONENTRYMODE)
       {
       if (savedModeChangeMessage)
       delete savedModeChangeMessage;
       savedModeChangeMessage = new SessionMessage(msg->getDeviceType(), msg->getEventType());

       addDialogMessage(ErrOperationMode, ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::ProductionEntryProductsEntered, HfcResponse::RESPONSESTYLE_OKCANCEL);
       return;
       }
       }
 */
    
    // If we are in monitor mode, don't do anything.
    // The user must get out of monitor mode (Session receives 100,18 message) before a mode change is allowed.
    if (currentMode == OpModeMonitor)
        return;

	if (currentMode == OpModeProductionEntry || currentMode == OpModePrintMultipleLabels)
	{
			SessionMessage msg(ProductDev, ProductStopPrintAllLabels);
			msg.setFormatType(BrowserFormat);
			send(&msg, Messenger::DEST_UI);
	}
     
    if (newMode == OpModeStatus)
    {
        HfcOperationModeStatus *opModeStatus = dynamic_cast<HfcOperationModeStatus *>(msg->getHfcObject());
        if (opModeStatus && responseMessageManager)
        {
            if (Debug::isTracing("Clare"))
            {
                HobString msgDebug = "Session:  Evol Wrapper status:  ErrorSource = ";
                msgDebug.append(Integer::toString(opModeStatus->getEvolWrapperErrorSource()));
                msgDebug.append("  Error = ").append(Integer::toString(opModeStatus->getEvolWrapperError()));
                Debug::trace("Clare", msgDebug);

                if (productManager)
                {
                    Product *product = productManager->getCurrentProduct();
                    if (product)
                    {
                        ProductTransaction *pTrans = product->getCurrentTransaction();
                        if (tracing && pTrans)
                        {
                            msgDebug = "Session:  For Evol Wrapper error, transactionNum = ";
                            msgDebug.append(Integer::toString(pTrans->getTransactionNum()));
                            Debug::trace("Clare", msgDebug);
                        }
                    }
                }
            }
            Debug::trace("Clare", "Passing the opModeStatus msg to the ResponseMessageManager.");
            responseMessageManager->manageStatus(opModeStatus);
        }
        return;
    }

    // If we are in monitor mode, don't do anything.
    // The user must get out of monitor mode (Session receives 100,18 message) before a mode change is allowed.
    if (currentMode == OpModeMonitor)
        return;


	// If we change modes, then we're not looking for sweethearting any more
	if (productManager)
	{
		Debug::trace("sweetheartWeight", "*** Session - changing modes - calling stopRecordingSweetheartWeight() ***");
		productManager->stopRecordingSweetheartWeight();
	}


	// Clear the current printer statuses when changing modes, if we aren't set to notify UI only when we haven't previously notified it.
	if (!cfg->toBoolCfgI(CfgInternal::PRINTERSTATUSNONREPEATINGMSGSTOUI))
	{
		SessionMessage clearPS(Printer, PrClearCurrentStatus);
		handleMessage(&clearPS);
	}

	bool scaleIsPrepack = cfg->isPrepackScale();

    // If changing from Supervisor or Service to another mode, then backup the database to compact flash.
    if (currentMode == OpModeService || currentMode == OpModeSupervisor)
	{
		for (int i = 0; i < MaxPrinterTypes; i++)
		{
		    PrinterRequestStatus prs(this, (PrinterType)i);
			send(&prs, Messenger::DEST_RTC);
		}

        backupDatabase("ModeSwitch");
	}

    // If we are leaving Service Mode, determine if we need to update the weigher configuration
    // date and time, and do it.
    if (currentMode == OpModeService && configurationManager)
        configurationManager->prepareToLeaveServiceMode();

    bool goingToOperateMode =
   		(newMode == OpModeOperate) && (currentMode == OpModeService || currentMode == OpModeSupervisor);

    bool leavingPrePackMode = false;
    if ( Configuration::getInstance()->isServiceCounterScale() )	//	HA-185: Do this only for Service counter
	{
	    leavingPrePackMode =
			(OpModeAutoWeigh != newMode) && (OpModeAutoManualApply != newMode) &&
			((OpModeAutoWeigh == currentMode) || (OpModeAutoManualApply == currentMode));
	}

	if ( (currentMode == OpModeProductionEntry || currentMode == OpModePrintMultipleLabels) && productManager )
		productManager->setHasProductionEntryItems(false);

	if ( currentMode == OpModeAccumulateItems && productManager)
		productManager->setHasAccumulateItems(false);

	//  Log out the current operator going from Supervisor/Service to Operate mode.
    //  FT-548:  Log out operator when switching out of pre-pack mode
    if (( goingToOperateMode ) || ( leavingPrePackMode ))
    {
        if (operatorManager && operatorManager->getLoggedInOperator() && cutTestManager && !cutTestManager->isCuttingTestOn() )
        {
            SessionMessage logoutMsg(OperatorDev, OpLogout);
            operatorManager->handleMessage(&logoutMsg);
        }
    }

	// We'll be leaving Accumulate Items mode, so reset the flag about having sent the master product to the Accumulate Items mode.
	if (isInAnAccumulatingMode())
	{
		accumulateItemsAddedMasterProduct = false;
		if( productManager->hasAccumulateItems() == false )
			productManager->setHasAccumulateItems( false );
	}
    if (cfg->isRFIDHWPresent())
    {
        SessionMessage getCurrentRFIDMsg(RFIDReader, RFIDRequestCurrentID);
        getCurrentRFIDMsg.setFormatType(InternalFormat);
        send(&getCurrentRFIDMsg, Messenger::DEST_RTC, false);
    }

    // check for cutter hardware
	PrinterGeneric getCutterStatus(PrReqCutterStatus, this, FirstPrinter);
    send(&getCutterStatus, Messenger::DEST_RTC, false);

    if (scaleIsPrepack == true)
    {
        if (isInOperateMode())
            saveOpModeSettings();
        else if (isInService())
            restoreOpModeSettings();        
    }
    // Tell the real-time controller that the mode is changing.
    OperationModeGeneric *omg = (OperationModeGeneric *)NULL;

    switch (newMode)
    {
        case OpModeIdle:
		{
            // todo - What constitutes this mode???
            if (tracing)
                Debug::trace("Session", getName() + HobString(" - Changing to OpModeIdle."));
            if (scaleIsPrepack || (!scaleIsPrepack && SessionManager::getInstance()->isWeigherAttached()))
            {
                registerEvent(Weigher, WgStatus);
                if (isPrimarySession())
				    registerEvent(Weigher, WgStatusAndPrint);
            }
            else
            {
                deregisterEvent(Weigher, WgStatus);
                deregisterEvent(Weigher, WgStatusAndPrint);
            }
            registerEvent(Printer, PrStatus);
            registerEvent(Printer, PrCutterStatus);
            registerEvent(Printer, PrCutterEnableSave); 
            registerEvent(LabelApplier, LaStatus);
            registerEvent(LabelApplier, LaEvolInquire);
            registerEvent(OperationMode, OpModeStatus);
            registerEvent(Wrapper, WrStatus);
            registerEvent(Wrapper, WrEvolStatus);
            registerEvent(Wrapper, WrSettings);
            registerEvent(Wrapper, WrEvolSettings);
            registerEvent(Wrapper, WrInquire);
            registerEvent(Wrapper, WrEvolInquire);
            registerEvent(Wrapper, WrProductLoaded);
            registerEvent(Wrapper, WrEvolManufacturingRunInNotice);
			registerEvent(Wrapper, WrReqUIWrapperModel);
			registerEvent(Wrapper, WrUIWrapperModel);
			registerEvent(Wrapper, WrNGWWrapperDetached);

            deregisterEvent(Wrapper, WrEvolSensors);
            deregisterEvent(Wrapper, WrCameraCalibration);
            deregisterEvent(Wrapper, WrEvolPackageDesc);
            deregisterEvent(Wrapper, WrEvolRemoveUpgradeFiles);
            // deregisterEvent(Wrapper, WrEvolSystemInfo); SOF-1251 we want this event


            deregisterEvent(OperatorDev, OpLogin);

            if (isPrimarySession())
                omg = new OperationModeIdle(this);
            break;
		}

        case OpModeOperate:
        case OpModeManualWeigh:
        case OpModeSemiWeigh:
        case OpModeAutoWeigh:
        case OpModeWrapOnly:
        case OpModeSelfService:
        case OpModeAutoManualApply:
        case OpModeProductionEntry:
        case OpModePrintMultipleLabels:
        case OpModeAccumulateItems:
        case OpModeBuildAMeal:
        {
            /* Careful, OpModeOperate isn't an actual mode the user can change to.
             * The Browser sends this particular event to change to the most recent run mode,
             * whether that was manual, semi-weigh, or auto-weigh mode.
             */
			
			//Debug::trace("Session.ChangeMode", "Change mode, Incoming mode: " + HobString(Integer::toString(newMode)) );

			EvolMessage msgGCD( GCDSessionDev, (newMode == OpModeSelfService) ? GcdSessionColorTestBlankScreenOn : GcdSessionColorTestBlankScreenOff );
			dispatchMessage(&msgGCD);

            if (scaleIsPrepack || (!scaleIsPrepack && SessionManager::getInstance()->isWeigherAttached()))
            {
                registerEvent(Weigher, WgStatus);
                if (isPrimarySession())
				    registerEvent(Weigher, WgStatusAndPrint);

                // register LaSysInfo to get new model number for CLA Refresh
                registerEvent(LabelApplier, LaSysInfo);
            }
            else
            {
                deregisterEvent(Weigher, WgStatus);
                deregisterEvent(Weigher, WgStatusAndPrint);
            }
            registerEvent(Printer, PrStatus);
            registerEvent(Printer, PrCutterStatus);
            registerEvent(Printer, PrCutterEnableSave); 
            registerEvent(Printer, PrResetTeach);
            registerEvent(OperationMode, OpModeStatus);
            registerEvent(ProductDev, ProductComplete);
            registerEvent(ProductDev, ProductPackageComplete);
            registerEvent(ProductDev, ProductIncomplete);


            if (scaleIsPrepack == true)
            {
            	Debug::trace("Session", "Register for Pre-pack Events.");
            	registerEvent(LabelApplier, LaInquire);
                registerEvent(LabelApplier, LaEvolInquire);
                registerEvent(LabelApplier, LaStatus);
                
                registerEvent(Wrapper, WrStatus);
                registerEvent(Wrapper, WrEvolStatus);
                registerEvent(Wrapper, WrSettings);
                registerEvent(Wrapper, WrEvolSettings);
                registerEvent(Wrapper, WrInquire);
                registerEvent(Wrapper, WrEvolInquire);
                registerEvent(Wrapper, WrProductLoaded);

                registerEvent(Wrapper, WrEvolPackageDesc);
                registerEvent(Wrapper, WrEvolEventCapture);
                registerEvent(Wrapper, WrEvolErrorHistory);
                registerEvent(Wrapper, WrEvolSystemInfo);
                registerEvent(Wrapper, WrEvolManufacturingRunInNotice);
				registerEvent(Wrapper, WrReqUIWrapperModel);
				registerEvent(Wrapper, WrUIWrapperModel);
				registerEvent(Wrapper, WrNGWWrapperDetached);
                Debug::trace("SessionWrapper", "registerEvent: WrPPPDist WrPPPCalibration");
                registerEvent(Wrapper, WrPPPDist);
                registerEvent(Wrapper, WrPPPCalibration);
                registerEvent(Wrapper, WrPPPConfig);
                registerEvent(Wrapper, WrPPPVersion);
                registerEvent(Wrapper, WrPPPStatus);
                //registerEvent(Wrapper, WrWrapperPause);			// not processing the printer plate positioning message any longer
                registerEvent(Wrapper, WrPPPBootError);

				Debug::trace("Session", "Register for NGW Events.");
    			registerEvent(NGWrapper, _UpgradeRequired);
    			registerEvent(NGWrapper, _UpgradeUIReady);
    			registerEvent(NGWrapper, _StartUpgrade);
    			registerEvent(NGWrapper, _UpgradeStatus);
    			registerEvent(NGWrapper, _UpgradeProgress);
    			registerEvent(NGWrapper, _UpgradeFinished);
    			registerEvent(NGWrapper, _UpgradeRequired);
    			registerEvent(NGWrapper, _ReqUpgradeInfo);
    			registerEvent(NGWrapper, _UpgradeInfo);
    			registerEvent(NGWrapper, _WrNGWScan);
    			registerEvent(NGWrapper, _WrNGWScanFinished );


                deregisterEvent(Wrapper, WrEvolSensors);
                deregisterEvent(Wrapper, WrCameraCalibration);
                deregisterEvent(Wrapper, WrEvolRemoveUpgradeFiles);
                deregisterEvent(Wrapper, WrEvolCameraThreshold);
                deregisterEvent(Wrapper, WrEvolWriteCameraThreshold);


            }

			if (productManager)
			{
				// If we're in Super or Service, and changing to Operate mode, then clear the current PLU. (don't notify UI)
				if (!isInOperateMode())
				{
					if( !productManager->isPrintingRemoteLabel() )			// added bc we dont want to do this is we are printing remote labels
					    productManager->clearCurrentProduct();
				}

				// If we're changing to Production Entry mode, or leaving Production Entry mode, then clear the current PLU. (don't notify UI)
				if (newMode == OpModeProductionEntry || currentMode == OpModeProductionEntry)
				{
					if( !productManager->isPrintingRemoteLabel() )
						productManager->clearCurrentProduct(true);
				}

				// If we're changing to Accumulate Items mode, or leaving Accumulate Items mode, then clear the current PLU. (don't notify UI)
				if (newMode == OpModeAccumulateItems || currentMode == OpModeAccumulateItems)
				{
					if( !productManager->isPrintingRemoteLabel() )
						productManager->clearCurrentProduct(true);
				}

				// If we're changing to BuildAMeal mode, or leaving BuildAMeal mode, then clear the current PLU. (don't notify UI)
				if (newMode == OpModeBuildAMeal || currentMode == OpModeBuildAMeal)
				{
					if( !productManager->isPrintingRemoteLabel() )
					    productManager->clearCurrentProduct(true);
				}
				
				if (isInOperateMode() && productManager && productManager->getCurrentProduct())
				{
					// For the 'label types by mode' project, we have a current product and we're changing modes, so we need to re-evaluate the label types and see if they need to change.

					// We're only concerned with setting up for new label types when switching from manual to prepack mode, and vice versa.  All other mode changes seem to clear the current plu.
					productManager->resetUpLabelTypes( (OpModeEvent)newMode );
				}

                if(newMode == OpModeWrapOnly)
                {
                    // This will stop the printer plate positioning screen from popping when we switch to wrapper only mode
                    // on the same plu
                    if(FpmManager::getInstance()->isWrapperTwoPrinter())
                    {
                        // send blank position info
                        LabelApplierPositionInfo newMsg(this, 0, 0, 0, LocNoneSelected, LocNoneSelected, 0, 0, 0);

                        dispatchEvent(&newMsg);

                        Debug::trace("Session.SendLabelInfo", "Sending Blank Info ");
                    }
                }

                if(newMode == OpModeAutoWeigh || newMode == OpModeSemiWeigh)
                {
                    if(FpmManager::getInstance()->isWrapperTwoPrinter())
                    {
                        if(productManager && productManager->getCurrentProduct())
                        {
                            int primaryRotation = productManager->getCurrentProduct()->getLabelRotation(FirstPrinter);
                            int primaryPosition = productManager->getCurrentProduct()->getLabelPlacement(FirstPrinter);

                            int secondaryRotation = productManager->getCurrentProduct()->getLabelRotation(SecondPrinter);
                            int secondaryPosition = productManager->getCurrentProduct()->getLabelPlacement(SecondPrinter);

			    // Fix SOF-5151 Changing Wrap modes changes label positions
                            // check if PLU contains second label
                            LabelType *secondLabelType = productManager->getCurrentProduct()->getLabelType(SecondPrinter);
                            if(secondLabelType)
                            {
                                secondaryPosition = (LaPosLoc)productManager->getCurrentProduct()->getLabelPlacement(SecondPrinter);

                                if( secondaryPosition == LocNoneSelected ) // system default
                                {
                                    Configuration* pConfig = Configuration::getInstance();
                                    if ( pConfig )
                                    {
                                        secondaryPosition = (LaPosLoc)pConfig->getSecDefaultLabelPlacement();
                                        Debug::trace("Session.SendLabelInfo", "2nd Label Pos, setting to sys default: " + Integer::toString(secondaryPosition));
                                    }
                                    
                                }
                            }

                            if( primaryPosition == LocNoneSelected ) // system default
                            {
                                Configuration* pConfig = Configuration::getInstance();
                                if ( pConfig )
                                {
                                    primaryPosition = (LaPosLoc)pConfig->getDefaultLabelPlacement();
                                    Debug::trace("Session.SendLabelInfo", "1st Label Pos, setting to sys default: " + Integer::toString(primaryPosition));
                                }
                                
                            }

                            Debug::trace("Session.SendLabelInfo", "PrimaryRotation = " + inttostring(primaryRotation));
                            Debug::trace("Session.SendLabelInfo", "PrimaryPosition = " + inttostring(primaryPosition));
                            Debug::trace("Session.SendLabelInfo", "Primary length = " + inttostring(firstLabelLength));

                            Debug::trace("Session.SendLabelInfo", "secondaryRotation = " + inttostring(secondaryRotation));
                            Debug::trace("Session.SendLabelInfo", "secondaryPosition = " + inttostring(secondaryPosition));
                            Debug::trace("Session.SendLabelInfo", "secondary length = " + inttostring(secondLabelLength));
							Debug::trace("Session.SendLabelInfo", "secondary rotated = " + inttostring(secondLabelRotated));

                            LabelApplierPositionInfo newMsg(
                                this,
                                0,
                                primaryRotation,
                                secondaryRotation,
                                (LaPosLoc)primaryPosition,
                                (LaPosLoc)secondaryPosition,
                                firstLabelLength,
                                secondLabelLength,
								secondLabelRotated
                            );

                            dispatchEvent(&newMsg);
                        }
                        else
                        {
                            Debug::trace("Session.SendLabelInfo", "productManager or Product is NULL");
                        }
                    }
                }
			}

			if (newMode == OpModeBuildAMeal)
			{
				registerEvent(ProductDev, ProductAddItemToAccumulatedItems);
			}
			else
			{
				if( productManager && !productManager->isPrintingRemoteLabel() )
				    deregisterEvent(ProductDev, ProductAddItemToAccumulatedItems);
			}

            if (newMode == OpModeSelfService)
				// If we're going to self-serve mode, we have to turn off Pick5.
				cfg->setCfg(fieldOffsetCfgPick5Active, Bool::toString(false));

/*
			if (newMode == OpModeSelfService)
			{
                if (tracing)
                    Debug::trace("Session", getName() + HobString(" - Changing to OpModeSelfService mode."));
                newMode = OpModeManualWeigh;
                mostRecentRunMode = OpModeManualWeigh;

                // jhcSS - dont know if I need SS configuration, can I just use the regular config?
                //if (configurationManager)
                //{
                //    SessionMessage readSSConfig(ConfigurationDev, CfgSelfServiceRead);        // jhcSS - dont know if I care about this or not
                //    configurationManager->handleMessage(&readSSConfig);
                //}
            }
*/

            if (isPrimarySession())
            {
                //restoreOperateLanguageSelection();

                registerEvent(SessionDev, SnPrintKeyPressed);
                registerEvent(SessionDev, SnClearKeyPressed);

                //NOTE: SOF-5607 Removed the creation of the boot upgrader. This is now a manual process so an instance 
                //               will need to be created before the KPC bootloader can be upgraded to a new version.
                //               This was done to resolve issue with core upgrade bricking KPC units when the bootloader
                //               needed to be upgraded. This need to be a manual process so that power cannot be removed
                //               or a reboot occur during upgrade. If the upgrade does not complete the KPC is bricked. 
                //KPCBootUpgrader::getInstance()

				// KPC Upgrader HFCs
				registerEvent(KPCBootUpdater, KpcBootUpgraderUpgradeStatus);
				registerEvent(KPCBootUpdater, KpcBootUpgraderUpgradeProgress);
				registerEvent(KPCBootUpdater, KpcBootUpgraderUpgradeInfo);
				registerEvent(KPCBootUpdater, KpcBootUpgraderUpgradeFinished);		// not sure on this one. Move to generic?
				// KPC Upgrader Generics
				registerEvent(KPCBootUpdater, KpcBootUpgraderUpgradeRequired);
				registerEvent(KPCBootUpdater, KpcBootUpgraderStartUpgrade);
				//registerEvent(KPCBootUpdater, KpcBootUpgraderReqUpgradeInfo);		// From UI doesnt have to register
				//registerEvent(KPCBootUpdater, KpcBootUpgraderUpgradeUIReady);		// From UI doesnt have to register
				
                deregisterEvent(OperatorDev, OpLogin);

                HobString modeDesc( "??? (" + Integer::toString(newMode) + ")" );

                if (newMode == OpModeManualWeigh)
                {
                    modeDesc = "Manual Weigh mode";
                    mostRecentRunMode = OpModeManualWeigh;
                }
                else if (newMode == OpModeSemiWeigh)
                {
                    modeDesc = "Semi-Auto Weigh mode";
                    mostRecentRunMode = OpModeSemiWeigh;
                }
                else if (newMode == OpModeAutoWeigh)
                {
                    modeDesc = "Auto Weigh mode";
                    mostRecentRunMode = OpModeAutoWeigh;
                }
                else if (newMode == OpModeWrapOnly)
                {
                    modeDesc = "Wrap Only mode";
                    mostRecentRunMode = OpModeWrapOnly;
                }
                else if (newMode == OpModeAutoManualApply)
                {
                    modeDesc = "Auto Print Manual Apply mode";
                    mostRecentRunMode = OpModeAutoManualApply;
                }
                else if (newMode == OpModeProductionEntry)
                {
                    modeDesc = "Production Entry mode";
                    mostRecentRunMode = OpModeProductionEntry;
                }
                else if (newMode == OpModePrintMultipleLabels)
                {
                    modeDesc = "Print Multiple Labels mode";
                    mostRecentRunMode = OpModePrintMultipleLabels;
                }
				else if (newMode == OpModeAccumulateItems)
				{
					modeDesc = "Accumulate Items mode";
					mostRecentRunMode = OpModeAccumulateItems;
				}
				else if (newMode == OpModeBuildAMeal)
				{
					modeDesc = "BuildAMeal mode";
					mostRecentRunMode = OpModeBuildAMeal;
				}
                else if (newMode == OpModeSelfService)
                {
                    modeDesc = "Self-Service mode";
                    mostRecentRunMode = OpModeSelfService;
                }
                else if (newMode == OpModeOperate)
                {
                    if (mostRecentRunMode == OpModeProductionEntry && !cfg->toBoolCfg(fieldOffsetCfgEnableProductionEntry))
                    {
                        modeDesc = "Manual Weigh mode";
                        mostRecentRunMode = OpModeManualWeigh;
                    }
					else if (mostRecentRunMode == OpModeAccumulateItems && !cfg->toBoolCfg(fieldOffsetCfgEnableAccumItems))
					{
						modeDesc = "Manual Weigh mode";
						mostRecentRunMode = OpModeManualWeigh;
					}
					else if ( mostRecentRunMode == OpModeBuildAMeal )
					{
						modeDesc = "Manual Weigh mode";
						mostRecentRunMode = OpModeManualWeigh;
					}
                }

                if (tracing)
                    Debug::trace("Session", getName() + HobString(" - Changing to ") + modeDesc);

                switch (mostRecentRunMode)
                {
                    case OpModeAutoWeigh:
                        omg = new OperationModeAutoWeigh(this);
                        break;
                    case OpModeSemiWeigh:
                        omg = new OperationModeSemiWeigh(this);
                        break;
                    case OpModeWrapOnly:
                        omg = new OperationModeWrapOnly(this);
                        break;
                    case OpModeManualWeigh:
                        omg = new OperationModeManualWeigh(this);
                        break;
					case OpModeSelfService:
						if (cfg->toBoolCfg(fieldOffsetCfgSelfServePrintOnStableWeight))
							omg = new OperationModeAutoManualApply(this);
						else
							omg = new OperationModeManualWeigh(this);
						break;
                    case OpModeAutoManualApply:
                        omg = new OperationModeAutoManualApply(this);
                        break;
                    case OpModeProductionEntry:
                    case OpModePrintMultipleLabels:
                        omg = new OperationModeProductionEntry(this);
                        break;
					case OpModeAccumulateItems:
						omg = new OperationModeAccumulateItems(this);
						break;
					case OpModeBuildAMeal:
						omg = new OperationModeBuildAMeal(this);
						break;
         	        default:
                        break;
                }


				if( productManager && !productManager->isPrintingRemoteLabel() )
				{
			        if (scaleIsPrepack == false)		//	No need to do this for a Prepack scale.
				    {
	                    MarqueeOpMode mqOpMode(MqOpModeNormal);
	                    send(&mqOpMode, Messenger::DEST_RTC);

	                    NumericOpMode nmOpMode(NmOpModeNormal);
	                    send(&nmOpMode, Messenger::DEST_RTC);

					    //PTR #644 - pre-pack annunciator if auto weigh mode
					    NmState state = NmOff;				
					
					    if ( (newMode == OpModeAutoWeigh) || (newMode == OpModeSemiWeigh) || (newMode == OpModeWrapOnly) || (newMode == OpModeAutoManualApply) )
						    state = NmOn;

					    NumericAnnunciator prepackAnnunciator(NmCode14, state);
					    send(&prepackAnnunciator, Messenger::DEST_RTC);

					    GCDSession* gcdSession = SessionManager::getInstance()->GetGCDSession();
					    if (gcdSession)
						    gcdSession->updatePrepackAnnunciator(state);
                    }
				}
            }


            // If we're in semi-weigh mode, and we have a current product, we don't want to get a weigher status just for
            // the sake of updating the product's current weight, because when this weight arrives in the Session (and
            // if it's stable), we will automatically print a label.  But we don't want to do this if we are coming out of
            // Print Multiple Labels mode.
            if ((productManager && !productManager->getCurrentProduct()) || (mostRecentRunMode != OpModeSemiWeigh && mostRecentRunMode != OpModeAutoManualApply && !productManager->isPrintingRemoteLabel()))
                requestAWeigherStatus();

            newMode = mostRecentRunMode;
            break;
        }

        case OpModeSupervisor:
		{
            Debug::trace("Session", getName() + HobString(" - Changing to Supervisor mode."));
            if (productManager)
                productManager->clearCurrentProduct(true);

			if( ( cutTestManager && cutTestManager->isCuttingTestOn() ) && ( Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1 ) )
			{
				SessionMessage suspendMsg(CutTestDev, SuspendCuttingTest);
				cutTestManager->handleMessage(&suspendMsg);
			}
            else if( cutTestManager && cutTestManager->isCuttingTestOn() )
            {
                if (responseMessageManager)
                    responseMessageManager->addMessage(ErrCutTestDev, CutTestErrFinishCutTestFirst, HfcResponse::RESPONSESTYLE_OK);
            }

            if (operatorManager && operatorManager->getLoggedInOperator())
            {
                SessionMessage logoutMsg(OperatorDev, OpLogout);
                operatorManager->handleMessage(&logoutMsg);
            }

            if (isPrimarySession())
            {
                // The language selection is always the primary language in this mode.
                //setCurrentLanguageSelection (PrimaryLanguage);

                removeAutoLogoutTimer();
                if (responseMessageManager)
                    responseMessageManager->removeAllMessages();

                deregisterEvent(OperatorDev, OpLogin);

                if ((accessKeys.isSupervisorKeyPresent() || accessKeys.isServiceKeyPresent() || isVirtualServiceJumperSet()) &&
					OperatorManager::isSupervisorLoginRequired(this)) //if the service mode jumper is set to service, then bypass login (active low)
                {
                    PAL_absclass::PAL_sleep(1000); // to let remote monitoring sessions to load their pages.

                    HobOperator op;
                    op.setOpNum(cfg->getServiceLoginId());
                    op.setCurrentMode(HobOperator::CURRENTMODE_SERVICE);

                    op.setAccessLevel(9);               //      To log service person directly into Supervisor mode
                    SessionMessage loginMsg(OperatorDev, OpLogin);
                    loginMsg.setFormatType(InternalFormat);
                    loginMsg.setHfcObject(&op);

                    handleMessage(&loginMsg);
                }

                omg = new OperationModeSupervisor(this);
            }
            else
            {
                //deregisterEvent(Weigher, WgStatus, msgQId);
                //deregisterEvent(Printer, PrStatus, msgQId);
                //deregisterEvent(LabelApplier, LaStatus, msgQId);
                //deregisterEvent(OperationMode, OpModeStatus, msgQId);
                deregisterEvent(ProductDev, ProductComplete);
                deregisterEvent(ProductDev, ProductPackageComplete);
                deregisterEvent(ProductDev, ProductIncomplete);
            }
            break;
		}

        case OpModeService:
        {
            Debug::trace("Session", getName() + HobString(" - Changing to Service mode."));
            if (productManager)
                productManager->clearCurrentProduct(true);

			if( ( cutTestManager && cutTestManager->isCuttingTestOn() ) && (Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1) )
			{
				SessionMessage suspendMsg(CutTestDev, SuspendCuttingTest);
				cutTestManager->handleMessage(&suspendMsg);
			}
            else if( cutTestManager && cutTestManager->isCuttingTestOn() )
            {
                if (responseMessageManager)
                    responseMessageManager->addMessage(ErrCutTestDev, CutTestErrFinishCutTestFirst, HfcResponse::RESPONSESTYLE_OK);
            }

            // setup to receive the initial Weigher Config when going into Service mode
            if (configurationManager)
                configurationManager->enterSeviceMode();

			if (scaleIsPrepack == true)
			{
				registerEvent(Wrapper, WrEvolSensors);
				registerEvent(Wrapper, WrCameraCalibration);
				registerEvent(Wrapper, WrEvolPackageDesc);
				registerEvent(Wrapper, WrEvolRemoveUpgradeFiles);
				registerEvent(Wrapper, WrEvolEventCapture);
				registerEvent(Wrapper, WrClearInstallKnifeTime);
				registerEvent(Wrapper, WrEvolReqErrorHistory);
				registerEvent(Wrapper, WrEvolErrorHistory);
				registerEvent(Wrapper, WrEvolPkgDetectorStatus);
				registerEvent(Wrapper, WrEvolReqElevatorPkgDetectorStatus);
				registerEvent(Wrapper, WrEvolReqSealerPkgDetectorStatus);
				registerEvent(Wrapper, WrEvolCameraThreshold);
				registerEvent(Wrapper, WrEvolWriteCameraThreshold);				
				registerEvent(LabelApplier, LaSysStatus);
				registerEvent(LabelApplier, LaSysInfo);
				registerEvent(LabelApplier, LaTestComplete);
				registerEvent(LabelApplier, LaRemoveUpgradeFiles);
				registerEvent(MfgTestDev, RequestPrinterLabelCount);
				registerEvent(MfgTestDev, ClearPrinterLabelCount);
				registerEvent(MfgTestDev, RequestWrapperCycleCount);
				registerEvent(MfgTestDev, RequestInstallDate);
				registerEvent(MfgTestDev, SetInstallDate);

				registerEvent(Wrapper, WrPPPDist);
				registerEvent(Wrapper, WrPPPCalibration);
                registerEvent(Wrapper, WrPPPConfig);
                registerEvent(Wrapper, WrPPPVersion);

			}

			// Commented out the register for the message, we are now always registered for the WgInquire.
            //registerEvent(Weigher, WgInquire, false);

            WeigherRequestInquire wri(this);
            send(&wri, Messenger::DEST_RTC);

            // If we uncomment the following code, the remote session won't see weigher status updates
            // when on the Weigher Information screen.
            //deregisterEvent(Weigher, WgStatus, msgQId);
            //deregisterEvent(Printer, PrStatus, msgQId);

            if (isPrimarySession())
            {
                // The language selection is always the primary language in this mode.
                //setCurrentLanguageSelection (PrimaryLanguage);

                removeAutoLogoutTimer();

                weigherSwitchPressed = false;
                omg = new OperationModeService(this);
            }
            else
            {
                deregisterEvent(ProductDev, ProductComplete);
                deregisterEvent(ProductDev, ProductPackageComplete);
                deregisterEvent(ProductDev, ProductIncomplete);
            }
            break;
        }

        case OpModeServiceLogin:
		{
            Debug::trace("Session", getName() + HobString(" - Changing to ServiceLogin."));
            if (productManager)
                productManager->clearCurrentProduct(true);

			if ( ( cutTestManager && cutTestManager->isCuttingTestOn() ) &&(Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1) )
			{
				SessionMessage suspendMsg(CutTestDev, SuspendCuttingTest);
				cutTestManager->handleMessage(&suspendMsg);
			}
            else if( cutTestManager && cutTestManager->isCuttingTestOn() )
            {
                if (responseMessageManager)
                    responseMessageManager->addMessage(ErrCutTestDev, CutTestErrFinishCutTestFirst, HfcResponse::RESPONSESTYLE_OK);
            }

            if (isPrimarySession())
            {
                removeAutoLogoutTimer();

                weigherSwitchPressed = false;

                /* Registering for this allows the PeripheralPowerupManager to send a login message to the
                 * Primary Console session when the Service Technician plugs in the thumb drive that has the
                 * secret "key" to automatically go into Service mode without entering a Service Login ID.
                 */
                //registerEvent(OperatorDev, OpLogin, msgQId);
                //EvolMessage e(OperationMode, OpModeServiceLogin);
                //dispatchEvent(&e, this);

                PAL_absclass::PAL_sleep(10);       //wait to send message to browser to allow menus to finish drawing.


                if (accessKeys.isServiceKeyPresent() || isVirtualServiceJumperSet())    //if the service mode jumper is set to service, then bypass login (active low)
                {
                    if (responseMessageManager)
                        responseMessageManager->removeMessage(ErrOperationMode,
                                                              ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::InsertServiceKey,
                                                              HfcResponse::RESPONSESTYLE_CANCEL);

                    Debug::trace("Session", "Going in to service");

                    HobOperator op;
                    op.setOpNum(cfg->getServiceLoginId());
                    op.setCurrentMode(HobOperator::CURRENTMODE_SERVICE);
                    op.setAccessLevel(9);               //      To log service person directly into Supervisor mode

                    SessionMessage loginMsg(OperatorDev, OpLogin);
                    loginMsg.setFormatType(BrowserFormat);
                    loginMsg.setHfcObject(&op);
                    handleMessage(&loginMsg);
                }
                else
                {
                    Debug::trace("Session", "Not going in to service");
                    if (responseMessageManager)
                        responseMessageManager->addMessage(ErrOperationMode,
                                ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::InsertServiceKey,
                                HfcResponse::RESPONSESTYLE_CANCEL);
                }
            }
            break;
		}

        case OpModeMonitor:
		{
            if (!isPrimarySession())
            {
                if (responseMessageManager)
                    responseMessageManager->addMessage(ErrSessionDev, SessionDevEnteringMonitorMode, HfcResponse::RESPONSESTYLE_OK);

                sendMonitorModeMessage(SnMonitorModeOn);

                deregisterEvent(Weigher, WgStatus);
                deregisterEvent(Printer, PrStatus);
                deregisterEvent(Printer, PrCutterStatus);
                
                if (cfg->isPrepackScale() == true)
                {
                    deregisterEvent(LabelApplier, LaStatus);
                    deregisterEvent(Wrapper, WrStatus);
                    deregisterEvent(Wrapper, WrEvolStatus);
                    deregisterEvent(Wrapper, WrSettings);
                    deregisterEvent(Wrapper, WrEvolSettings);
                    deregisterEvent(Wrapper, WrInquire);
                    deregisterEvent(Wrapper, WrEvolInquire);
                    deregisterEvent(Wrapper, WrProductLoaded);
                    deregisterEvent(Wrapper, WrEvolSensors);
                    deregisterEvent(Wrapper, WrCameraCalibration);                    
                }

                deregisterEvent(OperationMode, OpModeStatus);
                deregisterEvent(ProductDev, ProductComplete);
                deregisterEvent(ProductDev, ProductPackageComplete);
                deregisterEvent(ProductDev, ProductIncomplete);

                removeAutoLogoutTimer();

                // Tell this remote session to load the DOM currently loaded by the Primary Console.
                Session *primarySession = SessionManager::getInstance()->getPrimaryConsoleSession();
                if (primarySession)
                {
                    SessionMessage remoteMsg(SessionDev, SnWriteCurrentWebPage);
                    remoteMsg.setFormatType(BrowserFormat);
                    HobString remotePage(primarySession->getCurrentWebPageDOM());
                    std::string::size_type pos = remotePage.find("consoletype=");
                    if (pos != std::string::npos)
                        remotePage.replace(pos+12, 1, "2");

                    GenericHfcObject gho(remotePage);
                    remoteMsg.setHfcObject(&gho);
                    send(&remoteMsg, Messenger::DEST_UI);
                }

                PAL_absclass::PAL_sleep(1000);

                Debug::trace("Session.monitor", getName() + HobString(" - Sending SnSyncDOMRequest message to primary console Session ..."));
                // This message is going to the primary console Session.
                // Once the DOM's are synchronized, the primary console will tell the remote
                // browser which web page frames need to be loaded/reloaded.
                SessionMessage syncDomRequest(SessionDev, SnSyncDOMRequest);
                syncDomRequest.setFormatType(InternalFormat);
                send(&syncDomRequest, Messenger::DEST_RTC);
            }
            break;
		}

        case OpModePowerDown:
            if (isPrimarySession())
            {
                // todo - deregister for everything from the RTC
                omg = new OperationModePowerDown(this);
            }
            break;

        case OpModePluEntry:
            // todo - Careful, I don't think this is an actual mode the user can change to.
            break;

        default:
            break;
    }

    currentModeOp = OP_UNKNOWN;

    // Only tell the RTC that the mode is changing if this is the primary session.
    // The RTC doesn't care if the mode changes on a remote browser.
    if (omg)
    {
        send(omg, Messenger::DEST_RTC);
        //dispatchEvent(omg, this);

		PAL_absclass::PAL_sleep(200);

        delete omg;
    }

    currentMode = (OpModeEvent) newMode;

    if (operatorManager && operatorManager->getCurrentOperator())
        operatorManager->getCurrentOperator()->setCurrentMode(currentMode);

    if (isPrimarySession())
    {
#if defined( __linux__ )
        // send version info to hook
        if ( Configuration::getInstance()->getCustomerHooksEnabled() )
        {
            std::string scaleModeStr( "MANUAL" );
            std::string itemEntryModeStr( "NORMAL" );

            if ( OpModeSelfService == currentMode )
                scaleModeStr = "SELF_SERVICE";
            else if ( OpModeAutoManualApply == currentMode )
                scaleModeStr = "PREPACK";

            if ( OpModeProductionEntry == currentMode )
                itemEntryModeStr = "PRODUCTION_ENTRY";
            else if ( OpModeAccumulateItems == currentMode )
                itemEntryModeStr = "ACCUMULATE_ITEMS";

            std::string scaleModeDescStr( "Unknown" );

            switch ( currentMode )
            {
            case OpModeIdle:                    scaleModeDescStr = "Logged Out";         break;
            case OpModeOperate:                 scaleModeDescStr = "Operate";            break;
            case OpModePluEntry:                scaleModeDescStr = "Plu Entry";          break;
            case OpModeAutoWeigh:               scaleModeDescStr = "Auto Weigh";         break;
            case OpModeSemiWeigh:               scaleModeDescStr = "Semi Weigh";         break;
            case OpModeManualWeigh:             scaleModeDescStr = "Manual";             break;
            case OpModeSupervisor:              scaleModeDescStr = "Supervisor";         break;
            case OpModeService:                 scaleModeDescStr = "Service";            break;
            case OpModeServiceLogin:            scaleModeDescStr = "Service Login";      break;
            case OpModePowerDown:               scaleModeDescStr = "Power Down";         break;
            case OpModeMonitor:                 scaleModeDescStr = "Monitor";            break;
            case OpModeWrapOnly:                scaleModeDescStr = "Wrap Only";          break;
            case OpModeStatus:                  scaleModeDescStr = "Status";             break;
            case OpModeSelfService:             scaleModeDescStr = "Self Service";       break;
            case OpModeAutoManualApply:         scaleModeDescStr = "Prepack";            break; // "Automatic"
            case OpModeProductionEntry:         scaleModeDescStr = "PE Manual";          break; // "Production Entry"
            case OpModePrintMultipleLabels:     scaleModeDescStr = "Multiple Labels";    break;
            case OpModeAccumulateItems:         scaleModeDescStr = "AI Manual";          break; // "Accumulate Item"
            case OpModeBuildAMeal:              scaleModeDescStr = "Meal";               break; // "Build-A-Meal"
            default:
                break;
            }

            std::string currentModeStr( Long::toString( currentMode ) );

			hobart_cust_hook_update_scale_state(HOOK_SS_SCALE_MODE, scaleModeStr.c_str());
			hobart_cust_hook_update_scale_state(HOOK_SS_SCALE_MODE_VALUE, currentModeStr.c_str());
			hobart_cust_hook_update_scale_state(HOOK_SS_SCALE_MODE_VALUE_DESCRIPTION, scaleModeDescStr.c_str());
			hobart_cust_hook_update_scale_state(HOOK_SS_SCALE_ITEM_ENTRY_TYPE, itemEntryModeStr.c_str());
        }
#endif

		if (isInOperateMode() && !productManager->isPrintingRemoteLabel() )
		{
			cfg->updateField(fieldOffsetCfgMostRecentRunMode, inttostring(currentMode));
			cfg->saveField(fieldOffsetCfgMostRecentRunMode, this);
		}

		if ( !scaleIsPrepack && !productManager->isPrintingRemoteLabel() )		//	A Prepack scale does not have these displays.
		{
	        MarqueeMode mqMode(SuspendProductMode);
	        send(&mqMode, Messenger::DEST_RTC);

	        MarqueeUpdate mqClearProduct(MqProduct, " ");
	        send(&mqClearProduct, Messenger::DEST_RTC);

	        MarqueeUpdate mqClearDaily(MqDaily, "");
	        send(&mqClearDaily, Messenger::DEST_RTC);

	        NumericGeneric reset(NmReset);
	        send(&reset, Messenger::DEST_RTC);

	        HobString mqDesc = "";
	        if (cfg->isPermanentMarqueeDisplayed())
	        {
				// Permanent Marquee would be in dept 0, not the currently active Dept
	            Util::CSmartPtr<ProductText> mqText(
	                    ProductText::findRecord(psttMarqueeText, cfg->getPermanentMarqueeNumber(), 0, this));
	            if ( mqText.notNull() )
	                mqDesc = mqText->getText();
	        }
	        MarqueeUpdate mqPermanent(MqPermanent, mqDesc.c_str());
	        send(&mqPermanent, Messenger::DEST_RTC);

	        // FT-503 Pre-pack indicator is required now.
			// As of now, the prepack indicator is not required to be turned on.
			//PTR #644 - pre-pack annunciator if auto weigh mode
			NmState state = NmOff;
			if ( (currentMode == OpModeAutoWeigh) || (currentMode == OpModeSemiWeigh) || (currentMode == OpModeWrapOnly) || (currentMode == OpModeAutoManualApply) )
				state = NmOn;

			NumericAnnunciator prepackAnnunciator(NmCode14, state);
			send(&prepackAnnunciator, Messenger::DEST_RTC);

			//FT-580 prepack indicator always on
			GCDSession* gcdSession = SessionManager::getInstance()->GetGCDSession();
			if (gcdSession)
				gcdSession->updatePrepackAnnunciator(state);
		}
    }

	// Trying to determine the source of the Unknown Mode condition seen mostly at Walmart
	if (currentMode < OpModeIdle || currentMode >= OpModeMax)
	{
		Debug::trace("Session", getName() + string(" - chobart_d detected invalid mode ") + inttostring((int)currentMode) + " - creating SnInvalidModeDetected message");

		MessageLogger::logMessage(MessageLogger::ErrorsLog, string("Application detected invalid mode - indicated mode was ") + inttostring((int)currentMode));

		SessionMessage msg(SessionDev, SnInvalidModeDetected);
		msg.setFormatType(BrowserFormat);
		GenericHfcObject gho(inttostring((int)currentMode));
		msg.setHfcObject(&gho);
		handleMessage(&msg);
	}
}


void Session::handleRegistrationMessage()
{
	// This message is the rest of the handling of the Session Registration Message (100,1)
	// I separated it out from the handleSessionMessage() method.
	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	bool tracingSession = Debug::isTracing("Session");

    if (isPrimarySession())
    {
		if (config->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
            setupFakePrinterManager(FirstPrinter, false);   // PJR-444 - SOF-5063 - Print wider labels on Zebra printer

		for (int i = (int)SecondPrinter; i < (int)MaxPrinterTypes; i++)
		{
			Debug::trace( "Session", "checking if we need a fake PrinterManager for printer:" + inttostring((int)i) );

			Device *printerDevice = Device::findDevice(Printer, i, this);
			if ( printerDevice && printerDevice->isANonHobartLabelPrinter() )
			{
				// A Zebra printer will not automatically have a PrinterManager set up for it, so we are going to create a fake one, to render the label
				Debug::trace("Session", "found a non-Hobart label printer (Zebra printer), will now setup a fake printer manager ...");
                setupFakePrinterManager((PrinterType)i, true);   // PJR-444 - SOF-5063 - Print wider labels on Zebra printer
            }
		}
	}

    if (responseMessageManager)
        responseMessageManager->setDisplayingDialogs(false);

	registerEvent(GraphicDev, NewPlanogramReceived);
	registerEvent(RecallDev, CheckForRecalls);
	registerEvent(SessionDev, StopContinuousNetworkStatus);  // Always register for this because the NetworkConnectivityManager can send it if the supervisor turns off network checking.
    registerEvent(SessionDev, SnRequestIntranetAddress);

    // All Sessions need to register for this so that they will be notified when any
    // configuration value is saved to the database.  Each Session will then re-read
    // the configuration and send it to its UI.
    registerEvent(ConfigurationDev, CfgSaved);
    registerEvent(ConfigurationDev, CfgGenericTextUpdated);
    registerEvent(SessionDev, SnRefreshFPPFrame);

	registerEvent(TransactionTypeDev, TransactionTypeRefreshCurrentType);

    if (isPrimarySession())
    {
		registerEvent(HIDDev, HIDDeviceEvent::MOUSE_ATTACHED);
		registerEvent(HIDDev, HIDDeviceEvent::MOUSE_REMOVED);
		registerEvent(HIDDev, HIDDeviceEvent::KEYBOARD_ATTACHED);
		registerEvent(HIDDev, HIDDeviceEvent::KEYBOARD_REMOVED);
		registerEvent(HIDDev, HIDDeviceEvent::BARCODE_SCANNER_ATTACHED);
		registerEvent(HIDDev, HIDDeviceEvent::BARCODE_SCANNER_REMOVED);
        registerEvent(HIDDev, HIDDeviceEvent::STORAGE_DEVICE_ATTACHED);
        registerEvent(HIDDev, HIDDeviceEvent::STORAGE_DEVICE_REMOVED);

		registerEvent(SessionDev, SnRestoreIndependentData);
		registerEvent(SessionDev, SnUsingEmptyDatabase);
		registerEvent(SessionDev, SnUsingMirrorDatabase);

        deregisterEvent(SessionDev, SnShutdownNonPrimary);  // Registered for in AbstractSession, primary session deregisters for it.
		registerEvent(SessionDev, SnCheckForExpiredRecords);
		registerEvent(GraphicDev, CheckForNewPlanogramReceived);
		registerEvent(RecallDev, CheckForRecalls);
		registerEvent(BackupRestore, BackupEntireDB);
		registerEvent(SessionDev, SnExecuteTask);
		registerEvent(SessionDev, SnUpgradeFirmwareStatus);
		registerEvent(SessionDev, SnUpgradeSFTPBadAuthentication);

        registerEvent(OperationMode, OpModeSupervisor);

        registerEvent(VersionManagerDev, SystemShutDownRequest);
        registerEvent(VersionManagerDev, FirmwareUpdateStatus);
        registerEvent(VersionManagerDev, ProcessDBFilesReq);

        registerEvent(NetworkConnectivityDev, GoodComm);
        registerEvent(NetworkConnectivityDev, BadComm);
		// For Fastrack, we are eliminating the annoyance messages that we are still offline
        //registerEvent(NetworkConnectivityDev, BadCommRepeat);
			 
// TODO TODO TODO : These were in Windux GCD R2_1, but not in fastrack - brought these over, but not fully integrated from Windux.					 
//				registerEvent(NetworkConnectivityDev, ActivityNormal);
//				registerEvent(NetworkConnectivityDev, ActivityBusy);

        registerEvent(CGICallManagerDev, comm::CGICallPerformed);


        // Back-door to tell the Primary console to return to the self-serve start screen.
        registerEvent(SessionDev, SnTellBrowserGotoHomepage);

		// Lets the primary session always listen for remote print messages, but they will only be acted upon if told to do so
		// Only the Primary needs to listen for these, since they are passed using internalformat from the Remote Session to the Primary Session for printing the label(s)
		registerEvent(SessionDev, SnBackendRemotePrint);
		registerEvent(SessionDev, SnBackendIsScaleAvailable);
        registerEvent(SessionDev, SnBackendPrinterStatus);
		registerEvent(SessionDev, SnBackendRemotePrintTemplateLabel);

        // Tell the Event Manager that I want to start listening for messages indicating a Session
        // wants to enter or leave monitor mode.
        registerEvent(SessionDev, SnMonitorModeOn);
        registerEvent(SessionDev, SnMonitorModeOff);
        registerEvent(SessionDev, SnSyncDOMRequest);

        registerEvent(SessionDev, SnKeyActivation);
        // Starting with Fastrack, we will no longer receive this.
		// registerEvent(SessionDev, SnKeyDeactivation);
        registerEvent(SessionDev, SnUsbKbdAttachChange);

        // The primary session needs to be always listening for the following messages
        // because any remote session may be requesting statistics at any time.
        registerEvent(SessionDev, SnContinuousStatistics);
        registerEvent(SessionDev, SnStopStatistics);

        registerEvent(SessionDev, SnShutdownComplete);

        registerEvent(SessionDev, SnRequestSaveConfiguration);

        registerEvent(ProductDev, ProductFinishedPrintingAllLabels);
        registerEvent(ProductDev, ProductStartPrintForTransaction);
		registerEvent(ProductDev, ProductReadAllAccumulatedItems);
		registerEvent(ProductDev, ProductLeaveBuildAMealMode);
		registerEvent(ProductDev, ProductReadToo);

		registerEvent(SessionDev, SnDisplayDialog);

		// HT-326
		registerEvent(SessionDev, SnRdpActive);
		registerEvent(SessionDev, SnRdpInactive);

        if (config->isPrepackScale() == true)
        {
            registerEvent(VersionManagerDev, InitiateUpgradeProcess);
            registerEvent(VersionManagerDev, PeripheralUpgradeList);
            registerEvent(VersionManagerDev, UpgradeComplete);
            registerEvent(VersionManagerDev, UpgradeXferStatus);
            registerEvent(VersionManagerDev, UsbWrapperUpgradeStatus);                        
        }

		registerEvent(Weigher, WgBeginRemoteWgConfig);
		registerEvent(Weigher, WgEndRemoteWgConfig);

        operateLanguageSelection = PrimaryLanguage;
        setCurrentLanguageSelection(PrimaryLanguage);

		registerEvent(Weigher, WgReadAllAuditTrailForHTe);
		registerEvent(Weigher, WgReadWeigherConfigForHTe);

		// Can be received from ProductionEntryMode
		registerEvent(ProductDev, ProductStartPrintAllLabels);
		registerEvent(ProductDev, ProductPausePrintAllLabels);
		// set up listening for messages from smBackend
	}

    // CAMERA
    registerEvent(SoftkeyDev, ListCameraSystemPredictions);

	//  We always listen for a WgInquire and SysInquire messages coming in.
	//  They won't be coming in constantly, plus the RealTimeController may ask for them (if the FPC enumerates after the UI is up and running),
	//  so we always process any WgInquire/SysInquire that come in; they can be unsolicited.
	registerEvent(Weigher, WgInquire);
	registerEvent(WeigherSystem, SysInquire);
	registerEvent(Weigher, WgCat3Records);
	registerEvent(Weigher, WgCat3AuditStatistics);
	registerEvent(Weigher, WgCat3RecordStatus);
	registerEvent(Weigher, WgResetFactoryDefaultsComplete);
	registerEvent(Weigher, WgReqValueMax);
	registerEvent(Weigher, WgValueMaxFitParams);

    registerEvent(Printer, PrGapCalStatus); 
    registerEvent(Printer, PrStartGapCalibration);
    registerEvent(Printer, PrGapCalNext );
    
    if (isIntranetSession())
    {
        registerEvent(SessionDev, SnSetFlashkeyInternetWebPage);
        registerEvent(SessionDev, SnSwitchToIntranetFrame);
    }
    else
    {
        registerEvent(SynchronizationDev, synchronization::SynchronizationStatusUpdateEvent);
        EvolMessage reqSynchStatus(SynchronizationDev, synchronization::RequestSynchronizationStatusEvent, this);
        dispatchEvent(&reqSynchStatus);

        // For now, all Sessions can register to receive RFID messages.
        // We may want to display messages on remote consoles when tags are detected and removed.
        // We'll get one of these messages whenever an rfid tag is detected, or removed.
        registerEvent(RFIDReader, RFIDCurrentID);
        registerEvent(RFIDReader, RFIDCurrentStatus);

		registerEvent(BarcodeScannerDev, BarcodeReadScannerCode);

        registerEvent(TimerDev, TimerExpired);

        registerEvent(SoftkeyDev, LoadAllSoftkeyGroups);

        registerEvent(System, SysPeripheralList);
        registerEvent(PrinterSystem, SysStatus);
        registerEvent(Printer, PrHeadType);


        HfcRequestPeripheralList reqPeripheralList(this);	//	We do need this.
        dispatchEvent(&reqPeripheralList);
    }

    registerEvent(Diagnostics, DiagInitWizard);
	registerEvent(Diagnostics, DiagPerformWizardTesting);
	
    manageAutoLogoutTimer();

/*
    if (config->isPrepackScale())
    {
        // When first registering the Session, if we are an EPP, initially put it into OpModeAutoWeigh.
        if (tracingSession)
            Debug::trace("Session", "Is prepack scale, so setting up for OpModeAutoWeigh");

        SessionMessage mode(OperationMode, OpModeAutoWeigh);
        handleModeMessage(&mode);

        if (tracingSession)
            Debug::trace("Session", "Done setting up for OpModeAutoWeigh");
    }
    else
    {
        // When first registering the Session, initially put it into ManualWeighMode.
        if (tracingSession)
            Debug::trace("Session", "Setting up for OpModeManualWeigh");

        SessionMessage mwmode(OperationMode, OpModeManualWeigh);     // OpModeAutoManualApply, OpModeManualWeigh
        handleModeMessage(&mwmode);

        if (tracingSession)
            Debug::trace("Session", "Done setting up for OpModeManualWeigh");
    }
*/

	//The above was replaced by the following, since we are storing the current mode in the db, so we can return there when we reboot
	previousModeForStartup = config->toIntCfg(fieldOffsetCfgMostRecentRunMode);

	// We don't come back up in BuildAMeal mode - Try to fix Unknown Mode
	if ( previousModeForStartup <= OpModeIdle || previousModeForStartup >= OpModeMax || previousModeForStartup == OpModeBuildAMeal || previousModeForStartup == OpModePrintMultipleLabels )
		previousModeForStartup = OpModeManualWeigh;

	// SOF-789: Wrap-only
	if (config->isScaleModelWrapOnly())
		previousModeForStartup = OpModeWrapOnly;

	if ( previousModeForStartup != OpModeSelfService )
	{
		SessionMessage mode(OperationMode, previousModeForStartup);     // OpModeAutoManualApply, OpModeManualWeigh
		if (tracingSession)
			Debug::trace("Session", "Setting up for mode: " + inttostring(previousModeForStartup));
		handleModeMessage(&mode);
	}

    if (tracingSession)
        Debug::trace("Session", "handling read all config");

    if (configurationManager)
        configurationManager->handleReadAllConfigMsgs();

    if (tracingSession)
        Debug::trace("Session", "Done handling read all config");


    if (!isIntranetSession())
    {
        //  Register to receive any PrinterStockMetrics Messages.
		registerEvent(Printer, PrStockMetrics);

		//  SOF-2288 - Always register for the PrInquire.  We used to only register for it when the UI sent in a PrReqInquire,
		//  but then the ConfigurationManager wouldn't have the most recent copy of the printer configuration, until the PrReqInquire was received.
		registerEvent(Printer, PrInquire);

        //  Now request PrinterStockMetrics Messages from all printers.
        if (tracingSession)
            Debug::trace("Session", "Now request PrinterStockMetrics Messages from all printers");

        HfcPrinterRequestStockMetrics prReqStockMetricsMsg;
        SessionMessage request(Printer, PrReqStockMetrics);
        request.setFormatType(InternalFormat);
        request.setHfcObject(&prReqStockMetricsMsg);

        for (short printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
        {
            prReqStockMetricsMsg.setPrinterType((PrinterType)printerNum);

            if (tracingSession)
            {
                const char *msgdata = request.toString();
                int msglen = request.getSerializedLength();
                Debug::trace("Session",     HobString("printer request stock metrics response: ") + HobString(Hex::toString(msgdata, msglen)));
            }

            send(&request, Messenger::DEST_RTC, false);
        }

        if (tracingSession)
            Debug::trace("Session", "Requesting PrinterStatus from all printers");
        for (short printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
        {
            PrinterRequestStatus prs(this, (PrinterType)printerNum);
            send(&prs, Messenger::DEST_RTC, false);
        }

        if (tracingSession)
            Debug::trace("Session", "Now request weigher status");
        // Request the weigher status message
        requestAWeigherStatus(false);

        if (isPrimarySession())
        {
            SessionMessage keyActivationRequestMsg(SessionDev, SnRequestKeyActivation);
            keyActivationRequestMsg.setFormatType(InternalFormat);
            send(&keyActivationRequestMsg, Messenger::DEST_RTC, false);
        }

//	HW-196: Wait until the Operator acknowledges that Initialization is complete, instead of setting a timer.
//	New fix: Set timer (2 sec by default) to check if any peripherals have attached. We are seeing a delay under Windows 7
//			 where the peripherals come up after the Initialization complete message. Then you end up with an empty Missing Peripherals page.
		// jhc TODO: Need someway to reset this back and do this only on debug versions
		peripheralCheckCounter_ = 25;	//	35; // 40;
#ifdef _DEBUG
        peripheralCheckCounter_ = 3;    // override it bc its getting on my last nerve
#endif // _DEBUG

        Debug::trace("Session.initComplete", getName() + HobString(" - handleRegistrationMessage():  Set peripheralCheckCounter_ = ") + inttostring(peripheralCheckCounter_));
		setBootupCheckForPeripheralsTimer();
//	End of new fix
    }

	if (config->toBoolCfgI(CfgInternal::PLANOGRAMSENABLED))
	{
		if (config->toStringCfg(fieldOffsetCfgMostRecentPlanogramGraphic) != "" &&
			!config->toBoolCfg(fieldOffsetCfgMostRecentPlanogramAcknowledged))
		{
			SessionMessage newPlanogramMsg(GraphicDev, NewPlanogramReceived);
			newPlanogramMsg.setFormatType(InternalFormat);
			if (getGraphicManager())
				getGraphicManager()->handleMessage(&newPlanogramMsg);
		}
	}

	if (config->toBoolCfgI(CfgInternal::RECALLSENABLED))
	{
		SessionMessage msg(RecallDev, CheckForRecalls);
		handleMessage(&msg);
	}

    Debug::trace("Session.keys", getName() + HobString(" - handleRegistrationMessage():  SessionMessage keys(SessionDev, SnKeyActivation)"));
    SessionMessage keys(SessionDev, SnKeyActivation);
    handleSessionMessage(&keys);

	// FT-153 diagnostic logging
	// log out software version and serial number
	if ( isPrimarySession() )
	{
		hfc::MultiLanguageText internationalize;
		std::string info = "Scale information:";

        // Build Number
		info += "\n" + internationalize("buildNumber") + " " + BuildNumber::buildNumber;

        // Version
        HobComplexVersion *myVersion = VersionManager::getInstance()->getSystemControllerSoftwareVersion();
        if (myVersion)
        	info += "\n" + internationalize("appVersion") + " " + myVersion->getRevMajMinBuildStr();

        // OS Version
        HobVersion *Pver = VersionManager::getInstance()->getOSVersion();
        if (Pver)
        	info += "\n" + internationalize("osVersion") + " " + Pver->getRevMajMinStr();

		// Serial Number
        MtdManufacturingData *pMfgScaleData = FpmManager::getInstance()->readMfgScaleData();
        if ( !pMfgScaleData )
        	info += "\n" + internationalize("serialNumber") + " NOT AVAILABLE";
        else
        {
			HfcWriteFpmManData mfgData(pMfgScaleData, this);
			info += "\n" + internationalize("serialNumber") + " " + mfgData.getSerial();
        }

		MessageLogger::logMessage(MessageLogger::InfoLog, info);
	}

	if (isPrimarySession())
	{
		// Dispatch a message to get the printer's version data to update in 'FPCVersion' of the configs table.
		PrinterSystemRequestStatus psrs;
        dispatchEvent(&psrs);
	}

	if (isRemoteSession() && config->toIntCfg(fieldOffsetCfgRemoteAccessLoginMethod) != RemoteAccessLoginMethod_None)
	{
        SessionMessage loginRequiredMsg(OperatorDev, OpRemoteLoginRequired);
		loginRequiredMsg.setFormatType(BrowserFormat);
        GenericHfcObject gho(config->toStringCfg(fieldOffsetCfgRemoteAccessLoginMethod));
		loginRequiredMsg.setHfcObject(&gho);
        send(&loginRequiredMsg, Messenger::DEST_UI);

		currentModeOp = OP_WAITINGFORREMOTELOGIN;
	}
	else
	{
		// For Fastrack, get the response back to the UI right away to let them know we are connected/communicating.
		// The UI can receive its configuration messages later.
		SessionMessage registerResponse(SessionDev, SnRegister);
		registerResponse.setFormatType(BrowserFormat);
		send(&registerResponse, Messenger::DEST_UI);
	}

    if (tracingSession)
        Debug::trace("Session", "Finished registering console");
}

void Session::checkForExpiredRecordsThread()
{
	Debug::trace("PurgeOldRecords", getName() + HobString(" - SnCheckForExpiredRecords start"));

#if defined (__linux__)
	sched_param params;
	params.sched_priority = 0;

	// lower thread priority
	if ( 0 != pthread_setschedparam( pthread_self(), SCHED_IDLE, &params ) )
		Debug::trace( "WrapperManager.Timer", "failed to set priority!" );
#endif

	// Check for expired transactions, those that have been around too long, longer than the
	// number of days specified in the configuration variable fieldOffsetCfgCollectingTransactionsDays.
	// [Lets attempt to clean up transactions just in case there are transactions by mistake.]
	if ( transactionManager && /*Configuration::getInstance()->toBoolCfg(fieldOffsetCfgCollectingTransactions) &&*/ Configuration::getInstance()->toIntCfg(fieldOffsetCfgCollectingTransactionsDays) > 0 )
		transactionManager->purgeOldTransactions(Configuration::getInstance()->toIntCfg(fieldOffsetCfgCollectingTransactionsDays), MessageLogger::ScheduledEventsLog);

	// Check for shelf life expiration labels that have been expired for longer than the configured time (in days) to keep them around.
	if ( expirationLabelManager )
		expirationLabelManager->purgeOldExpirationLabels();

	// Check for product statistics that have been around longer than the configured number of days to keep them around.
	if ( productManager && Configuration::getInstance()->toIntCfg(fieldOffsetCfgResetStatisticsDays) > 0 )
		productManager->purgeOldProductStatistics(Configuration::getInstance()->toIntCfg(fieldOffsetCfgResetStatisticsDays));

	Debug::trace("PurgeOldRecords", getName() + HobString(" - SnCheckForExpiredRecords end"));
}


// Private method
void Session::handleSessionMessage(SessionMessage *msg)
{
    bool tracingSession = Debug::isTracing("Session");
    if (tracingSession)
        Debug::trace("Session", getName() + HobString(" - handling SESSION DataType"));

	Configuration *config = Configuration::getInstance();

    switch (msg->getEventType())
    {
        case SnPrintKeyPressed:
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnPrintKeyPressed Event"));
            if (isInOperateMode() && productManager)
                productManager->handleMessage(msg);
            break;

		case SnBackendRemotePrint:
		case SnBackendRemotePrintTemplateLabel:
		{
			UINT8 ev = msg->getEventType();
			Debug::trace("ProductManager.remotePrint", getName() + " - received Event " + ((ev == SnBackendRemotePrint) ? "SnBackendRemotePrint" : "SnBackendRemotePrintTemplateLabel") );

			if (isPrimarySession() && productManager)
			{
				// SECOND STEP: We are now a primary because we passed the same msg from the remote, to the primary, now lets process it.
				Debug::trace("ProductManager.remotePrint", getName() + " - I'm the primary session, so passing to ProductManager to handle this");

				int status_code = handleRemotePrintInitialAvailability();
				if (status_code != RemotePrintScaleIsAvailable)
				{
					if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::TEMPLATELABELSHOWOPERATORDIALOGS) )
					{
						switch (status_code)
						{
							case ProductErrPrintPluLoadedOnScale:			// warning, so will be automatically resubmitted
							case ProductErrCannotPrintInProdEntryMode:		// warning, so will be automatically resubmitted
							case ProductErrCannotPrintAccumulateMode:		// warning, so will be automatically resubmitted
							case ProductErrCannotPrintBuildAMeal:			// error, so will NOT be automatically resubmitted
							case ProductErrCannotPrintInSupervisorMode:		// error, so will NOT be automatically resubmitted
								if (responseMessageManager)
									responseMessageManager->addMessage(ErrProductDev, status_code, HfcResponse::RESPONSESTYLE_OK);
								break;
							default:
								break;
						}
					}

					Debug::trace("ProductManager.remotePrint", getName() + " - Scale is not available for remote print; handleRemotePrintInitialAvailability() returned: " + Integer::toString(status_code));
					remotePrintReturnMessage(status_code, 0);
				}
				else
					// had to check this, bc if we got an error which makes us not need to print, we went into
					// pm->handleMessage() here and try and print anyway which is not good bc we have an error (like on bootup not taking labels)
					productManager->handleMessage(msg);			// go print the label from remote connection
			}
			else
			{
				Debug::trace("ProductManager.remotePrint", getName() + " - I'm a remote session, so forwarding on to primary session");

				registerEvent(ProductDev, ProductRemotePrintStatus);

				msg->setFormatType(InternalFormat);
				send(msg, Messenger::DEST_RTC);
			}
			break;
		}

		case SnBackendIsScaleAvailable:
		{
			// Note: the remote print session can handle this message without passing it to the primary session

			Debug::trace("ProductManager.remotePrint", getName() + " - handling SessionDev | SnBackendIsScaleAvailable (100,161)");

			int status_code = handleRemotePrintInitialAvailability();	// Lets get the availableness party started
			Debug::trace("ProductManager.remotePrint", getName() + " - handleRemotePrintInitialAvailability() returned " + Integer::toString(status_code));

			// Send message back to the remote device who invoked this call. 
			HobString data = Integer::toString(status_code) + "|0|0|done";		// 101,92,retVal|resubmit|labels|done

			Debug::trace("ProductManager.remotePrint", getName() + " - sending result message to smBackend: " + data);

			SessionMessage remoteReturnMsg(ProductDev, ProductRemotePrintStatus);
			remoteReturnMsg.setFormatType(BrowserFormat);
			GenericHfcObject gho(data);
			remoteReturnMsg.setHfcObject(&gho);
			send(&remoteReturnMsg, Messenger::DEST_UI);

			Debug::trace("ProductManager.remotePrint", getName() + " - done handling SnBackendIsScaleAvailable, will now terminate");
			PAL_absclass::PAL_sleep(250);
			//terminate(false);
			EvolMessage terminateMsg(SessionDev, SnTerminate);
			dispatchEvent(&terminateMsg, this);
			break;
		}

        case SnBackendPrinterStatus:
        {
            Debug::trace("ProductManager.remotePrint", getName() + " - handling SessionDev | SnBackendPrinterStatus (100,164)");
            // Get printer status
            int status_code = checkForRemotePrinterStatus();
            Debug::trace("ProductManager.remotePrint", string("handleRemotePrintInitialAvailability() - return from checkForRemotePrinterStatus: ") + Integer::toString(status_code));

            if (status_code == 0) {
                status_code = RemotePrintScaleIsAvailable;
            }

            // Send message back to the remote device who invoked this call. 
            HobString data = Integer::toString(status_code) + "|0|0|done";		// 101,92,retVal|resubmit|labels|done

            Debug::trace("ProductManager.remotePrint", getName() + " - sending result message to smBackend: " + data);

            SessionMessage remoteReturnMsg(ProductDev, ProductRemotePrintStatus);
            remoteReturnMsg.setFormatType(BrowserFormat);
            GenericHfcObject gho(data);
            remoteReturnMsg.setHfcObject(&gho);
            send(&remoteReturnMsg, Messenger::DEST_UI);

            Debug::trace("ProductManager.remotePrint", getName() + " - done handling SnBackendPrinterStatus, will now terminate");
            PAL_absclass::PAL_sleep(250);
            //terminate(false);
            EvolMessage terminateMsg(SessionDev, SnTerminate);
            dispatchEvent(&terminateMsg, this);
            break;
        }

		case SnSelfServReimageLabels:
			if( config->isSelfServiceScale() )
				productManager->handleMessage(msg);
			break;

		case snMergeNow:
		{
			Debug::trace("SessionManager.ScheduledMerge", "- handling snMergeNow Event");

			msg->setFormatType(InternalFormat);
			send(msg, Messenger::DEST_RTC);
			break;
		}

		case SnClearKeyPressed:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnClearKeyPressed Event"));

            // Forward the Clear Key on to the browser.
            SessionMessage clearKeyMsg(SessionDev, SnClearKeyPressed);
            clearKeyMsg.setFormatType(BrowserFormat);
            send(&clearKeyMsg, Messenger::DEST_UI);
            break;
        }

        case SnRegister:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnRegister Event"));

            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (gho)
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - will try to set session type to : ") + gho->getData());

                setSessionType(Integer::valueOf(gho->getData().c_str()));
            }
            else
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - The sessionType was not passed in with this message, defaulting to a remote session"));

                setSessionType(SESSIONTYPE_REMOTE);
            }

            //register here instead of handleRegistrationMessage() because during upgrades that call is delayed
            if (isPrimarySession())
            {
				// KPC Loader HFCs
				registerEvent(KPCBootloader, KpcBootUpgradeStatus);
				registerEvent(KPCBootloader, KpcBootUpgradeProgress);
				registerEvent(KPCBootloader, KpcBootUpgradeInfo);

                // SOF-3478
                registerEvent(KPCBootloader, KpcBootUpgradeCancel);
                registerEvent(KPCBootloader, KpcBootUpgradeRequested);
                // SOF-3478 END

				registerEvent(KPCBootloader, KpcBootUpgradeFinished);		// not sure on this one. Move to generic?
				// KPC Loader Generics
				registerEvent(KPCBootloader, KpcBootUpgradeRequired);
				registerEvent(KPCBootloader, KpcBootStartUpgrade);
				//registerEvent(KPCBootloader, KpcBootReqUpgradeInfo);		// From UI doesnt have to register
				//registerEvent(KPCBootloader, KpcBootUpgradeUIReady);		// From UI doesnt have to register
            }

			if (isRemoteSession() && !config->toBoolCfg(fieldOffsetCfgRemoteAccessPermitted))
			{
	            if (responseMessageManager)
		            responseMessageManager->addMessage(ErrSessionDev, SessionDevRemoteAccessProhibited, HfcResponse::RESPONSESTYLE_OK);

                PAL_absclass::PAL_sleep(3000);

				terminate(true);
				break;
			}

	       registerEvent(ProductDev, ProductStatusEvent);

            // FT-804: Do not allow registration until DB restored
			if ( SessionManager::getInstance()->isRestoringDB() )
			{
				// We have to wait for the database to be restored after an upgrade before we continue the registration process
				currentModeOp = OP_WAITINGTOFINISHREGISTRATION;

	            // FT-804: Tell UI we are restoring a DB
	            SessionMessage goAway(SessionDev, DBRestoreInProgress);
	            goAway.setFormatType(BrowserFormat);
	            send(&goAway, Messenger::DEST_UI);

	            if (responseMessageManager)
		            responseMessageManager->addMessage(ErrVersionManagerDev, VersionManagerRestoreIndependentFormat, HfcResponse::RESPONSESTYLE_NOINPUT);

	            break;
			}

			if ( isRemotePrintSession() )
			{
				// reply to smBackend caller that a remote registration was made. YEA!!
				Debug::trace("ProductManager.remotePrint", "handleRegistrationMessage() complete - Send message back that our remote session is created");

				SessionMessage remoteReturnData(ProductDev, ProductRemotePrintStatus);
				remoteReturnData.setFormatType(BrowserFormat);
				HobString data(Integer::toString(RemotePrintRegistrationSuccess));			// 997 = success, 996 = failure - NOT LIKING THIS
				data = data.append("|0|0|done");											// 101,92,retVal|resubmit|labels|done

				// Just pass the registration success back to the backend - no termination needed on socket registration 
				GenericHfcObject gho(data);
				remoteReturnData.setHfcObject(&gho);
				send(&remoteReturnData, Messenger::DEST_UI);
			}
			else
			{
				// Finish the registration process
				handleRegistrationMessage();
			}

            break;
        }

        case SnVersionInfo:
        {
            SessionMessage versionMsg(SessionDev, SnVersionInfo);
            versionMsg.setFormatType(BrowserFormat);
            VersionInfo vi;
            versionMsg.setHfcObject(&vi);
            send(&versionMsg, Messenger::DEST_UI);
            break;
        }

		case SnUpgradeFirmwareStatus:
			{
				SessionMessage upgradeFirmwareStatusMsg(SessionDev,SnUpgradeFirmwareStatus);
			    upgradeFirmwareStatusMsg.setFormatType(BrowserFormat);
				GenericHfcObject gho;
				gho.setData("Finished");
				upgradeFirmwareStatusMsg.setHfcObject(&gho);
				send(&upgradeFirmwareStatusMsg, Messenger::DEST_UI);	
				break;
			}

     	case SnUpgradeSFTPBadAuthentication:
			{
				SessionMessage upgradeFirmwareStatusMsg(SessionDev,SnUpgradeSFTPBadAuthentication);
			    upgradeFirmwareStatusMsg.setFormatType(BrowserFormat);
				GenericHfcObject gho;
				gho.setData("BadAuthentication");
				upgradeFirmwareStatusMsg.setHfcObject(&gho);
				send(&upgradeFirmwareStatusMsg, Messenger::DEST_UI);	
				break;
			}

        case SnMonitorModeOn:
        {
            // Only allow monitoring of the primary console.
            if (isPrimarySession())
            {
                if (tracingSession)
                    Debug::trace("Session.monitor", getName() + HobString(" - handling MonitorMode ON Event:"));
                addMonitoringSession(dynamic_cast<Session *> (msg->getSourceObject()));
            }
            else
            {
                // A remote browser sent this message to its remote Session to put it in Monitor Mode.
                SessionMessage monitorModeMsg(OperationMode, OpModeMonitor);
                handleModeMessage(&monitorModeMsg);

                // zzzz what are these for?
                registerEvent(SessionDev, SnPrimaryToMonitoringSessionPopDialog);
                registerEvent(SessionDev, SnPrimaryToMonitoringSessionMsg1);
                registerEvent(SessionDev, SnPrimaryToMonitoringSessionMsg2);
            }
            break;
        }


        case SnMonitorModeOff:
        {
            // Only allow monitoring of the primary console.
            if (isPrimarySession())
            {
                if (tracingSession)
                    Debug::trace("Session.monitor", getName() + HobString(" - primary session is handling MonitorMode OFF Event:"));
                removeMonitoringSession(dynamic_cast<Session *> (msg->getSourceObject()));
            }
            else
            {
                // zzzz what are these for?
                deregisterEvent(SessionDev, SnPrimaryToMonitoringSessionPopDialog);
                deregisterEvent(SessionDev, SnPrimaryToMonitoringSessionMsg1);
                deregisterEvent(SessionDev, SnPrimaryToMonitoringSessionMsg2);

                sendMonitorModeMessage(SnMonitorModeOff);

                // If we don't set this before handling the OperationMode message, we won't process the mode change.
                // The handleModeMessage() won't process the mode change if the current mode is OpModeMonitor.
                currentMode = OpModeIdle;

                // A remote browser sent this message to its remote Session to leave Monitor Mode.
                SessionMessage monitorModeMsg(OperationMode, (config->isSelfServiceScale()) ? OpModeSelfService : OpModeManualWeigh);
                handleModeMessage(&monitorModeMsg);
            }
            break;
        }


        case SnPrimaryToMonitoringSessionPopDialog:
        {
            GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (msgData && !msgData->getData().empty())
            {
                HobString dialogData = msgData->getData();
                HobTokenizerCSV tok(&dialogData, BrowserFormatSeparator, 4);
                HobString token;

                HobString hsAddDialog, hsErrorDevice, hsErrorCode, hsResponseStyle;
                bool addDialog = false;
                ErrorEvent errorDevice = ErrUnknown;
                int errorCode = 0;
                short responseStyle = 0;

                if (tok.next(&hsAddDialog) != -1)
                    addDialog = (Bool::valueOf(hsAddDialog));
                if (tok.next(&hsErrorDevice) != -1)
                    errorDevice = (ErrorEvent)(Integer::valueOf(hsErrorDevice));
                if (tok.next(&hsErrorCode) != -1)
                    errorCode = (Integer::valueOf(hsErrorCode));
                if (tok.next(&hsResponseStyle) != -1)
                    responseStyle = (short)(Integer::valueOf(hsResponseStyle));


                if (Debug::isTracing("Session.monitormode"))
                    Debug::trace("Session.monitormode", getName() +
                            HobString("case SnPrimaryToMonitoringSessionPopDialog - add: ") + hsAddDialog +
                            HobString("  errorDevice: ") + hsErrorDevice +
                            HobString("  errorCode: ") + hsErrorCode +
                            HobString("  responseStyle: ") + hsResponseStyle);


                if (errorDevice != ErrUnknown)
                {
                    if (addDialog)
                        responseMessageManager->addMessage(errorDevice, errorCode, responseStyle);
                    else
                        responseMessageManager->removeMessage(errorDevice, errorCode, responseStyle);
                }

            }
            break;
        }

        case SnPrimaryToMonitoringSessionMsg1:
        {
            GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (msgData && !msgData->getData().empty())
            {

                HobString syncData = msgData->getData();


                if (Debug::isTracing("Session.monitormode"))
                    Debug::trace("Session.monitormode", getName() + HobString("case SnPrimaryToMonitoringSessionMsg1  data: ") + syncData);

                //zzzz  send(syncData, Messenger::DEST_UI);
            }
            break;
        }

        case SnPrimaryToMonitoringSessionMsg2:
        {
            GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (msgData && !msgData->getData().empty())
            {
                // Split the data into tokens (device, event, data).
                HobString syncData = msgData->getData();
                HobTokenizerCSV tok(&syncData, BrowserFormatSeparator, 2);
                HobString token;

                UINT8 dev = 0, event = 0;
                if (tok.next(&token) != -1)
                    dev = (Integer::valueOf(token));
                if (tok.next(&token) != -1)
                    event = (Integer::valueOf(token));
                if (tok.next(&token) == -1)
                    token = "";

                if (Debug::isTracing("Session.monitormode"))
                    Debug::trace("Session.monitormode", getName() +
                            HobString("case SnPrimaryToMonitoringSessionMsg2 - dev: ") + HobString(Integer::toString(dev)) +
                            HobString("  event: ") + HobString(Integer::toString(event)) +
                            HobString("  data: ") + token);

                SessionMessage sendMonitorMsg((EvolDeviceType)dev, event);
                sendMonitorMsg.setFormatType(BrowserFormat);
                GenericHfcObject gho(token);
                sendMonitorMsg.setHfcObject(&gho);
                send(&sendMonitorMsg, Messenger::DEST_UI);
            }
            break;
        }


        case SnSyncDOMRequest:  // SnSyncDOMRequest
        {
            if (isPrimarySession())
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - handling SnSyncDOMRequest Event"));
                SessionMessage syncDomRequest(SessionDev, SnSyncDOMRequest);
                syncDomRequest.setFormatType(BrowserFormat);
                send(&syncDomRequest, Messenger::DEST_UI);
            }
            break;
        }


        case SnSyncDOM:  // SnSyncDOM
        {
            /* This message contains DOM data from the Primary console that needs to be sent
             * to a remote console that is entering Monitor Mode.
             * Once all DOM data frames are sync'ed, the Primary Browser will send back one
             * more SnSyncDOM message without any data; in which the Primary console will
             * tell the remote console which web page frames need to be reloaded.
             */
            if (isPrimarySession())
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - handling SnSyncDOM Event"));
                GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
                if (gho && !gho->getData().empty())
                {
                    sendToMonitoringSessions(msg->getOriginalBuff(), Messenger::DEST_UI);
                    //sendToMonitoringSessions(gho->getData(), Messenger::DEST_UI);
                }
                else
                {
                    /* Done sending all DOM sync messages.  Tell the monitoring remote browsers
                     * to reload current web page frames.
                     *
                     * We only reload the frames that can change: function frame, and common frame.
                     * The other frames in the UI don't change, even when changing to Supervisor mode,
                     * so they never need to be reloaded; the remote browser already has them loaded.
                     */

                    SessionMessage remoteMsg(SessionDev, SnWriteCurrentWebPage);
                    remoteMsg.setFormatType(BrowserFormat);

                    SessionMessage *remoteMenuMsg;
                    remoteMenuMsg = new SessionMessage(SessionDev, SnBrowserMenuChanging);
                    remoteMenuMsg->setFormatType(BrowserFormat);

                    if (config->isSelfServiceScale())
                    {
                        {
                            HobString remotePage(getCurrentWebPage());
                            std::string::size_type pos = remotePage.find("consoletype=");
                            if (pos != std::string::npos)
                                remotePage.replace(pos+12, 1, "2");

                            GenericHfcObject gho(remotePage);
                            remoteMsg.setHfcObject(&gho);
                            sendToMonitoringSessions(&remoteMsg, Messenger::DEST_UI);
                        }

                        {
                            HobString remotePage(getCurrentWebPageMenu());
                            std::string::size_type pos = remotePage.find("consoletype=");
                            if (pos != std::string::npos)
                                remotePage.replace(pos+12, 1, "2");

                            GenericHfcObject gho(remotePage);
                            remoteMsg.setHfcObject(&gho);
                            sendToMonitoringSessions(&remoteMsg, Messenger::DEST_UI);
                        }

                        PAL_absclass::PAL_sleep(180);

                        Session *primarySession = SessionManager::getInstance()->getPrimaryConsoleSession();
                        if (primarySession->isInOperateMode())
                        {
                            {
                                GenericHfcObject gho((primarySession->getCurrentWebPageWeightFrame().size()) ? primarySession->getCurrentWebPageWeightFrame() : "4,");
                                remoteMsg.setHfcObject(&gho);
                                sendToMonitoringSessions(&remoteMsg, Messenger::DEST_UI);
                            }

                            {
                                GenericHfcObject gho((primarySession->getCurrentWebPageTarget().size()) ? primarySession->getCurrentWebPageTarget() : "5,#startIt");
                                remoteMsg.setHfcObject(&gho);
                                sendToMonitoringSessions(&remoteMsg, Messenger::DEST_UI);
                            }
                        }
                        PAL_absclass::PAL_sleep(30);

                    }
                    else  // not self service scale
                    {
                        // Send the function frame.
                        if (currentWebPages[1].size())
                        {
                            HobString remotePage = currentWebPages[1];
                            std::string::size_type pos = remotePage.find("consoletype=");
                            if (pos != std::string::npos)
                                remotePage.replace(pos+12, 1, "2");

                            GenericHfcObject gho(remotePage);
                            remoteMsg.setHfcObject(&gho);
                            sendToMonitoringSessions(&remoteMsg, Messenger::DEST_UI);
                        }

                        // Send the common frame.
                        if (currentWebPages[16].size())
                        {
                            HobString remotePage = currentWebPages[16];
                            std::string::size_type pos = remotePage.find("consoletype=");
                            if (pos != std::string::npos)
                                remotePage.replace(pos+12, 1, "2");

                            GenericHfcObject gho(remotePage);
                            remoteMsg.setHfcObject(&gho);
                            sendToMonitoringSessions(&remoteMsg, Messenger::DEST_UI);
                        }

                        if (currentWebPages[2].size())
                        {
                            HobString remotePage(getCurrentWebPageMenu());
                            /*std::string::size_type pos = remotePage.find("consoletype=");
                              if (pos != std::string::npos)
                              remotePage.replace(pos+12, 1, "2");   */
                            Debug::trace("Session", getName() + HobString("SnSyncDOM Event - remotePage: ") + remotePage);

                            GenericHfcObject gho(remotePage);
                            remoteMenuMsg->setHfcObject(&gho);
                            Debug::trace("Session", getName() + HobString("SnSyncDOM Event - remoteMenuMsg: ") + remoteMenuMsg->toString());
                            sendToMonitoringSessions(remoteMenuMsg, Messenger::DEST_UI);
                        }


                        // Now tell the browser to go to the current active frame that the Primary console is on.
                        SessionMessage setActivePageMsg(SessionDev, SnSelectActiveWebPageFrame);
                        setActivePageMsg.setFormatType(BrowserFormat);
                        GenericHfcObject gho(Integer::toString(currentWebPageFrame));
                        setActivePageMsg.setHfcObject(&gho);
                        sendToMonitoringSessions(&setActivePageMsg, Messenger::DEST_UI);
                    }

                    // Tell all monitoring sessions to remove the "please wait ... synchronizing with Primary" message from their screen.
                    sendDialogToMonitoringSessions(false, ErrSessionDev, SessionDevEnteringMonitorMode, HfcResponse::RESPONSESTYLE_OK);
                }
            }
            break;
        }


        case SnShutdownNonPrimary:
        case SnTerminate:
            // The primary console session cannot be told to terminate via this message.
            if (tracingSession)
                Debug::trace("ProductManager.remotePrint", getName() + " START handleSessionMessage :: case Terminate");
            if ( !isPrimarySession() || msg->getEventType() == SnTerminate )
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - handling TERMINATE Event"));

                GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
                bool notifyUI = false;
                if (gho)
                {
                    notifyUI |= Bool::valueOf(gho->getData());
 
                    if ( gho->getData().length() )
                        notifyUI |= (gho->getData()[0] == 0x01);

                    if (tracingSession)
                        Debug::trace("Session", getName() + HobString(" - handling TERMINATE Event - notify UI:") + gho->getData() +
                                HobString("  ") + HobString(Bool::toString(notifyUI)));
                }
                // The UI told us to terminate (currently the only source of this message),
                // So we don't need to tell it that we are terminating (pass in false to terminate method).
                terminate(notifyUI, msg->getEventType());
                if (tracingSession)
				    Debug::trace("ProductManager.remotePrint", "FINISH case Terminate session terminated");
            }
            break;


        case SnReadDateTime:
        {
            /* todo todo todo
             * Should this message just default to sending the data to the browser,
             * Wouldn't it be more flexible to write it out based on the format that the message
             * that I am currently processing is (from where did this message come, if it came from the
             * RTC, write the response in RTC format.?!?!?!?!?!?!?!?!
             */
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling READ DATE-TIME Event"));
            if (currentDateTime)
            {
                delete currentDateTime;
                currentDateTime = (HobDateTime *)NULL;
            }
            sendDateTimeMessage();
            if (currentDateTime)
            {
                delete currentDateTime;
                currentDateTime = (HobDateTime *)NULL;
            }
            break;
        }


        case SnWriteDateTime:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling WRITE DATE-TIME Event"));

            HobDateTime *hdt = dynamic_cast<HobDateTime *> (msg->getHfcObject());
            if (hdt)
                hdt->setTime();

            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - done handling WRITE DATE-TIME Event"));
            break;
        }


        case SnContinuousDateTime:
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling CONTINUOUS DATE-TIME Event"));
            constantlyUpdatingDateTime = true;
            break;


        case SnStopDateTime:
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling STOP DATE-TIME Event"));
            constantlyUpdatingDateTime = false;
            if (currentDateTime)
            {
                delete currentDateTime;
                currentDateTime = (HobDateTime *)NULL;
            }
            break;


        case SnReadWeighModeTransactionType:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling READ WEIGHMODE and TRANSACTIONTYPE Event"));

            // Mode should be: OpModeAutoWeigh, OpModeSemiWeigh, OpModeManualWeigh, OpModeAutoManualApply, or OpModeWrapOnly
            SessionMessage wmttMsg(SessionDev, SnWriteWeighModeTransactionType);
            wmttMsg.setFormatType(BrowserFormat);

            WeighModeTotalType wmtt(currentMode, transactionTypeManager->getOperatorModeTransactionType());
            wmttMsg.setHfcObject(&wmtt);

            send(&wmttMsg, Messenger::DEST_UI);
            break;
        }

        case SnWriteWeighModeTransactionType:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling WRITE WEIGHMODE and TRANSACTIONTYPE Event"));

            // Change to the current mode, but only if this is the primary console session.
            // Remote terminals shouldn't be able to change the mode between Manual to Semi-automatic, to automatic.
            if (isPrimarySession() && msg->getHfcObject())
            {
                WeighModeTotalType *wmtt = dynamic_cast<WeighModeTotalType *> (msg->getHfcObject());

                if (wmtt->isReceived(1))
                {
                    // Mode should be (from Event.h, OpModeEvent): OpModeAutoWeigh, OpModeSemiWeigh, OpModeManualWeigh, OpModeAutoManualApply, or OpModeWrapOnly
                    SessionMessage mode(OperationMode, wmtt->getRunWeighMode());
                    handleModeMessage(&mode);
                }
            }
            break;
        }


        case SnReadDirectoryList:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling READ DIRECTORY LIST Event"));

            DirectoryList *input = dynamic_cast<DirectoryList *> (msg->getHfcObject());
            if (!input)
                return;

            HobString location = input->getMyList().front();
            DirectoryList dl(location);

            SessionMessage dirListMsg(SessionDev, SnWriteDirectoryList);
            dirListMsg.setFormatType(BrowserFormat);
            dirListMsg.setHfcObject(&dl);
            send(&dirListMsg, Messenger::DEST_UI);
            break;
        }


        case SnReadDevicesList:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling READ DEVICES LIST Event"));

            DevicesList devs;

            SessionMessage devListMsg(SessionDev, SnWriteDevicesList);
            devListMsg.setFormatType(BrowserFormat);
            devListMsg.setHfcObject(&devs);
            send(&devListMsg, Messenger::DEST_UI);
            break;
        }

        case SnReadScaleStatus:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling WRITE SCALE STATUS Event"));

            SessionMessage statusMsg(SessionDev, SnWriteScaleStatus);
            statusMsg.setFormatType(msg->getFormatType());
            ScaleStatus status;
            statusMsg.setHfcObject(&status);
            send(&statusMsg, Messenger::DEST_UI);
            break;
        }

        case SnContinuousStatistics:
        {
            switch ( msg->getFormatType() )
            {
                case BrowserFormat:
                {
                    Debug::trace("Session.statistics", 20,
                            "My browser wants statistics. Registering for statistics updates");

                    // Listen for SnWriteStatistics because your UI wants statistics
                    registeredForContinuousStatistics_ = true;
                    registerEvent(SessionDev, SnWriteStatistics);

                    // If this is not the primary session, relay the message to it
                    if ( !isPrimarySession() )
                    {
                        Debug::trace("Session.statistics", 20,
                                "Since I'm not a Primary session, I'll transfer the request to the Primary");

                        msg->setFormatType(InternalFormat);
                        send(msg, Messenger::DEST_RTC);

                        break;
                    }

                    Debug::trace("Session.statistics", 20,
                            "Since I'm the Primary session, I'll proceed to process my browser's request");

                    // ... Otherwise process the message
                }
                case InternalFormat:
                {
                    // Only the primary session will execute this code ...
                    Debug::trace("Session", getName() + HobString(" - handling CONTINUOUS STATISTICS Event"));

                    Debug::trace("Session.statistics", 20, "There are currently " +
                            inttostring(statisticsListeners_) + " listeners of statistics updates");

                    // Create a timer to be notified when to display the next set of statistics
                    TimerManager *timerManager = TimerManager::getInstance();
                    if (timerManager)
                    {
                        // only add a new timer if one doesn't already exist.
                        if (!timerManager->getTimer(getId(), timedFuncContinuousStatistics))
                        {
                            timerManager->addTimer(getId(), config->toIntCfgI(CfgInternal::CONTINUOUSSTATISTICSDELAY), Timer::RepeatInMinutes, timedFuncContinuousStatistics);

                            Debug::trace("Session.statistics", 20, "Created the timer to report statistics updates");
                        }
                    }

                    ++statisticsListeners_;

                    Debug::trace("Session.statistics", 20, "Added one more listener and I now have " +
                            inttostring(statisticsListeners_) + " listeners of statistics updates");

                    // If the timer did not exist, we send the first request
                    // here so we do not need to wait for the timer to time
                    // out. We also send the request if the timer existed
                    // since we don't know how long it will take to time out.
                    Debug::trace("Session.statistics", 20,
                            "Sending request to read statistics to the primary browser");

                    SessionMessage statsMsg(SessionDev, SnReadStatistics);
                    statsMsg.setFormatType(BrowserFormat);
                    send(&statsMsg, Messenger::DEST_UI);

                    break;
                }
                default:
                {
                    break;
                }
            }

            break;
        }

        case SnReadStatistics:
        {
            // A remote session, specifically comm, may receive a request to read statistics ...
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling READ STATISTICS Event"));

            SessionMessage statsMsg(SessionDev, SnWriteStatistics);
            statsMsg.setFormatType(msg->getFormatType());
            //Debug::trace("Session", getName() + HobString(" - before creating Statistics object"));
            Statistics stats(this);
            //Debug::trace("Session", getName() + HobString(" - after creating Statistics object"));
            statsMsg.setHfcObject(&stats);
            //Debug::trace("Session", getName() + HobString(" - before sending Statistics object"));
            send(&statsMsg, Messenger::DEST_UI);
            //Debug::trace("Session", getName() + HobString(" - after sending Statistics object"));

            break;
        }

        case SnWriteStatistics:
        {
            switch ( msg->getFormatType() )
            {
                case BrowserFormat:
                {
                    // The primary console receives this message containing the EVM's
                    // memory usage statistics because it had requested them from the
                    // primary browser. The primary session sends it to any session
                    // that may be interested in it (perhaps this same session).
                    if (tracingSession)
                        Debug::trace("Session", getName() + HobString(" - handling WRITE STATISTICS Event"));

                    Debug::trace("Session.statistics", 20,
                            "Recieved statistics from the primary browser. Relaying to any session listening for them");
                    msg->setFormatType(InternalFormat);
                    send(msg, Messenger::DEST_RTC);

                    break;
                    /*
                       HfcEVMMemoryStatistics *evmStats = dynamic_cast<HfcEVMMemoryStatistics *> (msg->getHfcObject());
                       if ( evmStats )
                       evmStats->show();

                       HfcWeigherStatus testWeight;
                       testWeight.setDeviceType(Weigher);
                       testWeight.setEventType(WgStatus);
                       testWeight.setRawCounts((UINT32)15432);
                       SessionMessage counts(Weigher, WgStatus);
                       counts.setFormatType(InternalFormat);
                       counts.setHfcObject(&testWeight);
                       handleWeigherMessage(&counts);

                       break;
                       */
                }
                case InternalFormat:
                {
                    Debug::trace("Session.statistics", 20, "Recieved EVM statistics from the primary session.");

                    // The session that receives this message needs to send the
                    // statistics to its browser by adding the information received here.
                    HfcEVMMemoryStatistics *evmStats = dynamic_cast<HfcEVMMemoryStatistics *> (msg->getHfcObject());
                    if ( evmStats )
                    {
                        Debug::trace("Session.statistics", 20,
                                "Creating statistics record and filling EVM stats info");

                        // Create scale statistics object and add EVM statistics to it
                        Statistics stats(this);
                        stats.setUsedEVMMemory(evmStats->getUsedMemory());
                        stats.setFreeEVMMemory(evmStats->getFreeMemory());


                        // Now send it to the browser
                        Debug::trace("Session.statistics", 20,
                                "Sending full statistics information to my browser");

                        SessionMessage statsMsg(SessionDev, SnWriteStatistics);
                        statsMsg.setFormatType(BrowserFormat);
                        statsMsg.setHfcObject(&stats);
                        send(&statsMsg, Messenger::DEST_UI);
                    }

                    break;
                }
                default:
                {
                    break;
                }
            }

            break;
        }

        case SnStopStatistics:
        {
            switch ( msg->getFormatType() )
            {
                case BrowserFormat:
                {
                    Debug::trace("Session.statistics", 20,
                            "My browser doesn't want statistics. Deregistering for statistics updates");

                    if (!registeredForContinuousStatistics_)
                        break;

                    // Stop listening for SnWriteStatistics because your UI does not want them anymore
                    deregisterEvent(SessionDev, SnWriteStatistics);
                    registeredForContinuousStatistics_ = false;

                    // If this is not the primary session, relay the message to it
                    if ( !isPrimarySession() )
                    {
                        Debug::trace("Session.statistics", 20,
                                "Since I'm not a Primary session, I'll transfer the request to the Primary");

                        msg->setFormatType(InternalFormat);
                        send(msg, Messenger::DEST_RTC);

                        break;
                    }

                    Debug::trace("Session.statistics", 20,
                            "Since I'm the Primary session, I'll proceed to process my browser's request");

                    // ... Otherwise process the message
                }
                case InternalFormat:
                {
                    // Only the primary session will execute this code ...
                    if (tracingSession)
                        Debug::trace("Session", getName() + HobString(" - handling STOP STATISTICS Event"));

                    Debug::trace("Session.statistics", 20, "There are currently " +
                            inttostring(statisticsListeners_) + " listeners of statistics updates");

                    if ( statisticsListeners_ > 0 )
                    {
                        --statisticsListeners_;
                        if ( statisticsListeners_ == 0 )
                        {
                            // Delete the timer associated with the continuous statistics.
                            TimerManager *timerManager = TimerManager::getInstance();
                            if (timerManager)
                            {
                                Timer *statsTimer = timerManager->getTimer(getId(), timedFuncContinuousStatistics);
                                if (statsTimer)
                                {
                                    timerManager->removeTimer(statsTimer->getTimerId());

                                    Debug::trace("Session.statistics", 20,
                                            "Removed the timer to report statistics updates");
                                }
                            }
                        }

                        Debug::trace("Session.statistics", 20, "Removed one listener and I now have " +
                                inttostring(statisticsListeners_) + " listeners of statistics updates");
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }

            break;
        }

        case SnKeyActivation:
        {
		    bool tracingKeys = Debug::isTracing("Session.keys");
            if (tracingKeys)
            {
                Debug::trace("Session.keys", getName() + HobString(" - handling SNKEYACTIVATION Event"));
                Debug::trace("Session.keys", getName() + HobString(" -- 1 -- currentModeOp: ") + HobString(Integer::toString(currentModeOp)));
            }

            HfcAccessKeyActivation *haka = dynamic_cast<HfcAccessKeyActivation *> (msg->getHfcObject());
            if ((haka) && (restoreVersionIndependentData == false))
            {
				if (!isPrimarySession())
				{
					// For Fastrack, the key activations are coming in through the comm mechanism (Scott is sending them in essentially
					// as a part of Scale Management), so they are coming in to some comm Session.  That comm Session needs to dispatch
					// that message to the primary console, so it can be processed and relayed to the primary scale UI.

	                if (tracingKeys)
		                Debug::trace("Session.keys", getName() + HobString(" - I'm not the primary console, so I'm dispatching this message to it for handling"));
                    dispatchEvent(haka);
					break;
				}

                if (tracingKeys)
                    Debug::trace("Session.keys", getName() + HobString(" - keys before activation: ") + HobString(Hex::toString(accessKeys.getKeys())) +
                            HobString(" - keys passed in to be activated: ") + HobString(Hex::toString(haka->getKeys())));

				// Stating with Fastrack, we no longer OR the ones received with the AccessKeys we're tracking.
				// Now we get all flags, so we just replace the one we're tracking with the one received.
                //accessKeys |= haka->getKeys();
                accessKeys = haka->getKeys();

                if (tracingKeys)
                {
                    Debug::trace("Session.keys", getName() + HobString(" - keys after activation: ") + HobString(Hex::toString(accessKeys.getKeys())));
                    Debug::trace("Session.keys", getName() + HobString(" - activation - currentMode: ") + HobString(Integer::toString(currentMode)));
                    Debug::trace("Session.keys", getName() + HobString(" - activation - is in operate mode? ") + HobString(Bool::toString(isInOperateMode())));
                }

				if (currentModeOp == OP_WAITINGFORSHUTDOWNUSBREMOVAL)
				{
					if (!accessKeys.isAnyAccessKeyPresent())
					{
		                if (tracingKeys)
							Debug::trace("Session.keys", getName() + HobString(" - Session was waiting for USB Key removal to Shutdown, and now they're gone."));
						currentModeOp = OP_UNKNOWN;
						responseMessageManager->removeMessage(ErrSessionDev, SessionDevRemoveUSBDrive, HfcResponse::RESPONSESTYLE_NOINPUT);
                        PAL_absclass::PAL_sleep(100);

						SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
	                    shutdownMsg.setFormatType(BrowserFormat);
		                GenericHfcObject gho(inttostring(ShutdownRequest_Reboot));
			            shutdownMsg.setHfcObject(&gho);
				        handleSessionMessage(&shutdownMsg);
					}
				}

				else if (currentModeOp == OP_WAITINGFORUSBKEYREMOVAL)
				{
					if (!accessKeys.isAnyAccessKeyPresent())
					{
		                if (tracingKeys)
							Debug::trace("Session.keys", getName() + HobString(" - Session was waiting for USB Key removal, and now they're gone."));
						currentModeOp = OP_UNKNOWN;
						if (responseMessageManager)
							responseMessageManager->removeMessage(ErrVersionManagerDev, VersionManagerUsbDriveStillPresent, HfcResponse::RESPONSESTYLE_NOINPUT);
					}
				}

				else if (currentModeOp == OP_ENTERINGSUPERVISORMODE)
                    currentModeOp = OP_UNKNOWN;

                // if waiting for service key, and they put one in, then go into service mode
                else if ( currentMode == OpModeServiceLogin && (accessKeys.isServiceKeyPresent() || isVirtualServiceJumperSet()) )
                {
                    if (tracingKeys)
                        Debug::trace("Session.keys", getName() + HobString(" -----  OperationMode, OpModeServiceLogin"));
                    SessionMessage modeMsg(OperationMode, OpModeServiceLogin);
                    handleModeMessage(&modeMsg);
                }

                else if (accessKeys.isAnyAccessKeyPresent())
                {
                    if ((config->isSelfServiceScale() == true) &&
                            (isInOperateMode() == true) && (currentModeOp != OP_ENTERINGSUPERVISORMODE))
                    {
                        responseMessageManager->removeAllMessages();

                        //if (tracingSession)
                        //    Debug::trace("Session", getName() + HobString(" -----  OperationMode, OpModeSupervisor"));
                        //SessionMessage super(OperationMode, OpModeSupervisor);
                        //super.setFormatType(BrowserFormat);
                        //send(&super, Messenger::DEST_UI);

                        //currentModeOp = OP_ENTERINGSUPERVISORMODE;
                    }

                    if ( (accessKeys.isServiceKeyPresent() || isVirtualServiceJumperSet()) &&
                            currentMode == OpModeSupervisor &&
							OperatorManager::isSupervisorLoginRequired(this) &&
                            operatorManager && !operatorManager->getLoggedInOperator())
                    {
                        HobOperator op;
                        op.setOpNum(config->getServiceLoginId());
                        op.setCurrentMode(HobOperator::CURRENTMODE_SERVICE);

                        op.setAccessLevel(9);               //      To log service person directly into Supervisor mode
                        if (tracingKeys)
                            Debug::trace("Session.keys", getName() + HobString(" -----  OperatorDev, OpLoginOpModeSupervisor"));
                        SessionMessage loginMsg(OperatorDev, OpLogin);
                        loginMsg.setFormatType(InternalFormat);
                        loginMsg.setHfcObject(&op);

                        handleMessage(&loginMsg);
                    }

                    // Don't need to keep this one set in this Session's access keys object,
                    // except for when we're going into Supervisor mode, in which case we will clear this
                    // flag when we actually go into Supervisor mode.
                    if (accessKeys.isSupervisorFunctionKeyPressed())
                        accessKeys.setSupervisorFunctionKeyPressed(false);
                }

                if (tracingKeys)
                {
                    Debug::trace("Session.keys", getName() + HobString(" - keys after activation: ") + HobString(Hex::toString(accessKeys.getKeys())));
                    Debug::trace("Session.keys", getName() + HobString(" - The currentMode is now: ") + HobString(Integer::toString(currentMode)));
                }
            }

			// Send the current state of the keys to the Browser

			AccessKeys keysToBrowser = accessKeys;
			if (isVirtualServiceJumperSet())
				keysToBrowser.setServiceKeyPresent(true);
			if (isVirtualQAServiceJumperSet())
				keysToBrowser.setQAServiceKeyPresent(true);

			SessionMessage keysMsg(SessionDev, SnKeyActivationStates);
			keysMsg.setFormatType(BrowserFormat);
			keysMsg.setHfcObject(&keysToBrowser);
			send(&keysMsg, Messenger::DEST_UI);
            break;
        }

/*
        // Starting with Fastrack, we will no longer receive this.
        case SnKeyDeactivation:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SNKEYDEACTIVATION Event"));
            HfcAccessKeyDeactivation *hakd = dynamic_cast<HfcAccessKeyDeactivation *> (msg->getHfcObject());
            if (hakd)
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString("keys before deactivation: ") + HobString(Hex::toString(accessKeys.getKeys())) +
                            HobString("  keys passed in to be deactivated: ") + HobString(Hex::toString(hakd->getKeys())));
                accessKeys &= (~(hakd->getKeys()));

                if (tracingSession)
                    Debug::trace("Session", getName() + HobString("keys after deactivation: ") + HobString(Hex::toString(accessKeys.getKeys())));

				if (currentModeOp == OP_WAITINGFORSHUTDOWNUSBREMOVAL)
				{
					if (!accessKeys.isAnyAccessKeyPresent())
					{
						currentModeOp = OP_UNKNOWN;
						responseMessageManager->removeMessage(ErrSessionDev, SessionDevRemoveUSBDrive, HfcResponse::RESPONSESTYLE_NOINPUT);
                        PAL_absclass::PAL_sleep(100);

						SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
	                    shutdownMsg.setFormatType(BrowserFormat);
		                GenericHfcObject gho(inttostring(ShutdownRequest_Reboot));
			            shutdownMsg.setHfcObject(&gho);
				        handleSessionMessage(&shutdownMsg);
					}
				}
				else if (currentModeOp == OP_ENTERINGSUPERVISORMODE)
                    currentModeOp = OP_UNKNOWN;

                // Send the current state of the keys to the Browser
				AccessKeys keysToBrowser = accessKeys;
				if (isVirtualServiceJumperSet())
					keysToBrowser.setServiceKeyPresent(true);
				if (isVirtualQAServiceJumperSet())
					keysToBrowser.setQAServiceKeyPresent(true);

                SessionMessage keysMsg(SessionDev, SnKeyActivationStates);
                keysMsg.setFormatType(BrowserFormat);
                keysMsg.setHfcObject(&keysToBrowser);
                send(&keysMsg, Messenger::DEST_UI);
            }
            break;
        }
*/

        case SnUsbKbdAttachChange:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnUsbKbdAttachChange Event"));

            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (gho)
            {
                const char * tmp = gho->getData().c_str();
                int* attachState = (int*) tmp;
                HobString hsState(Integer::toString(*attachState));

                if (tracingSession)
                    Debug::trace("Session.UsbKbdStatus", HobString(" Usb Keyboard Status change = ") + hsState);

                SessionMessage keysMsg(SessionDev, SnUsbKbdAttachChange);
                keysMsg.setFormatType(BrowserFormat);
                GenericHfcObject newgho(hsState);
                keysMsg.setHfcObject(&newgho);
                send(&keysMsg, Messenger::DEST_UI);

            }
            break;
        }


        case SnBrowserMenuChanging:
        {
            // I received a message that the console is changing it's menu.
            // If I'm the primary console, send this message to all monitoring sessions.
            if (tracingSession)
                Debug::trace("Session", getName() + HobString("SnBrowserMenuChanging Event - msg: ") + msg->toString());

            string msgStr = msg->toString();
            currentWebPages[2] = msgStr.at(msgStr.find_last_of(",") + 1);

            if (isPrimarySession())
            {
                sendToMonitoringSessions(msg, Messenger::DEST_UI);
            }

            break;
        }


        case SnSimulateTouch:
        {
            if (isPrimarySession())
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - handling SnSimulateTouch Event"));

                // The data associated with this message is: x-coordinate, y-coordinate, quiet-mode(if TRUE, don't beep).
                // These parameters are optional, and will have the following defaults.
                UINT32 x = 0, y = 0;
                bool quiet = true;

                GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
                if (msgData && !msgData->getData().empty())
                {
                    HobString coordinatesData = msgData->getData();
                    HobTokenizerCSV tok(&coordinatesData, BrowserFormatSeparator);
                    HobString token;

                    if (tok.next(&token) != -1)
                        if (token.size())
                            x = (UINT32)(ULong::valueOf(token));
                    if (tok.next(&token) != -1)
                        if (token.size())
                            y = (UINT32)(ULong::valueOf(token));
                    if (tok.next(&token) != -1)
                        if (token.size())
                            quiet = Bool::valueOf(token);

                    if (tracingSession)
                        Debug::trace("Session", getName() +
                                HobString(" x-coordinate: ") + HobString(ULong::toString(x)) +
                                HobString("y-coordinate: ") + HobString(ULong::toString(y)) +
                                HobString("quiet: ") + HobString(Bool::toString(quiet)));
                }
                else if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - no parameters received, using default of 0, 0, true"));

                //lcg rev 0.1 simulateTouch(x, y, quiet);
            }
            else if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - received SessionDev,SnSimulateTouch message, but ignoring because I'm not the primary console"));

            break;
        }

        case SnReadCurrentWebPage:
        {
            // A remote session is requesting the current web page strings from the primary console session.

            // todo - IS the following sufficient to get a remote browser into monitor mode?
            //        Specifically, will the DOM contain all the data it needs to display the page
            //        that the remote browser will be told to display, via this message???????????

            if (!isPrimarySession())
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - handling SnReadCurrentWebPage Event"));

                // The page strings are sent in the following order: DOM, page, menu (3,1,2,4,5)

                Session *primarySession = SessionManager::getInstance()->getPrimaryConsoleSession();
                if (primarySession)
                {
                    SessionMessage remoteMsg(SessionDev, SnWriteCurrentWebPage);
                    remoteMsg.setFormatType(BrowserFormat);

                    SessionMessage *remoteMenuMsg;
                    remoteMenuMsg = new SessionMessage(SessionDev, SnBrowserMenuChanging);
                    remoteMenuMsg->setFormatType(BrowserFormat);

                    if (config->isSelfServiceScale())
                    {
                        // The variables containing the web page strings are stored in the primary console session
                        // with the data indicator (page=1, DOM=3, menu=2), so don't need to tack it onto the data to send.

                        {
                            HobString remotePage(primarySession->getCurrentWebPageDOM());
                            std::string::size_type pos = remotePage.find("consoletype=");
                            if (pos != std::string::npos)
                                remotePage.replace(pos+12, 1, "2");

                            GenericHfcObject gho(remotePage);
                            remoteMsg.setHfcObject(&gho);
                            send(&remoteMsg, Messenger::DEST_UI);
                        }

                        {
                            HobString remotePage(primarySession->getCurrentWebPage());
                            std::string::size_type pos = remotePage.find("consoletype=");
                            if (pos != std::string::npos)
                                remotePage.replace(pos+12, 1, "2");

                            GenericHfcObject gho(remotePage);
                            remoteMsg.setHfcObject(&gho);
                            send(&remoteMsg, Messenger::DEST_UI);
                        }

                        {
                            HobString remotePage(primarySession->getCurrentWebPageMenu());
                            std::string::size_type pos = remotePage.find("consoletype=");
                            if (pos != std::string::npos)
                                remotePage.replace(pos+12, 1, "2");

                            GenericHfcObject gho(remotePage);
                            remoteMsg.setHfcObject(&gho);
                            send(&remoteMsg, Messenger::DEST_UI);
                        }

                        PAL_absclass::PAL_sleep(120);
                        if (primarySession->isInOperateMode())
                        {
                            {
                                GenericHfcObject gho(primarySession->getCurrentWebPageWeightFrame());
                                remoteMsg.setHfcObject(&gho);
                                send(&remoteMsg, Messenger::DEST_UI);
                            }

                            {
                                GenericHfcObject gho(primarySession->getCurrentWebPageTarget());
                                remoteMsg.setHfcObject(&gho);
                                send(&remoteMsg, Messenger::DEST_UI);
                            }
                        }
                        PAL_absclass::PAL_sleep(30);
                        {
                            Debug::trace("Session.monitor", getName() + HobString(" - Sending SnSyncDOMRequest message to primary console Session ..."));
                            // This message is going to the primary console Session.
                            SessionMessage syncDomRequest(SessionDev, SnSyncDOMRequest);
                            syncDomRequest.setFormatType(InternalFormat);
                            send(&syncDomRequest, Messenger::DEST_RTC);
                        }
                    }
                    else  // not self service scale
                    {
                        // Send the function frame.
                        if (currentWebPages[1].size())
                        {
                            HobString remotePage = currentWebPages[1];
                            std::string::size_type pos = remotePage.find("consoletype=");
                            if (pos != std::string::npos)
                                remotePage.replace(pos+12, 1, "2");

                            GenericHfcObject gho(remotePage);
                            remoteMsg.setHfcObject(&gho);
                            send(&remoteMsg, Messenger::DEST_UI);
                        }

                        // Send the common frame.
                        if (currentWebPages[16].size())
                        {
                            HobString remotePage = currentWebPages[16];
                            std::string::size_type pos = remotePage.find("consoletype=");
                            if (pos != std::string::npos)
                                remotePage.replace(pos+12, 1, "2");

                            GenericHfcObject gho(remotePage);
                            remoteMsg.setHfcObject(&gho);
                            send(&remoteMsg, Messenger::DEST_UI);
                        }

                        if (currentWebPages[2].size())
                        {
                            HobString remotePage(primarySession->getCurrentWebPageMenu());
                            /*std::string::size_type pos = remotePage.find("consoletype=");
                              if (pos != std::string::npos)
                              remotePage.replace(pos+12, 1, "2");*/
                            Debug::trace("Session", getName() + HobString("SnReadCurrentWebPage Event - remotePage: ") + remotePage);

                            GenericHfcObject gho(remotePage);
                            remoteMenuMsg->setHfcObject(&gho);
                            Debug::trace("Session", getName() + HobString("SnReadCurrentWebPage Event - remoteMenuMsg: ") + remoteMenuMsg->toString());
                            send(remoteMenuMsg, Messenger::DEST_UI);
                        }


                        // Now tell the browser to go to the current active frame that the Primary console is on.
                        SessionMessage setActivePageMsg(SessionDev, SnSelectActiveWebPageFrame);
                        remoteMsg.setFormatType(BrowserFormat);
                        GenericHfcObject gho(Integer::toString(currentWebPageFrame));
                        setActivePageMsg.setHfcObject(&gho);
                        sendToMonitoringSessions(&setActivePageMsg, Messenger::DEST_UI);
                    }
                }
            }
            break;
        }

        case SnSelectActiveWebPageFrame:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling 70 - SnSelectActiveWebPageFrame Event"));

            GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (msgData && !msgData->getData().empty())
            {
                short dataIndicator = Integer::valueOf(msgData->getData());
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - currentWebPageFrame changing from ") + HobString(Integer::toString(currentWebPageFrame)) + HobString("  to ") + HobString(Integer::toString(dataIndicator)));
                currentWebPageFrame = dataIndicator;

                    if (config->isSelfServiceScale())
                    {
                        if(isInOperateMode())
                        {
                            if(currentWebPageFrame == 100 || currentWebPageFrame == 0)                            
                            {
                                LanguageSelection cls = getCurrentLanguageSelection();

                                if(cls == PrimaryLanguage)
                                    setOnSelfServiceStartupScreen(true);    // returned to start page in pri lang, clear timers
                                else
                                    onStartUpScreenInSecondLanguage = true;    // set a flag to check next pass thru the timer timeout test
                            }
                            else
                                setOnSelfServiceStartupScreen(false);
                            
                            // else - could add other frame conditions here

                        }
                        //else
                        //{
                        //    // no timers in super and service
                        //    // may need to remove or stop timers here
                        //}
                    }

                // Need to notify monitoring sessions that the Primary console is changing to a different frame.
                if (isPrimarySession())
                    sendToMonitoringSessions(msg, Messenger::DEST_UI);
            }
            break;
        }

        case SnWriteCurrentWebPage:
        {
            if (isPrimarySession())
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - handling SnWriteCurrentWebPage Event"));

                GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
                if (msgData && !msgData->getData().empty())
                {
                    // Split the data into at most 2 tokens (the data indicator, and the page string).
                    // If there are any commas after the first one, then it is considered part of the page string.
                    HobString pageData = msgData->getData();

                    if (tracingSession)
                        Debug::trace("Session", getName() + HobString(" - SnWriteCurrentWebPage: ") + pageData);

                    HobTokenizerCSV tok(&pageData, BrowserFormatSeparator, 1);
                    HobString token;
                    short dataIndicator = -1;
                    if (tok.next(&token) != -1)
                        dataIndicator = (EvolDeviceType)(Integer::valueOf(token));
                    if (dataIndicator >= 0 && dataIndicator <= MAX_WEBPAGE_FRAME_NUMBER)
                    {
                        if (pageData.size())
                        {
                            currentWebPages[dataIndicator] = pageData;
                            if (tracingSession)
                                Debug::trace("Session", getName() + HobString(" - SnWriteCurrentWebPage: set webpage ") + token + HobString(" to ") + pageData);

                            HobString remotePage(pageData);
                            std::string::size_type pos = remotePage.find("consoletype=");
                            if (pos != std::string::npos)
                                remotePage.replace(pos+12, 1, "2");

                            // The only time we should send the weight frame or target data to monitoring sessions
                            // is if the primary console is in operate mode
                            if (isInOperateMode() || (dataIndicator != 4 && dataIndicator != 5))
                            {
                                SessionMessage remoteMsg(SessionDev, SnWriteCurrentWebPage);
                                remoteMsg.setFormatType(BrowserFormat);
                                GenericHfcObject gho(remotePage);
                                remoteMsg.setHfcObject(&gho);
                                sendToMonitoringSessions(&remoteMsg, Messenger::DEST_UI);
                            }
                            //taskDelay(60);
                        }
                    }
                }
            }
            break;
        }

        case SnCommStart:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnCommStart Event"));
            setSessionType(SESSIONTYPE_COMM);

			receivedCommHeader_ = true;

            SessionMessage startResponse(SessionDev, SnCommStart);
            startResponse.setFormatType(EvolutionTaggedFormat);
            GenericHfcObject responseData("DT1");
            startResponse.setHfcObject(&responseData);
            send(&startResponse, Messenger::DEST_UI);

            // Send the header back to the UI (DataGateWeigh)
            //send(msg, Messenger::DEST_UI);

            if (commHistory)
            {
                delete commHistory;
                commHistory = 0;
            }
            commHistory = new CommHistory();


            if ( (ScaleSynchronizationType)config->toIntCfg(fieldOffsetCfgScaleSynchronizationType) != ScaleSynchronizationTypeStandAlone )
            {
                //Use Case 3.21: Processing HQ upload
                //  2.  Execute Use Case 5.40: Creating batch? passing local?and HQ batch?parameters,
                //      and receiving the new batch identifier.
                resetBatchId();
                synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
                if ( proxy )
                    proxy->createBatch(&currentBatchId, hfc::Batch::OriginLocal, hfc::Batch::TypeHQ);

                //                                  if (!isBatching())
                //                                          ;  // todo - uh, what should I do??  Terminate the restore/merge?? Throw an exception??
            }


            break;
        }

        case SnCommEnd:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnCommEnd Event"));

            // Send any records that need to go before the footer.

            // If something needs to be done before we send the footer, this is the place to do it.
            // [This is not the best implementation. If we need to add more things here we should
            // consider a mechanism to register callbacks (or something similar).]

			if (receivedCommHeader_)
			{
				// Send the footer back to the UI (DataGateWeigh)
				// Only send it back if we received the RT01 message to begin the Session.
				receivedCommHeader_ = false;
		        send(msg, Messenger::DEST_UI);
			}

            if (commHistory)
            {
                commHistory->saveCommHistory(this);
                delete commHistory;
                commHistory = 0;
            }

            if (isBatching())
            {
                //4. Trigger the execution of Use Case 5.21: Processing HQ batch? passing the
                //   batch identifier (the batch will be completed during the execution of this use case).
                Util::CSmartPtr<synchronization::ProcessHQBatchMessage> message(new synchronization::ProcessHQBatchMessage(currentBatchId));
                dispatchEvent(message.get());
                resetBatchId();
            }

            if (configurationManager)
            {
                if (configurationManager->hasPendingSynchronizationChange())
                    configurationManager->applyPendingSynchronizationChange();
            }

			// SOF-7353 - Coctco Columbus wrapper is dropping labels - this may be triggering a ProductChange
			// refresh the Current Transaction Type
			//SessionMessage transTypeMsg(TransactionTypeDev, TransactionTypeRefreshCurrentType);
			//transTypeMsg.setFormatType(InternalFormat);
			//send(&transTypeMsg, Messenger::DEST_RTC);
			//
			//transactionTypeManager->handleMessage(&transTypeMsg);

            /*
               SessionMessage compressDB(SessionDev, SnFreeSpaceBasedRequestDBCompress);
               compressDB.setFormatType(InternalFormat);
               send(&compressDB, Messenger::DEST_RTC);
               */
            // Terminate this session, we're done communicating with the other end of this comm session.
            terminate(false, msg->getEventType());
            break;
        }

        case SnTouchTypeSetting:
        {
            // return the registry setting of the TouchScreen Driver

            SessionMessage touchMsg(SessionDev, SnTouchTypeSetting);
            touchMsg.setFormatType(BrowserFormat);

            // this was for returning an int
            // touchScreenSettingType = getTouchScreenSettingType();
            // GenericHfcObject gho;
            // gho.setData(Integer::toString(touchScreenSettingType));
            // touchMsg.setHfcObject(&gho);

            // this is for returning a HobString
            touchScreenSettingType = getTouchScreenSettingType();
            GenericHfcObject gho;
            gho.setData(touchScreenSettingType);
            touchMsg.setHfcObject(&gho);
            send(&touchMsg, Messenger::DEST_UI);
            break;
        }

        case SnSelectCurrentLanguage:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnSelectCurrentLanguage Event"));
            GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (msgdata)
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - set current language selection to: ") + msgdata->getData());
                LanguageSelection language = (LanguageSelection)Integer::valueOf(msgdata->getData());
                if (language == 1 || language == 2)
                {
                    sendToMonitoringSessions(msg, Messenger::DEST_UI);
                    if (isPrimarySession())
                        changeOperateLanguageSelection(language);
                }
            }

            if (responseMessageManager)
                responseMessageManager->resendMessage(HfcResponse::RESPONSESTYLE_STATUS);   //      Resend any Status messages
            break;
        }

        case SnReadLocaleSettings:
#ifndef returnToLater
            Debug::trace("Session.Clare",       HobString("Processing SnReadLocaleSettings"));
            if (isPrimarySession())
            {
                //      Send the Locale Settings
                SessionMessage localeMsg(SessionDev, SnWriteLocaleSettings);
                localeMsg.setFormatType(BrowserFormat);
                LocaleSettings ls;
                localeMsg.setHfcObject(&ls);
                send(&localeMsg, Messenger::DEST_UI);
                Debug::trace("Session.Clare",   HobString("SnWriteLocaleSettings to the Browser"));
            }
#endif
            break;

#ifndef NOTSELFSERVICE
// jhcSS - not doing a streaming graphic, though I may need to reset the secondary lang here to primary
        case SnStartStreamingGraphic:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnStartStreamingGraphic Event"));

            if (isInOperateMode())
                setOnSelfServiceStartupScreen(true);
            else
                setOnSelfServiceStartupScreen(false);

            if (!currentStreamingGraphic)
            {
                // Only start the streaming graphic if it hasn't already been started.
                HobRecordNum firstStreamingGraphicNum = config->getInitialStreamingGraphicNum();
                currentStreamingGraphic = StreamingGraphic::findStreamingGraphic(firstStreamingGraphicNum, this);
                if (currentStreamingGraphic)
                {
                    SessionMessage imageMsg(SessionDev, SnWriteStreamingGraphic);
                    imageMsg.setFormatType(BrowserFormat);
                    imageMsg.setHfcObject(currentStreamingGraphic);
                    send(&imageMsg, Messenger::DEST_UI);

                    // Create a timer to be notified when this streaming graphic should be replaced with the next one.
                    TimerManager *timerManager = TimerManager::getInstance();
                    if (timerManager)
                        timerManager->addTimer(getId(), currentStreamingGraphic->getDisplayTime(), Timer::RepeatNO, timedFuncStreamingGraphic);
                }
            }
            break;
        }

        case SnStopStreamingGraphic:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnStopStreamingGraphic Event"));

            setOnSelfServiceStartupScreen(false);

            // Delete the timer associated with the streaming graphic.
            TimerManager *timerManager = TimerManager::getInstance();
            if (timerManager)
            {
                Timer *sgTimer = timerManager->getTimer(getId(), timedFuncStreamingGraphic);
                if (sgTimer)
                    timerManager->removeTimer(sgTimer->getTimerId());
            }
            if (currentStreamingGraphic)
            {
                delete currentStreamingGraphic;
                currentStreamingGraphic = (StreamingGraphic *)NULL;
            }
            break;
        }
#endif

        case SnCreateDirectory:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnCreateDirectory Event"));

            createNewDirectory(msg);
            break;
        }

        case SnRestoreIndependentData:
		{
			// This is how Windux and before restored the db data backed up before an upgrade; the Primary Session handled it.
			// Now the SessionManager creates a 'helper' Session to restore the data, not the Primary Session.
			// We still use this code no matter which Session handles the restoring of this data.

            //if (isPrimarySession())
            //{
                if (tracingSession)
                    Debug::trace("Session", "preparing to restore db independent data ...");

                /* Have to allocate one from the heap.  The BackupRestore Manager is going to want to take
                 * possession of the HfcObject in the message, so can't just create one on the stack.  The
                 * life of the HfcObject will be longer than that of its SessionMessage.
                 */
                BackupRestoreSelections *brs = new BackupRestoreSelections();
                brs->setFileName(config->getVersionIndependentBackupFileName());
                brs->setMode(BRMerge);  // merge is good enough; there won't be any data in the database to delete if we specify BROverWrite
                brs->setFormat(BRVersionIndependent);
                brs->setEveryDataTypeSelected(true);
                brs->setDisplayStats(false);

                SessionMessage restore(BackupRestore, Restore);
                restore.setFormatType(BrowserFormat);
                restore.setHfcObject(brs);

                currentModeOp = OP_RESTORINGDBINDEPENDENTDATA;
                handleBackupRestoreMessage(&restore);
                restoreVersionIndependentData = true;
            //}
            break;
		}

        case SnUsingMirrorDatabase:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnUsingMirrorDatabase Event"));

            if (responseMessageManager)
                responseMessageManager->addMessage(ErrSessionDev, SessionDevUsingMirrorDatabase, HfcResponse::RESPONSESTYLE_OK);
            break;
        }

        case SnUsingEmptyDatabase:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnUsingEmptyDatabase Event"));

            if (responseMessageManager)
                responseMessageManager->addMessage(ErrSessionDev, SessionDevUsingEmptyDatabase, HfcResponse::RESPONSESTYLE_OK);
            break;
        }


        case SnCompactFlashMakeBootrom:
        {
            if (isPrimarySession())
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - handling SnCompactFlashMakeBootrom Event"));
                bool copyEntireFlash = false;       //      Bootrom only
                setupBootableCompactFlash(copyEntireFlash);
            }
            break;
        }

        case SnCompactFlashCopyInternal:
        {
            if (isPrimarySession())
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - handling SnCompactFlashCopyInternal Event"));
                bool copyEntireFlash = true;        //      Duplicate Internal flash
                setupBootableCompactFlash(copyEntireFlash);
            }
            break;
        }


        case SnDBCompressStarting:
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnDBCompressStarting Event"));

            currentModeOp = OP_WAITFORDBCLOSEDTEMPORARILY;
			MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, " Starting a Compress Database event");

            if (!isCommSession())
            {
                if (responseMessageManager)
                    responseMessageManager->addMessage(ErrSessionDev, ((msg->getEventType() == SnDBCompressStarting) ? SessionDevDBCompressStarting : SessionDevDBClosingTemporarily), HfcResponse::RESPONSESTYLE_NOINPUT);
            }
            else
            {
                // todo - anything????????????????????????????????????????????????????????
            }

            break;

        case SnDBCompressDone:
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnDBCompressDone Event"));

            currentModeOp = OP_UNKNOWN;
			MessageLogger::logMessage(MessageLogger::ScheduledEventsLog, " Finishing a Compress Database event");

            if (responseMessageManager)
                responseMessageManager->removeMessage(ErrSessionDev, ((msg->getEventType() == SnDBCompressDone) ? SessionDevDBCompressStarting : SessionDevDBClosingTemporarily), HfcResponse::RESPONSESTYLE_NOINPUT);

            break;

        case SnDBBackupStarting:
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnDBBackupStarting Event"));

            currentModeOp = OP_WAITFORDBCLOSEDTEMPORARILY;

            if (!isCommSession() && responseMessageManager)
                responseMessageManager->addMessage(ErrSessionDev, SessionBackingUpDatabase, HfcResponse::RESPONSESTYLE_NOINPUT);
            break;

        case SnDBBackupDone:
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnDBBackupDone Event"));

            currentModeOp = OP_UNKNOWN;

            if (responseMessageManager)
                responseMessageManager->removeMessage(ErrSessionDev, SessionBackingUpDatabase, HfcResponse::RESPONSESTYLE_NOINPUT);
            break;


        case SnPrinterMetrics:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnPrinterMetrics Event"));

            // SEND A PrinterStockMetrics message to browser for each printer.
            for (short printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
            {
                if (printerStockMetrics[printerNum] != NULL)
                {
                    SessionMessage sessionMessage(Printer, PrStockMetrics);
                    sessionMessage.setFormatType(BrowserFormat);
                    sessionMessage.setHfcObject(printerStockMetrics[printerNum]);
                    send(&sessionMessage, Messenger::DEST_UI);
                }
            }
            break;
        }

        case SnShutdownComplete:
        {
            currentModeOp = OP_SCALESHUTDOWN;
            break;
        }

        case SnShutdownRequest:
        {
            if ( msg->getFormatType() == EvolutionTaggedFormat )
            {
                /* The Scale Management Software has requested to shutdown. Mark this condition
                   and continue processing the communication stream until it ends. */
                commRequestedShutdown_ = true;
                return;
            }

            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - Begin of SnShutdownRequest Event"));

			ShutdownRequestType shutdownRequestType = ShutdownRequest_Reboot;

            GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (msgdata)
				shutdownRequestType = (ShutdownRequestType) Integer::valueOf(msgdata->getData());

            if (tracingSession)
                Debug::trace("Session", getName() + string(" - shutdown request given is: ") + inttostring(shutdownRequestType));

            // The user is cancelling the shutdown request; they'll wait for communications to finish and shutdown later.
            if (shutdownRequestType == ShutdownRequest_Cancel)
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - User chose to cancel the SnShutdownRequest Event"));

                if (responseMessageManager)
                    responseMessageManager->removeMessage(ErrSessionDev, SessionDevCommExistingBeforeShutdown, HfcResponse::RESPONSESTYLE_OKCANCEL);
                currentModeOp = OP_UNKNOWN;
                return;
            }

            // The user is forcing the shutdown request to occur immediately, not waiting for comm to finish.
            if (shutdownRequestType == ShutdownRequest_Force)
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - User chose to force the SnShutdownRequest Event, don't wait for comm to finish"));

                if (responseMessageManager)
                    responseMessageManager->removeMessage(ErrSessionDev, SessionDevCommExistingBeforeShutdown, HfcResponse::RESPONSESTYLE_OKCANCEL);

                SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
                shutdownMsg.setFormatType(InternalFormat);
                send(&shutdownMsg, Messenger::DEST_RTC, false);

                if (getSessionType() == Session::SESSIONTYPE_REMOTE || getSessionType() == Session::SESSIONTYPE_COMM)
                {
                    if (tracingSession)
                        Debug::trace("Session", getName() + HobString(" - force reboot request if it came from a remote browser"));

                    RebootMessage rm(false);  // passing in false will perform a Windows restart, not a power down
                    dispatchEvent(&rm);
                }

                currentModeOp = OP_UNKNOWN;
                return;
            }

            // The user is forcing the reboot or shutdown to occur immediately
            if (shutdownRequestType == ShutdownRequest_ForceReboot || shutdownRequestType == ShutdownRequest_ForceShutdown)
            {
                if (tracingSession)
					Debug::trace("Session", getName() + string(" - User chose to force ") + ((shutdownRequestType == ShutdownRequest_ForceReboot) ? "reboot" : "shutdown"));

                SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
                shutdownMsg.setFormatType(InternalFormat);
                send(&shutdownMsg, Messenger::DEST_RTC, false);

				RebootMessage rm((shutdownRequestType == ShutdownRequest_ForceReboot) ? false : true);  // passing in false will perform a reboot, not a power down
				dispatchEvent(&rm);

                currentModeOp = OP_UNKNOWN;
                return;
            }

			if (SessionManager::getInstance()->commSessionExists() &&
				!(shutdownRequestType == ShutdownRequest_ConfigureOS && !config->toBoolCfgI(CfgInternal::FBWFACTIVATED)))
            {
                // If any comm session is active, then display a message so the user has the chance to wait
                // for the communication to finish before shutting down the scale.  Only display this message
                // on the console requesting the shutdown.

				// However, don't wait for active comm sessions if we're trying to 'Configure Windows' (msgdata->getData() == ShutdownRequest_ConfigureOS)
				// and FBWF is not active

                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - Not forcing shutdown, at least one comm session exists ..."));

                if (responseMessageManager)
                    responseMessageManager->addMessage(ErrSessionDev, SessionDevCommExistingBeforeShutdown, HfcResponse::RESPONSESTYLE_OKCANCEL);

                if (shutdownRequestType == ShutdownRequest_Reboot)  // shutdown and automatically reboot
                    currentModeOp = OP_WAITINGFORCOMMBEFORESHUTDOWNANDREBOOT;
                else if (shutdownRequestType == ShutdownRequest_ConfigureOS)  // turn off FBWF and reboot
                    currentModeOp = OP_WAITINGFORCOMMBEFOREFBWFREBOOT;
                else  // just shutdown, don't automatically reboot
                    currentModeOp = OP_WAITINGFORCOMMBEFORESHUTDOWN;
            }
            else
            {
                if (responseMessageManager)
                    responseMessageManager->removeMessage(ErrSessionDev, SessionDevCommExistingBeforeShutdown, HfcResponse::RESPONSESTYLE_OKCANCEL);
                currentModeOp = OP_UNKNOWN;

#ifndef NOFBWF
				// If the scale is not using the FBWF, then we don't need to turn it off to configure Windows,
				// nor do we need to set the scale's services to manual start.

				if (shutdownRequestType == ShutdownRequest_ConfigureOS && config->toBoolCfgI(CfgInternal::FBWFACTIVATED))  // turn off the file-based write filter
                {
                    if (disableFBWF() == NO_ERROR)
                        VersionManager::getInstance()->disableHobartServicesOnBootUp();
                    else if (responseMessageManager)
                    {
                        responseMessageManager->addMessage(ErrSessionDev, SessionDevCouldNotUnprotectOS, HfcResponse::RESPONSESTYLE_OK);
                        break;
                    }
                }
#endif

                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - forwarding SnShutdownRequest Event to SessionManager"));

                // Forward this message on to the SessionManager, the one that actually performs the shutdown (and reboot).

				if (shutdownRequestType == ShutdownRequest_ConfigureOS && !config->toBoolCfgI(CfgInternal::FBWFACTIVATED))  // turn off the file-based write filter
				{
					// FBWF is not active, just logoff the current user if they have restricted logins.
					// If they don't have restricted logins, just go to the desktop.
					if (config->toBoolCfgI(CfgInternal::USINGRESTRICTEDLOGINS))
					{
						if (tracingSession)
							Debug::trace("Session", getName() + HobString(" - CfgIUsingRestrictedLogins, so forward SnLogoffRequest Event to SessionManager"));

						SessionMessage logoffRequestMsg(SessionDev, SnLogoffRequest);
						logoffRequestMsg.setFormatType(InternalFormat);
						send(&logoffRequestMsg, Messenger::DEST_RTC, false);
					}
					else
					{
						// Never should get into this block.  The handling of SessionDevConfigWindowsWithoutRestrictedLogin
						// goes directly to the desktop via accessSystemDesktop().
						if (tracingSession)
							Debug::trace("Session", getName() + HobString(" - not CfgIUsingRestrictedLogins, so just go to the desktop"));

						accessSystemDesktop();
					}
				}
				else
				{
					// Pass the shutdown request to the SessionManager who will finish the process of shutting down.
					SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
					shutdownMsg.setFormatType(InternalFormat);
					send(&shutdownMsg, Messenger::DEST_RTC, false);

					if (getSessionType() == Session::SESSIONTYPE_REMOTE)
					{
						if (tracingSession)
							Debug::trace("Session", getName() + HobString(" - force reboot request if it came from a remote browser"));

						RebootMessage rm(false);  // passing in false will perform a Windows restart, not a power down
						dispatchEvent(&rm);
					}
				}

#ifndef NOFBWF
				if (shutdownRequestType == ShutdownRequest_ConfigureOS && config->toBoolCfgI(CfgInternal::FBWFACTIVATED))  // shutdown and automatically reboot, set to 4 when FBWF being turned off and rebooting
                {
                    SessionMessage rebootMsg(SessionDev, SnReboot);
                    rebootMsg.setFormatType(InternalFormat);
                    send(&rebootMsg, Messenger::DEST_RTC, false);
                }
#endif

            }
            break;
        }


        case SnTestMessage:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnTestMessage Event"));

            // Just turn the message around and send it back to the Browser.
            // I'm assuming the RTC will never send me this kind of message,
            // especially since I'm not registered to receive it.
            //                              send(msg, Messenger::DEST_UI);


            SessionMessage testresp(SessionDev, SnTestMessage);
            testresp.setFormatType(BrowserFormat);
            GenericHfcObject gho;

            GenericHfcObject *msgdataalloc( NULL );
            GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (!msgdata)
                msgdata = msgdataalloc = new GenericHfcObject("huh?");

            gho.setData(getName() + HobString(": ") + msgdata->getData());
            testresp.setHfcObject(&gho);
            send(&testresp, Messenger::DEST_UI);

            if (msgdataalloc)
            {
            	delete msgdataalloc;
            	msgdataalloc = NULL;
            }

            /*
               SessionMessage testresp(SessionDev, SnTestMessage);
               testresp.setFormatType(BrowserFormat);
               GenericHfcObject ht;
               int len=255;
               char asdf[len];
               memset(asdf, 0, sizeof(asdf));
               for (int i = 1; i < len; i++)
               if (i != 44)
               asdf[i-1] = (char)i;
               ht.setData(HobString(asdf, len));
               testresp.setHfcObject(&ht);
               send(&testresp, Messenger::DEST_UI);
               */
            /*
               GenericHfcObject *gho = (GenericHfcObject *) msg->getHfcObject();

               SessionMessage response(msg->getDeviceType(), msg->getEventType());
               response.setFormatType(msg->getFormatType());
               response.setHfcObject(gho);

               if (Debug::isTracing("Session"))
               {
               const char *msgdata = response.toString();
               int msglen = response.getSerializedLength();
               Debug::trace("Session", HobString("TestMessage response: ") +
               HobString(Hex::toString(msgdata, msglen)));
               }

               int bytesSent =  myMessenger.send(&response, Messenger::DEST_UI);
               if (bytesSent < 0)
               {
            // todo - error handling?
            Debug::trace("Session", "RESPONSE MESSAGE COULD NOT BE SENT");
            }
            */
        }
            break;

        case SnDelay:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnDelay Event"));

            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (gho)
            {
                int dtime = Integer::valueOf(gho->getData().c_str());
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - starting DELAY of ") + HobString(Integer::toString(dtime)));
                PAL_absclass::PAL_sleep(dtime);
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - ending   DELAY of ") + HobString(Integer::toString(dtime)));
            }
        }
            break;

        case SnDebugTrace:
        {
            /* This message allows the browser to turn on and off debugging via the Debug class.
             * The data passed in with this message will be tokenized via the CSV tokenizer;
             * each token should be of the following formats:
             *
             * +Debug : turn debugging on, output will be sent to the stdout.
             * -Debug : turn debugging off.
             * +Debugfile : turn debugging on, output will be sent to the file specified in the next token.
             * -Debugfile : turn debugging off, the filename should not be passed in the next token.
             * +string : turn tracing on for the 'string' following the '+' sign
             * -string : turn tracing off for the 'string' following the '-' sign
             * number : set trace level to this number
             *
             */

            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (gho)
            {
                HobString temp(gho->getData().c_str(), gho->getData().length());
                HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
                HobString token;
                while (tok.next(&token) != -1)
                {
                    if (token.length() > 0)
                    {
                        if (token == "+Debug")
                            Debug::setEnabled(true);
                        else if (token == "-Debug" || token == "-Debugfile")
                            Debug::setEnabled(false);
                        else if (token == "+Debugfile")
                        {
                            if (tok.next(&token) != -1)
                                Debug::setEnabled(true, token.c_str());
                        }
                        else if (token.at(0) == '-' && token.length() > 1)
                            Debug::removeTraceCategory(HobString(token.substr((string::size_type)1)));
                        else if (token.at(0) == '+' && token.length() > 1)
                            Debug::addTraceCategory(HobString(token.substr((string::size_type)1)));
                        else
                        {
                            int level = Integer::valueOf(token);
                            if ( level > 0 )
                                Debug::setTraceLevel(level);
                        }
                    }
                }
            }
        }
            break;

        case SnDebugWrite:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnDebugWrite Event"));

            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (gho)
            {
                HobString temp(gho->getData().c_str(), gho->getData().length());
                HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
                HobString category;
                HobString msg;
                if (tok.next(&category) != -1 && tok.next(&msg) != -1)
                    Debug::trace(category.c_str(), msg);
            }
        }
            break;
            /*
               case SnWeigherFrequency:
               {
               GenericHfcObject *gho = (GenericHfcObject *) msg->getHfcObject();
               if (gho)
               {
               int wghFreq = Integer::valueOf(gho->getData().c_str());
               Debug::trace("Session", getName() + HobString(" - setting Dummy Weigher Frequency to ") + HobString(Integer::toString(wghFreq)));
               config->setFakeWeigherFrequency(wghFreq);
               }
               }
               break;
               */

        case SnTestAllProducts:
        {

            CMFile *f = new CMFile("/tgtsvr/abcdefg", CommMechanism::MODE_OUTPUT);
            CMReaderWriterEvolUI *frw = new CMReaderWriterEvolUI();
            f->setMyReaderWriter(frw);
            f->setHandlingLineSeparator(true);

            int myBackupID = getMessenger().addCommMechanism(f);

            Product *prod = productManager->firstProduct();
            while (prod)
            {
                SessionMessage msg(ProductDev, ProductWrite);
                msg.setFormatType(BrowserFormat);
                msg.setHfcObject(prod);
                send(&msg, myBackupID);
                delete prod;

                prod = productManager->nextProduct();
            }

            getMessenger().removeCommMechanism(myBackupID);
            delete f;
            break;
        }

        case SnFileHandlesUsage:  // report on all open files
        {
			// variables used by the code comment out by "lcg rev 0.1"
//			char fname[257] = {0};
//			int ret = 0;
//			int fd = 0;
//			cout << "List of open files:" << endl;
            /*lcg rev 0.1 while (fd < 800)
              {
              if ((ret = ioctl(fd, FIOGETNAME, (int)fname)) == 0)
              cout << "fd:" << fd << "  ret:" << ret << "  fname:" << fname << endl;
              fd++;
              memset(fname,0,sizeof(fname));
              }*/
            break;
        }


        case SnSetNewFirmwareVersionSearchTime:
        {
            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (gho)
            {
                config->setNewFirmwareVersionSearchTime(gho->getData());
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - set firmware lookup time to ") + gho->getData());
            }
            break;
        }

        case SnSendShutdownSystemRequest:
        {
            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (gho)
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - 109 flag: ") + gho->getData());
                ShutdownSystemRequest msg(Bool::valueOf(gho->getData()));
                dispatchEvent(&msg);
            }
            break;
        }

            // - add a timer, for this session, that repeats
            // - delete a timer
            // - remove a specific timer for this session - it shouldn't go off anymore
        case SnTestAddTimer:
        {
            // add a timer, for this session, that does not repeat
            TimerManager *timerManager = TimerManager::getInstance();
            if (timerManager)
                timerManager->addTimer(getId(), 15, Timer::RepeatNO, timedFuncGeneralTimer3);
            break;
        }

        case SnTestDeleteAllSessionTimers:
        {
            // delete all timers for this session
            TimerManager *timerManager = TimerManager::getInstance();
            if (timerManager)
                timerManager->removeAllEventTimersForSession(getId());
            break;
        }

        case SnTestDisplayAllTimers:
        {
            // display all timers
            TimerManager *timerManager = TimerManager::getInstance();
            if (timerManager)
                timerManager->displayAllTimers();
            break;
        }

        case SnShowAllTasks:
        {
            // show all tasks
            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            if (gho)
            {
				// taskId : unused variable
				//unsigned int taskId = Hex::valueOf(gho->getData().c_str());
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - taskShow for taskId") + gho->getData());
                //taskShow(taskId,0);
            }
            else
            {
                if (tracingSession)
                    Debug::trace("Session", getName() + HobString(" - taskShow for all tasks"));
                //taskShow(0,2);
            }

            break;
        }

        case SnDispatchConfigurationSaved:
        {
            config->setScaleModel((config->isSelfServiceScale()) ? ScaleModel_EPP : ScaleModel_ESS);

            if (configurationManager)
                configurationManager->dispatchConfigurationSaved();
            break;
        }

        case SnTellBrowserGotoHomepage:
        {
            if (config->isSelfServiceScale())
            {
                if (isPrimarySession())
                {
                    if (tracingSession)
                        Debug::trace("Session", getName() + HobString(" - telling browser to goto homepage"));

                    SessionMessage gotoPageMsg(SessionDev,SnWriteCurrentWebPage);
                    gotoPageMsg.setFormatType(BrowserFormat);
                    GenericHfcObject gho1("3,./domOperate.asp?consoletype=1&languageCode=en&countryCode=US");
                    gotoPageMsg.setHfcObject(&gho1);
                    send(&gotoPageMsg, Messenger::DEST_UI);
                    GenericHfcObject gho2("1,./selfservStart.asp?consoletype=1&priLang=en&secLang=es&languageCode=en&countryCode=US");
                    gotoPageMsg.setHfcObject(&gho2);
                    send(&gotoPageMsg, Messenger::DEST_UI);
                    GenericHfcObject gho3("2,./emptyMenu.asp?consoletype=1&languageCode=en&countryCode=US");
                    gotoPageMsg.setHfcObject(&gho3);
                    send(&gotoPageMsg, Messenger::DEST_UI);
                }
                else
                {
                    Debug::trace("Session", getName() + HobString(" - sending message onto the Primary Session"));
                    msg->setFormatType(InternalFormat);
                    send(msg, Messenger::DEST_RTC);
                }
            }
            break;
        }

        case SnAddSessionAsTraceListener:
            Trace::getInstance()->addTraceListener(getMessenger().getMyUI());
            break;

        case SnRemoveSessionAsTraceListener:
            Trace::getInstance()->removeTraceListener(getMessenger().getMyUI());
            break;


//	HW-196: Wait until the Operator acknowledges that Initialization is complete, instead of setting this timer.
//        case SnCreateLostPeripheralsTimer:
//        {
//            TimerManager *timerManager = TimerManager::getInstance();
//            if (timerManager)
//            {
//                timerManager->addTimer(getId(), config->toIntCfgI(CfgInternal::LOSTPERIPHERALSCHECKTIME), Timer::RepeatNO, timedFuncCheckMissingPeripherals);
//                if (tracingSession)
//                    Debug::trace("Session", 20, "Created the timer for when to check for lost peripherals.");
//            }
//            break;
//        }

        case SnSoundBeep:
        {
            if (tracingSession)
                Debug::trace("Session", getName() + HobString(" - handling SnSoundBeep Event"));
            GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());

            if (msgData && !msgData->getData().empty())
            {
                // Split the data into tokens (type, count).
                HobString beepData = msgData->getData();
                HobTokenizerCSV tok(&beepData, BrowserFormatSeparator);
                HobString token;

                unsigned int type = 0, count = 0;
                if (tok.next(&token) != -1)
                    type = (Integer::valueOf(token));
                if (type > 0)
                {
                    if (tok.next(&token) != -1)
                        count = (Integer::valueOf(token));
                    if (count > 0)
                    {
                        if (type == 1)
                            ServiceSpeaker::badBeep(count);
                        else if (type == 2)
                            ServiceSpeaker::goodBeep(count);
                    }
                }
            }
            break;
        }

        case SnPlatterTareOn:
            turnPlatterTareOn();
            break;

        case SnPlatterTareOff:
            turnPlatterTareOff();
            break;

        case SnDesktopAccess:
            messageBeforeAccessSystemDesktop();
            break;

		case SnProductSwitchToManualWeighMode:
			productSwitchToManualWeighMode();
			break;

		case SnProductSwitchToAutoManualApplyMode:
			productSwitchToAutoManualApplyMode();
			break;

		case SnProductSwitchToBuildAMealMode:
			productSwitchToBuildAMealMode();
			break;

		case SnProductReturnToPriorOpMode:
			productReturnToPriorOpMode();
			break;

        case SnPopupVirtualKeyboard:
        {
            GenericHfcObject * msgData = dynamic_cast<GenericHfcObject *>(msg->getHfcObject());
            if ( msgData && !msgData->getData().empty() )
                popupVirtualKeyboard(util_string::stringtoint(msgData->getData()));
            else
                popupVirtualKeyboard();
            break;
        }
        case SnDismissVirtualKeyboard:
            dismissVirtualKeyboard();
            break;

        case SnPopupIntranetBrowserWindow:      // 79
        {
            GenericHfcObject * msgData = dynamic_cast<GenericHfcObject *>(msg->getHfcObject());
            popupIntranetBrowserWindow(util_string::stringtoint(msgData->getData()));
            break;
        }

        case SnPopupScaleBrowserWindow:         // 80
        {
            popupScaleBrowserWindow();

            // Send a message (that all sessions will be listening to) so all ui sessions can send
            // a request to their UI to refresh their FPP frames if present
            SessionMessage requestRefresh(SessionDev, SnRefreshFPPFrame);
            requestRefresh.setFormatType(InternalFormat);
            dispatchEvent(&requestRefresh);

            break;
        }

        case SnSetFlashkeyInternetWebPage:      // 82
        {
            if(isPrimarySession())
            {

                GenericHfcObject * msgData = dynamic_cast<GenericHfcObject *>(msg->getHfcObject());
                popupIntranetBrowserWindow(1, msgData->getData());
            }
            else if(isIntranetSession())
            {
                msg->setFormatType(BrowserFormat);
                send(msg, Messenger::DEST_UI);
                ieWindowToFront();
            }

            break;
        }

        case SnSwitchToIntranetFrame:           // 83
        {
            // send message to the Intranet Session, telling the page to resize the frames to make the Intranet Frame the active and shown frame
            if(isIntranetSession())
            {
                msg->setFormatType(BrowserFormat);
                send(msg, Messenger::DEST_UI);
            }
            break;
        }

        case SnRequestIntranetAddress:          // 84
        {
            // when the intranet session boots up, it sends a 100,84, requesting the Intranet Address (set in page 2 or normal scale operation)
            if(isIntranetSession())
            {
                // when the message first comes in, its from the intranet session, now PASS THE SAME MESSAGE request on to the primary session
                msg->setFormatType(InternalFormat);
                send(msg, Messenger::DEST_RTC);
            }
            else if(isPrimarySession())
            {
                // the intranet session passes it's copy of 100,84 to the primary (HERE).  Now reply with the address
                GenericHfcObject msgData(savedIntranetAddress);

                SessionMessage frameMsg(SessionDev, SnSetFlashkeyInternetWebPage);          // 100,83, set Frame 0, display the defaultWebPage
                frameMsg.setFormatType(InternalFormat);
                frameMsg.setHfcObject(&msgData);
                send(&frameMsg, Messenger::DEST_RTC);
                savedIntranetAddress = "";
                if (responseMessageManager)
                    responseMessageManager->removeMessage(ErrSessionDev, SessionDevLoadingIntranetIE, HfcResponse::RESPONSESTYLE_NOINPUT);
            }
            break;
        }

        case SnRefreshFPPFrame:       // 85
        {
            // Send message to the browser to refresh FPP frame
            if ( isPrimarySession() || isRemoteSession() )
            {
                SessionMessage refresh(CGICallManagerDev, comm::CGICallPerformed);
                refresh.setFormatType(BrowserFormat);
                send(&refresh, Messenger::DEST_UI);
            }

            break;
        }

		case SnDisplayDialog:
		{
			UIDialogMsg dialog(msg->getOriginalBuff().c_str());
			if (responseMessageManager)
			{
				if (dialog.isRemoveDialog())
					responseMessageManager->removeMessage((ErrorEvent)dialog.getErrorDevice(), dialog.getStatusCode(), dialog.getResponseType());
				else
					responseMessageManager->addMessage((ErrorEvent)dialog.getErrorDevice(), dialog.getStatusCode(), dialog.getResponseType());
			}
			break;
		}

        case SnStopDialogs: // 90
            if (responseMessageManager)
                responseMessageManager->setDisplayingDialogs(false);
            break;

        case SnRestartDialogs: // 91
            if (responseMessageManager)
                responseMessageManager->setDisplayingDialogs(true);
            break;

		case SnCheckForExpiredRecords:
		{
			Debug::trace("PurgeOldRecords", getName() + HobString(" - SnCheckForExpiredRecords spawning thread.."));

		    if ( checkForExpiredRecords_.joinable() )
		    	checkForExpiredRecords_.join();

			checkForExpiredRecords_ = std::thread( &Session::checkForExpiredRecordsThread, this );
            break;
		}

		case SnProcessDownloadedFiles:
		{
			//In processing the files, we won't have a UI to display status on, so keep track of all and write to the info log upon completion.
			//In processing, the BackupRestoreManager determines the format of the data in the file, so it knows how to process it.

			Debug::trace("ProcessDownloadedFiles", getName() + HobString(" - SnProcessDownloadedFiles start"));

			// currentModeOp will get set to OP_PROCESSINGDOWNLOADEDFILES in here if there is a file to process
			// If no file to process, this will be the indication at the end of this event to terminate this Session;
			// there is nothing more for it to do;
			currentModeOp = OP_UNKNOWN;

			string processingDirStr = config->toStringCfgI(CfgInternal::DOWNLOADPROCESSINGDIR);
			if (processingDirStr.size())
			{
				util_filesystem::Directory::DirectoryFileList directoryFileList;
				util_filesystem::Directory processingDir("");
				processingDir.appendPath(processingDirStr);
				if (processingDir.exists())
				{
					//	Let's parse the pending directory and create a list of the pending files
					util_filesystem::Directory::getAllAvailableFiles(processingDirStr, &directoryFileList, "*.*");
					Debug::trace("ProcessDownloadedFiles", getName() + HobString(" - Found ") + inttostring(directoryFileList.size()) + " files to process.");
				}

				int importCounter = 0;
				util_filesystem::Directory::DirectoryFileListIterator dflIter = directoryFileList.begin();
				while (dflIter != directoryFileList.end())
				{
					File downloadFile(dflIter->first);
					if ( downloadFile.exists() && dflIter->second == downloadFile.size())
					{

						// If we want to make a backup of files we've processed, do that now, before we process it.
						if ( config->toBoolCfgI(CfgInternal::KEEPCOPYOFPROCESSEDDATA) )
						{
							std::string processedDirStr = config->toStringCfgI(CfgInternal::PROCESSEDDIR);
							if (processedDirStr.size())
							{
								util_filesystem::Directory processedDir("");
								processedDir.appendPath(processedDirStr);
								if (!processedDir.exists())
									processedDir.create();
								if (!processedDir.exists())
									Debug::trace("ProcessDownloadedFiles", "handleBackupRestoreMessage():  Could not create the processed directory for keeping files we've processed!");
								else
								{
									string processedFilename = "";
									bool shouldMakeFilenameUnique = config->toBoolCfgI(CfgInternal::MAKEPROCESSEDFILENAMEUNIQUE);
									if (shouldMakeFilenameUnique)
									{
										HobDateTime now;
										processedFilename.append(now.formatDate(FormatDate_YYMMDD_NoDelimiter));
										processedFilename.append("_");
										processedFilename.append(now.formatTime(FormatTime_HHMMSS_NoDelimiter, HourFormat_MILITARY));
										processedFilename.append("_");
									}
									processedFilename.append(downloadFile.getFileName());
									File processedFile(processedDir.getFullPath(), processedFilename);
									Debug::trace("ProcessDownloadedFiles", "handleBackupRestoreMessage():  Copying file to processed directory, file " + downloadFile.getFullPath() + " to " + processedFile.getFullPath());

									if ( downloadFile.copyTo( processedFile.getFullPath() ) )
										Debug::trace("ProcessDownloadedFiles", "handleBackupRestoreMessage():  File copied successfully: " + processedFile.getFullPath());
								}
							}
						}

						//	Process this file
						// See if the file to be processed is firmware (and get the device type for which it is intended)
						HobVersion::VersionType firmwareDeviceType = HobVersion::UnknownComponent;
						// firmwareDeviceType gets filled in by fileIsFirmware 
						if ( fileIsFirmware(downloadFile.getFileName(), firmwareDeviceType) )
						{
                            Debug::trace("ProcessDownloadedFiles", "Found device: " + Integer::toString(firmwareDeviceType));
							if ( firmwareDeviceType == HobVersion::UnknownComponent )
							{
								// If this file is firmware, but we couldn't determine the device for which it is intended, then log it and delete the file.
								MessageLogger::logMessage(MessageLogger::InfoLog, "Error!  Could not determine the device for this new firmware:  " + downloadFile.getFullPath());
								downloadFile.remove();
							}
#if defined ( __linux__ )
                            else if(firmwareDeviceType == HobVersion::KpcPrinterSW || firmwareDeviceType == HobVersion::KpcWeigherSW ){
                                Debug::trace("ProcessDownloadedFiles", getName() + " Found new firmware in processing directory, setting up to reburn the device: " + downloadFile.getFullPath());

                                downloadFile.copyTo("/lib/firmware/kpc/app/" + downloadFile.getFileName());
                                downloadFile.remove();
                                GpioDeviceReset kpcReset;
                                kpcReset.setDeciceTypeKpc();
                                Debug::trace("ProcessDownloadedFiles", "Resetting KPC....");
                                kpcReset.releaseResetDevice();
                                PAL_API::usleep(500);
                                kpcReset.resetDevice();
                                
                                Debug::trace("ProcessDownloadedFiles", "Reset Complete");
                            }
                            else if(firmwareDeviceType == HobVersion::KpcBootloaderSW ){
                                Debug::trace("ProcessDownloadedFiles", getName() + " Found new firmware in processing directory, setting up to reburn the device: " + downloadFile.getFullPath());
                                downloadFile.copyTo("/lib/firmware/kpc/boot/" + downloadFile.getFileName());
                                downloadFile.remove();
                                GpioDeviceReset kpcReset;
                                kpcReset.setDeciceTypeKpc();
                                Debug::trace("ProcessDownloadedFiles", "Resetting KPC....");
                                kpcReset.releaseResetDevice();
                                PAL_API::usleep(500);
                                kpcReset.resetDevice();
                                Debug::trace("ProcessDownloadedFiles", "Reset Complete");
                            }
                            else if(firmwareDeviceType == HobVersion::FSSPrinterSW){
                                Debug::trace("ProcessDownloadedFiles", getName() + " Found new firmware in processing directory, setting up to reburn the device: " + downloadFile.getFullPath());
                                downloadFile.copyTo("/lib/firmware/fsss/app/" + downloadFile.getFileName());
                                downloadFile.remove();
                                GpioDeviceReset kpcReset;
                                kpcReset.setDeciceTypeKpc();
                                Debug::trace("ProcessDownloadedFiles", "Resetting FSSSS....");
                                kpcReset.releaseResetDevice();
                                PAL_API::usleep(500);
                                kpcReset.resetDevice();
                                Debug::trace("ProcessDownloadedFiles", "Reset Complete");
                            }
                            else if(firmwareDeviceType == HobVersion::FSSBootloaderSW){
                                Debug::trace("ProcessDownloadedFiles", getName() + " Found new firmware in processing directory, setting up to reburn the device: " + downloadFile.getFullPath());
                                downloadFile.copyTo("/lib/firmware/fsss/boot/" + downloadFile.getFileName());
                                downloadFile.remove();
                                GpioDeviceReset kpcReset;
                                kpcReset.setDeciceTypeKpc();
                                Debug::trace("ProcessDownloadedFiles", "Resetting FSSSS....");
                                kpcReset.releaseResetDevice();
                                PAL_API::usleep(500);
                                kpcReset.resetDevice();
                                Debug::trace("ProcessDownloadedFiles", "Reset Complete");
                            }
#endif
							else
							{
								Debug::trace("ProcessDownloadedFiles", getName() + "Found new firmware in processing directory, setting up to reburn the device: " + downloadFile.getFullPath());

								// Move the firmware file out of the processing directory, so VersionManager and other related classes
								// know where to find it to burn it to the device, and so we won't process it again.
								Directory directoryFromWhereFirmwareGetsBurnedToDevice("");

								HobComplexVersion::getExtractionDir(firmwareDeviceType, directoryFromWhereFirmwareGetsBurnedToDevice, 0); // the last parameter isn't used, but has to be supplied
								if (!directoryFromWhereFirmwareGetsBurnedToDevice.exists())
									directoryFromWhereFirmwareGetsBurnedToDevice.create();

								File newLocationFile(directoryFromWhereFirmwareGetsBurnedToDevice.getFullPath(), config->toStringCfgI(CfgInternal::FIRMWAREGENERICFILENAME));
								Debug::trace("ProcessDownloadedFiles", "Moving firmware file to processing directory, file moving from " + downloadFile.getFullPath() + " to " + newLocationFile.getFullPath());
								downloadFile.moveTo(newLocationFile);

								if ( downloadFile.exists() )
								{
									// We should never get into here, because the file should have been moved.
									// But if we couldn't move the file, then I have to do something or I would never 
									// continue on to the next file to be processed, so I'm going to just delete it.

									MessageLogger::logMessage(MessageLogger::InfoLog, "Error!  Could not move the firmware so it could be burned to the device:  " + downloadFile.getFullPath());
									downloadFile.remove();
								}

								// Reset the device; after resetting, it will re-enumerate and ask for any new firmware, which we just put into the directory from which it gets burned to the device.
								if ( newLocationFile.exists() )
								{
									Debug::trace("ProcessDownloadedFiles", "Successfully moved firmware file to burning directory, preparing to have it burned - Burn, baby, burn!: ResetFPCComponent");
									EvolMessage resetFPCMsg(VersionManagerDev, ResetFPCComponent);
									dispatchEvent(&resetFPCMsg);
								}
							}

							// Reset the mode so this Session will go and process the next file in the processing directory.
							currentModeOp = OP_PROCESSDOWNLOADEDFILES;
						}
						else if ( fileIsGraphic(downloadFile.getFileName()) )
						{
							++importCounter;
							Debug::trace("ProcessDownloadedFiles", getName() + " - The file [" + downloadFile.getFullPath() + "] is a graphic. importCounter = " + inttostring(importCounter));
						}
						else
						{
							Debug::trace("ProcessDownloadedFiles", getName() + " - Creating a BackupRestoreManager to process file: " + downloadFile.getFullPath());

							currentModeOp = OP_PROCESSINGDOWNLOADEDFILES;

							std::string outputDirStr = config->toStringCfgI(CfgInternal::DOWNLOADOUTPUTDIR);

							util_filesystem::Directory outputDir("");
							outputDir.appendPath(outputDirStr);
							if (!outputDir.exists())					//	Make sure the directory exists.
								outputDir.create();

							util_filesystem::File outputFile (outputDirStr, downloadFile.getFileName());
							outputFile.setFileNameExtension("out");		//	TODO: make configurable

							/* Have to allocate one from the heap.  The BackupRestore Manager is going to want to take
							 * possession of the HfcObject in the message, so can't just create one on the stack.  The
							 * life of the HfcObject will be longer than that of its SessionMessage.
							 */
							BackupRestoreSelections *selections = new BackupRestoreSelections();
							selections->setFileName(downloadFile.getFullPath());
							selections->setAltFileName(outputFile.getFullPath());
Debug::trace("ProcessDownloadedFiles", getName() + " - Input/Alt file: [" + downloadFile.getFullPath() + "],  Output file: [" + outputFile.getFullPath() + "]");
							selections->setMode(BRMerge);
							/// Don't need to set this - it is auto-determined.   selections->setFormat(BREvolution);
							selections->setEveryDataTypeSelected(true);
							selections->setDisplayStats(false);

							SessionMessage brMsg(BackupRestore, ProcessFile);
							brMsg.setFormatType(BrowserFormat);
							brMsg.setHfcObject(selections);

							std::string logmsg = "Processing file: " + downloadFile.getFullPath() + ".  Output file (if necessary): " + outputFile.getFullPath();
							MessageLogger::logMessage(MessageLogger::ScaleMaintenanceLog, logmsg);

							handleBackupRestoreMessage(&brMsg);
						}
					}
					++dflIter;
				}
				if (importCounter)
				{
//					//	First, set the modeOp back to OP_PROCESSDOWNLOADEDFILES, so the ImportManager
//					//	can do it's thing without the Session being deleted before IM is done.
//					currentModeOp = OP_PROCESSINGDOWNLOADEDFILES;
					waitingForImportTerminate_ = true;

					//	Now we can process all of the Graphics at one time.
					ImportSelections *selections = new ImportSelections();
					selections->setDirName(config->toStringCfgI(CfgInternal::DOWNLOADPROCESSINGDIR));	//	It will find the images subdir.
					selections->setMoveFile(true);		//	We want to move the graphics from the pending directory.

					SessionMessage importMsg(ImportDev, ImportStart);
					importMsg.setFormatType(BrowserFormat);
					importMsg.setHfcObject(selections);

					std::string logmsg = "Importing " + inttostring(importCounter) + " graphic(s).";
					MessageLogger::logMessage(MessageLogger::ScaleMaintenanceLog, logmsg);

					handleImportMessage(&importMsg);
				}
			}

			if (( currentModeOp == OP_UNKNOWN ) && ( waitingForImportTerminate_ == false ))
			{
				if ( !isPrimarySession() )
				{
					Debug::trace("ProcessDownloadedFiles", getName() + " - Finished processing downloaded files.");
					std::string logmsg = "Processing session " + getName() + HobString(" Finished processing downloaded files.");
					MessageLogger::logMessage(MessageLogger::ScaleMaintenanceLog, logmsg);

					// Make this session go away.
					PAL_absclass::PAL_sleep(1000);
					//terminate(false);
					//quitGracefully();
					Debug::trace("Session.terminate", getName() + " - SnProcessDownloadedFiles:  setStatus(STATUS_TERMINATED).");
					setStatus(STATUS_TERMINATED);
					 
					//HTA-89 updating sftp files need to send a message to the UI that processing is complete.
					// stub
					SessionMessage testresp(SessionDev,SnUpgradeFirmwareStatus);
					testresp.setFormatType(InternalFormat);
					send(&testresp, Messenger::DEST_RTC);

				}
			}

			Debug::trace("ProcessDownloadedFiles", getName() + HobString(" - SnProcessDownloadedFiles end"));
			break;
		}

		case SnExecuteTask:
		{
            GenericHfcObject *gho( dynamic_cast<GenericHfcObject*>( msg->getHfcObject() ) );

			string taskName("");
            string hookAction("");
            string hookRequest("");

            if ( gho )
				taskName = gho->getData();
			else if ( tracingSession )
				Debug::trace("Session", getName() + " - handling SnExecuteTask Event, no data passed in");

			if ( tracingSession )
				Debug::trace("Session", getName() + string(" - handling SnExecuteTask Event, task name data: ") + taskName);

			if ( taskName.size() > 2 && taskName.substr(0,2) == Tags::taskNameTag )
				taskName = taskName.substr(2);
			else if ( taskName.size() > 2 && taskName.substr(0,2) == Tags::hookActionTag )
				hookAction = taskName.substr(2);
			else if ( taskName.size() > 2 && taskName.substr(0,2) == Tags::hookRequestTag )
				hookRequest = taskName.substr(2);
			else
				taskName = "";

			if ( tracingSession )
				Debug::trace("Session", getName() + " - handling SnExecuteTask Event, task name: " + taskName);

			// Both of the following tasks need to keep track of the IP address from which they came,
			// so the response that their corresponding files have been created gets sent back to the sources of the requests.
			if ( config->isCAT3AuditTrailLicensed() && ( taskName == "readCat3AuditTrail" || taskName == "readCat3WeigherConfig" ) )
			{
				Debug::trace("cat3", getName() + " - handling Cat3 SnExecuteTask Event, taskName: " + taskName);

				string ipAddress;
				CommMechanism *myUI = getMessenger().getMyUI();
				if (myUI)
				{
					Socket *sock = dynamic_cast<Socket *> (myUI);
					if ( sock )
						ipAddress = sock->getAddress();
					Debug::trace("cat3", ("ipAddress of socket is: ") + ipAddress);
				}

				// Pass this message along to the primary console session, who is equipped to handle it.
				// Also pass the IP Address of where the response is supposed to go (back to HTe).
				SessionMessage readCat3DataMsg( Weigher, ((taskName == "readCat3WeigherConfig") ? WgReadWeigherConfigForHTe : WgReadAllAuditTrailForHTe) );
				GenericHfcObject gho;
				gho.setData(ipAddress);
				readCat3DataMsg.setHfcObject(&gho);
				readCat3DataMsg.setFormatType(InternalFormat);
				send(&readCat3DataMsg, Messenger::DEST_RTC);

				break;
			}

#if defined( __linux__ )
            // Perform Custom Hook Action
            if ( !hookAction.empty() )
            {
                // invoke custom action
                performHookAction( hookAction.c_str() );

                break;
            }
            // Handle Hook Request
            else if ( !hookRequest.empty() )
            {
            	if ( !Configuration::getInstance()->getCustomerHooksEnabled() )
    				Debug::trace("Session", getName() + " - hooks disabled cannot perform request");
                else
            	{
                    static const char delim( ',' );

                    // split string
                    std::vector<std::string> request;
                    splitString( hookRequest, request, delim );

                    handleHookRequest( request );
                }

                break;
            }
#endif

			// All other messages get passed along to the primary console session for processing
			if ( !isPrimarySession() && taskName != "StartLvds1Test" )
			{
				// Any comm session that receives the command to execute some function/task, RTDO,
				// needs to pass it along to the Primary Console Session, which will handle the command.
				Debug::trace("Session", getName() + HobString(" - handling SnExecuteTask Event, but I'm not the primary console session, so forwaring it on... "));
				msg->setFormatType(InternalFormat);
		        send(msg, Messenger::DEST_RTC);
				break;
			}

			if (taskName == "BackupEntireDB")
			{
				SessionMessage backItUp(BackupRestore, BackupEntireDB);
				backItUp.setFormatType(BrowserFormat);
				handleBackupRestoreMessage(&backItUp);
			}
			else if (taskName == "CheckForDownload")
			{
				SessionMessage checkMsg(SessionDev, SnCheckForDownloadFiles);
				checkMsg.setFormatType(InternalFormat);
				send(&checkMsg, Messenger::DEST_RTC);
			}
			else if (taskName == "PerformAutoBackupDB")
			{
				SessionMessage checkMsg(SessionDev, SnPerformAutoBackupDB);
				checkMsg.setFormatType(InternalFormat);
				send(&checkMsg, Messenger::DEST_RTC);
			}
			else if (taskName == "ResizeLabels")
			{
				Debug::trace("Session", getName() + HobString(" - handling SnExecuteTask ResizeLabels Event, dispatching Printer, PrReqLabelSizing ... "));
				// I'm calling setPrinterStockMetrics to set the label length to 0, erasing what the current label size is,
				// so that when the labels size with the message directly after this, it will let the user know if it now sizes continuous.
				// If the current size is continuous, and is now sizes continuous, it wasn't displaying the message about continuous stock to the user.
				HfcPrinterStockMetrics *hpsm = new HfcPrinterStockMetrics();
				hpsm->setLabelWidth( (printerStockMetrics[FirstPrinter]) ? printerStockMetrics[FirstPrinter]->getLabelWidth() : WideLabel );
				setPrinterStockMetrics(hpsm);

				PrinterGeneric prReqLS(PrReqLabelSizing, this, FirstPrinter);
				dispatchEvent(&prReqLS);
			}
			else if (taskName.substr(0,14) == "PrePackUpgrade")
            {
                //HobString prePackData = gho->getData();
                HobString firmwarePath = taskName.substr(14);

                UpdateSoftwareManually upgradeMsg(firmwarePath, this);
                dispatchEvent(&upgradeMsg);
                /*
                HobTokenizerCSV tok( &prePackData, BrowserFormatSeparator, 2);

                //First tok.next skips TNPrePackUpgrade
                tok.next(&firmwarePath);
                if( tok.next(&firmwarePath) != -1 )
                {
                    SessionMessage upgradeMsg(VersionManagerDev, UpgradeManual, firmwarePath);
                    upgradeMsg.setFormatType(InternalFormat);
                    send(&upgradeMsg, Messenger::DEST_RTC);
                }*/
            }
            else if (taskName.substr(0,18) == "PrePackUpgradeAWS3")
            {
                //reset the wrapper node boards to force into bootstrap mode
                GpioDeviceReset  wrapperReset;

                wrapperReset.setDeviceTypeAWS3Wrapper();
                wrapperReset.resetDevice();

                #if defined ( __linux__ )
                system("reboot");
                #endif

            }
			else if (taskName == "UpgradeSFTPBadAuthentication")
            {
				Debug::trace("Session", getName() + " - handling SnExecuteTask Event, caught UPGRADE SFTP BAD AUTHEN: " + taskName);
				SessionMessage upgradeFirmwareStatusMsg(SessionDev,SnUpgradeSFTPBadAuthentication);
				upgradeFirmwareStatusMsg.setFormatType(BrowserFormat);
				GenericHfcObject gho;
				gho.setData("BadAuthentication");
				upgradeFirmwareStatusMsg.setHfcObject(&gho);
				send(&upgradeFirmwareStatusMsg, Messenger::DEST_UI);
            }
			else if ( taskName == "andEverything" && config->isCAT3AuditTrailLicensed() )
			{
				if ( config->toIntCfgI("Life") == 42 && Debug::isTracing("theUniverse") )
				{
					Debug::trace("theUniverse", "Cat3 Audit Trail Clear in progress ...");

					HobList data;
					data.setQuotingElements(true);
					data.push_back("Cat3 Audit Trail Clear in progress ...");
					responseMessageManager->addMessage(ErrUnknown, 1, HfcResponse::RESPONSESTYLE_NOINPUT, &data);

					registerEvent(Weigher, WgCat3LogEraseComplete);

					WeigherGeneric cat3AuditClear( WgCat3ReqEraseLog, this );
					dispatchEvent( &cat3AuditClear );

					MessageLogger::logMessage( MessageLogger::InfoLog, "Clearing Cat3 Audit Trail" );
				    PAL_absclass::PAL_sleep(2000);
				}
			}
            else if ( taskName == "StartLvds1Test" ) {   
                Debug::trace("Session.BoardSite", "taskName: " + taskName);             
                BoardSiteTest tstMsg( this, _ARK_LVDS1, (unsigned char)0, (unsigned char)0, (unsigned char)0,(unsigned char)0,
                                                (unsigned char)0,(unsigned char)0,(unsigned char)0 );
                send( &tstMsg, Messenger::DEST_RTC );

            }
			else if (tracingSession)
				Debug::trace("Session", getName() + " - handling SnExecuteTask Event, don't know how to handle: " + taskName);

			break;
		}

		// FT-1197
        case SnReadNetworkInfo:
        {
			if (tracingSession)
				Debug::trace("Session", getName() + " - Received SnReadNetworkInfo message.");

			string networkInfo;
			if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) )
			{
				const string command( PythonScripter::buildPythonCall("getNetworkInfo.py") );
				string networkInfoOutput;
				PAL_absclass::PAL_executeWithResults(command, networkInfoOutput);
				if (tracingSession)
					Debug::trace("Session", getName() + " - process SnReadNetworkInfo - Data received from getNetworkInfo.py: " + networkInfoOutput);

				if ( !networkInfoOutput.empty() )
					networkInfo = networkInfoOutput;
			}

			GenericHfcObject result(networkInfo);
			SessionMessage msg(SessionDev, SnWriteNetworkInfo);
			msg.setFormatType(BrowserFormat);
			msg.setHfcObject(&result);
			send(&msg, Messenger::DEST_UI);
            break;
        }

		case SnReadLicensedPackages:
		{
			HobString packages = Configuration::getInstance()->getLicensedPackages( true );
			packages.quoteForCSV();
			GenericHfcObject gho(packages);
			SessionMessage msg(SessionDev, SnWriteLicensedPackages);
			msg.setFormatType(BrowserFormat);
			msg.setHfcObject(&gho);
			send(&msg, Messenger::DEST_UI);

			break;
		}

		// FT-40
		case SnManufacturingTestSummaryReport:
		{
			if (tracingSession)
				Debug::trace("Session", "snManufacturingTestSummaryReport - begin");

			Product *myProd = new Product();
			if (myProd)
			{
				myProd->setDBAccess(this);
				myProd->setProductNum(999999);
				myProd->setProductType(Product::PRODUCTTYPE_FIXEDWEIGHT);
				myProd->setDescription("Manufacturing Test Summary Report");
				myProd->setLabelType(FirstPrinter, LabelType::findRecord(Configuration::getInstance()->toIntCfgI(CfgInternal::PRODUCTSUPPORTTEXTLABELNUM), myProd->getDeptNum(), this));

				ProductText *dummyExpText = new ProductText(psttExpandedText);
				if (dummyExpText)
				{
					GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
					if ( !gho || gho->getData().empty() )
					{
						if (tracingSession)
							Debug::trace("Session", "snManufacturingTestSummaryReport - no report data received");

						dummyExpText->setText("NO REPORT DATA");
					}
					else
					{
						HobString rptData = gho->getData();
						rptData.unQuoteForCSV();
						dummyExpText->setText(rptData);
					}
					dummyExpText->setTextNum(myProd->getProductNum());
					myProd->setProductText(psttExpandedText, dummyExpText);
				}
				else
				{
					if (tracingSession)
						Debug::trace("Session", "snManufacturingTestSummaryReport - could not create dummy expanded text object");
					myProd->setDescription("Manufacturing Test Summary Report - ERROR");
				}

				productManager->setCurrentProduct(myProd);

				if (tracingSession)
					Debug::trace("Session", "snManufacturingTestSummaryReport - creating message to tell productManager to print it");

				SessionMessage printProductSupportLabel(ProductDev, ProductPrintProductSupportText);
				ProductTextCfg ptc(psttExpandedText);
				ptc.setPrinterNum(PrinterChoice_PrimaryPrinter);
				ptc.setLabelNum((HobRecordNum)Configuration::getInstance()->toIntCfgI(CfgInternal::PRODUCTSUPPORTTEXTLABELNUM));
				printProductSupportLabel.setHfcObject(&ptc);
				productManager->handleMessage(&printProductSupportLabel);
			}

			if (tracingSession)
				Debug::trace("Session", "snManufacturingTestSummaryReport - end");
			break;
		}

		// FT-1458
		case SnNowServingQueueManagement:
			handleNowServingQueueManagementMessage( msg );
			break;

		// HT-326
		case SnRdpActive:
		{
			SessionMessage rdpActiveMsg(SessionDev, SnRdpActive);
			rdpActiveMsg.setFormatType(BrowserFormat);
			send(&rdpActiveMsg, Messenger::DEST_UI);
			break;
		}

		// HT-326
		case SnRdpInactive:
		{
			SessionMessage rdpInActiveMsg(SessionDev, SnRdpInactive);
			rdpInActiveMsg.setFormatType(BrowserFormat);
			send(&rdpInActiveMsg, Messenger::DEST_UI);
			break;
		}

		case SnRequestSaveConfiguration:
			config->save(this);
			break;

		case SnInvalidModeDetected:
		{
			Debug::trace("Session", getName() + HobString(" - handling SnInvalidModeDetected"));
			string indicatedMode("???");

			GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
			if (gho && !gho->getData().empty())
				indicatedMode = gho->getData();

			MessageLogger::logMessage(MessageLogger::ErrorsLog, string("Received invalid mode message - indicated mode was ") + indicatedMode);

			Debug::trace("Session", getName() + HobString(" SnInvalidModeDetected - switching to Prepack mode"));
			SessionMessage ppmode(OperationMode, OpModeAutoManualApply);
			ppmode.setFormatType(BrowserFormat);
			handleModeMessage(&ppmode);

			PAL_absclass::PAL_sleep(1000);

			SessionMessage readppmode(SessionDev, SnReadWeighModeTransactionType);
			handleSessionMessage(&readppmode);

			PAL_absclass::PAL_sleep(500);

			Debug::trace("Session", getName() + HobString(" SnInvalidModeDetected - switching to Manual mode"));
			SessionMessage mwmode(OperationMode, OpModeManualWeigh);
			mwmode.setFormatType(BrowserFormat);
			handleModeMessage(&mwmode);

			PAL_absclass::PAL_sleep(1000);

			SessionMessage readmwmode(SessionDev, SnReadWeighModeTransactionType);
			handleSessionMessage(&readmwmode);

			PAL_absclass::PAL_sleep(500);
			break;
		}

		default:
            //todo - write warning log message indicating received bad message.
            break;
    }
}


int Session::handleRemotePrintInitialAvailability()
{
	Debug::trace("ProductManager.remotePrint", getName() + " - START handleRemotePrintInitialAvailability()");

	//***********************************************************************************************************
	//***********************************************************************************************************
	//*****                                                                                                 *****
	//*****  Most of the next block of code is based on handleProductPrintKey1()                            *****
	//*****                                                                                                 *****
	//***********************************************************************************************************
	//***********************************************************************************************************


	/* Can't print the label if the current product is not accepting weights;
	* i.e. when the weigher was not zeroed when the user entered the plu number.
	* No current need to display an error message.  There will already be an error message displayed
	* indicating the user needs to remove the weight or press the rezero button.
	*/

	// If I am the remote print session (as when calling this for the SessionDev|SnBackendIsScaleAvailable message), need to get the primary session to help determine return statusi.
	// If I am the primary session, then use me.
	Session *s = 0;
	if (isPrimarySession())
		s = this;
	else
		s = SessionManager::getInstance()->getPrimaryConsoleSession();
	if (!s)
		return ProductErrPrintCantFindMainSession;

	Debug::trace("ProductManager.remotePrint", getName() + " - handleRemotePrintInitialAvailability, my Session* " + util_string::ptrtostring(this) + "  using s Session* " + util_string::ptrtostring(s));

	ProductManager *pm = 0;
	pm = s->getProductManager();
	if (!pm)
		return ProductErrPrintCantFindMainSession;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return ProductErrPrintCantFindConfiguration;

	if (!config->isRemotePrintingLicensed())
		return ProductErrPrintScaleNotLicensed;

	// Can't print a remote label if we are currently processing/printing a remote label
	if (pm->isPrintingRemoteLabel())
		return ProductErrPrintPrintingRemoteLabel;

	// Can't print a remote label if there is a PLU loaded
	if (pm->getCurrentProduct())
		return ProductErrPrintPluLoadedOnScale;

	//if (!pm->currentProductCanAcceptWeight)
	//	return ProductErrPrintWeigherIssue;		// weigher unplugged, nozero tare, etc

	// You may not use a wrapper, or hand-wrap station to print remote labels, the EPP can't stream labels or use the print key in Automatic mode.
	if (config->isPrepackScale())
	{
		return ProductErrPrepackPrintWrongOpMode;
	}

	OpModeEvent currMode = s->getCurrentMode();

	if (currMode < OpModeIdle || currMode >= OpModeMax)
		return ProductErrPrintUnknownModeError;		        // bail if we are in Unknown Mode
	else if (OpModeSupervisor == currMode)
		return ProductErrCannotPrintInSupervisorMode;		// bail if we are in Supervisor Mode
	else if (OpModeService == currMode || OpModeServiceLogin == currMode)
		return ProductErrCannotPrintInServiceMode;			// bail if we are in Service Mode
	else if (OpModeSelfService == currMode)
		return ProductErrCannotPrintInSelfServiceMode;		// bail if we are in Self Service Mode
	else if (OpModeBuildAMeal == currMode)
		return ProductErrCannotPrintBuildAMeal;				// bail if we are build a meal 
	else if (OpModeAccumulateItems == currMode && pm->hasAccumulateItems())   // check of we are in accumulate items mode and have items in the list
		return ProductErrCannotPrintAccumulateMode;
	else if (OpModeProductionEntry == currMode && pm->hasProductionEntryItems())   // check if we are in production entry mode and have items in the list
		return ProductErrCannotPrintInProdEntryMode;

	// Check for various printer errors
	// If there are any outstanding printer statuses, for any of the printers (and we are internally configured to
	// not keep displaying them, which we started doing with the HTi),
	// then redisplay those statuses, and leave; since we can't print with any printer being in some "error" state.

	int status_code = checkForRemotePrinterStatus();
	Debug::trace("ProductManager.remotePrint", string("handleRemotePrintInitialAvailability() - return from checkForRemotePrinterStatus: ") + Integer::toString(status_code));
	if (status_code > 0)
		return status_code;

	if (!config->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
	{
		//	GCDR-158:	Don't allow the operator to print if this a Service counter scale with NO Customer display
		//				and the scale is NOT set to operate as a Prepack.
		if (config->isServiceCounterScale() && !SessionManager::getInstance()->isCustomerDisplayAttached() && !config->toBoolCfg(fieldOffsetCfgGCDPrintWithoutCustomerDisplay))
			return ProductErrCannotPrintWhenCustomerDisplayNotConnected;

		// We now know that it is a service scale and it has a customer display attached and it is not set to operate as a prepack. Now
		// we need to make sure if the customer display is a GCD that it is operational. GCDR-183
		if (SessionManager::getInstance()->isGCDAttached() && !SessionManager::getInstance()->isGCDOperational())
			return ProductErrCannotPrintWhenCustomerDisplayNotConnected;
	}

	Debug::trace("ProductManager.remotePrint", "FINISHED handleRemotePrintInitialAvailability() - returning RemotePrintScaleIsAvailable");
	return RemotePrintScaleIsAvailable;
}

void Session::messageBeforeAccessSystemDesktop()
{
    Debug::trace("Session.accessSystemDesktop", HobString("beforeAccessSystemDesktop"));
#ifndef NOFBWF
	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::FBWFACTIVATED))
	{
		ULONG current = 0, next = 0;
		ULONG result = FbwfIsFilterEnabled(&current, &next);

		if (result == NO_ERROR && isPrimarySession() && responseMessageManager)
			responseMessageManager->addMessage(ErrSessionDev, SessionDevNoUpdateMessageBeforeDesktop, HfcResponse::RESPONSESTYLE_OK);
		else
			accessSystemDesktop();
	}
    else
        accessSystemDesktop();
#else
    accessSystemDesktop();
#endif
}

void Session::accessSystemDesktop()
{
    Debug::trace("Session.accessSystemDesktop", HobString("accessSystemDesktop"));
#if defined ( __linux__ )
    int result = 0;

	result = system("/usr/bin/win_cmd.sh desktop");

	if (result == -1)
	{
		Debug::trace("Session", "WMCTRL command to show desktop failed!");
	}
#else
    if (isPrimarySession())
    {
        HWND taskbar = FindWindow(_T("Shell_traywnd"), 0);
        if (taskbar)
        {
            ShowWindow(taskbar, SW_SHOW);
        }
        // Only allow users on the primary session access the desktop because they will be in full screen access mode.

        // other things I did
        // #include "shellapi.h"
        //ShellExecute (NULL,_T("open"),_T("notepad.exe"),_T(""),_T(""),SW_SHOW);                                                       // worked
        //ShellExecute (NULL,_T("open"),_T("c:\hobarttest\gotoDesktop.scf"),_T(""),_T(""),SW_SHOW);                     // did not work
        //ShellExecute (NULL,_T("open"),_T("gotoDesktop.scf"),_T(""),_T(""),SW_SHOW);                                           // did not work

        keybd_event( VK_LWIN, MapVirtualKey(VK_LWIN, 0), 0, 0);
        keybd_event( 'D', MapVirtualKey('D', 0), 0, 0);
        keybd_event( 'D', MapVirtualKey('D', 0), KEYEVENTF_KEYUP, 0);
        keybd_event( VK_LWIN, MapVirtualKey(VK_LWIN, 0), KEYEVENTF_KEYUP, 0);
    }
#endif
}


bool Session::validateVirtualKeyboard(HWND UNUSED(hwnd), Util::ValidateWindowParameters* UNUSED(parameters))
{
#if defined ( __linux__ )
    Debug::trace("Session.virtualKeyboard", HobString("validateVirtualKeyboard"));

    return PAL_absclass::PAL_findWindow("xvkbd");
#else
    if ( IsWindow(hwnd) )
    {
        if ( GetClassNameA(hwnd, parameters->windowClassName, sizeof(parameters->windowClassName)) )
        {
            if ( strcmp(parameters->windowClassName, parameters->myClassName) == 0 )
            {
                if ( GetWindowInfo(hwnd, &parameters->windowInfo) )
                    return (parameters->windowInfo.dwStyle & WS_CAPTION) == WS_CAPTION;
                else
                    Debug::trace("Session.virtualKeyboard", "GetWindowInfo failed (error = " +
                            util_string::uinttostring(GetLastError()) + ")");
            }
        }
        else
            Debug::trace("Session.virtualKeyboard", "GetClassName failed (error = " +
                    util_string::uinttostring(GetLastError()) + ")");
    }
    return false;
#endif      // __linux__
}

void Session::popupVirtualKeyboard(int down)
{
    Debug::trace("Session.virtualKeyboard", HobString("popupVirtualKeyboard"));
	
	bool foundKeyboard = false;

    while(isTryingToDismissKeyboard)
    {
        Debug::trace("Session.virtualKeyboard", HobString("Waiting for keyboard to dismiss before we can launch new one:") );
        PAL_absclass::PAL_sleep(1000);
    }
#if defined ( __linux__ )
    if ( PAL_absclass::PAL_findWindow("xvkbd") )
    {
		//coming from intranet browser window, let's close since we need to re-associate keyboard with intranet window		
		PAL_absclass::PAL_closeWindow("xvkbd");
	}
    
	if (!foundKeyboard)
    {
        std::string lang;
        std::string geometry;
        std::string cmd("/usr/bin/xvkbd.sh ");

        isTryingToCreateKeyboard = true;

        if (down == 0)
            geometry = " 0";
        else if (down == 1)
            geometry = " 1";
		else if (down == 2)
			geometry = " 2";
		else if (down == 3)
			geometry = " 3";

        Configuration * config = Configuration::getInstance();

        if (currentLanguageSelection == SecondaryLanguage)
        {
            Platform::getInstance().setupLocale(config->getSecondaryLanguageCode(), config->getSecondaryCountryCode());
            lang = Configuration::getInstance()->getSecondaryLanguageCode();
        }
        else
        {
            Platform::getInstance().setupLocale(config->getPrimaryLanguageCode(), config->getPrimaryCountryCode());
            lang = Configuration::getInstance()->getPrimaryLanguageCode();
        }
        cmd += lang;

        cmd += geometry;

        int result = system(cmd.c_str());
		
		if (result == -1)
		{
			Debug::trace("Session", "Failed to launch virtual keyboard!");
		}
		

        while ( PAL_absclass::PAL_findWindow("xvkbd") == false )
            PAL_absclass::PAL_sleep(1000);

        isTryingToCreateKeyboard = false;

		PAL_absclass::PAL_showWindow("xvkbd");
    }
#else
    HWND hwnd = Util::findWindowHandle(&virtualKeyboardParams_);
    if ( hwnd != NULL )
    {

        // we found the keyboard, now make sure we have the right language bar requirements when we popit
        setLanguageBarKeyboardLanguage();

        WINDOWPLACEMENT placement;

        GetWindowPlacement(hwnd, &placement);

        // All methods cause the Keyboard window, after manually sending
        // it down, to come up without focus (only the first time)

        placement.flags = WPF_ASYNCWINDOWPLACEMENT;
        placement.showCmd = SW_RESTORE;

        int move = 0;

        if(down == 0 || down == 2)
            move = placement.rcNormalPosition.top - Configuration::getInstance()->toIntCfgI(CfgInternal::DOWNKEYBOARDYCOORDINATE);     // place at the bottom of the screen
        else if(down == 1 || down == 3)
            move = placement.rcNormalPosition.top - Configuration::getInstance()->toIntCfgI(CfgInternal::UPKEYBOARDYCOORDINATE);       // place at the top of the screen

        placement.rcNormalPosition.left = 1;
        placement.rcNormalPosition.top -= move;
        placement.rcNormalPosition.right = 799;
        placement.rcNormalPosition.bottom -= move;

        SetWindowPlacement( hwnd, &placement );
        //                      ShowWindow( hwnd, SW_RESTORE );
        //                      ShowWindow( hwnd, SW_SHOWDEFAULT );
        //                      ShowWindow( hwnd, SW_SHOWNOACTIVATE );
    }
    else
    {
        // Since the keyboard handle can't be found, and probably didn't get created, create it.
        isTryingToCreateKeyboard = true;

        Configuration * config = Configuration::getInstance();

        std::string localeStr("");
        std::string pathstr("");

        if (currentLanguageSelection == SecondaryLanguage)
        {
            Platform::getInstance().getLocaleString(localeStr, Configuration::getInstance()->getSecondaryLanguageCode(),
                    Configuration::getInstance()->getSecondaryCountryCode());

            pathstr.assign("c:/program files/Click-N-Type/");
            pathstr.append(Configuration::getInstance()->getSecondaryLanguageCode()).append(1, '_');
            pathstr.append(Configuration::getInstance()->getSecondaryCountryCode());
            pathstr.append("/Click-N-Type.exe");
        }
        else
        {
            Platform::getInstance().getLocaleString(localeStr, Configuration::getInstance()->getPrimaryLanguageCode(),
                    Configuration::getInstance()->getPrimaryCountryCode());

            pathstr.assign("c:/program files/Click-N-Type/");
            pathstr.append(Configuration::getInstance()->getPrimaryLanguageCode()).append(1, '_');
            pathstr.append(Configuration::getInstance()->getPrimaryCountryCode());
            pathstr.append("/Click-N-Type.exe");
        }

        locale currentLocale = locale(localeStr.c_str());
        locale::global(currentLocale);

        std::string path ("\"" + pathstr + "\"");

        setLanguageBarKeyboardLanguage();

        // if the keyboard does not exist, it opens minimized in taskbar.  Is this a keyboard property?
        //HINSTANCE res2 = ShellExecuteA (NULL,"open",path.c_str(),"","",SW_SHOWNORMAL);
        HINSTANCE res2 = ShellExecuteA (NULL,"open",path.c_str(),"","",SW_SHOWMINNOACTIVE);

        isTryingToCreateKeyboard = false;
    }
#endif
}

void Session::setLanguageBarKeyboardLanguage()
{
#if defined ( __linux__ )
    // we do the language setup for the VK at VK startup
    Debug::trace("Session.virtualKeyboard", HobString("setLanguageBarKeyboardLanguage"));
#else
    std::string languageStr("");
    LanguageSelection cls = getCurrentLanguageSelection();

    // find out what the current language string is, because we need to use the windows language bar to switch to it
    if (cls == SecondaryLanguage)
        languageStr.append(Configuration::getInstance()->getSecondaryLanguageCode());
    else
        languageStr.append(Configuration::getInstance()->getPrimaryLanguageCode());

    // we know the language of the keyboard we want to use. Now do a Left+Alt+numberkey emulation, to essentially
    // change to that language's language bar setting.  The language bar is set up in windows, in the image, before reseal.

    if(languageStr == "en")
    {
        keybd_event(  VK_LMENU, MapVirtualKey(VK_LMENU,  0), 0, 0);
        keybd_event( VK_LSHIFT, MapVirtualKey(VK_LSHIFT, 0), 0, 0);
        keybd_event( '1',       MapVirtualKey('1',     0), 0, 0);
        keybd_event( '1', MapVirtualKey('1', 0), KEYEVENTF_KEYUP, 0);
        keybd_event( VK_LSHIFT, MapVirtualKey(VK_LSHIFT, 0), KEYEVENTF_KEYUP, 0);
        keybd_event( VK_LMENU, MapVirtualKey(VK_LMENU, 0), KEYEVENTF_KEYUP, 0);
    }
    else if(languageStr == "fr")
    {
        keybd_event(  VK_LMENU, MapVirtualKey(VK_LMENU,  0), 0, 0);
        keybd_event( VK_LSHIFT, MapVirtualKey(VK_LSHIFT, 0), 0, 0);
        keybd_event( '2',       MapVirtualKey('2',     0), 0, 0);
        keybd_event( '2', MapVirtualKey('2', 0), KEYEVENTF_KEYUP, 0);
        keybd_event( VK_LSHIFT, MapVirtualKey(VK_LSHIFT, 0), KEYEVENTF_KEYUP, 0);
        keybd_event( VK_LMENU, MapVirtualKey(VK_LMENU, 0), KEYEVENTF_KEYUP, 0);
    }
    else if(languageStr == "es")
    {
        keybd_event(  VK_LMENU, MapVirtualKey(VK_LMENU,  0), 0, 0);
        keybd_event( VK_LSHIFT, MapVirtualKey(VK_LSHIFT, 0), 0, 0);
        keybd_event( '3',       MapVirtualKey('3',     0), 0, 0);
        keybd_event( '3', MapVirtualKey('3', 0), KEYEVENTF_KEYUP, 0);
        keybd_event( VK_LSHIFT, MapVirtualKey(VK_LSHIFT, 0), KEYEVENTF_KEYUP, 0);
        keybd_event( VK_LMENU, MapVirtualKey(VK_LMENU, 0), KEYEVENTF_KEYUP, 0);
    }
#endif      // __linux__
}

void Session::dismissVirtualKeyboard()
{
    while(isTryingToCreateKeyboard)
    {
        Debug::trace("Session.virtualKeyboard", HobString("Waiting for keyboard to get created before we can minimize this one:") );
        PAL_absclass::PAL_sleep(2000);
    }

    isTryingToDismissKeyboard = true;

#if defined ( __linux__ )
    Debug::trace("Session.virtualKeyboard", HobString("dismissVirtualKeyboard"));
    PAL_absclass::PAL_hideWindow("xvkbd");
#else
    HWND hwnd = Util::findWindowHandle(&virtualKeyboardParams_);
    if ( hwnd != NULL )
    {
        if ( !IsIconic(hwnd) )
        {
            ShowWindow( hwnd, SW_MINIMIZE );
        }
    }
#endif
    isTryingToDismissKeyboard = false;
}


bool Session::validatePrimaryScaleTitle(HWND UNUSED(hwnd), Util::ValidateWindowParameters* UNUSED(parameters))
{
#if defined ( _WIN32 )
    if ( IsWindow(hwnd) )
    {
        if ( GetClassNameA(hwnd, parameters->windowClassName, sizeof(parameters->windowClassName)) )
        {
            if ( strcmp(parameters->windowClassName, parameters->myClassName) == 0 )
            {
                // at this point, we know we have an IEFrame class, now find if that class has the right title.
                // if it does have the right title, return true and be done.
                TCHAR szBufTitle[256];
                int cTxtLen = GetWindowTextLength(hwnd);
                GetWindowText(hwnd, szBufTitle, cTxtLen + 1);
                if ( wcsstr(szBufTitle, parameters->myScaleTitle) != NULL )
                    return true;
            }
        }
    }
    return false;
#else
    return true;
#endif
}

bool Session::validateBrowserScaleTitle(HWND UNUSED(hwnd), Util::ValidateWindowParameters* UNUSED(parameters))
{
#if defined ( _WIN32 )
    if ( IsWindow(hwnd) )
    {
        if ( GetClassNameA(hwnd, parameters->windowClassName, sizeof(parameters->windowClassName)) )
        {
            if ( strcmp(parameters->windowClassName, parameters->myClassName) == 0 )
            {
                // at this point, we know we have an IEFrame class, now find if that class has the right title.
                // if it does have the right title, return true and be done.
                TCHAR szBufTitle[256];
                int cTxtLen = GetWindowTextLength(hwnd);
                GetWindowText(hwnd, szBufTitle, cTxtLen + 1);

                if ( wcsstr(szBufTitle, parameters->myBrowserTitle) != NULL )
                    return true;
            }
        }
    }
    return false;
#else
    return true;
#endif
}

void Session::popupScaleBrowserWindow()
{
#if defined ( __linux__ )
    Debug::trace("Session.popupScaleBrowserWindow", HobString("popupScaleBrowserWindow"));
    if (PAL_absclass::PAL_findWindow("Intranet") == true)
    {
        startingIntranetBrowser = false;
        PAL_absclass::PAL_hideWindow("Intranet");
    }
    PAL_absclass::PAL_showWindow("HLX Scale");
#else
    // need to find the primary console window, obtain it's handle, and bring it to the front and make it active.
    // also need to find the 2nd internet\intranet browser and minimize it to the taskbar

    BOOL retVal = 0;

    HWND pWndBrowser = Util::findBrowserWindowHandle(&browserScaleParams_);
    if ( pWndBrowser != NULL )
    {
        startingIntranetBrowser = false;
        retVal = ShowWindow( pWndBrowser, SW_MINIMIZE );                                // Minimize the Intranet Window
    }

    HWND pWndScale = Util::findScaleWindowHandle(&primaryScaleParams_);
    if ( pWndScale != NULL )
        retVal = SetForegroundWindow(pWndScale);                                        // I was able to find the Scale window, so now bring it to the front
#endif
}

#if defined( __linux__ )
void Session::popupIntranetBrowserWindow(int whatFrame, HobString pageAddress)
#else
void Session::popupIntranetBrowserWindow(int whatFrame, HobString pageAddress)
#endif
{
	if (pageAddress.find(".wmv") != std::string::npos || pageAddress.find(".mp4") != std::string::npos)
	{
	    Debug::trace("Session.popupIntranetBrowserWindow", "is a wmv/mp4 video");
	    PAL_absclass::PAL_showVideoPlayer(pageAddress.c_str());
		return;
	}

	if (responseMessageManager && NetworkConnectivityManager::getInstance() && !NetworkConnectivityManager::getInstance()->checkNetworkAvailability())
	{
		responseMessageManager->addMessage(ErrNetworkConnectivityDev, NetworkConnectivityManagerErrLostComm, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

#if defined ( __linux__ )
    Debug::trace("Session.popupIntranetBrowserWindow", pageAddress);
    if (PAL_absclass::PAL_findWindow("Intranet") == true)
    {
        startingIntranetBrowser = false;
        PAL_absclass::PAL_showWindow("Intranet");
        PAL_absclass::PAL_hideWindow("HLX Scale");

        // send message here to set the Intranet Frame as the active frame (because the flashkey internet site key may have set it to its own frame)
        if(whatFrame == 0)
        {
            SessionMessage frameMsg(SessionDev, SnSwitchToIntranetFrame);               // 100,83, set Frame 0, display the defaultWebPage
            frameMsg.setFormatType(InternalFormat);
            send(&frameMsg, Messenger::DEST_RTC);
        }
        else if(whatFrame == 1)
        {
            // 100,82, clicked on flashkey, pass the page data and show frame 1
            GenericHfcObject msgData(pageAddress);
            SessionMessage frameMsg(SessionDev, SnSetFlashkeyInternetWebPage);          // 100,83, set Frame 0, display the defaultWebPage
            frameMsg.setFormatType(InternalFormat);
            frameMsg.setHfcObject(&msgData);
            send(&frameMsg, Messenger::DEST_RTC);
            savedIntranetAddress = "";
        }
    }
    else
    {
        // The Intranet Window does not exist at this time.  Therefore create it by calling the file
        if (!startingIntranetBrowser)
        {
            if (responseMessageManager)
                responseMessageManager->addMessage(ErrSessionDev, SessionDevLoadingIntranetIE, HfcResponse::RESPONSESTYLE_NOINPUT);

            PAL_absclass::PAL_launchBrowswer();
            /*
            std::string cmd("/home/hlx/prism/prism --display :0 -override /home/hlx/.webapps/hlx.intranet@prism.app/override.ini -webapp /home/hlx/.webapps/hlx.intranet@prism.app ");

            system(cmd.c_str());
            */
            startingIntranetBrowser = true;
            ieWindowToFront();
            PAL_absclass::PAL_hideWindow("HLX Scale");
        }

        if(whatFrame == 1)
            savedIntranetAddress = pageAddress;
    }
#else
    // need to find the Intranet console window, obtain it's handle, and bring it to the front and make it active.
    // If the window does not exist, then run the launchvb batch file and create it

    BOOL retVal = 0;
    HWND pWndBrowser = NULL;

    pWndBrowser = Util::findBrowserWindowHandle(&browserScaleParams_);

    if ( pWndBrowser != NULL )
    {
        startingIntranetBrowser = false;

        retVal = ShowWindow( pWndBrowser, SW_MAXIMIZE );                        // Maximize the window and then bring it to the front.  May not need the next line. Test it.
        retVal = SetForegroundWindow(pWndBrowser);                                      // I was able to find the Intranet window, so now bring it to the front

        // send message here to set the Intranet Frame as the active frame (because the flashkey internet site key may have set it to its own frame)
        if(whatFrame == 0)
        {
            SessionMessage frameMsg(SessionDev, SnSwitchToIntranetFrame);               // 100,83, set Frame 0, display the defaultWebPage
            frameMsg.setFormatType(InternalFormat);
            send(&frameMsg, Messenger::DEST_RTC);
        }
        else if(whatFrame == 1)
        {
            // 100,82, clicked on flashkey, pass the page data and show frame 1
            GenericHfcObject msgData(pageAddress);
            SessionMessage frameMsg(SessionDev, SnSetFlashkeyInternetWebPage);          // 100,83, set Frame 0, display the defaultWebPage
            frameMsg.setFormatType(InternalFormat);
            frameMsg.setHfcObject(&msgData);
            send(&frameMsg, Messenger::DEST_RTC);
            savedIntranetAddress = "";
        }
    }
    else
    {
        // The Intranet Window does not exist at this time.  Therefore create it by calling the vbs file
        // when creating the Intranet Window here, could minimize the scale application here, but not doing it right now.
        if (!startingIntranetBrowser)
        {
            if (responseMessageManager)
                responseMessageManager->addMessage(ErrSessionDev, SessionDevLoadingIntranetIE, HfcResponse::RESPONSESTYLE_NOINPUT);

            ShellExecute(NULL, _T("open"), _T("C:\\Program Files\\Hobart\\Access Scale\\scripts\\launchIntranet.vbs"), NULL, NULL, SW_MINIMIZE);

            startingIntranetBrowser = true;

            ieWindowToFront();
        }

        if(whatFrame == 1)
            savedIntranetAddress = pageAddress;
    }
#endif
}

void Session::ieWindowToFront()
{
#if defined ( __linux__ )
    PAL_absclass::PAL_showWindow("Intranet");
#else
    // need to find the Intranet console window, obtain
    // it's handle, and bring it to the front and make it active.

    BOOL retVal = 0;
    HWND pWndBrowser = Util::findBrowserWindowHandle(&browserScaleParams_);

    if (pWndBrowser != NULL )
    {
        startingIntranetBrowser = false;

        retVal = ShowWindow( pWndBrowser, SW_MAXIMIZE );                        // Maximize the window and then bring it to the front.  May not need the next line. Test it.
        retVal = SetForegroundWindow(pWndBrowser);                                      // I was able to find the Intranet window, so now bring it to the front
    }
#endif
}

void Session::handleReadAll(SessionMessage *msg)
{
    if (!msg)
        return;

    Debug::trace("Session", getName() + HobString(" - handleReadAll via BackupRestoreManager  device:") + HobString(Integer::toString(msg->getDeviceType())) +
            HobString("  event:") + HobString(Integer::toString(msg->getEventType())));

    if (!backupRestoreManager)
        backupRestoreManager = new BackupRestoreManager(this);
    PAL_absclass::PAL_sleep(10);

    if (!backupRestoreManager)
        return;

    currentModeOp = Session::OP_READALL;
    BackupRestoreInitiateReadAllMsg iraMsg(this, msg->getDeviceType(), msg->getEventType(), getMessenger().getMyUI());
    dispatchEvent(&iraMsg, backupRestoreManager);

    Debug::trace("Session", getName() + HobString(" - handleReadAll via BackupRestoreManager initiated"));
}


void Session::handleBackupRestoreMessage(SessionMessage *msg)
{
    if (!msg)
        return;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

    switch (msg->getEventType())
    {
        case Terminate:
        {
			/* The session will only receive the Terminate event from the BackupRestoreManager that it created,
			 * because the BackupRestoreManager will dispatch the Terminate event directly to the Session with which it is associated.
			 * The Session will NOT receive a Terminate event from any other BackupRestoreManager that has been instantiated (from some other session).
			 */
			Debug::trace("Session", getName() + " - handling Terminate Event for Backup/Restore");

			if ( backupRestoreManager )
			{
				const BackupRestoreSelections selections = backupRestoreManager->getSelections();
				if ( currentModeOp == OP_PROCESSINGDOWNLOADEDFILES || restoreVersionIndependentData )
				{
					HobString processedFilename = selections.getFileName();

					bool tracing = Debug::isTracing("ProcessDownloadedFiles");
					if (tracing)
						Debug::trace("ProcessDownloadedFiles", getName() + " - handleBackupRestoreMessage():  Finished processing file: " + processedFilename);

					string loggerHeader = (restoreVersionIndependentData) ? "Restored database after upgrade: " : "Processed downloaded file: ";
					MessageLogger::logMessage(MessageLogger::InfoLog, loggerHeader + processedFilename);

					File downloadFile(selections.getFileName());
					if (downloadFile.exists())
					{
						bool fileMoved = false;

						// If we want to make a backup of files we've processed, do that now, before we delete it.
						bool shouldKeepFile = config->toBoolCfgI(CfgInternal::KEEPCOPYOFPROCESSEDDATA) || (restoreVersionIndependentData && config->toBoolCfgI(CfgInternal::MAKECOPYOFVERSIONINDEPENDENTDATA) );
						if (shouldKeepFile)
						{
							std::string processedDirStr = config->toStringCfgI(CfgInternal::PROCESSEDDIR);
							if (processedDirStr.size())
							{
								util_filesystem::Directory processedDir("");
								processedDir.appendPath(processedDirStr);
								if (!processedDir.exists())
									processedDir.create();
								if (!processedDir.exists())
								{
									if (tracing)
										Debug::trace("ProcessDownloadedFiles", "handleBackupRestoreMessage():  Could not create the processed directory for keeping files we've processed!");
								}
								else
								{
									string processedFilename = "";
									bool shouldMakeFilenameUnique = config->toBoolCfgI(CfgInternal::MAKEPROCESSEDFILENAMEUNIQUE);
									if (shouldMakeFilenameUnique)
									{
										HobDateTime now;
										processedFilename.append(now.formatDate(FormatDate_YYMMDD_NoDelimiter));
										processedFilename.append("_");
										processedFilename.append(now.formatTime(FormatTime_HHMMSS_NoDelimiter, HourFormat_MILITARY));
										processedFilename.append("_");
									}
									processedFilename.append(downloadFile.getFileName());
									File processedFile(processedDir.getFullPath(), processedFilename);
									if (tracing)
										Debug::trace("ProcessDownloadedFiles", "handleBackupRestoreMessage():  Moving file to processed directory, file moving from " + downloadFile.getFullPath() + " to " + processedFile.getFullPath());
									if ( downloadFile.moveTo(processedFile) )
									{
										fileMoved = true;
										if (tracing)
											Debug::trace("ProcessDownloadedFiles", "handleBackupRestoreMessage():  File moved successfully: " + processedFile.getFullPath());
									}
								}
							}
						}

						if (!fileMoved)
						{
							bool deleted = downloadFile.remove();
							if (tracing)
								Debug::trace("ProcessDownloadedFiles", getName() + " - handleBackupRestoreMessage():  file " + ((deleted) ? "" : "NOT") + " deleted: " + downloadFile.getFullPath());
						}
					}
				}

				// SOF - 7353 Coctco Polaris wrapper is dropping labels
				// Refresh the Current Transaction Type
				// Could it be possible that this request is causing labels to drop on 2P Printers?  Maybe
				// SessionMessage transTypeMsg(TransactionTypeDev, TransactionTypeRefreshCurrentType);
				// transTypeMsg.setFormatType(InternalFormat);
				// send(&transTypeMsg, Messenger::DEST_RTC);
				// transactionTypeManager->handleMessage(&transTypeMsg);

				// if (backupRestoreManager->displayStats && backupRestoreManager->subState != BRCancelSubState)
				//    notify the browser to cancel the backup/restore since something went wrong.

				delete backupRestoreManager;
				backupRestoreManager = (BackupRestoreManager *)NULL;
				Debug::trace("Session", getName() + HobString(" - handleBackupRestoreMessage():  Deleted Backup/Restore Manager"));

#if defined ( __linux__ )
				system("sync");
#endif
			}

			if (mergeFlashFileData)
			{
				mergeFlashFileData = false;
				if (flashFileManager)
					getNextFlashFile();
			}
			else if (restoreVersionIndependentData)
			{
				// Done restoring the db independent data, delete the data file.
				File dbIndData(config->getVersionIndependentBackupFileName());
				if (dbIndData.exists())
				{
					Debug::trace("Session", getName() + HobString(" - handleBackupRestoreMessage():  Deleting the db independent data file"));
					dbIndData.remove();
				}

				// check for service or supervisor keys now
				SessionMessage keyActivationRequestMsg(SessionDev, SnRequestKeyActivation);
				keyActivationRequestMsg.setFormatType(InternalFormat);
				send(&keyActivationRequestMsg, Messenger::DEST_RTC);

				{
					// backdoor to increment the time for auto-searching for new firmware via SFTP,
					// when we were debugging a rare crash when restoring the version independent data file after an upgrade
					int hoursInc = Configuration::getInstance()->toIntCfgI("incrementFirmwareSearchAtBootup");
					if (hoursInc > 0)
					{
						HobString searchTime = Configuration::getInstance()->getNewFirmwareVersionSearchTime();
						HobDateTime sTime;
						sTime.setTimeParts(searchTime, FormatTime_HHMM, HourFormat_AMPM);
						sTime.incrementHours(hoursInc);
						searchTime = sTime.formatTime(FormatTime_HHMM, HourFormat_AMPM);
						Configuration::getInstance()->setNewFirmwareVersionSearchTime(searchTime);
						Configuration::getInstance()->save(this);
					}
				}

				// Write the database back to flash.
				backupDatabase("Restore db independent data");

				// assume the configuration has been updated, send it to all browsers.
				if (configurationManager)
					configurationManager->dispatchConfigurationSaved();

				// FT-804: Make this session go away.
				PAL_absclass::PAL_sleep(1000);
				//terminate(false);
				//quitGracefully();
				setStatus(STATUS_TERMINATED);

				restoreVersionIndependentData = false;
			}

			if ( currentModeOp == OP_PROCESSINGDOWNLOADEDFILES )
				// Reset the mode so this Session will go and process the next file in the processing directory.
				currentModeOp = OP_PROCESSDOWNLOADEDFILES;
			else if ( currentModeOp == OP_PROCESSINGAUTOBACKUPDATABASE )
				// Reset the mode so this Session will go and process the next data-backup group for auto-backing-up.
				currentModeOp = OP_AUTOBACKUPDATABASE;
			else
				currentModeOp = OP_UNKNOWN;

			if (currentModeOpState == OPSTATE_VMSHUTDOWN)
				finishVersionManagerShutdownRequest(true);

			break;
		}

        case BRCancel:
        {
			if (msg->getFormatType() == InternalFormat)
			{
				// Received a cancel command from the Backup/Restore Manager.
				if (!isCommSession())
				{
					// Need to notify the browser of this; just forward the message on to the browser.
					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);
				}
				// else, Do we want to send a message to the connected comm session?  Probably not since we just had problems sending the last message.
			}
			else if (msg->getFormatType() == BrowserFormat)
			{
				// Received a cancel command from the browser to stop the backup/restore.
				// Forward the message on to the BackupRestore Mananger that this Session has created.
				// If the Session doesn't have a backupRestoreManager instantiated, then just ignore the cancel command.
				if (backupRestoreManager)
				{
					Debug::trace("Session", getName() + HobString(" - handleBackupRestoreMessage():  Handling BRCancel Event for Backup/Restore"));
					msg->setFormatType(InternalFormat);
					dispatchEvent(msg, backupRestoreManager);
				}
			}

			break;
		}

		case BackupEntireDB:
		{
			if (!isPrimarySession())
			{
				// Any comm session that receives the command to backup the db to db-independant format, RTDO,
				// needs to pass it along to the Primary Console Session, which will handle the command.
				Debug::trace("Session", getName() + HobString(" - Handling BackupRestore|BackupEntireDB Event, but I'm not the primary console session, so forwaring it on... "));
				msg->setFormatType(InternalFormat);
		        send(msg, Messenger::DEST_RTC);
				break;
			}

			Debug::trace("Session", getName() + HobString(" - handleBackupRestoreMessage():  Handling BackupRestore|BackupEntireDB Event, because I'm the primary console session"));

			backupRestoreManager = new BackupRestoreManager(this);
			PAL_absclass::PAL_sleep(100);

			if (backupRestoreManager)
			{
				// By setting the following currentModeOp, we won't receive any more messages
				// from the UI until the backup is completed.
				currentModeOp = Session::OP_READALL;
				BackupRestoreInitiateReadAllMsg backupMsg(this, SessionDev, SnDBIndependentBackup, 0);
				dispatchEvent(&backupMsg, backupRestoreManager);

				// Now that we started the backup, we need to wait till it's finished.
				// This will happen when we receive the BackupRestore|Terminate message.
			}
			break;
		}

		default:
		{
			if (!backupRestoreManager)
			{
				// Don't have to register for a BackupRestore|Terminate message because
				// the BRM will send one directly to this Session when the BRM is done.
				Debug::trace("Session", getName() + " - handleBackupRestoreMessage():  Creating a BackupRestoreManager ...");
				backupRestoreManager = new BackupRestoreManager(this, msg);
			}
			break;
		}
	}
}


void Session::handleImportMessage(SessionMessage *msg)
{
    switch (msg->getEventType())
    {
        case ImportStart: //start import
            Debug::trace("ImportManager", HobString("session:  Start import "));
            registerEvent(ImportDev, ImportTerminate);
            importManager = new ImportManager(this, msg);
            break;
        case ImportTerminate: //terminate
            Debug::trace("ImportManager", HobString("session:  Terminate import "));
            deregisterEvent(ImportDev, ImportTerminate);
            if (importManager)
            {
                Debug::trace("ImportManager", HobString("session:  Deleting import manager "));
                delete importManager;
                importManager = NULL;
            }
			if ( waitingForImportTerminate_  )
			{
				if ( !isPrimarySession() )
				{
					Debug::trace("ProcessDownloadedFiles", getName() + " - Finished importing downloaded files.");
					std::string logmsg = "Processing session " + getName() + HobString(" Finished importing downloaded files.");
					MessageLogger::logMessage(MessageLogger::ScaleMaintenanceLog, logmsg);

					// Make this session go away.
					PAL_absclass::PAL_sleep(1000);
					Debug::trace("Session.terminate", getName() + " - handleImportMessage():  setStatus(STATUS_TERMINATED).");
					setStatus(STATUS_TERMINATED);
				}
			}
            break;
        default:
            break;
    }
}


void Session::handleReportManagerMessage(SessionMessage *msg)
{
    Debug::trace("Session", 20, "Entered handleReportMessage");

	if (!msg)
        return;

	Debug::trace("Session", 20, "handleReportMessage after !msg");

    if (msg->getEventType() == TerminateReport)
    {
        Debug::trace("Session", 20, "handleReportMessage entered Terminate Report");

		GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
        if (msgdata)
        {
            const HobString& data = msgdata->getData();
            bool cancelled = (!data.empty() && data.at(0));
            Debug::trace("Session", getName() + " - handling Terminate Event for ReportManager (it " +
                    string(cancelled ? "was" : "was not") + "cancelled");

            if ( !cancelled )
            {
                SessionMessage message = SessionMessage(ReportManagerDev, TerminateReport);
                message.setFormatType(BrowserFormat);
                send(&message, Messenger::DEST_UI);
            }
        }

        Debug::trace("Session", getName() + HobString(" - handling Terminate Event for ReportManager"));
        if (reportManager)
        {
            Debug::trace("Session", 24, "Going to delete the ReportManager ...");
            delete reportManager;
            reportManager = 0;
        }
        Debug::trace("Session", getName() + HobString(" - deleted ReportManager"));

        deregisterEvent(ReportManagerDev, TerminateReport);

        Debug::trace("Session", "Memory after deleting ReportManager ...");
        if (Debug::isTracing("Memory"))
            memShow(0);
        return;
    }
    else if (msg->getEventType() == CancelReport)
    {
        Debug::trace("Session", getName() + HobString(" - handling CancelReport Event for ReportManager"));
        msg->setFormatType(InternalFormat);
        send(msg, Messenger::DEST_RTC);
        return;
    }

	Debug::trace("Session", 20, "handleReportMessage Just before !reportManager");

    if (!reportManager)
    {
        Debug::trace("Session", 20, "Going to start the ReportManager for a message received");
        if (Debug::isTracing("Memory"))
        {
            Debug::trace("Session", "Memory before starting ReportManager ...");
            memShow(0);
        }

        registerEvent(ReportManagerDev, TerminateReport);
        reportManager = new ReportManager(msg);
    }
    else
    {
        Debug::trace("Session.event", 5,
                "Received a ReportManager message, but since there is already an instance, the message is dropped!!!!");
    }
}



/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Network Connectivity processing                                                           ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

void Session::handleNetworkConnectivityManagerMessage(SessionMessage *msg)
{
    if (!msg)
        return;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

    switch (msg->getEventType())
    {
		case RequestNetworkType:
		{
            Debug::trace("Session.NetworkConnectivityMgr", "received RequestNetworkType message");

            registerEvent(NetworkConnectivityDev, NetworkType, false);

			msg->setFormatType(InternalFormat);
            send(msg, Messenger::DEST_RTC);
			break;
		}
		case NetworkType:
		{
            Debug::trace("Session.NetworkConnectivityMgr", "received NetworkType message");
			int networkType = 0;

			// I reused the HfcNetworkStatus class for this message; only the network type attribute is valid.
			HfcNetworkStatus *networkStatus = dynamic_cast<HfcNetworkStatus *> (msg->getHfcObject());
			if (networkStatus)
				networkType = networkStatus->getNetworkType();

			SessionMessage commStatusMsg(NetworkConnectivityDev, NetworkType);
			commStatusMsg.setFormatType(BrowserFormat);
			GenericHfcObject gho(inttostring(networkType));
			commStatusMsg.setHfcObject(&gho);
			send(&commStatusMsg, Messenger::DEST_UI);

			break;
		}

        case GoodComm:
        {
            Debug::trace("Session.NetworkConnectivityMgr", "received goodComm message");

			// For Fastrack, we are keeping when the scale goes offline or comes back online (which is the same as the last time that it was online).
			// Only write this to the database if there isn't a date/time already recorded, so we don't let the user think we just went offline.
			// We will reset this config param to "" when we come back online.
			if (isPrimarySession() && config->toStringCfg(fieldOffsetCfgOfflineDateTime) != "")
			{
				config->updateField(fieldOffsetCfgOfflineDateTime, "");
				config->saveField(fieldOffsetCfgOfflineDateTime, this);

				hfc::MultiLanguageText internationalize;
				MessageLogger::logMessage(MessageLogger::InfoLog, internationalize("logMessageScaleOnline"));

				EmailManager::getInstance()->handleMessage(msg, this);
			}
/*
			if ( isPrimarySession() )
			{
				// Delete the timer associated with beeping while offline, if it exists.
				TimerManager *timerManager = TimerManager::getInstance();
				if (timerManager)
				{
					Timer *timer = timerManager->getTimer(getId(), timedFuncOfflineBeep);
					if (timer)
					{
		                Debug::trace("Session", getName() + HobString(" - deleting timedFuncOfflineBeep timer due to GoodComm"));
						timerManager->removeTimer(timer->getTimerId());
					}
				}
			}
*/
            registerEvent(NetworkConnectivityDev, CurrentStatus);

			EvolMessage reqStatus(NetworkConnectivityDev, RequestCurrentStatus, this);
            send(&reqStatus, Messenger::DEST_RTC);

            responseMessageManager->removeMessage(ErrNetworkConnectivityDev, NetworkConnectivityManagerErrLostComm, HfcResponse::RESPONSESTYLE_OK);
            break;
        }
        case BadComm:
        {
            Debug::trace("Session.NetworkConnectivityMgr", "received badComm message");

			// For Fastrack, we are keeping when the scale goes offline or comes back online (which is the same as the last time that it was online).
			// Only write this to the database if there isn't a date/time already recorded, so we don't let the user think we just went offline.
			// We will reset this config param to "" when we come back online.
			if (isPrimarySession() && config->toStringCfg(fieldOffsetCfgOfflineDateTime) == "")
			{
				HobDateTime dt;
				string lastTimeOnline = dt.convertToDBFormat();
				config->updateField(fieldOffsetCfgOfflineDateTime, lastTimeOnline);
				config->saveField(fieldOffsetCfgOfflineDateTime, this);

				hfc::MultiLanguageText internationalize;
				MessageLogger::logMessage(MessageLogger::InfoLog, internationalize("logMessageScaleOffline"));

				EmailManager::getInstance()->handleMessage(msg, this);
			}
/*
			if ( isPrimarySession() && config->toIntCfg(fieldOffsetCfgPeriodicOfflineBeep) > 0 )
			{
				// if enabled, start the offline-beeper timer
				TimerManager *timerManager = TimerManager::getInstance();
				if (timerManager)
				{
					// only add a new timer if one doesn't already exist.
					if (!timerManager->getTimer(getId(), timedFuncOfflineBeep))
					{
		                Debug::trace("Session", getName() + HobString(" - adding timedFuncOfflineBeep timer due to BadComm"));
						timerManager->addTimer(getId(), config->toIntCfg(fieldOffsetCfgPeriodicOfflineBeep), Timer::RepeatInMinutes, timedFuncOfflineBeep);

						Debug::trace( "Session", "Created the timer to beep when offline every " + config->toStringCfg(fieldOffsetCfgPeriodicOfflineBeep) + " seconds." );
					}
				}
			}
*/
           registerEvent(NetworkConnectivityDev, CurrentStatus);

			EvolMessage reqStatus(NetworkConnectivityDev, RequestCurrentStatus, this);
            send(&reqStatus, Messenger::DEST_RTC);

            if (!isInService())
                responseMessageManager->addMessage(ErrNetworkConnectivityDev, NetworkConnectivityManagerErrLostComm, HfcResponse::RESPONSESTYLE_OK);
            break;
        }
        case BadCommRepeat:
        {
            Debug::trace("Session.NetworkConnectivityMgr", "received BadCommRepeat message");

            registerEvent(NetworkConnectivityDev, CurrentStatus);

			EvolMessage reqStatus(NetworkConnectivityDev, RequestCurrentStatus, this);
            send(&reqStatus, Messenger::DEST_RTC);

            if (!isInService())
                responseMessageManager->addMessage(ErrNetworkConnectivityDev, NetworkConnectivityManagerErrLostComm, HfcResponse::RESPONSESTYLE_OK);
            break;
        }
        case RequestCurrentStatus:
        {
            Debug::trace("Session.NetworkConnectivityMgr", "received RequestCurrentStatus message");

            registerEvent(NetworkConnectivityDev, CurrentStatus);

            msg->setFormatType(InternalFormat);
			send(msg, Messenger::DEST_RTC);
            break;
        }
        case CurrentStatus:
        {
            Debug::trace("Session.NetworkConnectivityMgr", "received RequestCurrentStatus message");

			deregisterEvent(NetworkConnectivityDev, CurrentStatus);

			HfcNetworkStatus *networkStatus = dynamic_cast<HfcNetworkStatus *> (msg->getHfcObject());
			if (networkStatus)
			{
				HfcNetworkStatus networkStatusCopy(*networkStatus);
				networkStatusCopy.setLastOnlineDateTime(config->toStringCfg(fieldOffsetCfgOfflineDateTime));

				// FT-153 diagnostic logging
				// log out wireless strength (1=wireless network)
				if (( isPrimarySession() ) && ( 1 == networkStatusCopy.getNetworkType() ))
				{
					int newWirelessStrength = networkStatusCopy.getWirelessStrength();

					if ( previousWirelessStrength_ != newWirelessStrength )
					{
						// FT-799 only log when strength one bar or less
						if ( newWirelessStrength <= 1 )
						{
							hfc::MultiLanguageText internationalize;
							MessageLogger::logMessage(MessageLogger::InfoLog, internationalize("wirelessStrength") + " " + util_string::inttostring(newWirelessStrength));
						}

						previousWirelessStrength_ = newWirelessStrength;
					}
				}

				if ( isPrimarySession() )
				{
					if ( networkStatusCopy.isOnline() )
					{
						// Delete the timer associated with beeping while offline, if it exists.
						TimerManager *timerManager = TimerManager::getInstance();
						if (timerManager)
						{
							Timer *timer = timerManager->getTimer(getId(), timedFuncOfflineBeep);
							if (timer)
							{
				                Debug::trace("Session", getName() + HobString(" - deleting timedFuncOfflineBeep timer due to CurrentStatus indicating Online"));
								timerManager->removeTimer(timer->getTimerId());
							}
						}
					}
					else
					{
						// if enabled, start the offline-beeper timer, if we already don't have one running
						TimerManager *timerManager = TimerManager::getInstance();
						if (timerManager)
						{
							// only add a new timer if one doesn't already exist.
							if ( !timerManager->getTimer(getId(), timedFuncOfflineBeep) )
							{
								if ( config->toIntCfg(fieldOffsetCfgPeriodicOfflineBeep) > 0 )
								{
									Debug::trace("Session", getName() + HobString(" - adding timedFuncOfflineBeep timer due to CurrentStatus indicating Offline"));
									timerManager->addTimer(getId(), config->toIntCfg(fieldOffsetCfgPeriodicOfflineBeep), Timer::RepeatInMinutes, timedFuncOfflineBeep);

									Debug::trace( "Session", "Created the timer to beep when offline every " + config->toStringCfg(fieldOffsetCfgPeriodicOfflineBeep) + " seconds." );
								}
							}
						}
					}
				}

	            SessionMessage commStatusMsg(NetworkConnectivityDev, CurrentStatus);
		        commStatusMsg.setFormatType(BrowserFormat);
				commStatusMsg.setHfcObject(&networkStatusCopy);
	            send(&commStatusMsg, Messenger::DEST_UI);
			}
            break;
        }
		case StartContinuousNetworkStatus:
		{
			// FT-895 do this regardless, they may have wifi and need to be updated of signal strength
            //if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgCheckNetworkConnectivity))
			{
				Debug::trace("NetworkStatus", getName() + HobString(" - handling StartContinuousNetworkStatus Event"));
				constantlyUpdatingNetworkStatus = true;
				registerEvent(NetworkConnectivityDev, CurrentStatus);

				EvolMessage reqStatus(NetworkConnectivityDev, RequestCurrentStatus, this);
				send(&reqStatus, Messenger::DEST_RTC);

				// Create a timer to be notified when to display the next set of statistics
				TimerManager *timerManager = TimerManager::getInstance();
				if (timerManager)
				{
					// only add a new timer if one doesn't already exist.
					if (!timerManager->getTimer(getId(), timedFuncContinuousNetworkStatus))
					{
						timerManager->addTimer(getId(), Configuration::getInstance()->toIntCfgI(CfgInternal::CONTINUOUSNETWORKSTATUSDELAY), Timer::RepeatInMinutes, timedFuncContinuousNetworkStatus);

						Debug::trace("NetworkStatus", "Created the timer to report network statuses");
					}
				}
			}
            break;
		}
        case StopContinuousNetworkStatus:
		{
            Debug::trace("NetworkStatus", getName() + HobString(" - handling StopContinuousNetworkStatus Event"));
            constantlyUpdatingNetworkStatus = false;
			deregisterEvent(NetworkConnectivityDev, CurrentStatus);

			// Delete the timer associated with the continuous statistics.
			TimerManager *timerManager = TimerManager::getInstance();
			if (timerManager)
			{
				Timer *timer = timerManager->getTimer(getId(), timedFuncContinuousNetworkStatus);
				if (timer)
					timerManager->removeTimer(timer->getTimerId());
			}
            break;
		}

        default:
        {
            PAL_absclass::PAL_OutputDbgStr( L"Session: received unknown NetworkConnectivityDev message\r\n" );
            break;
        }
    }
}

// FT-1458
void Session::handleNowServingQueueManagementMessage( SessionMessage *msg )
{
	// now serving message
	NowServingQueueManagementMessage *nowServingMsg( dynamic_cast<NowServingQueueManagementMessage*>( msg->getHfcObject() ) );
	if ( !nowServingMsg )
	{
		Debug::trace( "NowServing", HobString( "Error casting message!" ) );
		return;
	}

	// process!
	if ( !nowServingMsg->process( this ) )
		Debug::trace( "NowServing", HobString( "Error processing message!" ) );
	else if ( 0 == nowServingMsg->resultCode() )
	{
		// for next customer actions need to update gcd
		if (( NowServingQueueManagementMessage::ServingNextCustomer == nowServingMsg->action() ) ||
			( NowServingQueueManagementMessage::ServeSpecificCustomer == nowServingMsg->action() ) ||
			( NowServingQueueManagementMessage::IncrementCustomerNumber == nowServingMsg->action() ))
		{
			std::string value( "" );

			// first value is customer number
			std::list<std::string> values( nowServingMsg->values() );
			if ( !values.empty() )
				value = values.front();

			GCDSystemMsg nowServingCustomer(
			    GCDSessionDev,
			    GcdSessionNowServingCustomer,
			    value );

			Debug::trace( "NowServing", HobString( "Display now serving to GCD" ) );
			dispatchMessage( &nowServingCustomer );
		}

		// for dismiss messages we need to update gcd
		else if ( NowServingQueueManagementMessage::DismissCustomerAttractDialog == nowServingMsg->action() )
		{
			std::string value( "" );

			GCDSystemMsg dismissNowServingCustomer(
			    GCDSessionDev,
			    GcdSessionDismissNowServingCustomer,
			    value );

			Debug::trace( "NowServing", HobString( "Dismiss now serving to GCD" ) );
			dispatchMessage( &dismissNowServingCustomer );
		}
	}

	// give result to caller
    SessionMessage response( SessionDev, SnNowServingQueueManagement );
    response.setFormatType( BrowserFormat );
    response.setHfcObject( nowServingMsg );
    send( &response, Messenger::DEST_UI );
}

/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   CGI Call processing                                                                       ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

void Session::handleCGICallManagerMessage(SessionMessage *msg)
{
    if ( msg->getEventType() == comm::CGICallPerformed )
    {
        GenericHfcObject* gho = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
        if ( gho )
        {
            bool* result = (bool*)gho->getData().data();
            int* intResult = (int*)gho->getData().data();

            Debug::trace("Session", getName() + " - Received CGICallManager message with result of : " +
                    util_string::booltostring(*result) + "(int result is " + util_string::inttostring(*intResult) +
                    " and sizeof(bool) is " + util_string::inttostring(sizeof(bool)) + ")");

            if ( !*result )
            {
            	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::HTTPREPORTERRORS))
            		addDialogMessage(ErrProductDev, ProductErrFPPFailedToUpdate, HfcResponse::RESPONSESTYLE_OK);
            }
            else if (
                    (currentMode != OpModeProductionEntry) &&
                    (!Configuration::getInstance()->isClearPluAfterPrinting())
                    )
            {

                msg->setFormatType(BrowserFormat);
                send(msg, Messenger::DEST_UI);
            }

            SendErrorEmailEvent::ErrorEmailType errorEmailType( (*result) ? SendErrorEmailEvent::ErrorEmailTypeResolved : SendErrorEmailEvent::ErrorEmailTypeError );
			SendErrorEmailEvent errorEmail(errorEmailType, SendErrorEmailEvent::ErrorEmailFPPUpdateFailed, this);
			send(&errorEmail, Messenger::DEST_RTC);
        }

    }
}


/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Timer expiration processing                                                               ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

void Session::manageAutoLogoutTimer()
{
    int signalTime = 0;

    if (isPrimarySession() || isIntranetSession())
    {
		/*  In the HT series, the UI is controlling the timeout for switching back to the start screen, so this is now commented out.
		 *  The Primary Console no longer has any UI timeouts.
		 *
        if (Configuration::getInstance()->isSelfServiceScale())
        {
            if (!isInOperateMode() || isOnSelfServiceStartupScreen())
            {
                Debug::trace("Session.timeout", getName() + HobString(" - manageAutoLogoutTimer - removing timer, in operate mode:") +
                        HobString(Bool::toString(isInOperateMode())) + HobString("  isOnSelfServiceStartupScreen:") + HobString(Bool::toString(isInOperateMode())) + HobString("   handling TIMEOUT Event"));
                removeAutoLogoutTimer();
                return;
            }
            signalTime = Configuration::getInstance()->getSelfServeInactivityTimeout();  // is already in terms of seconds
        }
        else
        {
            Debug::trace("Session.timeout", getName() + HobString(" - manageAutoLogoutTimer - removing timer because not in self service mode"));
            // The primary console can't time out.  If there is an autoLogoutTimerId, there shouldn't be;
            // so tell the TimerManager to remove it, and return.
            removeAutoLogoutTimer();

            return;
        }
		*/

        Debug::trace("Session.timeout", getName() + HobString(" - manageAutoLogoutTimer - removing timer because I'm the primary console"));
        // The primary console can't time out.  If there is an autoLogoutTimerId, there shouldn't be;
        // so tell the TimerManager to remove it, and return.
        removeAutoLogoutTimer();
		return;
    }
    else  // remote session
    {
        // For a remote Session in monitor mode, we don't need to auto-logout.
        // The user would rather watch the user without being inadvertently terminated.
        if (currentMode != OpModeMonitor)
            signalTime = Configuration::getInstance()->getInactivityAutoLogoutTime() * 60;  // is in terms of minutes, so multiply by 60 (seconds/minute)
    }

    Debug::trace("Session.timeout", getName() + HobString(" - manageAutoLogoutTimer - signal time:") + HobString(Integer::toString(signalTime)));
    if (signalTime)
    {
        TimerManager *timerManager = TimerManager::getInstance();
        if (!timerManager)
            return;

        if (autoLogoutTimerId)
            // already have an auto-logout timer, just update it with the new InactivityAutoLogoutTime
            timerManager->resetTimer(autoLogoutTimerId, signalTime);
        else
            // Don't have an auto-logout timer yet, add one to the TimerManager.
            autoLogoutTimerId = timerManager->addTimer(getId(), signalTime, Timer::RepeatInMinutes, timedFuncInactivityAutoLogout);
    }
    else
        // InactivityAutoLogoutTime has been set to 0; tell the TimerManager to remove the auto-logout timer, if it exists.
        if (autoLogoutTimerId)
            removeAutoLogoutTimer();
}

void Session::removeAutoLogoutTimer()
{
	Debug::trace("Session.timeout", getName() + " removeAutoLogoutTimer()");
    if (autoLogoutTimerId)
    {
		Debug::trace("Session.timeout", getName() + " removeAutoLogoutTimer():  autoLogoutTimerId");
        TimerManager *timerManager = TimerManager::getInstance();
        if (timerManager)
        {
			Debug::trace("Session.timeout", getName() + " removeAutoLogoutTimer():  Have a timerManager. Removing the timer.");
            timerManager->removeTimer(autoLogoutTimerId);
            autoLogoutTimerId = (int)NULL;
        }
    }
}

void Session::setExpiredTimer(const Timeout &timeout)
{
    expiredTimer = timeout;
}

void Session::handleTimeout(Timeout *timeout, bool shouldExitWhenFinished)
{
    if (!timeout)
        return;

    TimerManager *timerManager = TimerManager::getInstance();
    if (!timerManager)
        return;

    Timer *timer = timerManager->getTimer(timeout->getTimerId());
    if (timer)
    {
        if ( Debug::isTracing("Session.timeout") )
            timer->show();

        TimedFunction timerPurpose = timer->getTimerPurpose();
        switch (timerPurpose)
        {
            case timedFuncBackupDatabase:
                Debug::trace("Session", getName() + HobString(" - handleTimeout(timer) - is a timedFuncBackupDatabase"));
                backupDatabase(HobString("handleTimeout"));
                break;

            case timedFuncInactivityAutoLogout:
                {
                    Debug::trace("Session.timeout", getName() + HobString(" - handling TIMEOUT Event"));

                    // The primary console doesn't create this type of timer, so it isn't supposed to get in here,
                    // but just in case, check for it and don't allow the Primary Console Session to timeout.
                    if (isPrimarySession())
                    {
                        if (Configuration::getInstance()->isSelfServiceScale() && isInOperateMode())
                        {
                            Debug::trace("Session.timeout", getName() + HobString(" - Self-Service TIMEOUT - Returning to startup screen."));
                            // Send a message to the browser telling it to return to the startup screen.

                            if(onStartUpScreenInSecondLanguage)
                            {
                                onStartUpScreenInSecondLanguage = false;
                                setOnSelfServiceStartupScreen(true);    // returned to start page in sec lang, so now clear timers

                                // Right here, don't know what it is, but I need to put a message together
                                // to tell the UI to timeout to the primary language
                                SessionMessage timeoutMsg(SessionDev, SnTimedOut);
                                timeoutMsg.setFormatType(BrowserFormat);
                                send(&timeoutMsg, Messenger::DEST_UI);

                                if (responseMessageManager)
                                    responseMessageManager->removeAllMessages(ErrUnknown, -1, true);}
                            else
                            {                
                                SessionMessage timeoutMsg(SessionDev, SnTimedOut);
                                timeoutMsg.setFormatType(BrowserFormat);
                                send(&timeoutMsg, Messenger::DEST_UI);

                                if (productManager)
                                    productManager->clearCurrentProduct(true);

                                if (responseMessageManager)
                                    responseMessageManager->removeAllMessages(ErrUnknown, -1, true);
                            }
                        }
                        else
                        {
                            Debug::trace("Session.timeout", getName() + HobString(" - The primary console session timed out, no messages received, but not gonna terminate"));
                        }
                    }
                    else
                    {
                        // Terminate the remote Session.  No messages received (from UI) within timeout time.
                        setStatus(STATUS_TIMEDOUT);
                        Debug::trace("Session.timeout", getName() + HobString(" - TIMEOUT TIMEOUT TIMEOUT - No messages received before timeout."));

                        if (responseMessageManager)
                        {
                            responseMessageManager->removeAllMessages();
                        }

                        // By passing in true to terminate, we will send a message to the connected UI, if there is one,
                        // and notify the SessionManager (to remove this session from its list).
                        terminate(true, timedFuncInactivityAutoLogout);
                    }
                    break;
                }

            case timedFuncStreamingGraphic:
                {
                    Debug::trace("Session", getName() + HobString(" - handleTimeout(timer) - is a timedFuncStreamingGraphic"));

                    // Send the next streaming graphic
                    if (currentStreamingGraphic)
                    {
                        HobRecordNum nextStreamingGraphicNum = currentStreamingGraphic->getNextStreamingGraphicNum();

                        if (nextStreamingGraphicNum != currentStreamingGraphic->getStreamingGraphicNum())
                        {
                            Debug::trace("Session", getName() + HobString(" - have a current streaming graphic, next one is num:") + HobString(Integer::toString(nextStreamingGraphicNum)));

                            delete currentStreamingGraphic;
                            currentStreamingGraphic = (StreamingGraphic *)NULL;

                            currentStreamingGraphic = StreamingGraphic::findStreamingGraphic(nextStreamingGraphicNum, this);
                            if (currentStreamingGraphic)
                            {
                                Debug::trace("Session", getName() + HobString(" - found this next streaming graphic - displayTime:") + HobString(Integer::toString(currentStreamingGraphic->getDisplayTime())));

                                SessionMessage imageMsg(SessionDev, SnWriteStreamingGraphic);
                                imageMsg.setFormatType(BrowserFormat);
                                imageMsg.setHfcObject(currentStreamingGraphic);
                                send(&imageMsg, Messenger::DEST_UI);

                                // Create a timer to be notified when this streaming graphic should be replaced with the next one.
                                TimerManager *timerManager = TimerManager::getInstance();
                                if (timerManager)
                                    timerManager->addTimer(getId(), currentStreamingGraphic->getDisplayTime(), Timer::RepeatNO, timedFuncStreamingGraphic);
                            }
                        }
                    }
                    break;
                }

            case timedFuncContinuousStatistics:
                {
                    Debug::trace("Session", getName() + HobString(" - handleTimeout(timer) - is a timedFuncContinuousStatistics"));

                    // This timer is only used by the primary session, so let's send a Read
                    // Statistics request to the Primary Browser
                    SessionMessage statsMsg(SessionDev, SnReadStatistics);
                    statsMsg.setFormatType(BrowserFormat);
                    send(&statsMsg, Messenger::DEST_UI);
                    break;
                }

            case timedFuncContinuousNetworkStatus:
                {
                    Debug::trace("Session", getName() + HobString(" - handleTimeout(timer) - is a timedFuncContinuousNetworkStatus"));

					registerEvent(NetworkConnectivityDev, CurrentStatus);

                    SessionMessage statusMsg(NetworkConnectivityDev, RequestCurrentStatus);
                    statusMsg.setFormatType(InternalFormat);
                    send(&statusMsg, Messenger::DEST_RTC);
                    break;
                }

            case timedFuncCheckMissingPeripherals:
            {
//	HW-196: Wait until the Operator acknowledges that Initialization is complete, instead of setting a timer.
                Debug::trace("Session.initComplete", getName() + HobString(" - handleTimeout(timer) - is a timedFuncCheckMissingPeripherals - now let's see if bootup is complete."));
				checkInitializationIsComplete();
                break;
            }

            case timedFuncWrTimingsComplete:
            {
                Debug::trace("SessionWrapper", getName() + HobString(" - handleTimeout(timer) - is a timedFuncWrTimingsComplete - now ..."));

                //    The Wrapper failed to send the WrEvolSavePackageTimingsComplete msg in the allotted time.
                //    Clear the wait msg and alert the operator so they can continue with other things.
                if (evolWrapperTimingsManager_)
                {
                    evolWrapperTimingsManager_->abortWaitForWrapperMsg();
                    if (responseMessageManager)
                        responseMessageManager->addMessage(ErrWrapper, 9710, HfcResponse::RESPONSESTYLE_OK);
				}
				break;
			}

			case timedFuncWrSystemInfoSaved:
			{
				Debug::trace("SessionWrapper", getName() + HobString(" - handleTimeout(timer) - is a timedFuncWrSystemInfoSaved - now ..."));

				if (responseMessageManager)
				{
					responseMessageManager->removeMessage(ErrWrapper, 9705, HfcResponse::RESPONSESTYLE_NOINPUT);
					responseMessageManager->addMessage(ErrWrapper, 9722, HfcResponse::RESPONSESTYLE_OK);
				}
				else
				{
					Debug::trace("SessionWrapper", getName() + HobString(" - handleTimeout(timer) - We have no responseMessageManager!"));
				}
                break;
            }

            case timedFuncOfflineBeep:
				Debug::trace("Session", getName() + HobString(" - handleTimeout(timer) - is a timedFuncOfflineBeep"));
				ServiceSpeaker::offlineBeep(2);
                break;

			case timedFuncGetHandsFreeDevicesInLoginRange:
			{
				Debug::trace("HandsFree", getName() + HobString(" - handleTimeout(timer) - is a timedFuncGetHandsFreeDevicesInLoginRange"));

				// Don't get the list if we're currently sending alert messages to the hands-free devices
				if (handsFreeDeviceMessage.empty())
				{
					deviceListRequest request(deviceListRequest::HFPD_LOW_POWER_ADVERT, Configuration::getInstance()->toIntCfgI(fieldOffsetCfgIGetDevicesInLoginRangeAge));
					send(&request, Messenger::DEST_UI);
				}

				break;
			}

            default:
                break;
        }

        if (timer->getRepeatFrequency() == Timer::RepeatNO)
        {
            TimerManager *timerManager = TimerManager::getInstance();
            if (timerManager)
                timerManager->removeTimer(timer->getTimerId());
        }
    }

    if (shouldExitWhenFinished)
	{
        //      Then this session is done.
		Debug::trace("Session.terminate", getName() + " - handleTimeout():  setStatus(STATUS_TERMINATED).");
        setStatus(STATUS_TERMINATED);
	}
}



/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Flash file processing                                                                     ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

void Session::processFlashFile()
{
    if (!flashFileManager)
        return;

    Debug::trace("Session.FlashFile", HobString("  processFlashFile()"));
    if (flashFileManager->isCompleted())
    {
        if (responseMessageManager)
            responseMessageManager->removeMessage(ErrVersionManagerDev, VersionManagerFlashFileStart, HfcResponse::RESPONSESTYLE_NOINPUT);
        Debug::trace("Session.FlashFile", HobString("  *****  We are done with the FlashFiles!  *****"));
    }
    else
    {
        if (flashFileManager->isGraphicFile() && flashFileManager->getFileDestination())
        {
            //  Call GraphicManager method to move the graphic file
            Debug::trace("Session.FlashFile", HobString("  Moving the GraphicFile."));
            graphicManager->moveGraphicFile(flashFileManager->getLocalFlashFileName(), flashFileManager->getFileDestination()->getPath(), flashFileManager->getGraphicFileType());

            getNextFlashFile();
        }
        else if (flashFileManager->doLoadTheFile())
        {
            Debug::trace("Session.FlashFile", HobString("  Loading the database File."));
            startFlashFileMerge();
        }
        else
        {
            getNextFlashFile();
        }
    }
}

void Session::getNextFlashFile()
{
    if (!flashFileManager)
        return;

    Debug::trace("Session.FlashFile", HobString("  getNextFlashFile()"));
    flashFileManager->getNextRemoteFlashFile();
    processFlashFile();
}


void Session::startFlashFileMerge()
{
    if (!flashFileManager)
        return;

    Debug::trace("Session.FlashFile", HobString("  startFlashFileMerge()"));
    HobString fileName = flashFileManager->getLocalFlashFileName();
    if (!flashFileManager->doLoadTheFile())
        Debug::trace("Session.FlashFile", HobString("   We will NOT merge this file: ") + fileName);
    else
    {
        Debug::trace("Session.FlashFile", getName() + HobString("   Starting a Backup/Restore Manager to process the file: ") + fileName);

        /* Have to allocate one from the heap.  The BackupRestore Manager is going to want to take
         * possession of the HfcObject in the message, so can't just create one on the stack.  The
         * life of the HfcObject will be longer than that of its SessionMessage.
         */
        BackupRestoreSelections *brs = new BackupRestoreSelections();
        brs->setFileName(fileName);
        brs->setMode(BRMerge);  // merge is good enough; there won't be any data in the database to delete if we specify BROverWrite
        brs->setFormat(BRVersionIndependent);
        brs->setEveryDataTypeSelected(true);
        brs->setDisplayStats(false);

        SessionMessage restore(BackupRestore, Restore);
        restore.setFormatType(BrowserFormat);
        restore.setHfcObject(brs);

        currentModeOp = OP_READINGFLASHFILES;
        handleBackupRestoreMessage(&restore);
        mergeFlashFileData = true;
    }
}

/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Windows Configuration Manager processing                                                  ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

void Session::handleWindowsConfigurationManagerMessage(SessionMessage *msg)
{
    if (!msg)
        return;

    switch(msg->getEventType())
    {

        case backupWirelessProfiles:
        {
#if defined ( __linux__ )       //TODO: figure out with VPN how to do this
                // copy /etc/wicd/* to thumbdrive is the old way
#else
            WindowsConfigurationManager::getInstance()->handleMessage(msg);
#endif
            break;
        }
        default:
            break;
    }
}


/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Version Manager processing                                                                ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

void Session::handleVersionManagerMessage(SessionMessage *msg)
{
    if ( !msg )
        return;

	Configuration *config = Configuration::getInstance();
	if ( !config )
		return;

    switch ( msg->getEventType() )
    {
        case UpdateNow2016:
		{
            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());

            if ( gho )
            {
                Debug::trace("Session", getName() + HobString(" - flag to backup database or not: ") + gho->getData());

				if ( !config->toBoolCfgI(CfgInternal::PYTHONENABLED) )
					Debug::trace("VersionManager", "scaleManagementHandlesUpgrade=true, but python disabled");
                else
				{
                    const string command(
                        PythonScripter::buildPythonCall(
                            config->toStringCfgI(CfgInternal::PYTHONSENDSMBACKENDMESSAGESCRIPTNAME),
                            config->toIntCfgI(CfgInternal::SMBACKENDPORT),
                            gho->getData() ) );

                    const bool needToLock( unlockCritSectionForLongRunningProcess() );

                    Debug::trace("VersionManager", "scaleManagementHandlesUpdateNow2016=true, UpdateNow2016, command: " + command);

                    string connectionStatusOutput;
                    PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);

                    Debug::trace("VersionManager", "UpdateNow2016 : " + connectionStatusOutput);

                    if ( needToLock )
                        lockCritSectionAfterLongRunningProcess();

                    // We do care about any output from the command and send the result to the UI.
                    HobString ftpResponse = connectionStatusOutput;
                    GenericHfcObject newFtpResponse(ftpResponse);
                    SessionMessage newFTPResponseMsg(ConfigurationDev, CfgFTPResponseWrite);
                    newFTPResponseMsg.setFormatType(BrowserFormat);
                    newFTPResponseMsg.setHfcObject(&newFtpResponse);
                    send(&newFTPResponseMsg, Messenger::DEST_UI);
				}
			}

            break;
		}

		case SendToDept2016:
		{
            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());

            if ( gho )
            {
                Debug::trace("Session", getName() + HobString(" - flag to backup database or not: ") + gho->getData());

                if ( !config->toBoolCfgI(CfgInternal::PYTHONENABLED) )
                    Debug::trace("VersionManager", "scaleManagementHandlesUpgrade=true, but python disabled");

                // get list of secondary IPs and loop through
                else if ( getScaleManager()->initializeSecondaryScalesQuery() )
                {
                    Util::CSmartPtr<hfc::Scale> scale( getScaleManager()->getFirstSecondaryScale() );
                    
                    while ( scale.notNull() )
                    {
                        Debug::trace("VersionManager", "SendToDept2016 beginning loop IPAddressOfSecondary is : " + scale->getIPAddress());

                        const string command(
                            PythonScripter::buildPythonCallUpdateSecondaryScale(
                                config->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME),
                                config->toIntCfgI(CfgInternal::SMBACKENDPORT),
                                gho->getData(),
                                scale->getIPAddress() ) );

                        const bool needToLock( unlockCritSectionForLongRunningProcess() );

                        Debug::trace("VersionManager", "scaleManagementHandlesSendToDept2016=true, SendToDept2016, command: " + command);

                        string connectionStatusOutput;
                        PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);

                        Debug::trace("VersionManager", "SendToDept2016 : " + connectionStatusOutput);

                        if ( needToLock )
                            lockCritSectionAfterLongRunningProcess();

                        // We do care about any output from the command and send the result to the UI.
                        scale = getScaleManager()->getNextSecondaryScale();
                    }

                    GenericHfcObject newFtpResponse("done");

                    SessionMessage newFTPResponseMsg(ConfigurationDev, CfgFTPResponseWrite);
                    newFTPResponseMsg.setFormatType(BrowserFormat);
                    newFTPResponseMsg.setHfcObject(&newFtpResponse);

                    send( &newFTPResponseMsg, Messenger::DEST_UI );
                }
            }

            break;
		}

		case FirmwareUpdateStatus:
        {
            HfcVmStatusMsg *pMsg = dynamic_cast<HfcVmStatusMsg *> (msg->getHfcObject());

            if ((pMsg) && (responseMessageManager))
            {
                if (pMsg->getRemoveDialog() == true)
                {
                    responseMessageManager->removeMessage(ErrVersionManagerDev, pMsg->getStatusCode(), pMsg->getResponseType());
                }
                else
                {
                    switch(pMsg->getStatusCode())
                    {
                        case VersionManagerUsbDeviceDownloading:
                        case VersionManagerUsbDeviceDownloadSucceeded:
                        case VersionManagerUsbDeviceDownloadFailed:
                        case VersionManagerUsbDeviceResetting:
						{
							responseMessageManager->removeAllMessages(ErrWeigher);
							responseMessageManager->removeAllMessages(ErrPrinter);
							responseMessageManager->addMessage(ErrVersionManagerDev, pMsg->getStatusCode(), pMsg->getResponseType());
							break;
						}
                        case VersionManagerAtmelDeviceDownloading:
                        case VersionManagerAtmelDeviceDownloadSucceeded:
                        case VersionManagerAtmelDeviceDownloadFailed:
						{
							responseMessageManager->removeAllMessages(ErrWeigher);
							responseMessageManager->removeAllMessages(ErrPrinter);
							responseMessageManager->addMessage(ErrVersionManagerDev, pMsg->getStatusCode(), pMsg->getResponseType());
							break;
						}
                        case VersionManagerLoadingNewFirmware:
                        {
                            responseMessageManager->addMessage(ErrVersionManagerDev, pMsg->getStatusCode(), pMsg->getResponseType());
                        }
                        default:
                        {
                            responseMessageManager->removeMessage(ErrVersionManagerDev, VersionManagerLoadingNewFirmware, HfcResponse::RESPONSESTYLE_NOINPUT);
                            responseMessageManager->addMessage(ErrVersionManagerDev, pMsg->getStatusCode(), pMsg->getResponseType());
                            break;
                        }
                    }
                }
            }
            break;
        }
        case SystemShutDownRequest:
        {
            Debug::trace("Session", getName() + HobString(" - handling SystemShutDownRequest message"));
            if (responseMessageManager)
                responseMessageManager->addMessage(ErrVersionManagerDev, VersionManagerLoadingNewFirmware, HfcResponse::RESPONSESTYLE_NOINPUT);

            //bool okToShutdown = true;
            currentModeOpState = OPSTATE_VMSHUTDOWN;

            // Shutdown all remote connections so no other task can update the database while we back it up.
            TerminateMessage shutdownRemotes(this, true);
            dispatchEvent(&shutdownRemotes);

            // Delay for 3 seconds to give any remote sessions time to terminate
            PAL_absclass::PAL_sleep(3000);

            // First, write the database back to flash.
            Debug::trace("Session", getName() + HobString("ShutDownSystemRequest - writing the database back to flash"));
            backupDatabase("VersionManagerShutdown");

            // For now, we are using a GenericHfcObject to contain the single 'backup db' flag in this message.
            GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
            Debug::trace("Session", getName() + HobString(" - flag to backup database or not: ") + gho->getData());
            if (gho && gho->getData().at(0) == 0x01)
            {
                // Backup the database to version independent format if the message indicates this needs done.
                Debug::trace("Session", getName() + HobString("ShutDownSystemRequest - Backing up the database to version independent format"));

                if (responseMessageManager)
                    responseMessageManager->removeMessage(ErrVersionManagerDev, VersionManagerLoadingNewFirmware, HfcResponse::RESPONSESTYLE_NOINPUT);

				SessionMessage backItUp(BackupRestore, BackupEntireDB);
				backItUp.setFormatType(BrowserFormat);
				handleBackupRestoreMessage(&backItUp);

                if (!backupRestoreManager)
                {
                    Debug::trace("Session", getName() + HobString("ShutDownSystemRequest - Couldn't create the BackupRestoreManager to perform the db version-independent backup"));
                    // We were supposed to backup the database, but couldn't get a BackupRestoreManager;
                    // tell the version manager the backup failed.
                    finishVersionManagerShutdownRequest(false);
                }
            }
            else
            {
                Debug::trace("Session", getName() + HobString("ShutDownSystemRequest - Version Manager did not request to backup the database to version independent format"));
                finishVersionManagerShutdownRequest(true);
            }

            PAL_absclass::PAL_sleep(500);

            EvolMessage shutdownSync(SessionDev, SnTerminateSynchronization);
            dispatchEvent(&shutdownSync);

            break;
        }

        case UpgradeAutomatic:
        case UpgradeManual:
		case UpgradeAutomaticViaServer:
        case WriteFileToSCEEP:
        case BeginUpgrade:
        case RemoveWrapperSWComponents:
        case CancelUpgrade:    
		case RemoveCLASWComponents://Alex Zavala 10/3
        {
			if ( msg->getFormatType() == EvolutionTaggedFormat && (msg->getEventType() == UpgradeAutomatic || msg->getEventType() == UpgradeAutomaticViaServer) )
            {
                Configuration* pConfig = Configuration::getInstance();
                if ( pConfig )
                {
                    std::string serverName;
                    pConfig->getFTPServerIPorHostName().getAsString(serverName);
                    UpdateSoftwareAutomatically upgradeMessage(serverName, pConfig->toStringCfg(fieldOffsetCfgFTPUserName),
							pConfig->toStringCfg(fieldOffsetCfgFTPPassword), pConfig->toStringCfg(fieldOffsetCfgFTPSubDir), pConfig->toStringCfgI(CfgInternal::DEFAULTUPGRADEDIRECTORY)/*pConfig->Configuration::getServerUpgradeName()*/);
                    dispatchEvent(&upgradeMessage);
                }
                break;
            }

            msg->setFormatType(InternalFormat);
            send(msg, Messenger::DEST_RTC);
            break;
        }
        case ProcessDBFilesReq:
        {
            Debug::trace("Session.FlashFile", getName() + HobString("   Received a ProcessDBFilesReq msg."));
            if (flashFileManager)
            {
                if (responseMessageManager)
                    responseMessageManager->addMessage(ErrVersionManagerDev, VersionManagerFlashFileStart, HfcResponse::RESPONSESTYLE_NOINPUT);
                flashFileManager->startRemoteFlashFileLoad();
                processFlashFile();
            }
            break;
        }
        case InitiateUpgradeProcess:
        case PeripheralUpgradeList:
        case UpgradeComplete:
        case UpgradeXferStatus:
        case UsbWrapperUpgradeStatus:
        {
            Debug::trace("VersionManager", "Session::handleVersionManagerMessage():  DEST_UI,  Event = " + inttostring(msg->getEventType()));
            //    Pass this on to the Browser.
            msg->setFormatType(BrowserFormat);
            send(msg, Messenger::DEST_UI);
            break;
        }
        default:
            break;
    }
}

void Session::finishVersionManagerShutdownRequest(bool okToShutdown)
{
    Debug::trace("Session", getName() + HobString(" finishVersionManagerShutdownRequest"));

    if (responseMessageManager)
        responseMessageManager->removeAllMessages(ErrVersionManagerDev, -1);

    if (okToShutdown)
    {
        if (responseMessageManager)
            responseMessageManager->addMessage(ErrVersionManagerDev, VersionManagerLoadingNewFirmware, HfcResponse::RESPONSESTYLE_NOINPUT);

        SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
        shutdownMsg.setFormatType(InternalFormat);
        send(&shutdownMsg, Messenger::DEST_RTC, false);

        Debug::trace("Session", getName() + HobString("requesting to reboot machine"));

        RebootMessage rm(false);  // passing in false will perform a Windows restart, not a power down
        dispatchEvent(&rm);
    }
    else
    {
        // display a dialog that the Shutdown/Reboot due to Firmware upgrade isn't going to proceed due to the inability to backup the database.
    }

    currentModeOpState = OPSTATE_UNKNOWN;
}


/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Making a bootable compact flash processing                                                ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

int Session::getMkBootStatus()
{
    return mkbootStatus;
}
void Session::setMkBootStatus(int status)
{
    mkbootStatus = status;
}

void Session::setupBootableCompactFlash(bool UNUSED(copyEntireFlash))
{
#if 0 //lcg rev 0.1
    VxWorksTasks::registerDeleteTaskHook(this);

    Debug::trace("Session.CompactFlash", getName() + HobString(" - before creating task to setupBootableCompactFlash(), send the 'Working' message."));
    if (responseMessageManager)
        responseMessageManager->addMessage(ErrSessionDev, SessionDevBootableCFInProgress, HfcResponse::RESPONSESTYLE_NOINPUT);

    taskDelay(10);              //      To make sure the message above makes it to the display before we take over the processor.

    compactFlashTaskId = taskSpawn("tCompactFlash", 150, 0, 32700, (FUNCPTR) makeBootableCompactFlash, (int) this, (bool) copyEntireFlash,0,0,0,0,0,0,0,0);

    Debug::trace("Session.CompactFlash", getName() + HobString(" - after creating task to setupBootableCompactFlash() - taskid: ") + HobString(Integer::toString(compactFlashTaskId)));
#endif //lcg rev 0.1
}

void Session::makeBootableCompactFlash(Session *UNUSED(mySession), bool UNUSED(copyEntireFlash))
{
#if 0 //lcg rev 0.1
    HobString cfDriveName = Configuration::getInstance()->getCompactFlashDriveName() + HobString("/bootrom.sys");
    HobString usbBulkDriveName = Configuration::getInstance()->getUSBBulkDriveName();

    int status = OK;

    util_filesystem::Directory thumbDir( Configuration::getInstance()->getUSBBulkDriveName());
    util_filesystem::File serviceKeyFile(thumbDir.getFullPath(),  Configuration::getInstance()->getServiceKeyFile());
    util_filesystem::File superKeyFile(thumbDir.getFullPath(),  Configuration::getInstance()->getSuperKeyFile());

    if ((serviceKeyFile.exists()) || (superKeyFile.exists()))
        status = -2;
    else
        status = mkbootUSBMSD(const_cast<char *> (usbBulkDriveName.c_str()), const_cast<char *> (cfDriveName.c_str()), copyEntireFlash);


    mySession->setMkBootStatus(status);
#endif //lcg rev 0.1
}


void Session::terminate(bool shouldSendUINotification, int whyTerminate)
{
	if (getStatus() == STATUS_TERMINATED)
		return;

    Debug::trace("Session", getName() + HobString(" - TERMINATING!!! "));


    if (shouldSendUINotification && !isCommSession())
    {
        /* NOTE:  Don't use the Session::send() method to send this message, since we can call terminate()
         * from within that method.  Instead, call the Messenger's send method directly.
         */
        Debug::trace("Session", getName() + HobString(" - Sending Terminate message to UI ..."));
        SessionMessage terminateMsg(SessionDev, SnTerminate);
        terminateMsg.setFormatType(BrowserFormat);

        GenericHfcObject gho;                           // this is not really the best way to do this, because there may be more than one item with the same enum
        if (whyTerminate == 2)                          // timedFuncInactivityAutoLogout
            gho.setData("1");
        else if (whyTerminate == 4)                     // SnTerminate
            gho.setData("2");
        else if (whyTerminate == 22)                    // SnCommEnd
            gho.setData("3");
        else if (whyTerminate == 92)                    // SnShutdownNowPrimary
            gho.setData("4");
        else
            gho.setData("0");                           // do not know why

        terminateMsg.setHfcObject(&gho);
        getMessenger().send(&terminateMsg, Messenger::DEST_UI);
    }

    if (!isPrimarySession())
    {
        if (currentMode == OpModeMonitor)
        {
            Debug::trace("Session.monitor", getName() + HobString(" - Sending SnMonitorModeOff message to primary console Session ..."));
            // This message is going to the primary console Session.
            SessionMessage msg(SessionDev, SnMonitorModeOff);
            msg.setSourceObject(this);
            msg.setFormatType(InternalFormat);
            getMessenger().send(&msg, Messenger::DEST_RTC);
        }
    }

    // Note: deregistering with the EventManager for all currently registered messages, happens in the destructor.


	// For Walmart: when a commSession restores a Product record, tell smBackend to process the tarball that sets up
	// plu fields for FPP, BuildAMeal, and anything else that may not be supported by their SMART system.
	// Is generic enough to do for any customer; there just won't be a tarball in the staging directory (where smBackend looks for it).
	if ( restoredProducts )
	{
		restoredProducts = false;

		string tarballName = Configuration::getInstance()->toStringCfgI(CfgInternal::STATICPRODUCTDATATARBALLNAME);

		// determine tarball's md5 checksum
		util_filesystem::File tarball(tarballName);
		tarball.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::STAGINGDIR));
		if ( tarball.exists() )
		{
			const string tarballFullPath( "\"" + tarball.getFullPath() + "\"" );
			const string command( PythonScripter::buildPythonCall("md5checksum.py", -1, tarballFullPath) );
			HobString checksumResult;
			Debug::trace("Session", getName() + " - getting md5checksum via python, command: " + command);
			PAL_absclass::PAL_executeWithResults(command, checksumResult);
			Debug::trace("Session", getName() + " - Data received from md5checksum.py: " + checksumResult);

			if ( checksumResult.size() )
				checksumResult.removeTrailingNewlinesAndSpaces();

			const string cmd( "\"processUpdate|" + tarballName + "|" + checksumResult + "|42\"" );
			const string command2( PythonScripter::buildPythonCall(Configuration::getInstance()->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), Configuration::getInstance()->toIntCfgI(CfgInternal::SMBACKENDPORT), cmd) );

			if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED))
			{
				Debug::trace("Session", "commSession has restored products, notifying smbackend to process Walmart's update, command: " + command);
				string connectionStatusOutput;
				PAL_absclass::PAL_executeWithResults(command2, connectionStatusOutput);
				// We don't care about any output from the command.
			}
			else
				Debug::trace("Session", "python disabled, python script not called, command: " + command2);
		}
		else
			Debug::trace("Session", "commSession has restored products, Walmart's tarball doesn't exist so don't need to tell smBackend");
	}


    // If this Session has a Backup/Restore Manager, tell it to stop running.
    if (backupRestoreManager)
    {
        EvolMessage cancel(BackupRestore, BRCancel, this);
        dispatchEvent(&cancel, backupRestoreManager);
    }
    PAL_absclass::PAL_sleep(100);

    AbstractSession::terminate(shouldSendUINotification);

    if ( commRequestedShutdown_ )
    {
        /* This is kind of kludgey since we pretend to have received this message from the UI. */
        GenericHfcObject hfcObject(inttostring(ShutdownRequest_Force));
        SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
        shutdownMsg.setFormatType(BrowserFormat);
        shutdownMsg.setHfcObject(&hfcObject);
        handleSessionMessage(&shutdownMsg);
    }
}


// Overridden from DBSession
//virtual
bool Session::addDialogMessage(ErrorEvent errorDevice, int errorCode, short responseStyle, const HobList *msgData, bool allowDuplicates)
{
    return (responseMessageManager) ? responseMessageManager->addMessage(errorDevice, errorCode, responseStyle, msgData, allowDuplicates) : false;
}
// Overridden from DBSession
//virtual
bool Session::removeDialogMessage(ErrorEvent errorDevice, int errorCode, short responseStyle)
{
    return (responseMessageManager) ? responseMessageManager->removeMessage(errorDevice, errorCode, responseStyle) : false;
}


void Session::handleNetworkDevMessage(SessionMessage *msg)
{
    //  STATUS result;

    if (!msg)
        return;

    switch(msg->getEventType())
    {
        /*
           case NetworkReqPing:
           {
           HfcRequestPing *pingRequest = dynamic_cast<HfcRequestPing *> (msg->getHfcObject());
           int pingRepeat = pingRequest->getRepeatCount();

           HfcPingResponse pingResponse;
           SessionMessage response(NetworkDev, NetworkPingResponse);
           response.setFormatType(BrowserFormat);
           response.setHfcObject(&pingResponse);

           while (pingRepeat-- > 0)
           {
           if ( Hobart::CPing::Ping( pingRequest->getAddress() ) )
           pingResponse.setReplies(4);
           else
           pingResponse.setReplies(0);

           pingResponse.setAttempts(4);

        // Send the ping response to the UI (Browser)
        int bytesSent = send(&response, Messenger::DEST_UI);
        if (bytesSent < 0)
        {
        // todo - check correct number of bytes sent?
        }
        }
        break;
        }
        case CfgTcpIpRead:
        {
        Configuration::getInstance()->loadCfgNetwork(Configuration::getInstance()->getCompactFlashDriveName().c_str());
        SessionMessage response(NetworkDev, CfgTcpIpWrite);
        response.setFormatType(BrowserFormat);
        CfgNetwork netConfig = Configuration::getInstance()->getCfgNetwork();
        netConfig.setFormatType(NetworkTcpIpFormat);
        response.setHfcObject(&netConfig);

        send(&response, Messenger::DEST_UI);

        if ((cfgNetworkTaskId != 0) || (cfgDelayedNetStartTaskId != 0))//if a previous configuration is still ongoing the kill the ability to start a new one and notify user
        {
        SessionMessage response(NetworkDev, CfgTcpConfigInProgress);
        response.setFormatType(BrowserFormat);

        send(&response, Messenger::DEST_UI);

        if (responseMessageManager)
        responseMessageManager->addMessage(ErrNetworkDev, ReinitializingNetwork, HfcResponse::RESPONSESTYLE_OK);
        }
        break;
        }
        case CfgDnsRead:
        {
        SessionMessage response(NetworkDev, CfgDnsWrite);
        response.setFormatType(BrowserFormat);
        CfgNetwork netConfig = Configuration::getInstance()->getCfgNetwork();
        netConfig.setFormatType(NetworkDnsFormat);
        response.setHfcObject(&netConfig);

        send(&response, Messenger::DEST_UI);
        break;
        }
        case CfgWirelessCardRead:
        {
        Configuration::getInstance()->loadCfgNetwork(Configuration::getInstance()->getCompactFlashDriveName().c_str());

        SessionMessage response(NetworkDev, CfgWirelessCardWrite);
        response.setFormatType(BrowserFormat);
        CfgNetwork netConfig = Configuration::getInstance()->getCfgNetwork();
        netConfig.setFormatType(NetworkWirelessCardFormat);
        response.setHfcObject(&netConfig);

        send(&response, Messenger::DEST_UI);
        break;
    }
    */
        case NetworkTftpUpgradeRead:
        {
            SessionMessage response(NetworkDev, NetworkTftpUpgradeWrite);
            response.setFormatType(BrowserFormat);
            CfgNetwork netConfig = Configuration::getInstance()->getCfgNetwork();
            netConfig.setFormatType(NetworkTftpSetupFormat);
            response.setHfcObject(&netConfig);

            send(&response, Messenger::DEST_UI);
            break;
        }
        case NetworkTftpUpgradeWrite:
        {
            Debug::trace("Session.CfgNetwork", getName() + HobString(" - handling NetworkTftpUpgradeWrite Message"));
            CfgNetwork *cfg = dynamic_cast<CfgNetwork *> (msg->getHfcObject());
            if (!cfg)
                return;

            Configuration::getInstance()->update(cfg);
            CfgNetwork netConfig = Configuration::getInstance()->getCfgNetwork();
            netConfig.show();
            break;
        }
        case NetworkTftpUpgradeSave:
        {
            Debug::trace("Session.CfgNetwork", getName() + HobString(" - handling NetworkTftpUpgradeSave Message"));
            CfgNetwork *cfg = dynamic_cast<CfgNetwork *> (msg->getHfcObject());

            Configuration::getInstance()->save(this, cfg);
            break;
        }
    /*
       case cfgLEAPRead:
       {
       Debug::trace("Session.CfgNetwork", getName() + HobString(" - handling cfgLEAPWrite Message"));
       SessionMessage response(NetworkDev, cfgLEAPWrite);
       response.setFormatType(BrowserFormat);
       CfgNetwork netConfig = Configuration::getInstance()->getCfgNetwork();
       netConfig.setFormatType(NetworkLeapFormat);
       response.setHfcObject(&netConfig);

       send(&response, Messenger::DEST_UI);
       break;
       }
       */
        default:
            break;
    }
}



// Private method
void Session::handleSystemMessage(SessionMessage *msg)
{
    if (!msg)
        return;

    Debug::trace("Session", HobString("handleSystemMessage()"));

    if (msg->getEventType() == SysPeripheralList && peripheralManager)
	    peripheralManager->handleMessage(msg);
}



/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Weigher message processing                                                                ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

HfcWeigherStatus *Session::getCurrentWeight() const
{
    return currentWeight;
}

void Session::deleteCurrentWeight()
{
    if (currentWeight)
    {
		// SOF-1684: queue up weigher message before we delete, may be in use right now
		currentWeightDoomed.push_back( currentWeight );
		currentWeight = NULL;

		if ( 8 < currentWeightDoomed.size() )
		{
			delete currentWeightDoomed.front();
			currentWeightDoomed.pop_front();
		}
    }

    // The following has the affect of clearing the weight field and ZERO indicator on the screen.
    HfcWeigherStatus hws;
    hws.clearPeripheralComm();
    SessionMessage msg(Weigher, WgStatus);
    msg.setFormatType(BrowserFormat);
    msg.setHfcObject(&hws);
    send(&msg, Messenger::DEST_UI);
}


// Private method
void Session::handleWeigherMessage(SessionMessage *msg)
{
    if (!msg)
        return;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

    //if (!SessionManager::getInstance()->isWeigherAttached() && currentMode != OpModeService /*&& msg->getEventType() != WgAttach*/)
    //{
    //    //if (responseMessageManager)
    //    //      responseMessageManager->addMessage(ErrWeigher, WeigherErrNotAttached, HfcResponse::RESPONSESTYLE_OK);
    //
    //    return;
    //}

    Debug::trace("Session", HobString("Session received a weigher message."));

    if (currentModeOp == Session::OP_WEIGHERCALIBRATE && weigherCalibration)
    {
        handleWeigherCalibration(msg);
        return;
    }

    switch(msg->getDeviceType())
    {
        case WeigherSystem:
        {
            switch(msg->getEventType())
            {
                case SysReqStatus:
                {
                    Debug::trace("Session.WgSystem", "handling Weigher System Request Status message");

                    if (msg == (SessionMessage *)NULL)
                        return;

                    //  Register for the WeigherSystem SysStatus message
                    registerEvent(WeigherSystem, SysStatus, false );
                    registerEvent(WeigherSystem, SysUsbWgStatus, false );

                    //  Not much to do here with a Weigher System Request Status message except pass it on.
                    //  But first, change the format from Browser to Internal.
                    msg->setFormatType(InternalFormat);

                    int bytesSent = send(msg, Messenger::DEST_RTC);
                    if (bytesSent < 0)
                    {
                        // todo - error handling?
                        Debug::trace("Session.WgSystem", "WeigherSystem, SysReqStatus RESPONSE MESSAGE COULD NOT BE SENT");
                    }
                    break;
                }

                case SysStatus:
                {
                    // LOG THIS IN DEBUG AND CHECK FOR TAPPING OF PACKAGE
                    Debug::trace("Session.WgSystem", "handling Weigher System Status message");
                    deregisterEvent(WeigherSystem, SysUsbWgStatus);

                    /* Change the format indicator, and send the same message to the Browser. */
                    msg->setFormatType(BrowserFormat);
                    int bytesSent = send(msg, Messenger::DEST_UI);
                    if (bytesSent < 0)
                    {
                        // todo - error handling?
                        Debug::trace("Session.WgSystem", "WeigherSystem, SysStatus RESPONSE MESSAGE COULD NOT BE SENT");
                    }
                    break;
                }

                case SysUsbWgStatus:
                {
                    Debug::trace("Session.WgSystem", "handling Weigher System Status message");
                    deregisterEvent(WeigherSystem, SysStatus);

					HfcUsbWeigherSystemStatus *uwss = dynamic_cast<HfcUsbWeigherSystemStatus *> (msg->getHfcObject());
					uwss->show();

#if defined( __linux__ )
                    // send version info to hook
                    if (( Configuration::getInstance()->getCustomerHooksEnabled() ) && ( SessionManager::getInstance()->getPrimaryConsoleSession() ))
                	{
                        UsbWeigherSystemStatus *status( dynamic_cast<UsbWeigherSystemStatus*>( msg->getHfcObject() ) );
						PeripheralManager *pm( SessionManager::getInstance()->getPrimaryConsoleSession()->getPeripheralManager() );

                        if (( status ) && ( pm ))
                        {
                            const std::string sw( status->getSoftwareVersion().getRevNTEPVersionStr() );
                            const std::string hw( status->getHardwareVersion().getRevMajMinStr() );

                            const HfcPeripheralList list( pm->getCurrentList() );

                            // UsbWeigherSystemStatus
                            status->show();

                            WeigherKind kind( WgNoWeigher );

                            if ( FirstWeigher == status->getWeigherType() )
                                kind = list.getFirstWeigher();
                            else if ( SecondWeigher == status->getWeigherType() )
                                kind = list.getSecondWeigher();

                            Debug::trace("Session", "usb weigher " + util_string::inttostring( status->getWeigherType() ) + " versions: sw " + sw + ", hw " + hw);
							hobart_cust_hook_update_peripheral_version( WeigherSystem, status->getWeigherType(), kind, sw.c_str(), hw.c_str() );
                        }
                    }
#endif

                    /* Change the format indicator, and send the same message to the Browser. */
                    msg->setFormatType(BrowserFormat);
                    int bytesSent = send(msg, Messenger::DEST_UI);
                    if (bytesSent < 0)
                    {
                        // todo - error handling?
                        Debug::trace("Session.WgSystem", "WeigherSystem, SysStatus RESPONSE MESSAGE COULD NOT BE SENT");
                    }
                    break;
                }

                case SysReqInquire:
                {
                    Debug::trace("Session.WgSystem", "handling Weigher System Request Inquire message");

                    if (msg == (SessionMessage *)NULL)
                        return;

                    registerEvent(WeigherSystem, SysInquire,false );

                    //  Not much to do here with a Weigher System Request Inquire message except pass it on.
                    //  But first, change the format from Browser to Internal.
                    msg->setFormatType(InternalFormat);

                    int bytesSent = send(msg, Messenger::DEST_RTC);
                    if (bytesSent < 0)
                    {
                        // todo - error handling?
                        Debug::trace("Session.WgSystem", "WeigherSystem, SysReqInquire RESPONSE MESSAGE COULD NOT BE SENT");
                    }
                    break;
                }

                case SysInquire:
                {
                    Debug::trace("Session.WgSystem", getName() + HobString(" - handling SysInquire"));
                    configurationManager->handleMessage(msg);
                    break;
                }

                case SysCalibSwitch:
                {
                    Debug::trace("Session", getName() + HobString(" - handling SysCalibSwitch"));

                    HfcSystemCalibrationSwitch *scs = dynamic_cast<HfcSystemCalibrationSwitch *> (msg->getHfcObject());
                    if (scs && scs->isSystemCalibrationSwitch())
                    {
                        if (currentModeOp == Session::OP_WEIGHERCONFIGURATION)
                        {
                            currentModeOp = Session::OP_UNKNOWN;
                            SessionMessage wri(Weigher, WgConfigurationEnd);    //      Eventually change to WgSwitchPressed
                            wri.setFormatType(BrowserFormat);

                            // Send the WgSwitchPressed to the UI (Browser)
                            int bytesSent = send(&wri, Messenger::DEST_UI);
                            if (bytesSent < 0)
                            {
                                // todo - check correct number of bytes sent?
                            }
                        }
                        deregisterEvent(WeigherSystem, SysCalibSwitch);
                    }
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case Weigher:
        {
			if (msg->getEventType() >= WgTestInit)
			{
				if (msg->getEventType() == WgTestSimulateWgStatus)
				{
					Debug::trace("Session", getName() + HobString(" - handling WgTestSimulateWgStatus"));

					GenericHfcObject *msgdata = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
					if (msgdata)
					{
						Debug::trace("Session", getName() + HobString(" - handling WgTestSimulateWgStatus, data entered = ") + msgdata->getData());
						HfcWeigherStatus *testWeight = new HfcWeigherStatus();
						testWeight->setDeviceType(Weigher);
						testWeight->setEventType(WgStatus);
						testWeight->setRawCounts((UINT32)12345);
						testWeight->clearManagerDeleted();
						testWeight->setPeripheralComm();
						if (currentWeight)
						{
							testWeight->setScaleFactor(currentWeight->getScaleFactor());
							testWeight->setDualRangeEnabled(currentWeight->isDualRangeEnabled());
						}
						else
						{
							testWeight->setScaleFactor(5);
							testWeight->setDualRangeEnabled(true);
						}

						HobString weight = msgdata->getData();
						long lweight = 0;
						if (weight.find(".") != std::string::npos)
							lweight = (long)(Double::valueOf(weight) * config->getWeightDivisor());
						else
							lweight = Long::valueOf(weight);

						//if (!testWeight->isDualRangeEnabled())
						//	lweight *= 10;
						lweight *= 10;
						testWeight->setCounts((UINT32) lweight);
						if (lweight == 0)
							testWeight->setWithinZeroQuarterGrad();

						SessionMessage counts(Weigher, WgStatus);
						counts.setFormatType(InternalFormat);
						counts.setHfcObject(testWeight);
						handleWeigherMessage(&counts);

						// SOF-3699
						fakeWeightMode = true;
						Debug::trace("Session", HobString("Setting fakeWeightMode to TRUE"));
					}
					break;
				}
				else if ( msg->getEventType() == WgEndTestSimulateWgStatus )
				{
					// SOF-3699
					fakeWeightMode = false;
					Debug::trace("Session", HobString("Setting fakeWeightMode to FALSE"));
				}
				else
				{
					msg->setFormatType(InternalFormat);
					send(msg, Messenger::DEST_RTC);
				}
			}
            else
            {
                switch(msg->getEventType())
                {
                    case WgStatus:
						// SOF-3699
						if (fakeWeightMode)
						{
							// Throw away any real WgStatus events
							Debug::trace("Session", HobString("Throw away WgStatus while in fakeWeightMode"));
							return;
						}
                        //SOF-4475
                        if( wghtDialogSet ) {
                            HfcWeigherStatus *newWeight = dynamic_cast<HfcWeigherStatus *> (msg->getHfcObject());                            
                            if( newWeight->isStableWeight() ) {
                                if( responseMessageManager ) {
                                    wghtDialogSet = false;
                                    responseMessageManager->removeMessage( ErrWrapper, 9744, HfcResponse::RESPONSESTYLE_NOINPUT );                                
                                    return;
                                }
                            }
                        }
                    case WgStatusAndPrint:
                    {
						// SOF-4432 - In self-serve mode, when displaying the label on the screen, the consumer has already finished the transaction by pressing
						// "Scan or Print". If they remove the product from the platter before pressing Print, to additionally print the label, you will have to reboot the scale.
						// The new weight(zero) was processed for the current plu, but it should have been ignored, since the transaction was already over.
						if (getSelfServeIgnoreWeightChangeForPrintToScreen())
						{
							Debug::trace("Session", getName() + HobString(" - ignoring WeigherStatus - selfServeIgnoreWeightChangeForPrintToScreen is true"));
							return;
						}

						//IR 3787 - We don't know what weigher type we have, so we don't know whether
						// to set dual range or not. So don't send weight data!
						if (config->getWeigherModel() == WmNone)
							return;
						
						Debug::trace("WrapperMode.WeigherStatus", "Session::handleWeigherMessage():  (EventType == WgStatusAndPrint) = " + booltostring((msg->getEventType() == WgStatusAndPrint)));
                        /* Just in case, check to make sure this weigher status came from the RTC.
                         * If not (received from the browser for example), ignore it.
                         * I should never get a status from anyone other than the Weigher (RTC).
                         */
                        if (msg->getFormatType() != InternalFormat)
                            return;

                        HfcWeigherStatus *newWeight = dynamic_cast<HfcWeigherStatus *> (msg->getHfcObject());
                        if (!newWeight)
                            return;

                        if (currentMode == OpModeService && currentWeight && *currentWeight == *newWeight)
                        {
                            Debug::trace("Weigher", getName() + HobString(" - handling WeigherStatus - Weight is the same as the current"));
                            return;
                        }

                        if (Debug::isTracing("Weigher"))
                        {
                            int counts = newWeight->getCounts();
                            int rawcounts = newWeight->getRawCounts();
                            HobString h = HobString("rawcounts=") + HobString(Integer::toString(rawcounts));
                            h += HobString("  counts=") + HobString(Integer::toString(counts));
                            Debug::trace("Weigher", getName() + HobString(" - handling WeigherStatus - ") + h);

                            newWeight->show();
                        }

                        if (responseMessageManager)
                            responseMessageManager->manageStatus(newWeight);

                        if (currentModeOp == Session::OP_WEIGHERINFO)// || isPrimarySession())
                        {
                            // Only send the raw weight on the weigher if there is no product currently selected. (per Mr. Crew, 4/16/02)
                            SessionMessage counts(Weigher, WgStatus);
                            counts.setFormatType(BrowserFormat);
                            newWeight->setFormatType(WeightCountsFormat);
                            counts.setHfcObject(newWeight);

                            // Send the current weight to the UI (Browser)
                            send(&counts, Messenger::DEST_UI);
                            return;
                        }

                        /* todo - If this Session is connected to a DataGateweigh Session,
                         *                              the current weight should probably not automatically be sent
                         *                              to the "UI", it may not have asked for it.
                         *                              However, when connected to a Browser, we always send the weight
                         *                              since the web page itself can be configured to display the
                         *                              weight, or not display it.
                         *                              Alternatively, any DG Session could just not register to receive weight statuses.
                         */

                        if (newWeight->isNewZero())
                            currentWeigherState = WeigherStateUnknown;

						bool hasCurrentProductInOperateMode = (productManager && productManager->getCurrentProduct() && isInOperateMode());
						bool changedDualRangeDirection = MetSig::checkToSwitchDualRangeDirection(newWeight, hasCurrentProductInOperateMode);

						if (productManager)
						{
							if (newWeight->isZeroStableWeight())
							{
								Debug::trace("sweetheartWeight", "*** Session - new weight is ZeroStableWeight - calling stopRecordingSweetheartWeight() ***");
								productManager->stopRecordingSweetheartWeight();
							}
							else
							{
								Debug::trace("sweetheartWeight", "*** Session - new weight - calling handleSweetheartWeight(newWeight) ***");
								productManager->handleSweetheartWeight(newWeight);
							}
						}

						if (productManager && productManager->getCurrentProduct() && isInOperateMode())
                        {
							Debug::trace("ProductManager.PlatterTare", "productManager=True AND haveCurrenetProduct()=True AND isInOperateMode=True");
                            if (productManager->getCurrentProduct()->isPrintingInProductionEntryMode())
                            {
                                SessionMessage sm(Weigher, WgStatus);
                                sm.setFormatType(BrowserFormat);
                                sm.setHfcObject(newWeight);
                                send(&sm, Messenger::DEST_UI);
                            }
                            else if (currentModeOp == Session::OP_PLATTERTAREMODE)
                            {
								Debug::trace("ProductManager.PlatterTare", "currentModeOp == Session::OP_PLATTERTAREMODE");
								Debug::trace("ProductManager.PlatterTare", "currentModeOp = " + HobString(Integer::toString(currentModeOp)));
                                productManager->handlePlatterTare(newWeight);
                            }
                            else
                            {
								Debug::trace("ProductManager.PlatterTare", "currentModeOp != Session::OP_PLATTERTAREMODE");
								Debug::trace("ProductManager.PlatterTare", "currentModeOp = " + HobString(Integer::toString(currentModeOp)) );
                                // tell the Product Manager that we have a new stable weight,
                                // since it needs to update the total price, net weight, etc. for the currently
                                // selected product (if it has a current product).

                                /* When in operate mode, with a current product selected, a Configuration flag in Weigher Configuration (in Service Mode)
                                 * determines whether or not to consider the total tare when determining if we are past the dual range limit
                                 * (gross weight vs. net weight) .  But it is the Product that keeps track of the total tare.  So we have to update the
                                 * Product and let it set the WeigherStatus dualRangeDirection attribute.  This Session doesn't determine the
                                 * dualRangeDirection, but can then get it from the WeigherStatus after the Product updates its total values.
                                 */

                                productManager->handleWeight(newWeight);

								if ( productManager->getCurrentProduct() )
								{
	                                ProductTransaction *trans = productManager->getCurrentProduct()->getCurrentTransaction();
	                                if (trans && trans->getWeigherStatus())
										MetSig::setDualRangeDirection( trans->getWeigherStatus()->getDualRangeDirection() );
								}

                                /*im not sure why this is called a second time for the same wgStatus message.
                                  all this does is popup or remove dialogs based on the current weigher status.
                                  it might have to with manageSelfServeMessages being called at the end of manageStatus.
                                  if (responseMessageManager)
                                  responseMessageManager->manageStatus(newWeight);*/
                            }

							// If we changed direction, we need to send the tare to the browser.
							if (changedDualRangeDirection)
							{
								WriteSingleField productUpdate;
								productUpdate.setRecordNum(1);
								productUpdate.setFieldCode(Product::fieldOffsetTareAValue);

								HobString newTare = "";
								productManager->getCurrentProduct()->getFieldAsString(Product::fieldOffsetTareAValue, newTare, BrowserFormatPLUOperateMode);
								productUpdate.setFieldNewValue(newTare);

								SessionMessage fieldUpdatedMsg(ProductDev, ProductWriteSingleField);
								fieldUpdatedMsg.setFormatType(BrowserFormat);
								fieldUpdatedMsg.setHfcObject(&productUpdate);

								send(&fieldUpdatedMsg, Messenger::DEST_UI);
							}
						}
                        else  // if (msg->getEventType() == WgStatus)
                        {
                            // Only send the raw weight on the weigher if there is no product currently selected. (per Mr. Crew, 4/16/02)
                            newWeight->serialize(BrowserFormat);
                            // Shouldn't need to serialize currentWeight; it was serialized
                            if (!currentWeight || requestedWeigherStatus ||
                                    currentWeight->getSerializedData() != newWeight->getSerializedData())
                            {
                                requestedWeigherStatus = false;
                                //Debug::trace("Session", getName() + HobString(" current weight data: ") + currw + HobString("* length of string: ") + HobString(Integer::toString(currw.size())));
                                //Debug::trace("Session", getName() + HobString("     new weight data: ") + neww + HobString("* length of string: ") + HobString(Integer::toString(neww.size())));
                                SessionMessage sm(Weigher, WgStatus);
                                sm.setFormatType(BrowserFormat);
                                sm.setHfcObject(newWeight);

                                // Send the current weight to the UI (Browser)
                                send(&sm, Messenger::DEST_UI);

                                // Update the customer display.
                                NumericField custDisplay(NmWeight, newWeight->getWeightDisplay().c_str());
                                send(&custDisplay, Messenger::DEST_RTC);

                                // Update the 'zero' annunciator
                                NmState state = (newWeight->isZeroStableWeight()) ? NmOn : NmOff;
                                NumericAnnunciator custAnnunciator(NmCode4, state);
                                send(&custAnnunciator, Messenger::DEST_RTC);
                            }
                            // else
                            //     Debug::trace("Session", getName() + HobString(" weight data is same as the previous, not sending to the browser. *****"));
                        }

                        // todo - Check for any other error or status messages that are supposed to be sent to the Browser.

						// Get rid of any existing weigher status object.
						if (currentWeight)
						{
							// SOF-1684: queue up weigher message before we delete, may be in use right now
							currentWeightDoomed.push_back( currentWeight );
							currentWeight = NULL;

							if ( 8 < currentWeightDoomed.size() )
							{
								delete currentWeightDoomed.front();
								currentWeightDoomed.pop_front();
							}
						}

                        // CAMERA
                        if (( isPrimarySession() ) && ( isInOperateMode() ) && ( !newWeight->isWeigherInError() ))
                        {
                            const bool haveProduct(( productManager ) && ( productManager->getCurrentProduct() ));

                            // If there is a stable weight or only small motion and is below the min weight to print
                            if (( newWeight->isStableWeight() || newWeight->isSmallMotion() ) && ( newWeight->getWeight() < config->getMinWeightToPrint() ) && ( !haveProduct ))
                            {
                                if ( cameraSystemReady )
                                    ; // do nothing, camera already ready
                                else
                                {
                                    Debug::trace( "Session.Camera", "camera ready" );
                                    cameraSystemReady = true;

                                    // reset camera prediction
                                    performHookAction( "ResetResult", false );
                                }
                            }

                            // all weight removed from platter when product exists and self service 'print to screen' mode
                            // treat this case like they printed a label for camera system
                            else if (( newWeight->isZeroStableWeight() ) && ( haveProduct ))
                            {
                                bool printToScreen( false );
                                
                                Debug::trace( "Session.Camera", "checking for print to screen" );

                                if ( Configuration::getInstance()->toBoolCfg( fieldOffsetCfgSelfServePrintToScreen ) )
                                    printToScreen = true;
                                else if ( SelfServOption_PrintToScreen == Configuration::getInstance()->toIntCfg( fieldOffsetCfgSelfServeIgnorePrinterErrors ) )
                                    printToScreen = true;
                                else if ( SelfServOption_PrintToScreenAndLabel == Configuration::getInstance()->toIntCfg( fieldOffsetCfgSelfServeIgnorePrinterErrors ) )
                                    printToScreen = true;
                                else if ( Configuration::getInstance()->toBoolCfg( fieldOffsetCfgSelfServeSearchKeypadFlashkeys ) )
                                    printToScreen = true;

                                Debug::trace( "Session.Camera", "print to screen " + HobString( Bool::toString( printToScreen ) ) );

                                if (( Configuration::getInstance()->isSelfServiceScale() ) && ( printToScreen ))
                                {
                                    const Product *p( productManager->getCurrentProduct() );

                                    if ( p )
                                    {
                                        const HobString action( "LabelLastResult|" + Integer::toString( p->getProductNum() ) + "|ScreenLabel" );

                                        Debug::trace( "Session.Camera", "have label" );

                                        // print!
                                        performHookAction( action.c_str() );
                                    }
                                }
                            }

                            // enable camera if possible
                            else if (( newWeight->isStableWeight() ) && ( config->getMinWeightToPrint() < newWeight->getWeight() ))
                            {
                                bool allowInItemScreen( false );

                                Debug::trace( "Session.Camera", HobString( "min weight to print: " ) + Long::toString( config->getMinWeightToPrint() ) );
                                Debug::trace( "Session.Camera", HobString( "net weight: " ) + Long::toString( newWeight->getWeight() ) );

                                // allow predictions after they select item (i.e. choose item, place item on scale, hit print)
                                if (( Configuration::getInstance()->isSelfServiceScale() ) && ( Configuration::getInstance()->getCameraSystemAllowedInItemScreen() ))
                                    allowInItemScreen = true;

                                if (( haveProduct ) && ( !allowInItemScreen ))
                                    ; // do nothing, have product
                                else if ( !cameraSystemReady )
                                    ; // do nothing, camera not ready
                                else
                                {
                                    Debug::trace( "Session.Camera", "enable camera due to stable weight" );
                                    enableCameraSystemProductRecognition( true );

                                    cameraSystemReady = false;
                                }
                            }
                        }

						/* If deleting and creating this object everytime a weigher status message is
						* received causes performance problems, we could look into just updating the
						* existing currentWeight object.
						*/
						currentWeight = newWeight;

						/* I want to take ownership of the HfcWeigherStatus object that is in this message.
						* Therefore, I have to tell the SessionMessage not to delete it automatically when the
						* SessionMessage is deleted, after processing.
						*/
						msg->setHfcObjectCreatedInternally(false);

						notifyExternalProcessOfWeight(currentWeight);

                        break;
                    }

                    case WgRezero:
                    {
                        // LAP backdoor - if they rezero the weigher, first clear all error messages that may be in the queues.
                        //if (responseMessageManager)
                        //    responseMessageManager->removeAllMessages();

                        //bool continueWithRezero = true;
                        if (config->isSelfServiceScale())
                        {
                            if (!currentWeight)
                            {
                                    if (responseMessageManager)
                                    responseMessageManager->addMessage(ErrWeigher, WeigherErrCantRezeroNegativeGrossWeight, HfcResponse::RESPONSESTYLE_OK);
                                break;
                            }
                            else if (isInOperateMode() && !isAnyAccessKeyPresent())
                            {
                                HobWeight weight = currentWeight->getWeight();
                                if (weight < 0)
                                    weight *= -1;
                                HobWeight weight2 = config->getUserRezeroWeightLimit();
                                if (weight > weight2)
                                {
                                    if (responseMessageManager)
                                        responseMessageManager->addMessage(ErrWeigher, WeigherErrCantRezeroNegativeGrossWeight, HfcResponse::RESPONSESTYLE_OK);
                                    break;
                                }
                            }
                        }

                        // I am using the current message, change the destination, and sending to the RTC.
                        // No need to change the SessionMessage's format, since there's no HfcObject within it.
                        msg->setFormatType(InternalFormat);
                        int bytesSent = send(msg, Messenger::DEST_RTC);
                        if (bytesSent < 0)
                        {
                            // todo - check correct number of bytes sent?
                        }

                        if (responseMessageManager)
                        {
							// Added log message as part of the remote scale maintenance feature, FHA 11/13
							if (responseMessageManager->addMessage(ErrWeigher, WeigherErrRezeroing, HfcResponse::RESPONSESTYLE_OK))	// FT-607 switched to ok instead of cancel
								MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Scale Rezero in progress.");
                            responseMessageManager->removeMessage(ErrWeigher, WeigherErrZeroOutOfMaintZone, HfcResponse::RESPONSESTYLE_NOINPUT);
                        }

                        currentWeigherState = Rezeroing;
                        break;
                    }

                    case WgReqStatus:
                    {
                        Debug::trace("Session", getName() + HobString(" - handling WgReqStatus"));
                        msg->setFormatType(InternalFormat);
                        send(msg, Messenger::DEST_RTC);
                        break;
                    }

                    case WgReqInquire:
                    {
                        Debug::trace("Session", getName() + HobString(" - handling WgReqInquire, passing along to configurationManager ..."));
                        configurationManager->handleMessage(msg);
                        break;
                    }

					case WgReqValueMax:
					{
						Debug::trace("Session", getName() + HobString(" - handling WgReqValueMax, passing along to configurationManager ..."));
						configurationManager->handleMessage(msg);
						break;
					}
					
					case WgValueMaxFitParams:
					{
						Debug::trace("Session", getName() + HobString(" - handling WgValueMaxFitParams, passing along to configurationManager ..."));
						configurationManager->handleMessage(msg);
						break;
					}

					case WgInquire:
                    {
                        Debug::trace("Session", getName() + HobString(" - handling WgInquire, passing along to configurationManager ..."));
                        configurationManager->handleMessage(msg);
                        break;
                    }

                    case WgConfig:
                    {
                        Debug::trace("Session", getName() + HobString(" - handling WgConfig, passing along to configurationManager ..."));
                        configurationManager->handleMessage(msg);
                        break;
                    }

					case WgRestoreWeigherSettings:
					{
						Debug::trace("Session", getName() + " - handling WgRestoreWeigherSettings");

						// SOF-1698
						if (isPrimarySession())
							configurationManager->handleMessage(msg);
						else
						{
							// A comm session can't just update the weigher config kept by the Configuration Manager, because it won't be populated.
							// The primary session's ConfigurationManager will always have the most up-to-date copy of the weigher configuration,
							// so get that one, set it in this Session's Configuration Manager, then update it with the data received.
							// Note that this is how the BackupRestoreManager dealt with receiving a WeigherConfig message in a .ht file.
							if (SessionManager::getInstance()->isWeigherAttached())
							{
								Session *priSession = SessionManager::getInstance()->getPrimaryConsoleSession();
								if (priSession)
								{
									ConfigurationManager *primaryConfigMgr = priSession->getConfigurationManager();
									if (primaryConfigMgr)
									{
										HfcWeigherConfig *currWgConfig = 0;
										HfcWeigherConfig *tmpWgConfig = primaryConfigMgr->getCurrentWeigherConfig();
										if (tmpWgConfig)
											currWgConfig = new HfcWeigherConfig(tmpWgConfig);

										if (currWgConfig)
										{
											configurationManager->setCurrentWeigherConfig(currWgConfig);
											configurationManager->handleMessage(msg);

											PAL_absclass::PAL_sleep(400);	//	Make sure we can get this to the Weigher before we possibly terminate.
											Debug::trace("Session", getName() + " - handling WgRestoreWeigherSettings - non-primary session handled WgConfig");
										}
									}
								}
								else
								{
									Debug::trace("Session", getName() + " - handling WgRestoreWeigherSettings - no primary session exists, can't handle WgConfig");
									MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Unable to change the settings for the Weigher.");
								}
							}
							else
							{
								MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Weigher is not attached.");
							}
						}


						break;
					}

                    case WgConfigurationBegin:              //      Eventually change to WgReqSwitchPress
                    {
                        Debug::trace("Session", getName() + HobString(" - handling WgReqSwitchPress"));
                        currentModeOp = Session::OP_WEIGHERCONFIGURATION;
                        Debug::trace("Session", getName() + HobString(" - registering for the WeigherSystem|SysCalibSwitch message"));
                        registerEvent(WeigherSystem, SysCalibSwitch);
                        break;
                    }

                    case WgConfigurationEnd:
                    {
                        currentModeOp = Session::OP_UNKNOWN;
                        break;
                    }

                    case WgCalibrationStep:
                    {
                        if (!weigherCalibration)
                        {
                            weigherCalibration = new WeigherCalibration(this);
                            currentModeOp = Session::OP_WEIGHERCALIBRATE;

                            // Delete the current weight received from the weigher.
                            // This allows weigher calibration to display the first weighStatus it receives.
                            if (currentWeight)
                            {
    							// SOF-1684: queue up weigher message before we delete, may be in use right now
    							currentWeightDoomed.push_back( currentWeight );
    							currentWeight = NULL;

    							if ( 8 < currentWeightDoomed.size() )
    							{
    								delete currentWeightDoomed.front();
    								currentWeightDoomed.pop_front();
    							}
                            }
                        }

                        if (weigherCalibration)
                            weigherCalibration->handleMessage(msg);
                        break;
                    }

                    case WgCalibrationEnd:
                    {
                        if (weigherCalibration)
                            weigherCalibration->handleMessage(msg);
                        else
                            // Just send it back to the browser.
                            send(msg, Messenger::DEST_UI);

                        break;
                    }

                    case WgReqInfo:
                    {
                        Debug::trace("Session", getName() + HobString(" - handling WgReqInfo, passing along to configurationManager ..."));
                        configurationManager->handleMessage(msg);

                        // Delete the current weight received from the weigher.
                        // This allows the weigher info page to display the first weighStatus it receives.
                        if (currentWeight)
                        {
							// SOF-1684: queue up weigher message before we delete, may be in use right now
							currentWeightDoomed.push_back( currentWeight );
							currentWeight = NULL;

							if ( 8 < currentWeightDoomed.size() )
							{
								delete currentWeightDoomed.front();
								currentWeightDoomed.pop_front();
							}
                        }
                        break;
                    }

                    case WgInfo:
                    {
                        Debug::trace("Session", getName() + HobString(" - handling WgInfo, passing along to configurationManager ..."));
                        configurationManager->handleMessage(msg);
                        break;
                    }

                    case WgSaveInfo:
                    {
                        Debug::trace("Session", getName() + HobString(" - handling WgSaveInfo, passing along to configurationManager ..."));
                        configurationManager->handleMessage(msg);
                        break;
                    }

                    case WgDoneInfo:
                    {
                        Debug::trace("Session", getName() + HobString(" - handling WgDoneInfo, passing along to configurationManager ..."));
                        configurationManager->handleMessage(msg);
                        break;
                    }

                    case WgAttach:
                    {
                        Debug::trace("Session", getName() + HobString(" - handling WgAttach, passing along to configurationManager ..."));
                        configurationManager->handleMessage(msg);
                        break;
                    }

                    case WgMode:
                    {
                        HfcWeigherMode *weigherMode = dynamic_cast<HfcWeigherMode *> (msg->getHfcObject());
                        WeigherMode wgMode(this, (WgOperatingMode) weigherMode->getMode());
                        send(&wgMode, Messenger::DEST_RTC);

                        if (weigherMode->getMode() == (WgOperatingMode) ScaleWeightTrackingMode)
                            setCurrentModeOp(OP_WEIGHERINFO);   //      Eventually change to OP_WEIGHERTRACKINGMODE
                        else if (getCurrentModeOp() == OP_WEIGHERINFO)
                            setCurrentModeOp(OP_UNKNOWN);

                        /*
                           HfcWeigherStatus testWeight;
                           testWeight.setDeviceType(Weigher);
                           testWeight.setEventType(WgStatus);
                           testWeight.setRawCounts((UINT32)15432);
                           SessionMessage counts(Weigher, WgStatus);
                           counts.setFormatType(InternalFormat);
                           counts.setHfcObject(&testWeight);
                           handleWeigherMessage(&counts);
                           */

                        break;
                    }

					case WgBeginRemoteWgConfig:
						// Only the primay console session is listening for this
						addDialogMessage(ErrWeigher, WeigherErrRemoteConfigurationStarted, HfcResponse::RESPONSESTYLE_NOINPUT);
						PAL_absclass::PAL_sleep(2000);

						break;

					case WgEndRemoteWgConfig:
						// Only the primay console session is listening for this
						removeDialogMessage(ErrWeigher, WeigherErrRemoteConfigurationStarted, HfcResponse::RESPONSESTYLE_NOINPUT);

						break;

					case WgReadAuditTrail:
					{
						if ( config->isCAT3AuditTrailLicensed() )
						{
							readingCat3AuditTrailForUI = true;
							if ( !readingCat3AuditTrailForComm && !readingCat3AuditTrailForHTe )
								handleReadCat3AuditTrail();
						}
						break;
					}

					case WgCat3AuditStatistics:
					{
						// We've received the Audit Trail statistics.  Just keep track of them.
                        Debug::trace("cat3", getName() + string(" - handling WgCat3AuditStatistics"));

						if (currentWeigherCat3Statistics)
						{
							delete currentWeigherCat3Statistics;
							currentWeigherCat3Statistics = 0;
						}

						msg->setHfcObjectCreatedInternally(false);
						currentWeigherCat3Statistics = dynamic_cast<HfcWeigherCat3Statistics *> ( msg->getHfcObject() );
						if (currentWeigherCat3Statistics)
						{
							if ( Debug::isTracing("cat3") )
								currentWeigherCat3Statistics->show();
							Debug::trace("cat3", string("WgCat3AuditStatistics - stats says the number of events in audit trail is: ") + Integer::toString((int)currentWeigherCat3Statistics->getTotalRecordedEvents()) );
						}
						break;
					}

					case WgCat3Records:
					{
						// We've received a set of 10 Audit Trail entries.  Add them to the object that keeps track of all of them, currentAllCat3AuditTrailRecords.
                        Debug::trace("cat3", getName() + string(" - handling WgCat3Records"));

						//Debug::trace("cat3", getName() + string(" WgCat3Records1 - stats says the number of events in audit trail is: ") + inttostring(currentWeigherCat3Statistics->getTotalRecordedEvents()) );

						if ( currentAllCat3AuditTrailRecords.null() || !currentWeigherCat3Statistics )
							break;

						// I didn't create an HFC 'Devices' class corresponding to the WeigherCat3AuditRecords class, since we aren't passing it to anyone else.
						// So I'm just creating the EvolMessage-level object with the data, getting that from the originalBuff in the SessionMessage.
						HfcWeigherCat3AuditRecords *cat3Records = dynamic_cast<HfcWeigherCat3AuditRecords *> ( msg->getHfcObject() );
						//cat3Records->show();

						Debug::trace("cat3", getName() + string(" - received page number: ") + inttostring(cat3Records->getPageNumber()));
						Debug::trace("cat3", getName() + string("  page index: ") + inttostring(cat3Records->getPageIndex()));

						// Since I'm keeping track of which page we're on, check that the page we received is for that page.
						// Just note it if it's different.
						if ( currentCat3AuditRecordPage != cat3Records->getPageNumber() )
							Debug::trace("cat3", getName() + string(" - handling WgCat3Records - page number incorrect: should be:") + inttostring(currentCat3AuditRecordPage) + string("  received: ") + inttostring(cat3Records->getPageNumber()) );
						// Since I'm keeping track of where we are on the current page, check which set of 10 records we just received to see if that is where we think we are.
						// Just note it if it's different.
						if ( (currentCat3NextPageRequests * CAT3_AUDIT_RECORDS_PER_MESSAGE) != cat3Records->getPageIndex() )
							Debug::trace("cat3", getName() + string(" - handling WgCat3Records - page index incorrect: should be:") + inttostring(currentCat3NextPageRequests * CAT3_AUDIT_RECORDS_PER_MESSAGE) + string("  received: ") + inttostring(cat3Records->getPageIndex()) );

						int validRecsInThisMessage = 0;
						Cat3AuditRecord rec;
						for (int i = 0; i < CAT3_AUDIT_RECORDS_PER_MESSAGE; i++)
						{
							// Iterate through all records in the message.  There's 10 in the message.
							// We first get the record in a Cat3AuditRecord struct.  The getRecord() call returns true if the record is valid,
							// false if not (record is empty / doesn't contain an audit record).
							bool valid = cat3Records->getRecord( i, &rec );
							if (valid)
							{
								// Create an object that holds the data from the struct.
								Cat3AuditTrailRecord *aRec = new Cat3AuditTrailRecord(&rec);
								if (aRec)
								{
									Debug::trace("cat3.WgCat3Records", getName() + string(" - handling WgCat3Records - have a valid audit rec struct"));

									//aRec->show();
									// Add it to the current set of Audit Trail records.

									validRecsInThisMessage++;

									SealableParamID param = aRec->getParameterId();
									if ( param == SOFTWARE_UPDATE || param == FRACTIONAL_PRICING || param == POUNDS_FOR_MULTIPLIER )
										currentNonWeigherCat3Records++;

									currentAllCat3AuditTrailRecords->addCat3AuditTrailRecord(aRec);
								}
							}
							else
								Debug::trace("cat3", getName() + string(" - handling WgCat3Records - have a INVALID audit rec struct"));

							//Debug::trace("cat3", getName() + string("WgCat3Records_loop - stats says the number of events in audit trail is: ") + inttostring(currentWeigherCat3Statistics->getTotalRecordedEvents()) );
							//Debug::trace("cat3", getName() + string("WgCat3Records_loop -  currentAllCat3AuditTrailRecords has records: ") + inttostring(currentAllCat3AuditTrailRecords->getRecordCount()) );

						}

						//Debug::trace("cat3", getName() + string("WgCat3Records2 - stats says the number of events in audit trail is: ") + inttostring(currentWeigherCat3Statistics->getTotalRecordedEvents()) );
						//Debug::trace("cat3", getName() + string("WgCat3Records_loop - done processing audit records in this message - total valid audit trail records in currentAllCat3AuditTrailRecords: ") + inttostring(currentAllCat3AuditTrailRecords->getRecordCount()) );

						Debug::trace("cat3", getName() + string(" - valid Recs In This Message: ") + inttostring(validRecsInThisMessage));

						bool keepRequestingMoreRecs = false;
						if ( ( currentAllCat3AuditTrailRecords->getRecordCount() < currentWeigherCat3Statistics->getTotalRecordedEvents() || validRecsInThisMessage > 0 ) && currentCat3AuditRecordPage < 16 )
							keepRequestingMoreRecs = true;

						// Request the next set of 10 audit trail records from the FPC
						if ( keepRequestingMoreRecs )
						{
							// If we haven't read through all 128 records from the current page, use the WgCat3ReqNextRecords to get the next set of 10.
							if ( currentCat3NextPageRequests < 12 )  // where 12 is the number of 'nextRec' messages we send for the current page
							{
								currentCat3NextPageRequests++;

								Debug::trace("cat3", getName() + string(" - sending request to read next set of records in the current page: ") + inttostring(cat3Records->getPageNumber()) + string("  currentCat3NextPageRequests: ") + inttostring(currentCat3NextPageRequests)  );

								WeigherGeneric cat3ReqStats(WgCat3ReqNextRecords, this);
								dispatchEvent(&cat3ReqStats);
							}
							else
							{
								// We've read through all of the audit records from the current page, go get the next page of audit trail records.
								currentCat3AuditRecordPage++;
								// Reset this counter to indicate that we are just starting the next page.
								currentCat3NextPageRequests = 0;

								Debug::trace("cat3", getName() + string(" - sending request to read next page of records - page: ") + inttostring(currentCat3AuditRecordPage) + string("  page index: ") + inttostring(currentCat3NextPageRequests) );

								if ( currentCat3AuditRecordPage < 16 )  // There's a total of 16 pages, 8 in sector 0 and 8 in sector 1
								{
									WeigherCat3PageRequest cat3ReqPage( this, currentCat3AuditRecordPage, currentCat3NextPageRequests );
									dispatchEvent(&cat3ReqPage);
								}
							}
						}

						if ( !keepRequestingMoreRecs || currentCat3AuditRecordPage >= 16 )
						{
							Debug::trace("cat3", getName() + string(" - done reading cat3 audit trail records - total records: ") + inttostring(currentAllCat3AuditTrailRecords->getRecordCount()));

							if ( readingCat3AuditTrailForUI )
							{
								readingCat3AuditTrailForUI = false;

								// Done reading the audit trail, send it to the UI.
								Debug::trace("cat3", getName() + string(" -  will now send them to the UI"));
								SessionMessage auditTrailmsg(Weigher, WgWriteAuditTrail);
								auditTrailmsg.setFormatType(BrowserFormat);
								auditTrailmsg.setHfcObject(currentAllCat3AuditTrailRecords.get());
								send(&auditTrailmsg, Messenger::DEST_UI);
							}

							if ( readingCat3AuditTrailForHTe )
							{
								readingCat3AuditTrailForHTe = false;

								Debug::trace("cat3", getName() + string(" -  will now write them to a file for HTe"));
								string filename = config->toStringCfgI(CfgInternal::CAT3AUDITTRAILDEFAULTFILENAME);

								// The following returns true if the records were saved to the file.
								if ( handleSaveCat3AuditTrailToDisk( filename ) )
								{
									const string cmd( "\"cat3AuditSaved|1|done\"" );
									const string command( PythonScripter::buildPythonCall(config->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), config->toIntCfgI(CfgInternal::SMBACKENDPORT), cmd, ipAddressForSavingAuditTrailResponse) );
									if (config->toBoolCfgI(CfgInternal::PYTHONENABLED))
									{
										Debug::trace("cat3", "before executing python script, command: " + command);
										string connectionStatusOutput;
										PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
										// We don't care about any output from the command.
									}
									else
										Debug::trace("cat3", "python disabled, python script not called, command: " + command);

									ipAddressForSavingAuditTrailResponse = "";
								}
							}

							if ( readingCat3AuditTrailForComm )
							{
								readingCat3AuditTrailForComm = false;
							}

							if ( readingCat3AuditTrailForSavingToDisk )
							{
								readingCat3AuditTrailForSavingToDisk = false;
								handleSaveCat3AuditTrailForUI( readingCat3AuditTrailForSavingToDiskFilename );
								readingCat3AuditTrailForSavingToDiskFilename = "";
							}

							if ( readingCat3AuditTrailForPrinting )
							{
								readingCat3AuditTrailForPrinting = false;
								handlePrintCat3AuditTrail();
							}
						}

						break;
					}

					case WgWriteAuditTrail:
						break;

					case WgPrintAuditTrail:
					{
						if ( config->isCAT3AuditTrailLicensed() )
						{
							// If we're currently reading the audit trail for one of the purposes for which we read it, then wait until it's done reading.
							// Don't initiate a read for it just to print it.  It should already be read, but if the log started to be read for SMBackend or Comm,
							// then wait until it is done being read.
							if ( readingCat3AuditTrailForUI || readingCat3AuditTrailForComm || readingCat3AuditTrailForHTe )
								readingCat3AuditTrailForPrinting = true;
							else
								handlePrintCat3AuditTrail();
						}
						break;
					}

					case WgSaveAuditTrailToDisk:
					{
						if ( config->isCAT3AuditTrailLicensed() )
						{
							GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
							if ( msgData )
							{
								// If we're currently reading the audit trail for one of the purposes for which we read it, then wait until it's done reading.
								// Don't initiate a read for it just to save it to disk.  It should already be read, but if the log started to be read for SMBackend or Comm,
								// then wait until it is done being read.
								if ( readingCat3AuditTrailForUI || readingCat3AuditTrailForComm || readingCat3AuditTrailForHTe )
								{
									readingCat3AuditTrailForSavingToDisk = true;
									readingCat3AuditTrailForSavingToDiskFilename = msgData->getData();
								}
								else
									handleSaveCat3AuditTrailForUI( msgData->getData() );
							}
						}
						break;
					}

					case WgReadAllAuditTrailForHTe:
					{
						if ( config->isCAT3AuditTrailLicensed() )
						{
							GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
							if ( msgData )
							{
								ipAddressForSavingAuditTrailResponse = msgData->getData();
								Debug::trace("cat3", string("primary console session handling WgReadWeigherConfigForHTe - ipAddress of socket is: ") + ipAddressForSavingAuditTrailResponse);
							}

							readingCat3AuditTrailForHTe = true;
							if ( !readingCat3AuditTrailForUI && !readingCat3AuditTrailForComm )
								handleReadCat3AuditTrail();
						}
						break;
					}

					case WgReadAllAuditTrailForComm:
					{
						// This is for the future, not yet implemented
						if ( config->isCAT3AuditTrailLicensed() )
						{
							readingCat3AuditTrailForComm = true;
							if ( !readingCat3AuditTrailForUI && !readingCat3AuditTrailForHTe )
								handleReadCat3AuditTrail();
						}
						break;
					}

					case WgReadWeigherConfigForHTe:
					{
						if ( config->isCAT3AuditTrailLicensed() )
						{
							Debug::trace("Session", "primary console session handling WgReadWeigherConfigForHTe ");
							ConfigurationManager *cfgMgr = getConfigurationManager();
							if (cfgMgr)
							{
								HfcWeigherConfig *wCfg = cfgMgr->getCurrentWeigherConfig();
								if ( wCfg && wCfg->getWeigherModel() != WmNone )
								{
									SessionMessage wCfgMsg(Weigher, WgRestoreWeigherSettings, this);
									wCfgMsg.setFormatType(EvolutionTaggedFormat);
									wCfg->setEventType(WgRestoreWeigherSettings);
									wCfgMsg.setHfcObject(wCfg);

									const char *serializedData = wCfgMsg.serialize();
									int serializedLength = wCfgMsg.getSerializedLength();

									string outData(serializedData, serializedLength);
									Debug::trace("cat3", string("primary console session handling WgReadWeigherConfigForHTe - serialized weigher config: ") + outData);
									Debug::trace("cat3", string("primary console session handling WgReadWeigherConfigForHTe - serialized weigher config: ") + HobString(Hex::toString(serializedData, serializedLength)));

									File wCfgOutput(config->toStringCfgI(CfgInternal::CAT3WEIGHERCONFIGFILENAME));
									Debug::trace("cat3", string("WgReadWeigherConfigForHTe - File object's full path: ") + wCfgOutput.getFullPath());
									if ( wCfgOutput.exists() )
									{
										Debug::trace("cat3", "WgReadWeigherConfigForHTe - output file exists so deleting it up front ");
										wCfgOutput.remove();
									}

									std::ofstream output(wCfgOutput.getFullPath().c_str(), std::ios::out|std::ios::app);
									Debug::trace("cat3", "WgReadWeigherConfigForHTe - just opened output file ");
									output << outData << CMReaderWriterEvolTag::EVOL_TAGGED_SEPARATOR /*0x1e*/;
									output.close();

#if defined ( __linux__ )
									system("sync");
#endif

									if ( wCfgOutput.exists() )
									{
										string ipAddr;
										GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
										if ( msgData )
										{
											ipAddr = msgData->getData();
											Debug::trace("cat3", string("primary console session handling WgReadWeigherConfigForHTe - ipAddress of socket is: ") + ipAddr);
										}

										const string cmd( "\"cat3WeigherConfigSaved|1|done\"" );
										const string command( PythonScripter::buildPythonCall(config->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), config->toIntCfgI(CfgInternal::SMBACKENDPORT), cmd, ipAddr) );
										if (config->toBoolCfgI(CfgInternal::PYTHONENABLED))
										{
											Debug::trace("cat3", "before executing python script, command: " + command);
											string connectionStatusOutput;
											PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
											// We don't care about any output from the command.
										}
										else
											Debug::trace("cat3", "python disabled, python script not called, command: " + command);
									}
								}
							}

						}
						break;
					}

					case WgCat3RecordStatus:
					{
                        Debug::trace("cat3", getName() + string(" - handling WgCat3RecordStatus"));
						HfcWeigherCat3RecordStatus *cat3RecordStatus = dynamic_cast<HfcWeigherCat3RecordStatus *> ( msg->getHfcObject() );
						Debug::trace("cat3", std::string("record saved: ") + Bool::toString(cat3RecordStatus->getStatus()) );
						break;
					}

					case WgCat3LogEraseComplete:
					{
						Debug::trace("theUniverse", "Finished clearing Cat3 Audit Trail");
						responseMessageManager->removeMessage(ErrUnknown, 1, HfcResponse::RESPONSESTYLE_NOINPUT);

						deregisterEvent(Weigher, WgCat3LogEraseComplete);

						MessageLogger::logMessage( MessageLogger::InfoLog, "Finished clearing Cat3 Audit Trail" );

						break;
					}

					case WgResetFactoryDefaults:
					{
						Debug::trace("Session", "received: WgResetFactoryDefaults, passing along to configurationManager ...");

						configurationManager->handleMessage(msg);

						break;
					}

					case WgResetFactoryDefaultsComplete:
					{
						Debug::trace("Session", "received: WgResetFactoryDefaultsComplete, passing along to configurationManager ...");

						configurationManager->handleMessage(msg);

						break;
					}

					default:
                        break;
                }
            }
            break;
        }

        default:
            break;
    }
}


void Session::handleKPCMessage(SessionMessage *msg)
{
    //NOTE: SOF-5607 Removed the creation of the boot upgrader. This is now a manual process so an instance 
    //               will need to be created before the KPC bootloader can be upgraded to a new version.
    //               This was done to resolve issue with core upgrade bricking KPC units when the bootloader
    //               needed to be upgraded. This need to be a manual process so that power cannot be removed
    //               or a reboot occur during upgrade. If the upgrade does not complete the KPC is bricked. 
    //KPCBootUpgrader::getInstance()

	if (!msg)
		return;

	switch (msg->getDeviceType())
	{
		case KPCSystem:
		{
			Debug::trace("Session.handleKPCMessage", "case KPCSystem()");
/*
			switch (msg->getEventType())
			{
				// stubbed for future use
			}
*/
		}
		break;

		case KPCBootloader:
		{
			Debug::trace("Session.handleKPCMessage", "case KPCBootloader");

			// We probably are not going to even use laoder messages so I prolly dont even need this whole block
			switch (msg->getEventType())
			{
				// from RTC
				case KpcBootUpgradeRequired:
				{
					// Step 1 - 1st message from RTC to UI telling UI we need to upgrade
					// Generic message FROM UI 20,28

					Debug::trace("Session.KPCBootLoader", std::string("case KpcBootUpgradeRequired") + getName());

					// Nothing much to do except change the destination and forward it on
					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);

					Debug::trace("Session.KPCBootLoader", std::string("DONE case KpcBootUpgradeRequired"));

					break;
				}

				// From the UI
				case KpcBootReqUpgradeInfo:
				{
					// Step 2 - 1st message from UI to RTC - well if we need to upgrade, send me some info
					// Generic message FROM UI 20,30
					Debug::trace("Session.KPCBootLoader", "handling KPCBootUpdater case KpcBootReqUpgradeInfo");

					if (msg == (SessionMessage *)NULL)
						return;

					KPCBootGeneric reqInfo(KPCBootloader,KpcBootReqUpgradeInfo, this);
					dispatchEvent(&reqInfo);

					Debug::trace("Session.KPCBootLoader", std::string("DONE case KpcBootReqUpgradeInfo"));

					break;
				}

				// From RTC
				case KpcBootUpgradeInfo:
				{
					// Step 3 - 2nd message from RTC to UI - here is the info you requested Mr UI
					// Upgrade Info message FROM UI 20,31
					Debug::trace("SessionPrinter.KPCBootLoader", std::string("case KpcBootUpgradeInfo") + getName());

					HfcKPCLoaderInfo *pHfcKPCInfo = dynamic_cast<HfcKPCLoaderInfo *> (msg->getHfcObject());

					SessionMessage response(KPCBootloader, KpcBootUpgradeInfo);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcKPCInfo);

					send(&response, Messenger::DEST_UI);

					Debug::trace("Session.KPCBootLoader", std::string("DONE case KpcBootUpgradeInfo"));

					break;
				}

				// from the UI
				case KpcBootUpgradeUIReady:
				{
					// Step 4 - 2nd message from UI to RTC - ok RTC, we are ready to receive your upgrade
					// Generic message FROM UI 20,29
					Debug::trace("Session.KPCBootLoader", "handling KPCBootUpdater case KpcBootUpgradeUIReady");

					if (msg == (SessionMessage *)NULL)
						return;

					KPCBootGeneric uiReady(KPCBootloader, KpcBootUpgradeUIReady, this);
					dispatchEvent(&uiReady);

					Debug::trace("Session.KPCBootLoader", std::string("DONE case KpcBootUpgradeUIReady"));

					break;
				}

				// From RTC
				case KpcBootStartUpgrade:
				{
					// Step 5 - 3rd message from RTC to UI - ok Mr UI, getting RTC ready to start upgrade so you better be ready
					// Generic message FROM UI 20,24
					Debug::trace("Session.KPCBootLoader", std::string("case KpcBootStartUpgrade") + getName());

					// Nothing much to do except change the destination and forward it on
					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);

					Debug::trace("Session.KPCBootLoader", std::string("DONE case KpcBootStartUpgrade"));

					break;
				}

				// FROM RTC
				case KpcBootUpgradeProgress:
				{
					// Step 6 - 4th message from RTC to UI - here is the upgrade progress Mr UI
					// Upgrade Progress message FROM UI 20,26
					Debug::trace("SessionPrinter.KPCBootLoader", std::string("case KpcBootUpgradeProgress") + getName());

					HfcKPCLoaderProgress *pHfcKPCProgress = dynamic_cast<HfcKPCLoaderProgress *> (msg->getHfcObject());

					SessionMessage response(KPCBootUpdater, KpcBootUpgradeProgress);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcKPCProgress);

					send(&response, Messenger::DEST_UI);

					Debug::trace("Session.KPCBootLoader", std::string("DONE case KpcBootUpgradeProgress"));
					break;
				}

				// From RTC
				case KpcBootUpgradeStatus:
				{
					// Step 7 - 5th message from RTC to UI - here is the upgrade status Mr UI
					// Upgrade Status message FROM UI 20,26
					Debug::trace("SessionPrinter.KPCBootLoader", std::string("case KpcBootUpgradeStatus") + getName());

					HfcKPCBootLoaderStatus *pHfcKPCBootLoaderStatus = dynamic_cast<HfcKPCBootLoaderStatus *> (msg->getHfcObject());

					SessionMessage response(KPCBootUpdater, KpcBootUpgradeStatus);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcKPCBootLoaderStatus);

					send(&response, Messenger::DEST_UI);

					Debug::trace("Session.KPCBootLoader", std::string("DONE case KpcBootUpgradeStatus"));
					break;
				}

				// Not sure yet we are going to use this one.  I dont think NGW did.  I think they used a status message to finish
				case KpcBootUpgradeFinished:
				{
					// Upgrade Finished message FROM RTC 20,27
					Debug::trace("SessionPrinter.KPCBootLoader", std::string("case KpcBootUpgradeFinished") + getName());
					break;
				}	

                // UI sent a cancel request to cancel upgrade - SOF-3478
                case KpcBootUpgradeCancel:
                {
					Debug::trace("Session.KPCBootLoader", std::string("case KpcBootUpgradeCancel"));

                    KPCBootGeneric cancelUpgrade(KPCBootloader, KpcBootUpgradeCancel, this);
					dispatchEvent(&cancelUpgrade);

                    Debug::trace("Session.KPCBootLoader", std::string("DONE case KpcBootUpgradeCancel"));

                    break;
                }

                case KpcBootUpgradeRequested:
                {
                    Debug::trace("Session.KPCBootLoader", std::string("case KpcBootUpgradeRequested"));

                    KPCBootGeneric upgradeRequest(KPCBootloader, KpcBootUpgradeRequested, this);
                    dispatchEvent(&upgradeRequest);
                    
                    break;
                }

                case KpcBootUpgradeUpdateUI:
                {
                    Debug::trace("Session.KPCBootLoader", std::string("case KpcBootUpgradeUpdateUI"));

                    // forward the message
					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);

                    break;
                }
			}
		}
		break;

		case KPCBootUpdater:
		{
			Debug::trace("Session.handleKPCMessage", "case KPCBootUpdater");
			switch (msg->getEventType())
			{
				// from RTC
				case KpcBootUpgraderUpgradeRequired:
				{
					// Step 1 - 1st message from RTC to UI telling UI we need to upgrade
					// Generic message FROM UI 21,28

					Debug::trace("Session.KPCBootUpdater", std::string("case KpcBootUpgraderUpgradeRequired") + getName());

					// Nothing much to do except change the destination and forward it on
					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);

					Debug::trace("Session.KPCBootUpdater", std::string("DONE case KpcBootUpgraderUpgradeRequired"));

					break;
				}

				// From the UI
				case KpcBootUpgraderReqUpgradeInfo:
				{
					// Step 2 - 1st message from UI to RTC - well if we need to upgrade, send me some info
					// Generic message FROM UI 21,30
					Debug::trace("Session.KPCBootUpdater", "handling KPCBootUpdater case KpcBootUpgraderReqUpgradeInfo");

					if (msg == (SessionMessage *)NULL)
						return;

					KPCBootUpgraderGeneric reqUpgradeInfo(KPCBootUpdater,KpcBootUpgraderReqUpgradeInfo, this);
					dispatchEvent(&reqUpgradeInfo);

					Debug::trace("Session.KPCBootUpdater", std::string("DONE case KpcBootUpgraderReqUpgradeInfo"));

					break;
				}

				// From RTC
				case KpcBootUpgraderUpgradeInfo:
				{
					// Step 3 - 2nd message from RTC to UI - here is the info you requested Mr UI
					// Upgrade Info message FROM UI 21,31
					Debug::trace("Session.KPCBootUpdater", std::string("START case KpcBootUpgraderUpgradeInfo ") );

					HfcKPCUpgradeInfo *pHfcKPCUpgradeInfo = dynamic_cast<HfcKPCUpgradeInfo *> (msg->getHfcObject());

					//Debug::trace("Session.KPCBootUpdater", std::string("call pHfcKPCUpgradeInfo->show()"));
					//pHfcKPCUpgradeInfo->show();
					//Debug::trace("Session.KPCBootUpdater", std::string("DONE pHfcKPCUpgradeInfo->show()"));

					SessionMessage response(KPCBootUpdater, KpcBootUpgraderUpgradeInfo);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcKPCUpgradeInfo);

					//Debug::trace("Session.KPCBootUpdater", std::string("DONE setting response object CALL SEND() ") );

					send(&response, Messenger::DEST_UI);

					Debug::trace("Session.KPCBootUpdater", std::string("DONE case KpcBootUpgraderUpgradeInfo"));

					break;
				}

				// from the UI
				case KpcBootUpgraderUpgradeUIReady:
				{
					// Step 4 - 2nd message from UI to RTC - ok RTC, we are ready to receive your upgrade
					// Generic message FROM UI 21,29
					Debug::trace("Session.KPCBootUpdater", "handling KPCBootUpdater case KpcBootUpgraderUpgradeUIReady");

					if (msg == (SessionMessage *)NULL)
						return;

					KPCBootUpgraderGeneric uiReady(KPCBootUpdater, KpcBootUpgraderUpgradeUIReady, this);
					dispatchEvent(&uiReady);

					Debug::trace("Session.KPCBootUpdater", std::string("DONE case KpcBootUpgraderUpgradeUIReady"));

					break;
				}

				// From RTC
				case KpcBootUpgraderStartUpgrade:
				{
					// Step 5 - 3rd message from RTC to UI - ok Mr UI, getting RTC ready to start upgrade so you better be ready
					// Generic message FROM UI 21,24
					Debug::trace("Session.KPCBootUpdater", std::string("case KpcBootUpgraderStartUpgrade") + getName());

					// Nothing much to do except change the destination and forward it on
					msg->setFormatType(BrowserFormat);
					send(msg, Messenger::DEST_UI);

					Debug::trace("Session.KPCBootUpdater", std::string("DONE case KpcBootUpgraderStartUpgrade"));

					break;
				}

				// FROM RTC
				case KpcBootUpgraderUpgradeProgress:
				{
					// Step 6 - 4th message from RTC to UI - here is the upgrade progress Mr UI
					// Upgrade Progress message FROM UI 21,26
					Debug::trace("SessionPrinter.KPCBootUpdater", std::string("case KpcBootUpgraderUpgradeProgress") + getName());

					HfcKPCUpgradeProgress *pHfcKPCUpgradeProgress = dynamic_cast<HfcKPCUpgradeProgress *> (msg->getHfcObject());

					Debug::trace("Session.KPCBootUpdater", std::string("call pHfcKPCUpgradeProgress->show()"));
					pHfcKPCUpgradeProgress->show();
					Debug::trace("Session.KPCBootUpdater", std::string("DONE pHfcKPCUpgradeProgress->show()"));

					SessionMessage response(KPCBootUpdater, KpcBootUpgraderUpgradeProgress);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcKPCUpgradeProgress);

					send(&response, Messenger::DEST_UI);

					break;
				}

				// From RTC
				case KpcBootUpgraderUpgradeStatus:
				{
					// Step 7 - 5th message from RTC to UI - here is the upgrade status Mr UI
					// Upgrade Status message FROM UI 21,26
					Debug::trace("SessionPrinter.KPCBootUpdater", std::string("case KpcBootUpgraderUpgradeStatus") + getName());
					
					HfcKPCBootUpgraderStatus *pHfcKPCBootUpgraderStatus = dynamic_cast<HfcKPCBootUpgraderStatus *> (msg->getHfcObject());

					SessionMessage response(KPCBootUpdater, KpcBootUpgraderUpgradeStatus);
					response.setFormatType(BrowserFormat);
					response.setHfcObject(pHfcKPCBootUpgraderStatus);

					send(&response, Messenger::DEST_UI);
					
					Debug::trace("Session.KPCBootUpdater", std::string("DONE case KpcBootUpgraderUpgradeStatus"));
					break;
				}

				// Not sure yet we are going to use this one.  I dont think NGW did.  I think they used a status message to finish
				case KpcBootUpgraderUpgradeFinished:
				{
					Debug::trace("SessionPrinter.KPCBootUpdater", std::string("case KpcBootUpgraderUpgradeFinished") + getName());
					break;
				}              
			}
		}
		break;
	}
}


void Session::handleBoardSiteMessage(SessionMessage *msg)
{
    
    switch ( msg->getEventType() )
    {
        case BrdSiteBoardTestCompleteUI: {
            Debug::trace("Session.BoardSite", "handleBoardSiteMessage() BrdSiteBoardTestCompleteUI");
			HfcBoardSiteTestCompleteUI *pHfcBoardSiteMsg = dynamic_cast<HfcBoardSiteTestCompleteUI *> (msg->getHfcObject());

			SessionMessage response( BoardSite, BrdSiteBoardTestCompleteUI );
			response.setFormatType( BrowserFormat );
			response.setHfcObject( pHfcBoardSiteMsg );

			send( &response, Messenger::DEST_UI );
            break;
        }
        case BrdSiteReqCompleteResults: {
            Debug::trace("Session.BoardSite", "handleBoardSiteMessage() BrdSiteReqCompleteResults");
            msg->setFormatType(InternalFormat);
            send( msg, Messenger::DEST_RTC );
            break;
        }
        case BrdSitePercentCompete: {
            Debug::trace("Session.BoardSite", "handleBoardSiteMessage() BrdSitePercentCompete");
            util_filesystem::File  boardSiteFile( boardSiteFile_ );
            
            HfcBoardSitePercentComplete *pHfcBoardSiteMsg = dynamic_cast<HfcBoardSitePercentComplete *> (msg->getHfcObject());
            //do we to register for board site msg?
            if ( boardSiteFile.exists() )  {                
                
                SessionMessage response( BoardSite, BrdSitePercentCompete );
                response.setFormatType( BrowserFormat );
                response.setHfcObject( pHfcBoardSiteMsg );

                send( &response, Messenger::DEST_UI ); 
            } else {
                if( pHfcBoardSiteMsg->getPercentComplete() == 100 ) {
                    const string cmd( "lvdsComplete" ); 
                    const string command( PythonScripter::buildPythonCall(Configuration::getInstance()->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), Configuration::getInstance()->toIntCfgI(CfgInternal::PYTHONDBBACKUPRESPONSEPORT), cmd) );

                    if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) ) {
                        Debug::trace("Session.BoardSite", "Python script, command: " + command);
                        string status;
                        PAL_absclass::PAL_executeWithResults(command, status);

                    }
                }
            }
           
            break;
        }
        case BrdSiteTestStatus: {
            Debug::trace("Session.BoardSite", "handleBoardSiteMessage() BrdSiteTestStatus");
            HfcBoardSiteTestStatus *pHfcBoardSiteMsg = dynamic_cast<HfcBoardSiteTestStatus *> (msg->getHfcObject());
            
			SessionMessage response( BoardSite, BrdSiteTestStatus );
			response.setFormatType( BrowserFormat );
			response.setHfcObject( pHfcBoardSiteMsg );

			send( &response, Messenger::DEST_UI ); 
            break;
        }
        default:
            Debug::trace("Session", "handleBoardSiteMessage() Unhandled event type!");
    }
}


void Session::handleReadCat3AuditTrail()
{
	// This message comes from the browser to read the audit trail and send it back.
	// Since we have to get the info from the weigher, this is an asynchronous request,
	// requiring many steps to accumulate the audit trail info needed for the user.

    Debug::trace("cat3", getName() + string(" - handling WgReadAuditTrail, initialization of reading the audit trail."));

	// Dispose of the old set of audit records.  Since it's wrapped in a smartPtr, don't 'delete' the smartPtr, just set it to 0, which will delete the object in the smartPtr
	if (currentAllCat3AuditTrailRecords.notNull())
		currentAllCat3AuditTrailRecords = 0;

	// Create a new object to hold all of the audit trail entries.
	currentAllCat3AuditTrailRecords = new AllCat3AuditTrailRecords();

	// Keep track of which page we are on
	currentCat3AuditRecordPage = 0;

	// The FPC will give us 10 log entries at a time, so we have to use the WgCat3ReqNextRecords message 12 times
	// to get all of the log entries stored in a single page.  Keep track of how many times we called 'next'.
	currentCat3NextPageRequests = 0;

	// Reset the number of audit trail messages related to the sealable configuration parameters that are stored in the serial flash of the system
	// controller board, not kept in the peripheral controller board serial flash.  The cat3Statistics do not keep track of the number of
	// audit records written for these sealable configuration parameters that are stored in the serial flash of the system controller board.
	currentNonWeigherCat3Records = 0;

	// Request the Audit Trail statistics, mostly so we know how many entries are in the log
	WeigherGeneric cat3ReqStats(WgCat3ReqStatistics, this);
	dispatchEvent(&cat3ReqStats);

	// Start reading the first page of audit trail entries.
	// Each page contains 128 log entries, each 32 bytes.
	// The FPC will give us 10 log entries at a time.
	WeigherCat3PageRequest cat3ReqPage( this, 0, 0 );
	dispatchEvent(&cat3ReqPage);

/* The following was test code to create records before I was actually reading them from the peripheral controller board
	Cat3AuditRecord rec;
	rec.event = CONFIGURATION;
	rec.eventType = LOCAL_EVENT;
	rec.parameterId = FRACTIONAL_PRICING;
	rec.epochTime = 54567800;
	rec.newParamValue = 256;
	rec.oldParamValue = 0;
	for (int i = 0; i < 4; i++)
	{
		// Create an object that holds the data from the struct.
		Cat3AuditTrailRecord *aRec = new Cat3AuditTrailRecord(&rec);
		if (aRec)
		{
			aRec->show();
			// Add it to the current set of Audit Trail records.
			currentAllCat3AuditTrailRecords->addCat3AuditTrailRecord(aRec);
		}
	}

	// Done reading the audit trail, send it to the UI.
	SessionMessage auditTrailmsg(Weigher, WgWriteAuditTrail);
	auditTrailmsg.setFormatType(BrowserFormat);
	auditTrailmsg.setHfcObject(currentAllCat3AuditTrailRecords);
	send(&auditTrailmsg, Messenger::DEST_UI);

	setCurrentModeOp(OP_UNKNOWN);
*/
}


// The following returns true if the records were saved to the file.
bool Session::handleSaveCat3AuditTrailToDisk( const string filename )
{
	if ( currentAllCat3AuditTrailRecords.null() )
		return false;

	Debug::trace("cat3", getName() + string(" handleSaveCat3AuditTrailToDisk - filename: ") + filename);

	if ( filename.size() && currentAllCat3AuditTrailRecords.notNull() )
	{
		currentAllCat3AuditTrailRecords->saveAuditTrailToDisk(filename);

		if ( Debug::isTracing("cat3.AllAuditTrailRecords") )
		{
			const std::string fname(filename);
			util_filesystem::Filename aoutputFile(fname);
			string newFileName = aoutputFile.getFileNameBase();
			newFileName.append("ALL");
			aoutputFile.setFileNameBase(newFileName);

			Debug::trace("cat3.AllAuditTrailRecords", getName() + string(" WgSaveAuditTrailToDisk - filename: ") + aoutputFile.getFullPath());
			currentAllCat3AuditTrailRecords->saveAuditTrailToDisk(aoutputFile.getFullPath(), true);
		}

#if defined ( __linux__ )
		system("sync");
#endif

		return true;
	}
	else
		return false;
}

void Session::handleSaveCat3AuditTrailForUI( string filename )
{
	if ( currentAllCat3AuditTrailRecords.null() )
		return;

	// The following returns true if the records were saved to the file.
	if ( handleSaveCat3AuditTrailToDisk( filename ) )
	{
		SessionMessage savedMsg(Weigher, WgSaveAuditTrailToDiskFinished);
		savedMsg.setFormatType(BrowserFormat);
		send(&savedMsg, Messenger::DEST_UI);
	}
}

void Session::handlePrintCat3AuditTrail()
{
	if ( currentAllCat3AuditTrailRecords.null() )
		return;

	SessionMessage reportMsg(ReportManagerDev, StartReport);
	HfcStartReport rpt;
	rpt.setPrinterType(FirstPrinter);
	rpt.setReportType(WeigherAuditTrailType);
	reportMsg.setHfcObject(&rpt);
	reportMsg.setFormatType(BrowserFormat);

	handleMessage(&reportMsg);
}



void Session::handleWeigherCalibration(SessionMessage *msg)
{
    if (currentModeOp == Session::OP_WEIGHERCALIBRATE && weigherCalibration)
    {
        Debug::trace("WeigherCalibration", HobString("Session received a weigher message, have a WeigherCalibration object to handle it."));

        if (!weigherCalibration->isWeigherCalibrationDone())
        {
            Debug::trace("WeigherCalibration", HobString("Session received a weigher message, have a WeigherCalibration object to handle it, will be handling it."));
            weigherCalibration->handleMessage(msg);
            //Debug::trace("WeigherCalibration", HobString("Session received a weigher message, have a WeigherCalibration object to handle it, done handling it."));
        }

        // Don't make this into an else clause of the above if clause;
        // isWeigherCalibrationDone() needs checked after the weigherCalibration->handleMessage(msg) call.
        if (weigherCalibration->isWeigherCalibrationDone())
        {
            Debug::trace("WeigherCalibration", HobString("Done with weigher calibration, deleting WeigherCalibration object."));
            delete weigherCalibration;
            weigherCalibration = (WeigherCalibration *)NULL;
            currentModeOp = Session::OP_UNKNOWN;
        }
    }
}


void Session::notifyExternalProcessOfWeight(HfcWeigherStatus *weight)
{
	bool isTracing = Debug::isTracing("Session");
	if (isTracing)
		Debug::trace("Session", "notifyExternalProcessOfWeight, begin");

	Configuration *config = Configuration::getInstance();
	if (config && weight && isPrimarySession())
	{
		if (isTracing)
			Debug::trace("Session", "notifyExternalProcessOfWeight, have weight and is primary session");
		int notifyType = 0;  // don't notify
		if (config->toBoolCfgI(CfgInternal::UNDIMSCREENONZEROWEIGHT) && weight->isZeroStableWeight())
			notifyType = 1;  // weightOffPlatter
		else if (config->toBoolCfgI(CfgInternal::UNDIMSCREENONNONZEROWEIGHT) && weight->isPositiveNonZeroStableWeight())
			notifyType = 2;  // weightOnPlatter
		else if (config->toBoolCfgI(CfgInternal::UNDIMSCREENONTAPPLATTER) && (weight->isSmallMotion() || weight->isLargeMotion()))
			notifyType = 2;  // weightOnPlatter

		if (notifyType)
		{
			const string cmd( (notifyType == 1) ? "weightOffPlatter" : "weightOnPlatter" );
			const string command( PythonScripter::buildPythonCall(config->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), config->toIntCfgI(CfgInternal::PYTHONSCRIPTPORT), cmd) );

			if (config->toBoolCfgI(CfgInternal::PYTHONENABLED))
			{
				if (isTracing)
					Debug::trace("Session", "before executing python script, command: " + command);
				string connectionStatusOutput;
				PAL_absclass::PAL_executeWithResults(command, connectionStatusOutput);
				// We don't care about any output from the command.
			}
			else if (isTracing)
				Debug::trace("Session", "python disabled, python script not called, command: " + command);
		}
	}

    // notify hook
    configurationManager->updateHookWeigherPrecision( weight->isDualRangeEnabled() && (DualRangeUp == weight->getDualRangeDirection()) );
}



/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Printer message processing                                                                ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

HfcPrinterStatus *Session::getCurrentPrinterStatus(int whichPrinter)
{
	if ((int)currentPrinterStatus.size() >= whichPrinter)
		return currentPrinterStatus[whichPrinter];
	else
		return 0;
}

HfcPrinterStockMetrics *Session::getPrinterStockMetrics(short printerNum)
{
    HfcPrinterStockMetrics *returnPrinterStockMetrics = (HfcPrinterStockMetrics *)NULL;

    if (RealTimeController::getInstance()->isVirtualPrintingEnabled() &&
    	!( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgVirtualPrintingOverride2ndPrinter) && printerNum == 1 && printerStockMetrics[printerNum] ) &&
    	!( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgVirtualPrintingOverride3rdPrinter) && printerNum == 2 && printerStockMetrics[printerNum] ) )
        returnPrinterStockMetrics = printerStockMetrics[FirstPrinter];
    else if (printerNum >= FirstPrinter && printerNum < MaxPrinterTypes)
        returnPrinterStockMetrics = printerStockMetrics[printerNum];

    return returnPrinterStockMetrics;
}

void Session::setPrinterStockMetrics(HfcPrinterStockMetrics *prStockMetrics)
{
	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

    if(prStockMetrics)
    {
        PrinterType printerNum = prStockMetrics->getPrinterType();
        if (printerNum >= FirstPrinter && printerNum < MaxPrinterTypes)
        {
			Debug::trace("Session.Printer", getName() + "Session::setPrinterStockMetrics - printer:" + inttostring(printerNum) );
			prStockMetrics->show();

            if(printerStockMetrics[printerNum])
                delete printerStockMetrics[printerNum];

			if (!config->isPrepackScale() &&
				config->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
			{
				prStockMetrics->setPrinterType(FirstPrinter);
				prStockMetrics->setLabelWidth(WideLabel);
				if ( config->toBoolCfg(fieldOffsetCfgChoseToUseContinuousStock) )
					prStockMetrics->setContinuousStock();
				else
					prStockMetrics->setLabelLengthInLines( (LabelLengthLines) config->toIntCfgI("fakePrinterLabelLength") );
			}

            printerStockMetrics[printerNum] = prStockMetrics;

#if defined( __linux__ )
			// send version info to hook
            if ( Configuration::getInstance()->getCustomerHooksEnabled() )
            {
                const std::string metrics( prStockMetrics->getHumanReadableLabelDimensions() );

                int state( HOOK_SS_SCALE_LABEL_STOCK_1 );

                if ( SecondPrinter == printerNum )
                    state = HOOK_SS_SCALE_LABEL_STOCK_2;
                else if ( ThirdPrinter == printerNum )
                    state = HOOK_SS_SCALE_LABEL_STOCK_3;

                hobart_cust_hook_update_scale_state( state, metrics.c_str() );
            }
#endif

			CfgFieldOffsets whichPrinterLabelLengthOffset = ( printerNum == FirstPrinter ) ? fieldOffsetCfgPrinter1LabelLength : ( ( printerNum == SecondPrinter ) ? fieldOffsetCfgPrinter2LabelLength : ( ( printerNum == ThirdPrinter ) ? fieldOffsetCfgPrinter3LabelLength : fieldOffsetCfgUnknown ) );

			// SOF-2763 The label length was not getting written to the database when it was continuous labels ...
			if (printerNum == FirstPrinter && !config->isPrepackScale() && config->toBoolCfgI(fieldOffsetCfgIConserveContinuousStockByNotSizing) )
			{
				// If anyone ever puts continuous stock into an EPP and we're a prepack scale (Access/EPCP),
				// which is an error condition - the wrapper can't apply continuous stock to a package since the label isn't cut,
				// we never need to keep track of the label stock size in any printer, nor do we if we are an EPP printer connected to a service counter scale.

				// Future consideration:  If we try to minimize label waste when sizing die-cut labels,
				// we may not want to save a length of 0 to the db, which would happen when the cassette is ejected;
				// we would lose what the length of the previous die-cut labels were.
				int len = prStockMetrics->getLabelLengthInLines();

				int prevLen = config->toIntCfg(whichPrinterLabelLengthOffset);
				if ( len != prevLen && whichPrinterLabelLengthOffset != fieldOffsetCfgUnknown )
				{
					Debug::trace("Session.Printer", getName() + "Session::setPrinterStockMetrics - saving new label length to configs table - PrinterLabelLengthOffset:" + inttostring(whichPrinterLabelLengthOffset) + "  previousLength:" + inttostring(prevLen) + "  newLength:" + inttostring(len) );

					if ( len <= 0 )
					{
						Debug::trace("Session.Printer", getName() + "Session::setPrinterStockMetrics - printer unsized" );

						// Printer is unsized, user popped the cassette.
						// Save the negative of the current length, to indicate what the previous length was before the cassette was ejected.
						if ( prevLen < 0 )
							prevLen *= -1;
						config->setCfg( whichPrinterLabelLengthOffset, Integer::toString( prevLen * -1 ) );
						config->saveField( whichPrinterLabelLengthOffset, this );
					}
					else
					{
						Debug::trace("Session.Printer", getName() + "Session::setPrinterStockMetrics - saving new label length to config table" );

						// Save the configuration table element for printer 1 label length to what was sized.
						config->setCfg( whichPrinterLabelLengthOffset, Integer::toString( len ) );
						config->saveField( whichPrinterLabelLengthOffset, this );
					}

					// SOF-1593 - user needs to opt-in for saving continuous label stock.
					/* No longer going to display messages to the operator regarding their use of continuous stock, since they are specifying that they are using continuous stock.
					if ( len == LengthContinuous_Lines && !config->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
					{
						Debug::trace("Session.Printer", getName() + "Session::setPrinterStockMetrics - new labels are continuous" );

						if ( (prevLen * -1) == LengthContinuous_Lines )
						{
							Debug::trace("Session.Printer", getName() + "Session::setPrinterStockMetrics - new labels are continuous and previous were continuous" );

							// Remove all messages queued that are printer messages
							removeDialogMessage( ErrPrinter, -1 );

							addDialogMessage( ErrPrinter, PrinterErrNoSizingDueToContinuous, HfcResponse::RESPONSESTYLE_OK );
							// If sized continuous, and the previous labels were also continuous, display a message for 2 seconds that sizing wasn't performed
							// due to continuous stock being used.
							//addDialogMessage( ErrPrinter, PrinterErrNoSizingDueToContinuous, HfcResponse::RESPONSESTYLE_NOINPUT );
							//PAL_absclass::PAL_sleep(2000);
							//removeDialogMessage( ErrPrinter, PrinterErrNoSizingDueToContinuous, HfcResponse::RESPONSESTYLE_NOINPUT );
						}
						else
						{
							// If sized continuous, and the previous labels were NOT continuous, display a message that if sizing wasn't supposed to result
							// in continuous stock being detected, then they should pop the cassette and reinsert it.
							addDialogMessage( ErrPrinter, PrinterErrSizedContinuousOK, HfcResponse::RESPONSESTYLE_OKCANCEL );
						}
					}
					*/
				}
			}

            int length = prStockMetrics->getLabelLengthInLines();
			secondLabelRotated = Configuration::getInstance()->toIntCfg(fieldOffsetCfgSecondaryLabelStockRotated);
            if(printerNum == FirstPrinter)
            {
                Debug::trace("Session.SendLabelInfo", "First length = " + inttostring(length));
                firstLabelLength = length;
            }
            else if(printerNum == SecondPrinter)
            {
                Debug::trace("Session.SendLabelInfo", "Second length = " + inttostring(length));
                secondLabelLength = length;
            }
        }
    }
}



/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Marquee Customer Display message processing                                               ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

void Session::handleMarqueeMessage(SessionMessage *msg)
{
    if (!msg)
        return;

    Debug::trace("Session", HobString("Session received a marquee message."));


    switch(msg->getDeviceType())
    {
        case Marquee:
        {
            switch (msg->getEventType())
            {
                case MqTest:
                {
                    HfcMarqueeTest *mqHfcTest = dynamic_cast<HfcMarqueeTest *> (msg->getHfcObject());
                    MarqueeTest mqTest(mqHfcTest->getTest(), this);

                    send(&mqTest, Messenger::DEST_RTC);

                    break;
                }
                default:
                    break;
            }
            break;
        }
        case MarqueeSystem:
        {
            switch(msg->getEventType())
            {
                case SysReqStatus:
                {
                    Debug::trace("Session.MqSystem", "handling Marquee System Request Status message");

                    if (msg == (SessionMessage *)NULL)
                        return;

                    //  Register for the MarqueeSystem SysStatus message
                    registerEvent(MarqueeSystem, SysStatus, false );

                    //  Not much to do here with a Marquee System Request Status message except pass it on.
                    //  But first, change the format from Browser to Internal.
                    msg->setFormatType(InternalFormat);

                    int bytesSent = send(msg, Messenger::DEST_RTC);
                    if (bytesSent < 0)
                    {
                        // todo - error handling?
                        Debug::trace("Session.MqSystem", "MarqueeSystem, SysReqStatus RESPONSE MESSAGE COULD NOT BE SENT");
                    }
                    break;
                }

                case SysStatus:
                {
                    Debug::trace("Session.MqSystem", "handling Marquee System Status message");
                    deregisterEvent(MarqueeSystem, SysStatus);

#if defined( __linux__ )
                    // send version info to hook
                    if (( Configuration::getInstance()->getCustomerHooksEnabled() ) && ( SessionManager::getInstance()->getPrimaryConsoleSession() ))
                	{
                        MarqueeSystemStatus *status( dynamic_cast<MarqueeSystemStatus*>( msg->getHfcObject() ) );
						PeripheralManager *pm( SessionManager::getInstance()->getPrimaryConsoleSession()->getPeripheralManager() );

                        if (( status ) && ( pm ))
                        {
                            const std::string sw( status->getSoftwareVersion().getRevMajMinStr() );
                            const std::string hw( status->getHardwareVersion().getRevMajMinStr() );

                            const HfcPeripheralList list( pm->getCurrentList() );

                            // MarqueeSystemStatus
                            status->show();

                            Debug::trace("Session", "marquee versions: sw " + sw + ", hw " + hw);
							hobart_cust_hook_update_peripheral_version( MarqueeSystem, 0, list.getMarquee(), sw.c_str(), hw.c_str() );
                        }
                    }
#endif

                    /* Change the format indicator, and send the same message to the Browser. */
                    msg->setFormatType(BrowserFormat);
                    int bytesSent = send(msg, Messenger::DEST_UI);
                    if (bytesSent < 0)
                    {
                        // todo - error handling?
                        Debug::trace("Session.MqSystem", "MarqueeSystem, SysStatus RESPONSE MESSAGE COULD NOT BE SENT");
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}




/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Graphical Customer Display message processing                                             ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/
void Session::handleGraphicalCustomerDisplayMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	if (msg->getDeviceType() == GCDSessionDev)
	{
		switch (msg->getEventType())
		{
		case GcdSessionSetVolume:
			{
				GCDSystemMsg msgGCD(GCDSessionDev, GcdSessionSetVolume,
					dynamic_cast<GenericHfcObject *>(msg->getHfcObject())->getData());
				dispatchMessage(&msgGCD);
				break;
			}
		case GcdSessionUpdateViaFtpNow:
			{
				GCDFtpUpdateMsg msgGCD(GCDSessionDev, GcdSessionUpdateViaFtpNow,
					dynamic_cast<GenericHfcObject *>(msg->getHfcObject())->getData());
				dispatchMessage(&msgGCD);
				break;
			}
		case GcdSessionUpdateViaUsbNow:
			{
				GCDSystemMsg msgGCD(GCDSessionDev, GcdSessionUpdateViaUsbNow,
					dynamic_cast<GenericHfcObject *>(msg->getHfcObject())->getData());
				dispatchMessage(&msgGCD);
				break;
			}
		case GcdSessionSetPlaybackWaitTime:
			{
				GCDSystemMsg msgGCD(GCDSessionDev, GcdSessionSetPlaybackWaitTime,
					dynamic_cast<GenericHfcObject *>(msg->getHfcObject())->getData());
				dispatchMessage(&msgGCD);
				break;
			}
		case GcdSessionColorTestOn:
			{
				GenericHfcObject *obj( dynamic_cast<GenericHfcObject*>( msg->getHfcObject() ) );

				if ( obj )
				{
					GCDSystemMsg msgGCD( GCDSessionDev, GcdSessionColorTestOn, obj->getData() );
					dispatchMessage( &msgGCD );
					break;
				}

				// fall through...
			}
		default:
			{
				EvolMessage msgGCD(GCDSessionDev, msg->getEventType());
				dispatchMessage(&msgGCD);
				break;
			}
		}
	}
}

/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   Numeric Customer Display message processing                                               ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

void Session::handleNumericMessage(SessionMessage *msg)
{
    if (!msg)
        return;

    Debug::trace("Session", HobString("Session received a Numeric message."));


    switch (msg->getDeviceType())
    {
        case Numeric:
        {
            switch (msg->getEventType())
            {
                case NmTest:
                {
                    HfcNumericTest *nmHfcTest = dynamic_cast<HfcNumericTest *> (msg->getHfcObject());
                    NumericTest nmTest(nmHfcTest->getTest(), this);

                    send(&nmTest, Messenger::DEST_RTC);

                    break;
                }
                default:
                    break;
            }
            break;
        }
        case NumericSystem:
        {
            switch (msg->getEventType())
            {
                case SysReqStatus:
                {
                    Debug::trace("Session.NmSystem", "handling Numeric System Request Status message");

                    if (msg == (SessionMessage *)NULL)
                        return;

                    //  Register for the NumericSystem SysStatus message
                    registerEvent(NumericSystem, SysStatus, false );

                    //  Not much to do here with a Numeric System Request Status message except pass it on.
                    //  But first, change the format from Browser to Internal.
                    msg->setFormatType(InternalFormat);

                    int bytesSent = send(msg, Messenger::DEST_RTC);
                    if (bytesSent < 0)
                    {
                        // todo - error handling?
                        Debug::trace("Session.MqSystem", "NumericSystem, SysReqStatus RESPONSE MESSAGE COULD NOT BE SENT");
                    }
                    break;
                }

                case SysStatus:
                {
                    Debug::trace("Session.NmSystem", "handling Numeric System Status message");
                    deregisterEvent(NumericSystem, SysStatus);

#if defined( __linux__ )
                    // send version info to hook
                    if (( Configuration::getInstance()->getCustomerHooksEnabled() ) && ( SessionManager::getInstance()->getPrimaryConsoleSession() ))
                	{
                        NumericSystemStatus *status( dynamic_cast<NumericSystemStatus*>( msg->getHfcObject() ) );
						PeripheralManager *pm( SessionManager::getInstance()->getPrimaryConsoleSession()->getPeripheralManager() );

                        if (( status ) && ( pm ))
                        {
                            const std::string sw( status->getSoftwareVersion().getRevMajMinStr() );
                            const std::string hw( status->getHardwareVersion().getRevMajMinStr() );

                            const HfcPeripheralList list( pm->getCurrentList() );

                            // NumericSystemStatus
                            status->show();

                            Debug::trace("Session", "numeric versions: sw " + sw + ", hw " + hw);
							hobart_cust_hook_update_peripheral_version( NumericSystem, 0, list.getNumeric(), sw.c_str(), hw.c_str() );
                        }
                    }
#endif

                    /* Change the format indicator, and send the same message to the Browser. */
                    msg->setFormatType(BrowserFormat);
                    int bytesSent = send(msg, Messenger::DEST_UI);
                    if (bytesSent < 0)
                    {
                        // todo - error handling?
                        Debug::trace("Session.NmSystem", "NumericSystem, SysStatus RESPONSE MESSAGE COULD NOT BE SENT");
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}



/***************************************************************************************************
 ***************************************************************************************************
 ***                                                                                             ***
 ***   RFID message processing                                                                   ***
 ***                                                                                             ***
 ***************************************************************************************************
 ***************************************************************************************************/

void Session::handleRFIDMessage(SessionMessage *msg)
{
    if (!msg)
        return;

    bool isTracing = Debug::isTracing("Session.rfid");
    if (isTracing)
        Debug::trace("Session.rfid", "Session received an RFID message.");

    switch (msg->getEventType())
    {
        case RFIDCurrentID:
        {
            if (isTracing)
                Debug::trace("Session.rfid", "handling RFIDCurrentID message");

            HfcRFIDTIRISReadOnlyId *rfidId = dynamic_cast<HfcRFIDTIRISReadOnlyId *> (msg->getHfcObject());
            if (!rfidId)
                return;

            std::string theId = rfidId->getCurrentId();
            if (isTracing)
            {
                Debug::trace("Session.rfid", HobString("RFIDCurrentID received is *") + rfidId->getCurrentId() + HobString("*"));
                Debug::trace("Session.rfid", HobString("RFIDCurrentID received len=") + HobString(Integer::toString((int)theId.size())));
                Debug::trace("Session.rfid", HobString("RFIDCurrentID received (hex) is *") + HobString(Hex::toString(theId.c_str(), (int)theId.size(), true)) + HobString("*"));
            }

			//Not needed for HT hands-free devices
            //theId = Hex::toString(theId.c_str(), (int)theId.size(), false);

            if (skipRFIDLoginAndNotifyUIofRFIDInput)
            {
                // Not logging in the rfid user.  Just notify the UI of the tag that was identified.
                if (isTracing)
                    Debug::trace("Session.rfid", "RFIDCurrentID -  sending the scanned id to the browser");
                msg->setFormatType(BrowserFormat);
                send(msg, Messenger::DEST_UI);
            }
            else if (isPrimarySession())
            {
                if (operatorManager)
                {
                    // If the id detected is not empty/blank, login the operator.
                    // If it's empty/blank, logout the currently logged in operator.

                    if (theId == "0000000000000000")
                    {
                        // Can't logoff by removing your tag when in Supervisor mode and supervisor login is required.
                        if (operatorManager->getLoggedInOperator() && operatorManager->getLoggedInOperator()->getCurrentMode() == HobOperator::CURRENTMODE_SUPERVISOR &&
							OperatorManager::isSupervisorLoginRequired(this))
                            return;

                        // logout current operator, if one is logged in; don't need to pass a HobOperator in to handle this.
                        if (isTracing)
                            Debug::trace("Session.rfid", "RFIDCurrentID - sending an OpLogoutRFID message to OperatorManager");

                        //HobOperator *loggedInOp = operatorManager->getLoggedInOperator();
                        // Only logout if the operator is in Operate mode.
                        // Don't automatically logout if in Supervisor or Service mode.
                        //if (loggedInOp && loggedInOp->getCurrentMode() == HobOperator::CURRENTMODE_OPERATE)
                        //{
                        SessionMessage opMsg(OperatorDev, OpLogoutRFID);
                        operatorManager->handleMessage(&opMsg);
                        //}
                    }
                    else
                    {
                        //HobOperator *loggedInOp = operatorManager->getLoggedInOperator();
                        // Only login the operator if there isn't an operator logged into Supervisor or Service mode.
                        //if (!loggedInOp || loggedInOp->getCurrentMode() == HobOperator::CURRENTMODE_OPERATE)
                        //{
                        // login operator
                        SessionMessage opMsg(OperatorDev, OpLoginRFID);
                        HobOperator rfidOp;
                        rfidOp.setOpRFID(theId);
                        if (isInOperateMode())
                            rfidOp.setCurrentMode(HobOperator::CURRENTMODE_OPERATE);
                        else if (isInSupervisor())
                            rfidOp.setCurrentMode(HobOperator::CURRENTMODE_SUPERVISOR);
                        else if (isInService() || getCurrentMode() == OpModeServiceLogin)
                            rfidOp.setCurrentMode(HobOperator::CURRENTMODE_SERVICE);
                        opMsg.setHfcObject(&rfidOp);
                        operatorManager->handleMessage(&opMsg);

                        // remove this in case it was displayed, as it would be when prompting for an ID when
                        // the operator changed plu data in run mode and wants to print a label.
                        if (operatorManager->getLoggedInOperator() && productManager && productManager->isWaitingForLoginBeforePrinting())
                        {
			                Debug::trace("Session.rfid", "RFID dismissing vkbd");
                            dismissVirtualKeyboard();
                            removeDialogMessage(ErrOperatorDev, OperatorErrEnterIDorRFID, HfcResponse::RESPONSESTYLE_PROMPTOKCANCEL);
							// If we are in Accum Items mode with strip mode off and totals only we need to print individual PLU Labels
							// when they are added to the list otherwise we are printing totals labels. Need to be able to distinguish between the 2
							// Added for Jira GCDR2-199
							if( getCurrentMode() == OpModeAccumulateItems && !Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableItemStripMode) &&
								!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableAccumItemsTotalsOnly) )
							{
								// Totals label or Individual labels ?
								if (softkeyManager->isAccumItemsPrintKeyHit())
								{ // totals label
									SessionMessage printIt(SessionDev, SnPrintKeyPressed);
									productManager->handleMessage(&printIt);
								}
								else
								{  // individual label (add item to list)
									SessionMessage printIt(ProductDev, ProductAddItemToAccumulatedItems);
									productManager->handleMessage(&printIt);
								}
							}  // end of add for jira GCDR2-199
							else
							{
								SessionMessage printIt(SessionDev, SnPrintKeyPressed);
								productManager->handleMessage(&printIt);
							}
                        }
                    }
                }
            }
            else
            {
                // As a remote browser, don't do anything with these messages, yet.
            }
            break;
        }

        case RFIDCurrentStatus:
        {
            if (isTracing)
                Debug::trace("Session.rfid", "handling RFIDCurrentStatus message");
            if (isPrimarySession())
            {
                // Currently, the current status message only indicates when the rfid is present or not.
                // We can find this out by calling Configuration::getInstance()->isRFIDHWPresent().
                // So for now, we aren't going to do anything with these messages.
            }
            else
            {
                // As a remote browser, don't do anything with these messages, yet.
            }
            break;
        }

        default:
            break;
    }
}


//int Session::getTouchScreenSettingType()
HobString Session::getTouchScreenSettingType()
{
#if defined ( __linux__ )
    Debug::trace("Session.virtualKeyboard", "getTouchScreenSettingType");
    HobString hobStrKeyResult=HobString("1");
    return hobStrKeyResult;
#else
    HobString hobStrKeyResult=HobString("ERROR!");
    HKEY hKey = 0;
    char touchVal[4];
    memset(touchVal,'\0',4);
    DWORD sizeOfTouchData = 3;
    int keyResult = 5;

    // HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\tsharcs\Tsharcs1\parameters
    // SoundFlag
    // hex:05 = keyup
    // hex:01 = normal

    if (isPrimarySession())
    {
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T ("SYSTEM\\CurrentControlSet\\Services\\tsharcs\\Tsharcs1\\Parameters"),
                    0, KEY_QUERY_VALUE, &hKey))
        {
            // This IF is reached if we could not find the key in the registry.
            // 5 = touchUp, 1 = Normal,   If we did not find the key,
            // then we are on a PC or laptop, and need to be normal mode.
            Debug::trace("Session.TouchScreen", HobString("getTouchScreenSettingType() did not find key so keyResult = 1."));
            keyResult = 1;
            hobStrKeyResult = HobString(Integer::toString(keyResult));
        }
        else
        {
            // This ELSE is reached if we found the key in the registry.
            // Now what is the key value? I need to convert it to an int I can
            // return in keyResult. Or a string and return that string and change
            // the return type. assign the registry value to the keyResult value.

            if ( RegQueryValueExA(hKey, "SoundFlag", NULL, NULL, (unsigned char*)&touchVal, &sizeOfTouchData) == ERROR_SUCCESS )
            {
                keyResult = Hex::valueOf(touchVal[0]);
                hobStrKeyResult = HobString(touchVal);
                Debug::trace("Session.TouchScreen", "getTouchScreenSettingType() found SoundFlag key as '" + hobStrKeyResult +
                        "'(ASCII codes " + util_string::inttostring(touchVal[0]) + ", " + util_string::inttostring(touchVal[1]) + ", " +
                        util_string::inttostring(touchVal[2]) + ", " + util_string::inttostring(touchVal[3]) + ")");
            }
            else
            {
                Debug::trace("Session.TouchScreen", HobString("getTouchScreenSettingType() did not find SoundFlag key so keyResult = 1."));
                keyResult = 1;
                hobStrKeyResult = HobString(Integer::toString(keyResult));
            }
        }
    }
    else
    {
        // Reached this ELSE because we are not a primary session.  Therefore we
        // are a remote session and need to be in Normal Clicking Mode
        keyResult = 1;
        hobStrKeyResult = HobString(Integer::toString(keyResult));
        Debug::trace("Session.TouchScreen", HobString("getTouchScreenSettingType() on s remote so keyResult = 1."));
    }

    if (hKey)
        RegCloseKey(hKey);

    return hobStrKeyResult;
#endif
}


void Session::handleMFGTestMessage(SessionMessage *msg)
{
    if (!msg)
        return;

    bool isTracing = Debug::isTracing("Session.mfgTest");
    if (isTracing)
        Debug::trace("Session.mfgTest", "Session received an mfg test message.");

    switch (msg->getEventType())
    {
    	case WriteScaleSWConfig:
    	{
    		Debug::trace("Session.mfgTest", "Write scale Software Configuration.");
			if( FpmManager::getInstance()->handleWriteMessage(msg) )
			{
				Debug::trace("Session.mfgTest", "Write of Scale Software Configuration passes");
				SessionMessage sm(MfgTestDev, WriteOfScaleSWConfigPassed);
                sm.setFormatType(BrowserFormat);
                Debug::trace("Session", "Send write data passed UI");
                send(&sm, Messenger::DEST_UI);

			}
			else
			{
				Debug::trace("Session.mfgTest", "Write of Scale Software Configuration failed.");
                SessionMessage sm(MfgTestDev, WriteOfScaleSWConfigFailed);
                sm.setFormatType(BrowserFormat);
                Debug::trace("Session", "Send write data failed UI");
                send(&sm, Messenger::DEST_UI);
			}

    		break;
    	}
    	case WriteScaleHwConfig:
    	{
    		Debug::trace("Session.mfgTest", "Write Scale Hardware Configuration.");
			if( FpmManager::getInstance()->handleWriteMessage(msg) )
			{
				Debug::trace("Session.mfgTest", "Write of Scale Hardware Configuration passed.");
                SessionMessage sm(MfgTestDev, WriteOfScaleHwConfigPassed);
                sm.setFormatType(BrowserFormat);
                Debug::trace("Session", "Send write data passed UI");
                send(&sm, Messenger::DEST_UI);

			}
			else
			{
				Debug::trace("Session.mfgTest", "Write of Scale Hardware Configuration failed.");
                SessionMessage sm(MfgTestDev, WriteOfScaleHwConfigFailed);
                sm.setFormatType(BrowserFormat);
                Debug::trace("Session", "Send write data failed UI");
                send(&sm, Messenger::DEST_UI);
			}

    		break;
    	}
    	case WriteMfgData:
		{
			Debug::trace("Session.mfgTest", "Write of Manufacturing Section.");
			if( FpmManager::getInstance()->handleWriteMessage(msg) )
			{
				Debug::trace("Session.mfgTest", "Write of Manufacturing Section passed");
                SessionMessage sm(MfgTestDev, WriteOfMfgDataPassed);
                sm.setFormatType(BrowserFormat);
                Debug::trace("Session", "Send write data passed UI");
                send(&sm, Messenger::DEST_UI);

			}
			else
			{
				Debug::trace("Session.mfgTest", "Write of Manufacturing Section failed.");
                SessionMessage sm(MfgTestDev, WriteOfMfgDataFailed);
                sm.setFormatType(BrowserFormat);
                Debug::trace("Session", "Send write data failed UI");
                send(&sm, Messenger::DEST_UI);
			}

			break;
		}
		case RequestMfgData:
		{
			HfcWriteFpmManData mfgData(FpmManager::getInstance()->readMfgScaleData(), this);

			SessionMessage response(MfgTestDev, ReadMfgData);
			response.setFormatType(BrowserFormat);
			response.setHfcObject(&mfgData);

			send(&response, Messenger::DEST_UI);
			break;
		}

		case RequestScaleHwConfig:
		{
			FpmManager::getInstance()->readScaleHWConfigData();
			break;
		}
		case RequestScaleSWConfig:
		{
			FpmManager::getInstance()->readScaleSWConfigData();
			break;
		}
		case RequestLoaderVersion:
		{
			Debug::trace("Session.mfgTest", "Session received a loader request.");
			std::string verStr = FpmManager::getInstance()->getLoaderVersion();
	        GenericHfcObject version(verStr);
	        SessionMessage msg(MfgTestDev, ReadLoaderVersion);
	        msg.setFormatType(BrowserFormat);
	        msg.setHfcObject(&version);
	        send(&msg, Messenger::DEST_UI);
	        break;
		}
		case WriteOfScaleSWConfigMetrologicalVersion:
		{
			Debug::trace("Session", "Session received a WriteOfScaleSWConfigMetrologicalVersion.");
			if( FpmManager::getInstance()->handleWriteMessage(msg) )
			{
				Debug::trace("Session.mfgTest", "Write of Scale Software Configuration passes");
				SessionMessage sm(MfgTestDev, WriteOfScaleSWConfigPassed);
                sm.setFormatType(BrowserFormat);
                Debug::trace("Session", "Send write data passed UI");
                send(&sm, Messenger::DEST_UI);

			}
			else
			{
				Debug::trace("Session.mfgTest", "Write of Scale Software Configuration failed.");
                SessionMessage sm(MfgTestDev, WriteOfScaleSWConfigFailed);
                sm.setFormatType(BrowserFormat);
                Debug::trace("Session", "Send write data failed UI");
                send(&sm, Messenger::DEST_UI);
			}

			break;
		}
		case RequestPrinterLabelCount:
		{
			int count( 0 );
			HfcRequestPrinterLabelCount *pRequest( dynamic_cast<HfcRequestPrinterLabelCount*>( msg->getHfcObject() ) );

			MtdNGWWrapperData *pWrapperData;
			MtdScaleData *pScaleData;

			if ( (pWrapperData = FpmManager::getInstance()->readWrapperData()) )
			{
				if( pRequest->getPrinterType() == FirstPrinter )
					count = pWrapperData->getPrimaryPrinterCounts();
				else if( pRequest->getPrinterType() == SecondPrinter )
					count = pWrapperData->getSecondaryPrinterCounts();
				else if( pRequest->getPrinterType() ==  ThirdPrinter )
					count = pWrapperData->getTertiaryPrinterCounts();
			}
			else if ( (pScaleData = FpmManager::getInstance()->readScaleData()) )
			{
				if( pRequest->getPrinterType() == FirstPrinter )
					count = pScaleData->getPrimaryPrinterCounts();
				else if( pRequest->getPrinterType() == SecondPrinter )
					count = pScaleData->getSecondaryPrinterCounts();
				else if( pRequest->getPrinterType() ==  ThirdPrinter )
					count = pScaleData->getTertiaryPrinterCounts();
			}

			std::string str = Integer::toString( count );
	        GenericHfcObject cnt( str );
	        SessionMessage msg(MfgTestDev, PrinterLabelCount);
	        msg.setFormatType(BrowserFormat);
	        msg.setHfcObject( &cnt );
	        send(&msg, Messenger::DEST_UI);
    		break;
		}
		case ClearPrinterLabelCount:
		{
			FpmManager::getInstance()->handleWriteMessage(msg);
			break;
		}
		case RequestWrapperCycleCount:
		{
			MtdNGWWrapperData *pWrapperData = FpmManager::getInstance()->readWrapperData();
			if( pWrapperData ) {

				std::string str = Integer::toString(  pWrapperData->getCycleCounts() );
		        GenericHfcObject cnt( str );
		        SessionMessage msg(MfgTestDev, WrapperCycleCount);
		        msg.setFormatType(BrowserFormat);
		        msg.setHfcObject( &cnt );
		        send(&msg, Messenger::DEST_UI);

			}
			break;
		}
		case RequestInstallDate:
		{
			std::string date;

			MtdNGWWrapperData *pWrapperData;
			MtdScaleData *pScaleData;

			if ( (pWrapperData = FpmManager::getInstance()->readWrapperData()) )
				date = pWrapperData->getInstallDate();
			else if ( (pScaleData = FpmManager::getInstance()->readScaleData()) )
				date = pScaleData->getInstallDate();

			GenericHfcObject cnt( date );
	        SessionMessage msg(MfgTestDev, WrapperInstallDate);
	        msg.setFormatType(BrowserFormat);
	        msg.setHfcObject( &cnt );
	        send(&msg, Messenger::DEST_UI);
			break;
		}
		case SetInstallDate:
		{
			FpmManager::getInstance()->handleWriteMessage(msg);
			break;
		}
    }
}

void Session::handleJoeMessage(SessionMessage *msg)
{
    if (!msg)
        return;

    if (Debug::isTracing("Session"))
    {
        HobString dev(Integer::toString(msg->getDeviceType()));
        HobString ev(Integer::toString(msg->getEventType()));
        Debug::trace("Session", "*********************************************");
        Debug::trace("Session", getName() + HobString(" - received a JOE message: ") + dev + HobString(" ") + ev);
        Debug::trace("Session", "*********************************************");
    }
    /*
       switch(msg->getEventType())
       {
       default:
       break;
       } */
}

#include "DBConfigurationsRM.h"
// override from AbstractSession
//virtual
void Session::initializationBeforeRun()
{
    DBSession::initializationBeforeRun();
/*
 {
		string sqlname, sqlcmd;
		try
		{
			DBConfigurationsRM *dbConfigurationsRM = getDBConfigurationsRM();
			if (dbConfigurationsRM)
			{
				int delay = Configuration::getInstance()->toIntCfgI(CfgInternal::DELAYBETWEENSESSIONMESSAGES);
				int i = 0;
				while (i>=0)
				{
					sqlname = "SQLInitialCommand" + inttostring(i++);
					sqlcmd = Configuration::getInstance()->toStringCfgI(sqlname);
					if (sqlcmd.size())
					{
						bool res = dbConfigurationsRM->execute(sqlcmd);
						PAL_absclass::PAL_sleep(delay);
					}
					else
						break;
				}
			}
		}
		catch (std::exception e)
		{
			string es = e.what();
			Debug::trace("SessionManager", "Initial SQL command - command not found - " + sqlname);
		}
}
*/

   responseMessageManager = new ResponseMessageManager(this);
}

// override from AbstractSession
//virtual
void Session::finalizationAfterRun()
{
    /* The following tells the Session Manager to delete this Session.
     * Currently, the SessionManager does a 'delete' on this opbject, afterwhich, the session will
     * be gone and no further processing will occur.  Therefore, the delete should be the last
     * thing done for the Session, since the Session is initiating the delete.
     */
    Debug::trace("Session", getName() + HobString(" - Notifying SessionManager to remove me ..."));

    DBSession::finalizationAfterRun();

    EvolMessage e(SessionDev, SnTerminated, this);
    dispatchEvent(&e);
    //SessionManager::getInstance()->removeSession(this);
}

// override from AbstractSession
//virtual
void Session::quitGracefully()
{
    finalizationAfterRun();
}

void Session::saveOpModeSettings()
{
    bool traceOn = Debug::isTracing("SessionWrapper");
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "saveOpModeSettings():  ";
        Debug::trace("SessionWrapper", msgDebug + "ENTER");
    }

    if (configurationManager && configurationManager->getCurrentEvolWrapperControl())
    {
        wrapperSealerBeltPower = configurationManager->getCurrentEvolWrapperControl()->getSealerBeltHeaterEnabled();
        if (traceOn)
            Debug::trace("SessionWrapper", msgDebug + "wrapperSealerBeltPower = " + Bool::toString(wrapperSealerBeltPower));
    }
    if (traceOn)
        Debug::trace("SessionWrapper", msgDebug + "EXIT");
}

void Session::restoreOpModeSettings()
{
    bool traceOn = Debug::isTracing("SessionWrapper");
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "restoreOpModeSettings():  ";
        Debug::trace("SessionWrapper", msgDebug + "ENTER");
    }

    if (SessionManager::getInstance()->isEvolutionWrapperAttached() && configurationManager)
    {
        HfcEvolWrapperControl *currentWrControl = configurationManager->getCurrentEvolWrapperControl();
        if (!currentWrControl)
        {
            if (traceOn)
                Debug::trace("SessionWrapper", msgDebug + "currentWrControl is 0.");
        }
        else
        {
            if (wrapperSealerBeltPower != currentWrControl->getSealerBeltHeaterEnabled())
            {
                currentWrControl->setSealerBeltHeaterEnabled(wrapperSealerBeltPower);

                SessionMessage wrControl(Wrapper, WrEvolControl);
                wrControl.setFormatType(InternalFormat);
                wrControl.setHfcObject(currentWrControl);

                int bytesSent = send(&wrControl, Messenger::DEST_RTC);
                if (bytesSent < 0)
                {
                    // todo - error handling?
                    if (traceOn)
                        Debug::trace("SessionWrapper", msgDebug + "WrEvolControl message could not be sent.");
                }
                if (traceOn)
                {
                    HobString msgDebug2 = msgDebug;
                    msgDebug2.append("Updated wrapperSealerBeltPower to ").append(Bool::toString(wrapperSealerBeltPower));
                    Debug::trace("SessionWrapper", msgDebug2);
                }
            }
            else
            {
                if (traceOn)
                    Debug::trace("SessionWrapper", msgDebug + "wrapperSealerBeltPower did not change.");
            }
        }
    }
    Debug::trace("SessionWrapper", msgDebug + "EXIT");
}


bool Session::isSupervisorKeyPresent() const
{
    return accessKeys.isSupervisorKeyPresent();
}

bool Session::isServiceKeyPresent() const
{
    return accessKeys.isServiceKeyPresent();
}

bool Session::isSelfServeKeyPresent() const
{
    return accessKeys.isSelfServeKeyPresent();
}

bool Session::isAnyAccessKeyPresent() const
{
    return accessKeys.isAnyAccessKeyPresent();
}


void Session::productSwitchToManualWeighMode()
{
	//	Only do this for a Prepack scale
	if ( Configuration::getInstance()->isPrepackScale() || productManager->isPrintingRemoteLabel() )
	{
		Debug::trace("Session.KeyboardNetWeight", "productSwitchToManualWeighMode(1):  currentMode = " + inttostring(currentMode));
		if (currentMode != OpModeManualWeigh)
		{
			toggleReturnToRunMode = currentMode;
			SessionMessage mwmode(OperationMode, OpModeManualWeigh);

			HobString b(booltostring(false));		//	Don't set mostRecentRunMode in Session::handleModeMessage()
			GenericHfcObject dsmrrm(b);
			mwmode.setHfcObject(&dsmrrm);
			mwmode.setFormatType(InternalFormat);

			handleModeMessage(&mwmode);
		}
		Debug::trace("Session.KeyboardNetWeight", "productSwitchToManualWeighMode(2):  currentMode = " + inttostring(currentMode) + "   toggleReturnToRunMode = " + inttostring(toggleReturnToRunMode));
	}
}

void Session::productSwitchToAutoManualApplyMode()
{
	//	Only do this for a Prepack scale
	if (Configuration::getInstance()->isPrepackScale())
	{
		Debug::trace("Session.KeyboardNetWeight", "productSwitchToAutoManualApplyMode(1):  currentMode = " + inttostring(currentMode));
		if (currentMode != OpModeAutoManualApply)
		{
			toggleReturnToRunMode = currentMode;
			SessionMessage mwmode(OperationMode, OpModeAutoManualApply);

			HobString b(booltostring(false));		//	Don't set mostRecentRunMode in Session::handleModeMessage()
			GenericHfcObject dsmrrm(b);
			mwmode.setHfcObject(&dsmrrm);
			mwmode.setFormatType(InternalFormat);

			handleModeMessage(&mwmode);
		}
		Debug::trace("Session.KeyboardNetWeight", "productSwitchToAutoManualApplyMode(2):  currentMode = " + inttostring(currentMode) + "   toggleReturnToRunMode = " + inttostring(toggleReturnToRunMode));
	}
}

void Session::productSwitchToBuildAMealMode()
{
	Debug::trace("Session", "productSwitchToBuildAMealMode:  currentMode = " + inttostring(currentMode));
	if (currentMode != OpModeBuildAMeal)
	{
		toggleReturnToRunMode = currentMode;
		SessionMessage mwmode(OperationMode, OpModeBuildAMeal);

		HobString b(booltostring(false));		//	Don't set mostRecentRunMode in Session::handleModeMessage()
		GenericHfcObject dsmrrm(b);
		mwmode.setHfcObject(&dsmrrm);
		mwmode.setFormatType(InternalFormat);

		handleModeMessage(&mwmode);
	}
	Debug::trace("Session", "productSwitchToBuildAMealMode:  new currentMode = " + inttostring(currentMode) + "   toggleReturnToRunMode = " + inttostring(toggleReturnToRunMode));
}

void Session::productReturnToPriorOpMode()
{
	//	Only do this for a Prepack scale
	if (Configuration::getInstance()->isPrepackScale())
	{
		if ( toggleReturnToRunMode < OpModeIdle || toggleReturnToRunMode >= OpModeMax )
			toggleReturnToRunMode = OpModeManualWeigh;


		Debug::trace("Session.KeyboardNetWeight", "productReturnToPriorOpMode(1):  toggleReturnToRunMode = " + inttostring(toggleReturnToRunMode));
		SessionMessage mwmode(OperationMode, toggleReturnToRunMode);

		HobString b(booltostring(true));		//	Don't set mostRecentRunMode in Session::handleModeMessage()
		GenericHfcObject dsmrrm(b);
		mwmode.setHfcObject(&dsmrrm);
		mwmode.setFormatType(InternalFormat);

		handleModeMessage(&mwmode);
		toggleReturnToRunMode = currentMode;
		Debug::trace("Session.KeyboardNetWeight", "productReturnToPriorOpMode(2):  currentMode = " + inttostring(currentMode) + "   toggleReturnToRunMode = " + inttostring(toggleReturnToRunMode));
	}
}

bool Session::fileIsFirmware(std::string firmwareFilename, HobVersion::VersionType &firmwareDeviceType)
{
	// Check if this filename is the name of a file containing firmware for some device.
	// We use internal config for this ...

	string knownFirmwareFilenames = Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREFILENAMES);
	if ( !knownFirmwareFilenames.empty() )
	{
		std::string::size_type namePos = knownFirmwareFilenames.find(firmwareFilename);
		if (namePos != std::string::npos)
		{
			int firmwareDev = stringtoint(knownFirmwareFilenames.substr(namePos-3, 3));
			if (firmwareDev > 0)
			{
				firmwareDeviceType = (HobVersion::VersionType)firmwareDev;
				return true;
			}
		}
	}

	firmwareDeviceType = HobVersion::UnknownComponent;
	return false;
}

bool Session::fileIsGraphic(std::string firmwareFilename)
{
	// Check if this filename is the name of a graphic file.
	// Compare against our Graphic file specifiers. Those are the only ones we will accept.

	if ( !firmwareFilename.empty() )
	{
		std::string downloadFileSpecifiers = Configuration::getInstance()->toStringCfgI(CfgInternal::GRAPHICFILESPECIFIERS);		//	TODO: Check if we should use generic or specific to the type?
		util_filesystem::File grFile (firmwareFilename);

		std::string asteriskStr("*");
		std::string dotStr(".");
		std::string::size_type pos;
		HobTokenizerCSV tok(&downloadFileSpecifiers, BrowserFormatSeparator);
		HobString token;
		while (tok.next(&token) != -1)
		{
//			Debug::trace("ProcessDownloadedFiles", getName() + " - token = [" + token + "]");
			pos = token.find(asteriskStr);
			while (pos != std::string::npos)
			{
				token.erase(pos, 1);
				pos = token.find(asteriskStr);
			}
			pos = token.find(dotStr);
			while (pos != std::string::npos)
			{
				token.erase(pos, 1);
				pos = token.find(dotStr);
			}
//			Debug::trace("ProcessDownloadedFiles", getName() + " - extension = [" + token + "]");
			if (token == grFile.getFileNameExtension())
				return true;
		}
	}
	return false;
}


/* PJR-444 - SOF-5063 - Print wider labels on Zebra printer
*   added second boolean parameter, which is used by the PrinterManager to know that it is a
*   PrinterManager for an external printer, so that the label type is the determinant for
*   rendering wider (than scale printers') labels */
void Session::setupFakePrinterManager(PrinterType thePrinter, bool externalLabelPrinter)
{
/*
Encountered the following debug when booting an HTi scale, the APC printer attached ...

[RealTimeController.Attach][0:00:06:451] USBAPCAttached -- Entry nodeId = 45DC9569
[RealTimeController.Attach][0:00:06:455] USBAPCAttached -- Exit
[APCCommManager.Printer.Message.In][0:00:06:465] PrStatus
[APCCommManager.Printer.Message.In][0:00:06:481] PrStatus
[APCCommManager.Printer.Message.In][0:00:06:497] PrWakeup
[APCCommManager.Printer.Message.In][0:00:06:497] ************************************************
[APCCommManager.Printer.Message.In][0:00:06:497] printerType: 0
[APCCommManager.Printer.Message.In][0:00:06:498] labelWidth: 1
[APCCommManager.Printer.Message.In][0:00:06:498] bufferSize: 11424
[APCCommManager.Printer.Message.In][0:00:06:498] printheadSize: 56  // which is then multiplied by 8 to get 448
[APCCommManager.Printer.Message.In][0:00:06:498] validConfig: 0
[PrinterManager][0:00:06:500] PrinterManager: 0 Started ...: 
[PrinterManager.PrinterImaging][0:00:06:500] PrinterManager: 0 Selecting ApcPrinter
[PrinterManager.PrinterReset][0:00:06:501] PrinterManager: 0 1printerResetting: 0
[PrinterManager.PrinterReset][0:00:06:501] PrinterManager: 0 resetPrinter: 0
[PrinterManager.PrinterImaging][0:00:06:501] PrinterManager: 0 pCurrentProduct: 0
[PrinterManager.StateMachine][0:00:06:503] PrinterManager: 0 PrinterUnsized
[APCCommManager.Printer.Message.Out][0:00:06:503] PrReqStatus
[PrinterManager.StateMachine][0:00:06:504] PrinterManager: 0 Teaching Printer: 0
[Session.send][0:00:06:506] tSsn000000_primary - message sent to UI: 31 31 32 2C 31 30 31 2C 22 2E 22 2C 31 2C 35 2C 35 2C 31 2C 30 2C 22 24 22 2C 32 2C 22 22 2C 30 
[PrinterManager][0:00:06:507] PrinterManager: 0 start run thread 
[PrinterManager.StateMachine][0:00:06:507] PrinterManager: 0 PrinterTryingToSize

*/
    Debug::trace("Session", getName() + HobString(" - entering setupFakePrinterManager ..."));

	USBD_NODE_ID nodeId = 0;
	if ( thePrinter == FirstPrinter )
		nodeId = FAKEPRINTER1_USBNODEID;
	else if ( thePrinter == SecondPrinter )
		nodeId = FAKEPRINTER2_USBNODEID;
	else if ( thePrinter == ThirdPrinter )
		nodeId = FAKEPRINTER3_USBNODEID;

	if ( nodeId != 0 )
	{
		RealTimeController::getInstance()->NewFakePrinter(
			nodeId,
			0, //Hobart::CEvolMsgReceiver *pCreationDevice,
			0, //PrinterTeachTable *pPrinterTeachTable,
			thePrinter,
            ((externalLabelPrinter) ? ExternalLabel : WideLabel), //WideLabel, NarrowLabel   // PJR-444 - SOF-5063 - Print wider labels on Zebra printer
			11424, //UINT16 bufferSize,
			448, //UINT16 printheadSize,
			1, //UINT16 physicalAddress,
			FakePrinter ); //PrinterKind prKind

		PAL_absclass::PAL_sleep(1000);

		Debug::trace("Session", getName() + HobString(" - setupFakePrinterManager - called NewFakePrinter"));

		HfcPrinterStockMetrics *prStockMetrics = new HfcPrinterStockMetrics();
		prStockMetrics->setPrinterType(thePrinter);
		prStockMetrics->setLabelWidth(WideLabel);
		if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgChoseToUseContinuousStock))
			prStockMetrics->setContinuousStock();
		else
			prStockMetrics->setLabelLengthInLines( (LabelLengthLines) Configuration::getInstance()->toIntCfgI("fakePrinterLabelLength") );
		setPrinterStockMetrics(prStockMetrics);
	}

    Debug::trace("Session", getName() + HobString(" - leaving setupFakePrinterManager ..."));
}

void Session::destroyFakePrinterManager(PrinterType thePrinter)
{
	USBD_NODE_ID nodeId = 0;
	if ( thePrinter == FirstPrinter )
		nodeId = FAKEPRINTER1_USBNODEID;
	else if ( thePrinter == SecondPrinter )
		nodeId = FAKEPRINTER2_USBNODEID;
	else if ( thePrinter == ThirdPrinter )
		nodeId = FAKEPRINTER3_USBNODEID;

	if ( nodeId != 0 )
		RealTimeController::getInstance()->detachFakePrinter(nodeId);
}

void Session::setBootupCheckForPeripheralsTimer(int seconds)
{
    TimerManager *timerManager = TimerManager::getInstance();
    if (timerManager)
    {
		timerManager->addTimer(getId(), seconds, Timer::RepeatNO, timedFuncCheckMissingPeripherals);
		Debug::trace("Session.initComplete", getName() + " - setBootupCheckForPeripheralsTimer(): " + inttostring(seconds) + " seconds");
    }
}

void Session::checkInitializationIsComplete()
{
	if (peripheralCheckCounter_ && browserBootupDone_)
	{
		--peripheralCheckCounter_;

		Debug::trace("Session.initComplete", getName() + HobString(" - checkInitializationIsComplete:  browserBootupDone_. Now check with the PeripheralManager."));

		bool haveWg = false;
		bool havePr = false;

		if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::FAKEWEIGHERMANAGERENABLED) )
		{
			haveWg = true;
			havePr = true;
		}
		else if (peripheralManager)
		{
            haveWg = peripheralManager->getCurrentList().isWeigherAttached();
            if(KPCBootManager::IsUpgrading()){
                //we have them you just cant see them yet
                havePr = true;
                haveWg = true;
            }
			else if (Configuration::getInstance()->isPrepackScale())
			{
				int printerNumber = FirstPrinter;
				while ((printerNumber < MaxPrinterTypes) && (havePr == false))
				{
					havePr = peripheralManager->getCurrentList().isPrinterAttached((PrinterType) printerNumber);
					++printerNumber;
				}
				Debug::trace("Session.initComplete", getName() + HobString(" - checkInitializationIsComplete:  haveWg = ") + booltostring(haveWg) + ", havePr = " + booltostring(havePr));
			}
			else
			{
				havePr = true;		//	Fake it for ServiceCounter b/c we don't care about it here.
			}
		}

		if (haveWg && havePr)
			peripheralCheckCounter_ = 0;
		Debug::trace("Session.initComplete", getName() + HobString(" - checkInitializationIsComplete:  browserBootupDone_ = ") + booltostring(browserBootupDone_) + ", peripheralCheckCounter_ = " + inttostring(peripheralCheckCounter_));

		if (peripheralCheckCounter_ > 0)
		{
	        Debug::trace("Session.initComplete", getName() + HobString(" - checkInitializationIsComplete:  Not done yet. Set CheckPeripherals timer again."));
			setBootupCheckForPeripheralsTimer();
		}
		else
		{
			//	Send bootup complete msg to UI
	        Debug::trace("Session.initComplete", getName() + HobString(" - checkInitializationIsComplete:  We are done. Send SessionDevBrowserBootupCompleted msg to UI."));
			responseMessageManager->removeMessage(ErrSessionDev, SessionDevBrowserBootupStarted, HfcResponse::RESPONSESTYLE_NOINPUT);
			responseMessageManager->addMessage(ErrSessionDev, SessionDevBrowserBootupCompleted, HfcResponse::RESPONSESTYLE_OK);
		}
	}
}

Session::DiagEventAction Session::diagnosticsEventAction( SessionMessage *msg, bool isPrimary )
{
	DiagEventAction a( Ignore );

	switch ( msg->getEventType() )
	{
	// ----------------------------------------------------
	// Dual Case events
	// ----------------------------------------------------
	case DiagConfiguration:
	case DiagAuditLogServiceVisitStatus:
		a = (( BrowserFormat == msg->getFormatType() ) ? ProcessToRTC : ProcessToUI );
		break;

	// ----------------------------------------------------
	// RTC events
	// ----------------------------------------------------

	// these events are ignored unless primary session
	case DiagAuditLogServiceVisitStart:
	case DiagAuditLogServiceVisitStop:
	case DiagCheckErrorHandlerReq:
	case DiagStartErrorHandler:
	case DiagStartErrorHandlerFromMaintRecord:
	case DiagInitWizardReq:
	case DiagPerformWizardTestingReq:
	case DiagExitWizard:
	case DiagClearServiceMaintRecord:
	case DiagPrintServiceMaintLog:
	case DiagExportDatabase:
		if ( !isPrimary )
			break;

	// forward to RTC
	case DiagConfigurationReq:
	case DiagStatusReq:
	case DiagAuditLogReq:
	case DiagAuditLogRecordReq:
	case DiagServiceMaintLogReq:
	case DiagServiceActionsReq:
	case DiagSendDatabaseEmail:
	case DiagSendDatabaseGatewayDevice:
		a = ProcessToRTC;
		break;

	// ----------------------------------------------------
	// UI events
	// ----------------------------------------------------

	// forward to UI
	case DiagStatus:
	case DiagAuditLog:
	case DiagAuditLogRecord:
	case DiagCheckErrorHandler:
	case DiagErrorHandlerFailed:
	case DiagShowWizard:
	case DiagInitWizard:
	case DiagPerformWizardTesting:
	case DiagServiceMaintLog:
	case DiagServiceActions:
	case DiagExportDatabaseComplete:
	case DiagSendDatabaseComplete:
		a = ProcessToUI;
		break;

	// ----------------------------------------------------
	// Reporting
	// ----------------------------------------------------

	case DiagReport:
		a = PrintDiagReport;
		break;

	default:
		break;
	}


	return a;
}

void Session::diagnosticsPrintDiagReport( SessionMessage *msg )
{
	if ( !msg )
		return;

	Debug::trace( "Session", "diagnosticsPrintDiagReport - BEGIN" );

	DiagnosticsReport *diagReportMsg( dynamic_cast<DiagnosticsReport*>( msg->getHfcObject() ) );

	if ( !diagReportMsg )
		Debug::trace( "Session", "diagnosticsPrintDiagReport - failed to cast report" );
	else
	{
		HfcStartReport report;
		report.setReportType( GenericTextReportType );
		report.setReportTitle( diagReportMsg->title_ );
		report.setReportContent( diagReportMsg->body_ );

		SessionMessage resp( ReportManagerDev, StartReport, this );
		resp.setHfcObject( &report );

		// go!
		handleReportManagerMessage( &resp );
	}

	Debug::trace( "Session", "diagnosticsPrintDiagReport - END" );
}


int Session::checkForRemotePrinterStatus()
{
	Debug::trace("ProductManager.remotePrint", string("START checkForRemotePrinterStatus() - onlyCheckForFatalErrors: "));

	Session *s = 0;
	if (isPrimarySession())
		s = this;
	else
		s = SessionManager::getInstance()->getPrimaryConsoleSession();
	if (!s)
		return 0;

	HfcPrinterStatus *status = s->getCurrentPrinterStatus(0);		// for just the primary printer
	if (status)
	{
		if (status->isOutOfMedia())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isOutOfMedia: true");
			return ProductErrPrintOutOfMedia;
		}
		else if (status->isLabelSizing() && !status->isTakeLabelError())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isLabelSizing: true");
			return ProductErrPrintLabelSizing;
		}
		else if (status->isTakeLabelError())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() bootup labels still hanging out of scale ->isLabelTakeError: true");
			return ProductErrPrintTakeLabelError;
		}
		else if (status->isHeadDisengaged())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isHeadDisengaged: true");
			return ProductErrPrintHeadDisengaged;
		}
		else if (status->isMediaJammed())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isMediaJammed: true");
			return ProductErrPrintMediaJammed;
		}
		else if (status->isCassetteMissing())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isCassetteMissing: true");
			return ProductErrPrintCassetteMissing;
		}
		else if (status->isLabelTakenError())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isLabelTakenError: true");
			return ProductErrPrintLabelTakenError;
		}
		else if (status->isThermalOverload())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isThermalOverload: true");
			return ProductErrPrintThermalOverload;
		}
		else if (status->isThermalShutdown())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isThermalShutdown: true");
			return ProductErrPrintThermalShutdown;
		}
		else if (status->isHardwareError())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isHardwareError: true");
			return ProductErrPrintHardwareError;
		}
		else if (status->isMediaShutdown())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isMediaShutdown: true");
			return ProductErrPrintMediaShutdown;
		}
		else if (!status->isPeripheralComm())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isPeripheralComm: false");
			return ProductErrPrintPeripheralComm;
		}
		else if (status->isManagerDeleted())
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() isManagerDeleted: true");
			return ProductErrPrintManagerDeleted;
		}
		else
		{
			Debug::trace("ProductManager.remotePrint", "checkForRemotePrinterStatus() no errors to report");
		}
	}
	return 0;
}

void Session::remotePrintReturnMessage(int returnVal, int numLabels)
{
	Debug::trace("ProductManager.remotePrint", string("START remotePrintReturnMessage() - returnVal: ") + inttostring(returnVal) + "  numLabels: " + inttostring(numLabels));

	SessionMessage remoteReturnData(ProductDev, ProductRemotePrintStatus);

	// see if this is a job we can resubmit
	bool redoJob = false;					// only a small num of jobs can be redone, under rare conditions, so default 0.
	std::vector<int>::iterator iter;
	iter = find(remoteResubmitNums.begin(), remoteResubmitNums.end(), returnVal);

	if (iter != remoteResubmitNums.end())
	{
		// found error code in the list of resubmit codes, so tell smBackend to resubmit the job.
		Debug::trace("ProductManager.remotePrint", "remotePrintReturnMessage() -  tell smBackend it can resubmit");

		redoJob = true;
	}
	else
		Debug::trace("ProductManager.remotePrint", "remotePrintReturnMessage() - returnCode not in list so we do not resubmit");

	// build return string
	HobString data(Integer::toString(returnVal));
	data.append("|");
	data.append(Bool::toString(redoJob));
	data.append("|");
	data.append(Integer::toString(numLabels));
	data.append("|done");				// 101,92,retVal|resubmit|labels|done

	Debug::trace("ProductManager.remotePrint", "remotePrintReturnMessage() - data: 101,92," + data);

	remoteReturnData.setFormatType(InternalFormat);
	GenericHfcObject gho(data);
	remoteReturnData.setHfcObject(&gho);

	// Sending this message with DEST_RTC, so that it is sent from the primary session to the remote print session that initiated this remote print
	send(&remoteReturnData, Messenger::DEST_RTC);

	Debug::trace("ProductManager.remotePrint", "FINISH remotePrintReturnMessage() ");
}


void Session::terminateRemotePrintConnection(int retVal)
{
	// need to check against a list for returnVal.  If we have 999 (success) or a
	// message code that is considered FATAL, we close the remote socket.

	Debug::trace("ProductManager.remotePrint", "START terminateRemotePrintConnection()");

	// see if retVal is in the list.
	std::vector<int>::iterator iter;
	iter = find(remoteReturnNums.begin(), remoteReturnNums.end(), retVal);

	if (iter != remoteReturnNums.end())
	{
		Debug::trace("ProductManager.remotePrint", "Found number in list so close the socket");

		// we dont want to send a messsage to terminate
		//SessionMessage remoteTerminate(SessionDev, SnTerminate);
		//remoteTerminate.setFormatType(InternalFormat);
		//mySession->send(&remoteTerminate, Messenger::DEST_RTC);

		// just terminate it directly now
		terminate(false);
	}
	else
		Debug::trace("ProductManager.remotePrint", "Did not find number in list so leave the socket open");
	
	Debug::trace("ProductManager.remotePrint", "FINISH terminateRemotePrintConnection()");
}


// static
void Session::setRemotePrintReturnCodes()
{
	Debug::trace("ProductManager.remotePrint", "setRemotePrintReturnCodes() setup remoteReturnNums vector");

	// not trusting this, so clear vector it everytime we come in here
	remoteReturnNums.clear();
	
	remoteReturnNums.reserve(RemotePrintErrorsMax);										// max codes to check = 71 add as needed
	remoteReturnNums.push_back(RemotePrintQueueStatusError);							// 993 - FATAL
	remoteReturnNums.push_back(RemotePrintQueueClearingError);							// 995 - FATAL
	remoteReturnNums.push_back(RemotePrintRegistrationError);							// 996 - FATAL
	remoteReturnNums.push_back(RemotePrintScaleIsAvailable);							// 998 final success
	remoteReturnNums.push_back(RemotePrintSuccessfulPrint);								// 999 final success
	remoteReturnNums.push_back(ProductErrPrintNoProduct);				                // 101011 - ERROR
	remoteReturnNums.push_back(ProductErrPrintNoTransaction);				            // 101012 - ERROR
	remoteReturnNums.push_back(ProductErrPriLabelNotFound); 							// 101016 - ERROR
	remoteReturnNums.push_back(ProductErrPriLabelWontFit); 								// 101019 - ERROR
	remoteReturnNums.push_back(ProductErrPrintBadByCount); 								// 101025 - ERROR
	remoteReturnNums.push_back(ProductErrPriLabelNotSized); 							// 101033 - ERROR
	remoteReturnNums.push_back(ProductErrForcedFieldNotEntered); 						// 101046 - ERROR
	remoteReturnNums.push_back(ProductErrLabelOnlyForRandomWeight);						// 101051 - ERROR
	remoteReturnNums.push_back(ProductErrCannotPrintMultLabelsForFPPType2);				// 101062 - ERROR
	remoteReturnNums.push_back(ProductErrCannotPrintWhenCustomerDisplayNotConnected);	// 101071 - ERROR
	remoteReturnNums.push_back(ProductErrOnlyForPrepackMode);							// 101078 - ERROR
	remoteReturnNums.push_back(ProductErrPrepackPrintWrongOpMode); 						// 101081 - ERROR
	remoteReturnNums.push_back(ProductErrFixedWeightVerificationNoProductionEntry);		// 101087 - ERROR
	remoteReturnNums.push_back(ProductErrFixedWeightVerificationNoPrintMultiple);		// 101088 - ERROR
	remoteReturnNums.push_back(ProductErrCannotPrintInSupervisorMode);					// 101094 - ERROR
	remoteReturnNums.push_back(ProductErrCannotPrintInServiceMode);						// 101095 - ERROR
	remoteReturnNums.push_back(ProductErrCannotPrintInSelfServiceMode);					// 101096 - ERROR
	remoteReturnNums.push_back(ProductErrCannotPrintInKeyboardNetWeightMode);			// 101097 - ERROR
	remoteReturnNums.push_back(ProductErrCannotPrintInFixedWeightVerificationMode);		// 101098 - ERROR
	//remoteReturnNums.push_back(ProductErrCannotPrintInProdEntryMode);					// 101101 - ERROR - moved to redo list per Walmart request
	//remoteReturnNums.push_back(ProductErrCannotPrintAccumulateMode);					// 101102 - ERROR - moved to redo list per Walmart request
	remoteReturnNums.push_back(ProductErrPrintBadUnitPrice);							// 101103 - ERROR
	remoteReturnNums.push_back(ProductErrPrintBadTare);									// 101104 - ERROR
	remoteReturnNums.push_back(ProductErrPrintBadCool);									// 101105 - ERROR
	remoteReturnNums.push_back(ProductErrPrintByCountAboveNineNine);					// 101106 - ERROR
	remoteReturnNums.push_back(ProductErrPrintWeigherIssue);							// 101107 - ERROR 	
	remoteReturnNums.push_back(ProductErrPrintCantFindMainSession);						// 101108 - FATAL
	remoteReturnNums.push_back(ProductErrPrintCantFindConfiguration);					// 101109 - FATAL
	remoteReturnNums.push_back(ProductErrPrintCheckScaleForPrinterRelatedError);		// 101110 - ERROR
	remoteReturnNums.push_back(ProductErrPrintCantPrintProductionPlanningLabel);		// 101111 - ERROR
	remoteReturnNums.push_back(ProductErrPrintTransactionMissingToPrint);				// 101112 - FATAL
	remoteReturnNums.push_back(ProductErrCannotPrintBuildAMeal);						// 101113 - ERROR
	remoteReturnNums.push_back(ProductErrPrintInvalidPLU);								// 101114 - ERROR
	remoteReturnNums.push_back(ProductErrPrintInvalidByCount);							// 101115 - ERROR
	remoteReturnNums.push_back(ProductErrPrintInvalidUnitPrice);						// 101116 - ERROR
	remoteReturnNums.push_back(ProductErrPrintInvalidTare);								// 101117 - ERROR
	remoteReturnNums.push_back(ProductErrPrintThermalOverload);							// 101123 - FATAL
	remoteReturnNums.push_back(ProductErrPrintThermalShutdown);							// 101124 - FATAL
	remoteReturnNums.push_back(ProductErrPrintHardwareError);							// 101125 - FATAL
	remoteReturnNums.push_back(ProductErrPrintMediaShutdown);							// 101126 - FATAL
	remoteReturnNums.push_back(ProductErrPrintManagerDeleted);							// 101127 - FATAL
	remoteReturnNums.push_back(ProductErrPrintScaleNotLicensed);						// 101130 - FATAL
	remoteReturnNums.push_back(ProductErrPrintCantCreateProductUpdate);					// 101131 - ERROR
	remoteReturnNums.push_back(ProductErrPrintPrintOnDefaultLabel);						// 101132 - ERROR
	remoteReturnNums.push_back(ProductErrPrintPrintOnCorrectedLabel);					// 101133 - ERROR
	remoteReturnNums.push_back(ProductErrPrintInvalidDefaultAndOverride);				// 101134 - ERROR
	remoteReturnNums.push_back(ProductErrPrintLabelDoesNotFit);							// 101135 - ERROR
	remoteReturnNums.push_back(ProductErrPrintUnknownLabelError);						// 101136 - ERROR
	remoteReturnNums.push_back(ProductErrPrintNoForcedByCounts);						// 101139 - ERROR
	remoteReturnNums.push_back(ProductErrPrintLabelMissing);							// 101140 - ERROR
	remoteReturnNums.push_back(ProductErrPrintPeripheralComm);							// 101141 - FATAL
	remoteReturnNums.push_back(ProductErrPrintRandomWeightPluReturned);					// 101142 - ERROR
	remoteReturnNums.push_back(ProductErrPrintWhiteListOnScaleRule);					// 101143 - ERROR
	remoteReturnNums.push_back(ProductErrPrintExceedsMaxPrice);							// 101144 - ERROR
	remoteReturnNums.push_back(ProductErrPrintNoPluPassedIn);							// 101145 - ERROR
	remoteReturnNums.push_back(ProductErrNumberOfStreamingLabelisZeroOrInvalid);		// 101146 - ERROR
	remoteReturnNums.push_back(ProductErrCantFindProduct);								// 101147 - ERROR
	remoteReturnNums.push_back(ProductErrPrintSocketTimedOutWaitingForLabels);			// 101148 - ERROR
	remoteReturnNums.push_back(ProductErrPrintSocketTimedOutWaitingForMsgs);			// 101149 - ERROR
	remoteReturnNums.push_back(ProductErrPrintUnknownModeError);						// 101150 - FATAL
	remoteReturnNums.push_back(ProductErrPrintUnknownPrinterError);						// 101151 - ERROR
	remoteReturnNums.push_back(ProductErrPrintPrintJobCancelViaUI);						// 101161 - ERROR
	remoteReturnNums.push_back(ProductErrPrintInvalidBarcodeForTemplate);				// 101162 - ERROR
	remoteReturnNums.push_back(ProductErrPrintInvalidTemplateNumber);					// 101163 - ERROR
	remoteReturnNums.push_back(ProductErrPrintPrintQueueIsFull);						// 101165 - ERROR
	remoteReturnNums.push_back(ProductErrPrintInvalidPrintOperation);					// 101166 - FATAL
	remoteReturnNums.push_back(ProductErrPrintQueueIsEmpty);                            // 101167
	remoteReturnNums.push_back(ProductErrNumberOfSellByDaysisZeroOrInvalid);			// 101168 - ERROR
	remoteReturnNums.shrink_to_fit();
	
	Debug::trace("ProductManager.remotePrint", "setup remoteResubmitNums vector");

	// not trusting this, so clear it everytime we come in here in case it already exists
	remoteResubmitNums.clear();
	remoteResubmitNums.reserve(RemotePrintRedoMax);
	remoteResubmitNums.push_back(ProductErrCannotPrintInProdEntryMode);					// 101101 - WARNING	
	remoteResubmitNums.push_back(ProductErrCannotPrintAccumulateMode);					// 101102 - WARNING	
	remoteResubmitNums.push_back(ProductErrPrintHeadDisengaged);						// 101118 - WARNING						
	remoteResubmitNums.push_back(ProductErrPrintLabelSizing);							// 101119 - WARNING
	remoteResubmitNums.push_back(ProductErrPrintMediaJammed);							// 101120 - WARNING
	remoteResubmitNums.push_back(ProductErrPrintOutOfMedia);							// 101121 - WARNING
	remoteResubmitNums.push_back(ProductErrPrintCassetteMissing);						// 101122 - WARNING
	remoteResubmitNums.push_back(ProductErrPrintLabelTakenError);						// 101128 - WARNING
	remoteResubmitNums.push_back(ProductErrPrintTakeLabelError);						// 101129 - WARNING
	remoteResubmitNums.push_back(ProductErrPrintPrintingRemoteLabel);					// 101137 - WARNING
	remoteResubmitNums.push_back(ProductErrPrintPluLoadedOnScale);						// 101138 - WARNING
	remoteResubmitNums.shrink_to_fit();
}

bool Session::isCutterInstalledAndEnabled() const
{
    return (( cutterInstalled ) && ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgIsCutterEnabled) ));
}

// WMHOOK
void Session::handleHookRequest( const std::vector<std::string>& request )
{
    if ( request.empty() )
        return;

    const std::string cmd( request[0] );

    // retrieve primary session
    Session *pri( SessionManager::getInstance()->getPrimaryConsoleSession() );

    if ( !pri )
    {
    	Debug::trace("Session", getName() + " - primary does not exist");
        return;
    }

    // ---- //

	Debug::trace("Session", getName() + " - handle hook request " + cmd);

    // Request all attached peripherals and their version information
    if ( "GetAttachedPeripherals" == cmd )
    {
        std::list<EvolMessage*> messages;

        // Request status from each device
        std::list<EvolDeviceType> devices;
        devices.push_back( WeigherSystem );
        devices.push_back( LabelApplierSystem );
        devices.push_back( NumericSystem );
        devices.push_back( MarqueeSystem );
        devices.push_back( WrapperSystem );
        devices.push_back( GCDSessionDev );

        for ( EvolDeviceType device: devices )
            messages.push_back( new SystemRequestStatus( device, pri ) );

        messages.push_back( new PrinterSystemRequestStatus( pri, FirstPrinter ) );
        messages.push_back( new PrinterSystemRequestStatus( pri, SecondPrinter ) );
        messages.push_back( new PrinterSystemRequestStatus( pri, ThirdPrinter ) );

        // Register for responses
        registerEvent( WeigherSystem, SysStatus, false );
        registerEvent( WeigherSystem, SysUsbWgStatus, false );
        registerEvent( NumericSystem, SysStatus, false );
        registerEvent( MarqueeSystem, SysStatus, false );
		registerEvent( WrapperSystem, SysUsbWrStatus, false);
		registerEvent( GCDSessionDev, SysStatus );
        registerEvent( PrinterSystem, SysStatus );

        // Send requests
        for ( EvolMessage *msg: messages )
        {
            send( msg, Messenger::DEST_RTC );
            delete msg;
        }
    }
    // Update softkey manager with vision system prediction results
    else if ( "PredictResults" == cmd )
    {
       	Debug::trace("Session.Camera", getName() + " - have prediction results");
        handleCameraPrediction( request );
    }
    else
    {
    	Debug::trace("Session", getName() + " - unknown command from hook request");
    }

	Debug::trace("Session", getName() + " - handle hook request " + cmd + " DONE");
}

void Session::performHookAction( const char *action, bool isLongRunningProcess )
{
#if defined( __linux__ )
    if ( !Configuration::getInstance()->getCustomerHooksEnabled() )
        Debug::trace("Session", getName() + " - hooks disabled cannot perform custom action " + HobString( action ));
    else
    {
        int rc = 0;
        
        char rcex[1024];
        memset( rcex, 0, sizeof(rcex) );

        const bool needToLock( isLongRunningProcess && unlockCritSectionForLongRunningProcess() );
 
        Debug::trace("Session", getName() + " - invoke hook custom action " + HobString( action ));
        hobart_cust_hook_custom_action(action, &rc, rcex, sizeof(rcex));

        Debug::trace("Session", getName() + " - hook custom action complete: " + inttostring(rc) + " '" + string(rcex) + "'");

        if ( needToLock )
        {
            lockCritSectionAfterLongRunningProcess();
            Debug::trace("Session", "Locking after executing performHookAction");
        }
    }
#endif
}

int Session::splitString( const std::string& s, std::vector<std::string>& result, char delim )
{
    // split string
    std::stringstream ss( s );
    std::string item;

    while ( std::getline( ss, item, delim ) )
        result.push_back( item );

    return result.size();
}

int Session::splitString( const std::string& s, HobList& result, char delim )
{
    // split string
    std::stringstream ss( s );
    std::string item;

    while ( std::getline( ss, item, delim ) )
        result.push_back( item );

    return result.size();
}
