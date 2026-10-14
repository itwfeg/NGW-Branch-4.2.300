/* LX Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SESSION_H
#define SESSION_H

#include "DBSession.h"
#include "evolution.h"
#include "OpModeEvent.h"

class SessionMessage;
class HobDateTime;
class HobList;

class StreamingGraphic;
class ResponseMessageManager;
class BackupRestoreManager;
class ImportManager;
class ReportManager;
class FlashFileManager;

#include "Weigher.h"
class HfcWeigherStatus;
class WeigherCalibration;
class HfcWeigherCat3Statistics;
#include "AllCat3AuditTrailRecords.h"

#include "Printer.h"
class HfcPrinterStockMetrics;

class HfcILASpeed;
class HfcPrinterStatus;

#include "Wrapper.h"
#include "WrapperPackSize.h"
class WrapperConfiguration;
class HfcWrapperIntakePkgSize;
													
#include "../EvolutionRTC/WrapperManager/Include/EvolWrapperCameraHeightScan.h"
class EvolWrapperTimingsManager;
#include "AccessKeys.h"

#include "WindowHandle.h"

#include "Timeout.h"
#include "CHobartTimer.h"

#include "HobVersion.h"

#include "../HFC/devices/HfcEvolWrapperControl.h"
#include "../HFC/devices/HfcEvolWrapperStatus.h"
#include "../EvolutionRTC/WrapperManager/Include/EvolWrapperSystemStatus.h"
#include "../core/MtdNGWWrapperData.h"

#include <thread>

class CommHistory;
class message;
class deviceList;


/**
 * Because we don't know what type of session (primary, secondary, remote, ...)
 * should be created at the instant a Session is instantiated, all functionality that is
 * specific to any particular session type will check the sessionType property before
 * allowing the particular functionality.
 * @stereotype threaded
 */
class Session : public DBSession, public Hobart::CHobartTimer
{
    public:

        static const int OP_UNKNOWN						= 0;
        static const int OP_WEIGHERINFO					= 1;
        static const int OP_WEIGHERCALIBRATE			= 2;
        static const int OP_READALL						= 3;
        static const int OP_HANDLETIMEOUT				= 4;
        static const int OP_WRAPPERSETTINGS				= 5;
        static const int OP_WRAPPERTESTS				= 6;
        static const int OP_PRINTKEYPRESSED				= 7;
        static const int OP_WRAPPERERRORTOTALS			= 8;
        static const int OP_WRAPPERPACKAGETOTALS		= 9;
        static const int OP_WRAPPERPACKCODENAMES		= 10;
        static const int OP_WRAPPERINTAKEPACKAGESIZES	= 11;
        static const int OP_WRAPPERAUTODETECT			= 12;
        static const int OP_WRAPPERAUTOCONFIG			= 13;
        static const int OP_WRAPPERNEWMODE3				= 14;
        static const int OP_WRAPPERNEWMODE4				= 15;
        static const int OP_WRAPPERNEWMODE5				= 16;

        static const int OP_WEIGHERCONFIGURATION		= 18;
        static const int OP_RESTOREDBINDEPENDENTDATA	= 19;
        static const int OP_RESTORINGDBINDEPENDENTDATA	= 20;
        static const int OP_MEDIASENSORCALIBRATION		= 21;
        static const int OP_LABELTAKENCALIBRATION		= 22;
        static const int OP_ENTERINGSUPERVISORMODE		= 23;

        static const int OP_READINGFLASHFILES			= 25;
        static const int OP_WAITINGFORCOMMBEFORESHUTDOWN = 26;
        static const int OP_WAITINGFORCOMMBEFORESHUTDOWNANDREBOOT = 27;
        static const int OP_PLATTERTAREMODE				= 28;
        static const int OP_SCALESHUTDOWN				= 29;
        static const int OP_WAITFORLABELAPPLIERINQUIRE	= 30;
        static const int OP_WAITFORDBCLOSEDTEMPORARILY	= 31;

        static const int OP_CHECKFOREXPIREDDBRECORDS    = 32;

        static const int OP_CONTINUELEAVINGPRODUCTIONENTRYMODE	= 33;

        static const int OP_WAITINGFORCOMMBEFOREFBWFREBOOT = 34;
        static const int OP_WAITINGFORSHUTDOWNUSBREMOVAL   = 35;

        static const int OP_WAITINGFORREMOTELOGIN = 36;

