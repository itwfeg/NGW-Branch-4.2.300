#ifndef SESSIONEVENT_H
#define SESSIONEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"

	// Session Generated Events - SessionDev
	typedef enum SessionEvent
	{
		SnUnknown,
		SnRegister, 	// meaning to set up for the indicated Session Type (Primary, Remote, ...)
		SnVersionInfo,
		SnTimedOut,
		SnTerminate,
		SnReadDateTime,						// 5
		SnWriteDateTime,
		SnContinuousDateTime,
		SnStopDateTime,
		SnReadWeighModeTransactionType,
		SnWriteWeighModeTransactionType,	// 10
		SnReadDirectoryList,
		SnWriteDirectoryList,
		SnReadDevicesList,
		SnWriteDevicesList,
		SnSyncDOMRequest,					// 15
		SnSyncDOM,
		SnMonitorModeOn,
		SnMonitorModeOff,
		SnReadCurrentWebPage,
		SnWriteCurrentWebPage,				// 20
		SnCommStart,
		SnCommEnd,
		SnTouchTypeSetting,					// the enum formerly known as SnDeleteAllFiles moved to Config
		SnAllFilesDeleted,
		SnPrintKeyPressed,					// 25
		SnClearKeyPressed,
		SnSelectCurrentLanguage,
		SnReadLocaleSettings,
		SnWriteLocaleSettings,
		SnStartStreamingGraphic, 			// 30
		SnStopStreamingGraphic,
		SnReadStreamingGraphic,
		SnWriteStreamingGraphic,
		SnCreateDirectory,
		SnInsertNewDirectory,				// 35
		SnPlatterTareOn,
		SnPlatterTareOff,
		SnRequestPlatterTare,
		SnDesktopAccess,
		SnUpdateUIFonts, 					// 40
		SnTarUserDirectory,
		SnTarSystemDirectory,
		SnRestoreIndependentData,
		SnTFTPdbDate,
		SnCompactFlashMakeBootrom,			// 45
		SnCompactFlashCopyInternal,
		SnUsingEmptyDatabase,
		SnReadScaleStatus,
		SnWriteScaleStatus,
		SnReadStatistics,					// 50
		SnWriteStatistics,
		SnContinuousStatistics,
		SnStopStatistics,
		SnBootupCompleted,
		SnDisableInputEvent,				// 55
		SnEnableInputEvent,
		SnShutdownTSInput,
		SnSimulateTouch,
		SnSoundBeep,
		SnKeyActivation,					// 60
		SnKeyDeactivation,
		SnRequestKeyActivation,
		SnKeyActivationStates,
		SnDBIndependentBackup,  // SnFreeSpaceBasedRequestDBCompress,
		SnRequestDBCompress,				// 65
		SnDBCompressStarting,
		SnDBCompressDone,
		SnPrinterMetrics,
		SnUsbKbdAttachChange,
		SnSelectActiveWebPageFrame,			// 70
		SnDisplayingModalPopupKeyPadOn,		// No longer needed for the LXScale (Windows)
		SnDisplayingModalPopupKeyPadOff,		// No longer needed for the LXScale (Windows)
		SnBrowserMenuChanging,
		SnAddNewSession,
		SnPopupVirtualKeyboard,				// 75
		SnDismissVirtualKeyboard,
		SnDBClosingTemporarily,
		SnDBReopened,
		SnPopupIntranetBrowserWindow,
		SnPopupScaleBrowserWindow,			// 80
		SnRemoveSessionStarter,
		SnSetFlashkeyInternetWebPage,
		SnSwitchToIntranetFrame,
		SnRequestIntranetAddress,

		SnUsingMirrorDatabase,				// 85

		SnRefreshFPPFrame,

		SnProductSwitchToManualWeighMode,		//	EPP - Essentially turn off the 'Label' button
		SnProductReturnToPriorOpMode,			//	EPP - Return to the mode prior to the event SnProductSwitchToManualWeighMode
		SnDisplayDialog = 89,

		SnStopDialogs = 90,					// 90
		SnRestartDialogs,

		SnShutdownNonPrimary,

		SnPrimaryToMonitoringSessionMsg1,
		SnPrimaryToMonitoringSessionMsg2,
		SnPrimaryToMonitoringSessionPopDialog,		// 95

		SnShutdownComplete,
		SnShutdownRequest,
		SnTerminated,
		SnReboot,

		SnTestMessage				= 100,			// 100
		SnDelay,
		SnDebugTrace,
		SnDebugWrite,
		SnWeigherFrequency,
		SnTestAllProducts,							// 105
		SnConfigUpdate,
		SnFileHandlesUsage,
		SnSetNewFirmwareVersionSearchTime,
		SnSendShutdownSystemRequest,

		SnLogoffRequest,							// 110

		SnRemoteLoginRequired,
		SnProductSwitchToAutoManualApplyMode,

		DBRestoreFinished,
		DBRestoreInProgress,

		SnDBBackupStarting,							// 115
		SnDBBackupDone,

		SnTestAddTimer = 120,						// 120
		SnTestDeleteAllSessionTimers,
		SnTestDisplayAllTimers,

		SnShowAllTasks = 125,						// 125

		SnCheckForExpiredRecords,
		SnCheckForDownloadFiles,
		SnProcessDownloadedFiles,

		SnPerformAutoBackupDB,

		SnDispatchConfigurationSaved = 130,			// 130
		SnTellBrowserGotoHomepage,

		SnReadNetworkInfo,
		SnWriteNetworkInfo,

		SnReadLicensedPackages,
		SnWriteLicensedPackages,					// 135

		SnManufacturingTestSummaryReport,

		SnAddSessionAsTraceListener = 140,			// 140
		SnRemoveSessionAsTraceListener,

		SnPlatterTareUnstable = 145,				// 145
		SnNowServingQueueManagement,

		SnUpgradeFirmwareStatus = 147,
		SnUpgradeSFTPBadAuthentication = 148,

		SnCreateLostPeripheralsTimer = 150,			// 150

		SnRequestSaveConfiguration,

		SnBackendRemotePrint = 160,				    // 160 - Start of remote print area of commands. In the end may not be large enough
		SnBackendIsScaleAvailable,
		SnBackendRemotePrintTemplateLabel,
		SnBackendRemoteTerminate,
		SnBackendPrinterStatus,

		SnDisplayAllWebPageFrames = 170,			// 170

		SnInitializeSynchronization = 180,			// 180
		SnTerminateSynchronization,
		SnChangeSynchronizationType,
		SnChangePrimaryScale,

		SnProductSwitchToBuildAMealMode,

		SnRdpActive,								// 185
		SnRdpInactive,

		SnSelfServReimageLabels,

		snMergeNow,									// 188 - SOF-4769
		snMergeNowAfterComm,

		SnExecuteTask = 200,

		SnInvalidModeDetected

		//	Beware adding any new items to the end here. They may be hard-coded in Session.cpp
	}   SessionEvent_t;

	typedef enum SessionDevError
	{
		SessionDevErrUnknownError = SessionDev * DEVICE_ERROR_OFFSET,
		SessionDevErrNotPrimary,
		SessionDevErrCommLoss,
		SessionDevAllFilesDeleted,
		SessionDevBootableCFInProgress,
		SessionDevBootableCFComplete,              // 5
		SessionDevBootableCFError,
		SessionDevReqNewDirName,
		SessionDevErrDuplicateDirectory,
		SessionDevDeleteAllFilesInProgress,
		SessionDevChangingLanguages,              // 10
		SessionDevDBCompressStarting,
		SessionDevEnteringMonitorMode,
		SessionDevRemoveThumbDrive,
		SessionDevDateTimeReadError,
		SessionDevDeleteAllFilesFailed,           // 15
		SessionDevBootableCFErrorSuperKeyPresent,
		SessionDevBrowserBootupStarted,				//	Used on the browser side
		SessionDevBrowserBootupCompleted,			//	Used on the browser side
		SessionDevTimedWrSealerBeltOff,
		SessionDevTimedWrSealerBeltOn,            // 20
		SessionDevCommExistingBeforeShutdown,
		SessionDevShuttingdown,
		SessionDevShutdownComplete,
		SessionDevInactivityAutoLogout,
		SessionDevShutdownAreYouSure,					// 25
		SessionDevDBClosingTemporarily,
		SessionDevUsingEmptyDatabase,
		SessionDevCheckForThumbdrive,
		SessionDevLoadingIntranetIE,
		SessionDevShutdownWithoutFBWFAreYouSure,	// 30
		SessionDevNoUpdateMessageBeforeDesktop,
		SessionDevCouldNotUnprotectOS,
		SessionDevUsingMirrorDatabase,
		SessionDevConfigWindowsWithoutRestrictedLogin,
		SessionDevConfigWindowsWithRestrictedLogin, // 35
		SessionDevShutdownWithoutFBWFLinuxAreYouSure,

		SessionDevAccumulateItemsNoProductsInList,
		SessionDevAccumulateItemsRenderingManyProducts,
		SessionDevProductionEntryNotEnabled,   
		SessionDevAccumulateItemsNotEnabled,	// 40
		SessionDevAccumulateItemsEmptyList,
		SessionDevAccumulateItemsItemRemoved,
		SessionDevAccumulateItemsUndefinedLabelType,
		SessionDevAccumulateItemsNoValidLabelType,
		SessionDevItemAddedToAccumulateItemsList,	// 45
		SessionDevAccumulateItemsMaxItemsExceeded,

		SessionDevRebootImmediate,
		SessionDevRemoveUSBDrive,

		SessionDevRemoteAccessProhibited,

		SessionBackingUpDatabase,					// 50

		SessionDevDBIsCorrupt,

		SessionDevDeleteAllFilesAlreadyInProgress,

		SessionDevAccumulateItemsFirstItemBarcodeFitError,
		SessionDevAccumulateItemsFirstItemBarcodeFitWarning,
		// 3 Self service messages are missing here from whomever wrote that code so... 
		SessionDevBeginSelfService,                 // 55
		SessionDevSelfServInitAndLoading,
		SessionDevSearceFeatureDisabled,
		SessionDevToggleModeNotEnabled = (SessionDev * DEVICE_ERROR_OFFSET) + 58,
		SessionDevRemotePrintWarning,

		SessionDevPick5ActiveStatus,				// 60
		SessionDevRemoveUSBServiceKey,
		SessionDevOnlineOrderingActive,
		SessionDevOnlineOrderingEnabled,
		SessionDevWillNotDisableExtraDay,
		SessionDevSecLabelEnableToggleStatus,
        
		SessionDevRfidPrintingEnabled,
		SessionDevRfidPrintingDisabled,

		SessionDevErrorLastEror

	}   SessionDevError_t;

#endif // SESSIONEVENT_H