        static const int OP_PLANOGRAMPROCESSING = 37;

        static const int OP_WAITINGFORUSBKEYREMOVAL = 38;

		static const int OP_PROCESSDOWNLOADEDFILES = 39;
		static const int OP_PROCESSINGDOWNLOADEDFILES = 40;

        static const int OP_WAITINGTOFINISHREGISTRATION = 41;

        static const int OP_MEAL_WAITINGTORESOLVEPRINTERSTATUS = 42;

        static const int OP_AUTOBACKUPDATABASE = 43;
        static const int OP_PROCESSINGAUTOBACKUPDATABASE = 44;

		// For cat3 weigher
        static const int OP_READINGCAT3WEIGHERAUDITLOG = 45;

		static const int OP_INITHANDSFREEDEVICEMGR = 46;

        static const int OP_SENDWRAPPERDATATOAPPHOOK = 47;

        static const int OPSTATE_UNKNOWN = 0;
        static const int OPSTATE_VMSHUTDOWN = 1;

        static const int PRINTKEY_PRESSED_NORESPONSE_TIME = 2; //in seconds

        Session(CommMechanism *uiClient = 0, bool enableInput = true);

        virtual ~Session();

        /**
         * This is the message handler routine.
         * This is the method that is called when a message is received by my Messenger.
         *
         **
         * Synchronizing this method results in allowing only one message to be handled, in its entirety,
         * at a time.  This will be required if we allow the Web Server to invoke this method to handle a
         * message as well as allow the Messenger to notify the Session of messages, and handle them
         * via the same message.
         * @stereotype synchronized
         */
        void handleMessage(SessionMessage *msg);


		int send(message *msg, int destination);
		int send(EvolMessage *message, int destination, bool shouldDelayAfterSending = true);  // is virtual override

        void sendDialogToMonitoringSessions(bool addDialog, ErrorEvent errorDevice, int errorCode, short responseStyle);
        void sendMonitorModeMessage(SessionEvent mode);

        const HobString &getCurrentWebPage() const;
        const HobString &getCurrentWebPageDOM() const;
        const HobString &getCurrentWebPageMenu() const;
        const HobString &getCurrentWebPageWeightFrame() const;
        const HobString &getCurrentWebPageTarget() const;

        bool getRequestedWeigherStatus() {return requestedWeigherStatus;}
        bool setRequestedWeigherStatus(bool val)
        {
            requestedWeigherStatus = val;
#if defined( __linux__ )
            return true;
#endif
        }
        void requestAWeigherStatus(bool shouldDelayAfterSending = true);

        bool isInOperateMode() const;
        bool isInSupervisor() const;
        bool isInService() const;
		bool isInAnAccumulatingMode() const;
		bool isProductionEntryMode() const;
		void leaveBuildAMealMode();
		void leaveAccumulateItemsMode();
		void leaveProductionEntryMode();
		void returnToAccumulateItems(bool sendToUI);
		void returnToProductionEntry(bool sendToUI);
		void returnToManualWeighMode();		// just in case

		void setCurrentMode(OpModeEvent curMode);
        OpModeEvent getCurrentMode() const;

        int getCurrentModeOp() const;
        void setCurrentModeOp(int modeOp);
		
        /* If the current operation consists of many steps (or states), the current state within the current operation. */
        int getCurrentModeOpState() const;
        void setCurrentModeOpState(int modeOpState);

        OpModeEvent getMostRecentRunMode() const;


        ResponseMessageManager *getResponseMessageManager() const;


        void turnPlatterTareOn();
        void turnPlatterTareOff();

        void prepareToPrint(bool fppTestPrint = false);
        bool currentPageCanPrintLabel() const;


        //		static const short PRIMARY_LANGUAGE_SELECTION = 1;
        //		static const short SECONDARY_LANGUAGE_SELECTION = 2;

        LanguageSelection getCurrentLanguageSelection() const;
        void setCurrentLanguageSelection(LanguageSelection cls);
        void modifiedLanguageNumber(LanguageSelection changedLanguageNumber);
        void changeOperateLanguageSelection(LanguageSelection cls);
        void restoreOperateLanguageSelection();

        bool isOnSelfServiceStartupScreen() const;

        void backupDatabase(HobString functionName);

        void createNewDirectory(SessionMessage *msg);

        void manageAutoLogoutTimer();
        void removeAutoLogoutTimer();

        void setExpiredTimer(const Timeout &timeout);


        int getMkBootStatus();
        void setMkBootStatus(int status);


        void handleNetworkDevMessage(SessionMessage *msg);


        HfcWeigherStatus *getCurrentWeight() const;
        void deleteCurrentWeight();


		HfcPrinterStatus *getCurrentPrinterStatus(int whichPrinter);
        HfcPrinterStockMetrics *getPrinterStockMetrics(short printerNum);


        void setWrapperSealerBeltPower(bool turnOnturnOn, WrapperKind wrKind);
        void sendWrapperSealerBeltMessage();
        void sendEvolWrapperSealerBeltMessage(SessionMessage *msg);
        void sendEvolWrapperSettingsMessage();

		void sendWrapperSaveCycleCount();

        virtual void terminate(bool shouldSendUINotification = false, int whyTerminate = 0);


        /* These methods display/remove dialog messages to the screen.  They are declared in the DBSession class,
         * however, the implementation for them is provided by this Session class, since it is the class with the
         * ResponseMessageManager; thus, they are overridden in Session.
         */
        virtual bool addDialogMessage(ErrorEvent errorDevice, int errorCode, short responseStyle = HfcResponse::RESPONSESTYLE_OK, const HobList *msgData = 0, bool allowDuplicates = false);
        virtual bool removeDialogMessage(ErrorEvent errorDevice, int errorCode, short responseStyle = HfcResponse::RESPONSESTYLE_OK);

        void popupVirtualKeyboard(int down = 0);
        void dismissVirtualKeyboard();
        void setLanguageBarKeyboardLanguage();
        bool isTryingToDismissKeyboard;
        bool isTryingToCreateKeyboard;

        virtual void timeoutProcess( __int32 timer_id );

        bool isFPPTestPrint() const { return bFPPTestPrint_; }
        void setFPPTestPrint(bool test = true) { bFPPTestPrint_ = test; }

		bool isSupervisorKeyPresent() const;
		bool isServiceKeyPresent() const;
		bool isSelfServeKeyPresent() const;
		bool isAnyAccessKeyPresent() const;

		void setWrTimingsCompleteTimer();
		void cancelWrTimingsCompleteTimer();

		void saveOpModeSettings();
		void restoreOpModeSettings();

		void handleWrapperRestoreMessage(SessionMessage *msg);

		bool fileIsFirmware(std::string firmwareFilename, HobVersion::VersionType &firmwareDeviceType);
		bool fileIsGraphic(std::string firmwareFilename);

        void setupFakePrinterManager(PrinterType thePrinter, bool externalLabelPrinter);   // PJR-444 - SOF-5063 - Print wider labels on Zebra printer
		void destroyFakePrinterManager(PrinterType thePrinter);
		void setWrSystemInfoSavedTimer();
		void cancelWrSystemInfoSavedTimer();
		void abortWrSystemInfoMsgWait();

        // HTe Enterprise wrapper-to-hook methods
        void sendWrapperDataToHook(HfcEvolWrapperControl* wrapperConfig);
        void sendWrapperDataToHook(HfcEvolWrapperStatus* wrapperStatus);
        void sendWrapperDataToHook(MtdNGWWrapperData* wrapperFlashData);
        void sendWrapperDataToHook(EvolWrapperSystemStatus* nodeStatus);
        void sendWrapperDataToHook(int printerType, std::string softwareVersions);

		Util::CSmartPtr<AllCat3AuditTrailRecords> getCurrentAllCat3AuditTrailRecords() const { return currentAllCat3AuditTrailRecords; }
		void handleReadCat3AuditTrail();


		// used to terminate remote printing connections
		static std::vector<int> remoteReturnNums;
		// Error codes in this indicate that smBackend can resubmit the remote print job
		static std::vector<int> remoteResubmitNums;

		int checkForRemotePrinterStatus();
		void remotePrintReturnMessage(int returnVal, int numLabels);
		static void setRemotePrintReturnCodes();

		bool isCutterInstalledAndEnabled() const;

		bool getSelfServeIgnoreWeightChangeForPrintToScreen() const {  return selfServeIgnoreWeightChangeForPrintToScreen; }
		void setSelfServeIgnoreWeightChangeForPrintToScreen(bool setting) { selfServeIgnoreWeightChangeForPrintToScreen = setting; }

    protected:

        virtual void initializationBeforeRun();	// override from AbstractSession
        virtual void finalizationAfterRun();	// override from AbstractSession
        virtual void quitGracefully();			// override from AbstractSession
		/* The current mode the scale is in: defined by OpModeEvent in Event.h */
		OpModeEvent currentMode;

		virtual void sessionMessageProcess( SessionMessage * msg );  // is virtual override


    private:

        enum SessionTimer
        {
            VirtServiceJumperSet = 1,
            CameraSystemProductRecognition = 2,
        };

        enum CameraSystemTrustLevels
        {
            CameraDisabled = 0,         // no camera!
            CameraEnabled,              // show product grid
            CameraEnabledTrusted,       // show product grid; unless one product only, then pull it up
            CameraLearningModeOnly      // no product grid
        };

        void init(CommMechanism *uiClient);

        bool isVirtualServiceJumperSet();
		bool isVirtualQAServiceJumperSet();

        void enableCameraSystemProductRecognition( bool enabled );

        void noMessageFromMessengerProcessing();  // is virtual override

        bool rtcMessageShouldBeSent(EvolMessage *msg);

        bool requestedWeigherStatus;
        int currentWebPageFrame;
        std::vector<HobString> currentWebPages;

        int firstLabelLength;
        int secondLabelLength;
		int secondLabelRotated;
        
        /* The current operation the user is performing within the currentMode. */
        int currentModeOp;

        /* If the current operation consists of many steps (or states), the current state within the current operation. */
        int currentModeOpState;

        OpModeEvent mostRecentRunMode;

		OpModeEvent toggleReturnToRunMode;		// The Operate mode to return to after toggling the 'Label' button off (Prepack).

		EvolWrapperTimingsManager* evolWrapperTimingsManager_;
		void createEvolWrapperTimingsManager();


        // flag set to true when restoring version independent data. Added to fix Bug 1218 <<SMM>>
        bool restoreVersionIndependentData;


        time_t startPrintKeyPressedTime;


        // This value is '1' for the Primary Language, and '2' for the Secondary Language.
        LanguageSelection currentLanguageSelection;
        LanguageSelection operateLanguageSelection;


        // Boolean to flag whether we have "registered" for SnContinuousStatistics
        // with the Primary Session. In the destructor we will "deregister" with
        // the Primary Session if we are still registered (if the remote session
        // is killed in the statistics page).
        bool registeredForContinuousStatistics_;
        // This is a counter of the number of sessions "registered" for continuous
        // statistics. It is used to start and stop the timer that triggers the
        // requests for EVM statistics and the sending of the statistics to the
        // corresponding browsers.
        int statisticsListeners_;


        AccessKeys accessKeys;
        Util::ValidateWindowParameters virtualKeyboardParams_;
        static bool validateVirtualKeyboard(HWND hwnd, Util::ValidateWindowParameters* parameters);

        Util::ValidateWindowParameters primaryScaleParams_;
        Util::ValidateWindowParameters browserScaleParams_;
        static bool validatePrimaryScaleTitle(HWND hwnd, Util::ValidateWindowParameters* parameters);
        static bool validateBrowserScaleTitle(HWND hwnd, Util::ValidateWindowParameters* parameters);

        bool constantlyUpdatingDateTime;
        HobDateTime *currentDateTime;
        void sendDateTimeMessage();

		bool constantlyUpdatingNetworkStatus;

        /* commRequestedShutdown_ : boolean variable
           true  - indicates Scale Management Software requested the scale to shutdown
           false - default value */
        bool commRequestedShutdown_;
        CommHistory *commHistory;

        StreamingGraphic *currentStreamingGraphic;

        ResponseMessageManager *responseMessageManager;
        BackupRestoreManager *backupRestoreManager;
        ImportManager* importManager;
        ReportManager *reportManager;

        void handleModeMessage(SessionMessage *msg);
		void handleRegistrationMessage();
        void handleSessionMessage(SessionMessage *msg);
        void handleReadAll(SessionMessage *msg);
        void handleBackupRestoreMessage(SessionMessage *msg);
        void handleImportMessage(SessionMessage *msg);
        void handleReportManagerMessage(SessionMessage *msg);
        void handleNetworkConnectivityManagerMessage(SessionMessage *msg);
        void handleNowServingQueueManagementMessage(SessionMessage *msg);
		int handleRemotePrintInitialAvailability();

        void handleCGICallManagerMessage(SessionMessage *msg);
        void handleMFGTestMessage(SessionMessage *msg);
        void handleJoeMessage(SessionMessage *msg);

        void performHookAction( const char *action, bool isLongRunningProcess = true );
        void handleHookRequest( const std::vector<std::string>& request );

        bool onSelfServiceStartupScreen;
		bool onStartUpScreenInSecondLanguage;
        void setOnSelfServiceStartupScreen(bool onStartupScreen);

        void messageBeforeAccessSystemDesktop();
        void accessSystemDesktop();

        // This was for when I was returning ints
        //int touchScreenSettingType;		// 5 = touchup, 1 = normal (make default 1 in future when we go that route)
        //int getTouchScreenSettingType();

        // This is for returning HobStrings
        HobString touchScreenSettingType;		// 5 = touchup, 1 = normal (make default 1 in future when we go that route)
        HobString getTouchScreenSettingType();

        bool startingIntranetBrowser;
#if defined ( __linux__ )
        void popupIntranetBrowserWindow(int whatFrame, HobString pageAddress = "http://127.0.0.1/mainRunMode.asp?consoletype=4");
#else
        void popupIntranetBrowserWindow(int whatFrame, HobString pageAddress = "");
#endif
        void popupScaleBrowserWindow();
        void ieWindowToFront();

        int autoLogoutTimerId;
        Timeout expiredTimer;
        void handleTimeout(Timeout *timeout, bool shouldExitWhenFinished = false);


        FlashFileManager	*flashFileManager;
        bool mergeFlashFileData;
        void processFlashFile();
        void getNextFlashFile();
        void startFlashFileMerge();

        void handleWindowsConfigurationManagerMessage(SessionMessage *msg);
        void handleVersionManagerMessage(SessionMessage *msg);
        void finishVersionManagerShutdownRequest(bool okToShutdown);


        int compactFlashTaskId;
        int mkbootStatus;
        void setupBootableCompactFlash(bool copyEntireFlash);
        static void makeBootableCompactFlash(Session *mySession, bool copyEntireFlash);


        void handleSystemMessage(SessionMessage *msg);
        void handleMarqueeMessage(SessionMessage *msg);
        void handleNumericMessage(SessionMessage *msg);

		void handleGraphicalCustomerDisplayMessage(SessionMessage *msg);

        HfcWeigherStatus *currentWeight;
        std::list<HfcWeigherStatus*> currentWeightDoomed;

        void handleWeigherMessage(SessionMessage *msg);

		HfcWeigherCat3Statistics *currentWeigherCat3Statistics;
		Util::CSmartPtr<AllCat3AuditTrailRecords>  currentAllCat3AuditTrailRecords;
		int currentCat3AuditRecordPage;
		int currentCat3NextPageRequests;
		int currentNonWeigherCat3Records;

        bool readingCat3AuditTrailForUI;
        bool readingCat3AuditTrailForPrinting;

        bool readingCat3AuditTrailForSavingToDisk;
		std::string readingCat3AuditTrailForSavingToDiskFilename;

        bool readingCat3AuditTrailForComm;
        bool readingCat3AuditTrailForHTe;
		std::string ipAddressForSavingAuditTrailResponse;
        bool readingCat3WeigherConfigForComm;

		bool handleSaveCat3AuditTrailToDisk( const std::string filename );
		void handleSaveCat3AuditTrailForUI( std::string filename );
		void handlePrintCat3AuditTrail();

        typedef enum WeigherStates
        {
            WeigherStateUnknown,
            WakingUp,
            Rezeroing
        }   WeigherStates_t;
        WeigherStates currentWeigherState;

        // Dual range will only be enabled in Operate mode, not in Supervisor or Service.
        bool dualRangeEnabled;

        bool weigherSwitchPressed;
        WeigherCalibration *weigherCalibration;
        void handleWeigherCalibration(SessionMessage *msg);

		void notifyExternalProcessOfWeight(HfcWeigherStatus *weight);


        std::vector<HfcPrinterStockMetrics *> printerStockMetrics;
        void setPrinterStockMetrics(HfcPrinterStockMetrics *prStockMetrics);

        static const short PrNoCalib = 0;
        static const short MSDisengage = 1;
        static const short MSSituateLabelStock = 2;
        static const short MSEngage = 3;
        static const short MSLabelStockTaut = 4;
        static const short MSCalibrating = 5;
        static const short MSComplete = 6;
        static const short LTClearSensor = 7;
        static const short LTCalibrating = 8;
        static const short LTComplete = 9;

        short prCalibStep;
        PrinterType currentPrinter;

		std::vector<HfcPrinterStatus *> currentPrinterStatus;

        void handlePrinterMessage(SessionMessage *msg);
        bool initPrCalib(PrinterCalibrateType prType);
        bool handlePrCalib(SessionMessage *msg);

		void handleKPCMessage(SessionMessage *msg);
        void handleBoardSiteMessage(SessionMessage *msg);

//	No longer used.        HfcILASpeed *currentILASpeed;
        void handleLabelApplierMessage(SessionMessage *msg);


//	No longer used.        WrPackCode			currentPackCode;
        WrapperPackSize		autoConfigSize;
        bool	wrapperSealerBeltOn;
        bool	wrapperSealerBeltOff;

        void handleWrapperMessage(SessionMessage *msg);
        void handleNGWrapperMessage(SessionMessage *msg);
        void sendPackageSizeToUI(HfcWrapperIntakePkgSize *packageSize);
        void initAutoConfigSize();
//	No longer used.        void sendPackCodeToUI(WrPackCode thisPackCode);
        void sendWrapperWrReqStatusMsg();
        void sendWrapperPrintersLabelCounts();
        void sendPrinterSysReqStatusMsgs(Session *session);

        void handleRFIDMessage(SessionMessage *msg);
        bool skipRFIDLoginAndNotifyUIofRFIDInput;

        HobString savedIntranetAddress;

        SessionMessage *savedModeChangeMessage;

        ULONG disableFBWF();

        bool bFPPTestPrint_;

		EvolWrapperCameraHeightScan curHeightScan;
		bool	wrapperSealerBeltPower;
		
		void productSwitchToManualWeighMode();
		void productSwitchToAutoManualApplyMode();
		void productSwitchToBuildAMealMode();
		void productReturnToPriorOpMode();

		bool accumulateItemsAddedMasterProduct;

		bool receivedCommHeader_;

		int previousWirelessStrength_;
		
		bool browserBootupDone_;
		unsigned int peripheralCheckCounter_;
		void setBootupCheckForPeripheralsTimer(int seconds = 2);
		void checkInitializationIsComplete();

        bool waitingForImportTerminate_;
		bool restoredProducts;

		int currentAutoBackupDatabaseGroup_;

// temporarily moved to be public
//		void handleReadAuditTrail();

		/// Diagnostics event actions.
		enum DiagEventAction
		{
			Ignore,
			ProcessToRTC,
			ProcessToUI,
			PrintDiagReport
		};

		static DiagEventAction diagnosticsEventAction( SessionMessage *msg, bool isPrimary );

		void diagnosticsPrintDiagReport( SessionMessage *msg );
		
		std::thread checkForExpiredRecords_;
		void checkForExpiredRecordsThread();


		void terminateRemotePrintConnection(int retCode);

		// Hands-Free Device stuff
		std::string handsFreeDeviceMessage;
		deviceList *currentHandsFreeDevicesInRangeList;
		int currentHandsFreeDevicesInRangeOffset;

		std::string hfpdLoggedIn;
		int hfpdLoggedInOutOfRangeCount;

		std::string hfpdStrongestSignal;
		int hfpdStrongestSignalCount;

        void handleCameraPrediction( const std::vector<std::string>& request );

        int cameraSystemProductRecognitionLoopCount;
        bool cameraSystemReady;

		std::string determineClosestHandsFreeDevice(const deviceList *list);
        static const std::string boardSiteFile_;
        static const std::string boardSiteLvdsFile_;
		// SOF-3699: Fake Weight
		bool fakeWeightMode;

        bool cutterInstalled;
        
        bool wghtDialogSet;
        
		int previousModeForStartup;

		bool selfServeIgnoreWeightChangeForPrintToScreen;

        // For some HTe Enterprise app hook-related code
        bool hasProcessedWrStatusMsg;
        bool hasProcessedPrSystemMsg;

        static int splitString( const std::string& s, std::vector<std::string>& result, char delim );
        static int splitString( const std::string& s, HobList& result, char delim );

#ifdef USE_ACCOUNTANT
        static int _dataType_;
        static std::string _dataTypeName_;
#endif
};

#endif //SESSION_H
