/**
 * @file Events.h
 *
 * Events contains every single event that may occur in the user interface. Every single message
 * in the scale is represented by a device and event. The following class represents each device
 * and an enumerated list that represents every single possible event that may be able to occur.
 * scale application.
 *
 * @author: Joe E. Crew
 * @addtogroup thinClientMessagingCsu
 */

#ifndef EVENTS_H
#define EVENTS_H

#define DEVICE_ERROR_OFFSET 1000
#define SELF_SERVE_ERROR_OFFSET 500
/**Events.h
 *
 * The list of all events in the user interface. The events listed are have a 1:1 relationship with the same events
 * in the scale application. Same names and enum values to promote familiarity and code flow. There are some simple
 * modifications such as all events listed in the same file, and events not used by the user interface are not included.
 */

// 1,# - WeighEvent
typedef enum WgEvent
{
    WgWakeup,                       // 0
    WgConfig,                       // 1
    WgInquire,                      // 2
    WgReset,                        // 3
    WgStatus,                       // 4
    WgEnable,                       // 5
    WgDisable,
    WgMode,
    WgRezero,
    WgControl,
    WgColdReset,                    // 10 - Sent to the Atmel Sam-ba weigher. [PTR 387 & 388]
    WgCat3ReqStatistics,
    WgCat3AuditStatistics,
    WgCat3DateTime,
    WgCat3ReqPage,
    WgCat3Records,                  // 15
    WgCat3ReqNextRecords,
    WgCat3WriteRecord,
    WgCat3RecordStatus,
    WgCat3ReqEraseLog,
    WgCat3LogEraseComplete,         // 20
    WgCat3Error,
    WgResetFactoryDefaults,
    WgResetFactoryDefaultsComplete,
    WgKPCDetached,
    WgReqSystemInfo,                // 25
    WgSystemInfo,                   // 26,
    WgNormal = 32,                  // 32 - this is a command from rtc
    WgLock,                         // this is a command from rtc
    WgReqWakeup,
    WgReqStatus,                    // 35 - Request Status Message From Peripheral
    WgReqInquire,                   // Request Inquire Message 1,2 From Peripheral
    WgReqKind,
    WgKind,
    WgCachedStatus,                 // old, cached status reported to RTC
    WgReqValueMax,                  // 40
//    WgValueMaxAssembly,
//    WgValueMaxWriteAssembly,
//    WgValueMaxHwVersion,
//    WgValueMaxWriteHwVersion,
//    WgValueMaxSerialNumber,         // 45
//    WgValueMaxWriteSerialNumber,
//    WgValueMaxDateOfManufacture,
//    WgValueMaxWriteDateOfManufacture,
//    WgValueMaxXGain,
//    WgValueMaxWriteXGain,           // 50
//    WgValueMaxYGain,
//    WgValueMaxWriteYGain,
//    WgValueMaxZGain,
//    WgValueMaxWriteZGain,
//    WgValueMaxAllGains,             // 55
    WgKPCError                      = 56,
    WgStatusAndPrint                = 99,
    WgCalibrationStep               = 100,
    WgCalibrationEnd,
    WgReqInfo,
    WgInfo,
    WgStatusCounts,
    WgSaveInfo,                     // 105
    WgAttach,                       // Set the weigher attached flag.
    WgDoneInfo,                     // 107 - The user is leaving the Weigher Info page.
    WgConfigurationBegin = 110,     // 110
    WgConfigurationEnd,
    WgBackupWeigherSettings,
    WgRestoreWeigherSettings,
    WgBeginRemoteWgConfig,
    WgEndRemoteWgConfig,            // 115
    WgReadAuditTrail,
    WgWriteAuditTrail,
    WgPrintAuditTrail,
    WgSaveAuditTrailToDisc,
    WgSaveAuditTrailToDiskFinished, // 120
    WgReadAllAuditTrailForHTe,
    WgReadAllAuditTrailForComm,
    WgReadWeigherConfigForHTe,      // 123

    WgReqSerialNum,
    WgSerialNumber,                 // 125
    WgReqVersion,
    WgVersion,
    WgValueMaxFitParams,

    // DO NOT ADD NEW WEIGHER EVENTS AFTER 200 ... See WeigherEvent.h
    WgTestInit = 200,               // 200
    WgTestRezero,
    WgTestReadConfig,
    WgTestUp1 = 205,                // 205
    WgTestUp10th,
    WgTestUp100th,
    WgTestUp1000th,                 // 208
    WgTestDown1 = 210,              // 210
    WgTestDown10th,
    WgTestDown100th,
    WgTestDown1000th,               // 213
    WgTestSwitch = 215,             // 215
    WgTestSimulateWgStatus,         // Fake weight
    WgEndTestSimulateWgStatus,      // End 'Fake Weight Mode'

    WgEventMax

} WgEvent_et;

// 2,# - WeighSystem
typedef enum WgSysEvent
{
    WgSysStatus         = 4,
    SysReqStatus        = 13,
    SysUsbWgStatus      = 20
} WgSysEvent_et;

// 4,# - OperationMode - Operator Mode Generated Events
typedef enum OpModeEvent
{
    OpModeIdle                  = 0,    // 0 - initial operating mode
    OpModeOperate               = 1,
    OpModePluEntry              = 2,
    OpModeAutoWeigh             = 3,    // 3 - NGW: Apply | Wrap | Label
    OpModeSemiWeigh             = 4,    // 4 - NGW: Apply | Label
    OpModeManualWeigh           = 5,    // 5 -  "Manual Operation" on Mode Dialog Counter Scale. ALSO selected "Normal" button while in Product Entry on Counter Scale
    OpModeSupervisor            = 6,
    OpModeService               = 7,
    OpModeServiceLogin          = 8,
    OpModePowerDown             = 9,
    OpModeMonitor               = 10,   // 10
    OpModeWrapOnly              = 11,   // 11 - NGW: WRAP ONLY mode
    OpModeStatus                = 12,
    OpModeSelfService           = 13,   // "Self Service Operation" on Mode Dialog Counter Scale
    // When there is a label applier,
    // OpModeAutoManualApply will automatically print a label,
    // but will not run the machine.
    OpModeAutoManualApply       = 14,   // "Prepack Operation" on Mode Dialog Counter Scale
    OpModeProductionEntry       = 15,   // "Production Entry" on Mode Dialog Counter Scale
    OpModePrintMultipleLabels   = 16,
    OpModeAccumulateItems		= 17,
    OpModeMealBuilder           = 18,
    OpModeMeatSheets            = 19,
    OpModeLabelsPrintedCounts   = 21
} OpModeEvent_t;

// 5,# - LabelApplier
typedef enum LaEvent
{
    LaConfig            = 1,
    LaInquire           = 2,
    LaMode              = 7,
    LaTest              = 8,
    LaSensors           = 12,
    LaSetup				= 14,
    LaCLASysStatus      = 21,
    LaSysInfo           = 25,
    LaSysInfoWrite      = 26,
    LaSysInfoErase      = 27,
    LaTestComplete      = 28,
    LaReqSysInfo        = 29,
    LaReqInquire        = 34,
    LaSensorsTracking   = 40,
    LaSndLabelPos       = 50,
    LaILASpeed          = 86,
    EvolLaConfig        = 100           // made up since I could not find 5,100 in app
} LaEvent_et;

// 6,# - LabelApplierSystem
typedef enum LaSysEvent
{
    LaSysStatus         = 4,
    LaSysReqStatus      = 13
} LaSysEvent_et;

// Completely in step with scale application 7.10.2019
// 7,# - Printer
typedef enum PrEvent
{
    PrWakeup,                       // 0
    PrConfig,
    PrInquire,
    PrReset,
    PrStatus,
    PrEnable,                       // 5
    PrDisable,
    PrMode,
    PrSensors,
    PrPrintheadPower,
    PrMask,                         // 10
    PrTeach,
    PrCommand,
    PrCalibrate,
    PrRam,
    PrTest,                         // 15
    PrFlush,
    PrNomSize,
    PrEnableTakeup = 19,
    PrReqWakeup = 21,               // Request Wakeup message from Peripheral
    PrReqInquire,
    PrReqStatus,
    PrReqSensors,
    PrReqLabelTransfer,             // 25
    PrReqCalibrate,
    PrSensorsTracking = 28,
    PrLabelTransferComplete,
    PrLabelTransferFailed,          // 30
    PrTestLabel,
    PrReqLabelWidth,
    PrLabelWidth,
    PrNewOrder,
    PrTakeLabelTime,                // 35
    PrLabelImageSize,
    PrReqPrinterType = 37,
    PrPrinterType,
    PrPrinterTypeTracking,
    PrReqStockMetrics,              // 40
    PrStockMetrics,
    PrReqAdvLabelStock,
    PrAdvLabelStock,
    PrNewAddress,
    PrSizing,                       // 45
    PrReqPrintheadPower,
    PrPreImage,
    PrPostImage,
    PrReqKind,
    PrKind,                         // 50
    PrLabelComplete,
    PrLabelReady,
    PrLabelTaken,
    PrDeletePackage,
    VpShutdown,                     // 55
    VpIOCfg,
    PrResetFactoryDefaults = 57,
    PrResetFactoryDefaultsComplete,
    PrReqHeadType,
    PrHeadType,                     // 60
    PrCutterCut,
    PrCutterHome,
    PrReqCutterStatus,
    PrCutterStatus,
	PrCutterEnabled,				// 65
	PrCutterDisabled,				// 66
    PrEjectCut,
    PrResetTeach,
    
    PrReqInfo = 100,                //	100 - Request data for the Printer Info page.
    PrInfo,                         //	The data for the Printer Info page.
    PrSaveInfo,                     //	The modified data from the Printer Info page.
    PrSetPrinters = 104,
    PrResetImmediate,               // 105
    PrReqCurrentStatus,
    PrClearCurrentStatus,
    PrStatusClear,
    PrStatusErrors,                 // 109 - no chobartd support
    PrReadAllUsbDriverNames = 111,
    PrWriteAllUsbDriverNames = 112,
    PrReadAllNetworkPrinterDriverNames  = 113,
    PrWriteAllNetworkPrinterDriverNames = 114,
    PrReadExternalPrinter = 120,    // 120
    PrReadAllExternalPrinters,
    PrWriteExternalPrinter,
    PrWriteAllExternalPrinter,      // 123
    PrSaveExternalPrinter = 126,
    PrReadAllProductTextPrintParams = 131,
    PrWriteProductTextPrintParam = 132,
    PrWriteAllProductTextPrintParams = 133,
    PrSaveProductTextPrintParam = 136,
    PrPrintPdfDocument = 140,       // 140
    PrPrintWebDocument,             // 141
    PrReqVirtualLabel = 150,        // 150
    PrVirtualLabelComplete,
    PrAgentAttached,
    PrAgentDetached,
    PrBackupPrinterSettings,
    PrRestorePrinterSettings,       // 155
    PrReqLabelSizing = 160,         // 160
    PrUseContinuousStock,
    PrSetupForNormalSizing,
    PrReqAreWeSizingLabels,
    PrAreWeSizingLabels,            // 164
    PrReqVersion = 170,             // 170
    PrVersion,
    PrKPCDetached,
    PrLabelTransferReady,
    PrReqHeadDotWear,
    PrHeadDotWear,                  // 175
    PrRS485FlowControl,
    PrKPCError,
    PrFlushCmplt,
    PrHeadDotStatus,
    PrReqHeadDotStatus = 180,       // 180
    PrHeadDotAck,
    PrReqTestCassette,
    PrTestCassette,
    PrCutterBitDflt,                // SOF-3887
    PrStartGapCalibration = 188,
    PrGapCalStatus,
    PrGapCalNext,
    PrRfidTestRequest,
    PrRfidTestPassed,
    PrRfidTestFailed,
    PrRfidCalRequest,
    PrRfidCalPassed,
    PrRfidCalFailed,
    PrRfidUpgradeRequest,
    PrRfidUpgradePassed,
    PrRfidUpgradeFailed,
    PrRfidQryRequest,
    PrRfidQryBusy,
    PrRfidQryPassed,
    PrRfidQryFailed


#if 0
    PrLabelTransferStatus,
    PrRequest,
    PrExecute,
    PrLabelTransfer,                // 185 NEW for NGW
    PrCANWakeup,                    // NEW for NGW
    PrCANSensors,                   // NEW for NGW
    PrError,                        // NEW for NGW
    PrEjectLabel,                   // NEW for NGW
    PrPrintLabel,                   // 190 NEW for NGW
    PrAdvanceLabel,                 // NEW for NGW
#endif

} PrEvent_et;

// 8,# - PrinterSystem
typedef enum PrSysEvent
{
    PrSysStatus             = 4,
    PrSysReqStatus          = 13
} PrSysEvent_et;

// 9,# - WrapperEvent
typedef enum WrEvent
{
//    WrInquire                       = 2,
//    WrStatus                        = 4,
//    WrSensors                       = 15,
//    WrErrorHistory                  = 18,
    WrReqInquire                    = 21,
    WrReqStatus                     = 22,
    WrReqSensors                    = 23,
	WrReqErrorHistory				= 24,
    WrStatusTracking                = 27,
    
    // EvolWrapperMessages
    WrEvolWakeup                    = 50,
    WrEvolStatus,
    WrEvolControl,
    WrEvolLoad,
    WrEvolWrap,
    WrEvolSettings,                 // 55
    WrEvolInquire,
    WrEvolConfig,
    WrEvolCamerasTracking,
    WrEvolCameraHeightScanTracking,
    WrEvolReqErrorHistory,          // 60
    WrEvolErrorHistory,
    WrEvolDoneErrorHistory,
    WrEvolSendDateTime,
    WrEvolRemoveUpgradeFiles,
    WrEvolUpgradeInProgress,        // 65
    WrEvolErrorHistoryList,
    WrEvolErrorHistoryClear,
    WrEvolErrorHistoryListPrevious,
    WrEvolErrorHistoryListNext,
    WrEvolErrorHistoryListDone,     // 70
    WrEvolSaveCycleCountToCF,
    WrEvolReqElevatorPkgDetectorStatus,
    WrEvolReqSealerPkgDetectorStatus,
    WrEvolPkgDetectorStatus,
    WrNGWWrapperReStart,            // 75
    WrNGWWrapperRecover,

    WrNGWEngInfoN1 = 80,            // 80
    WrNGWEngInfoN2,
    WrNGWEngInfoN3,
    WrNGWEngInfoN4,
    WrNGWEngInfoN5,
    WrNGWEngInfoN6,                 // 85
    WrNGWENGException,
    WrNGWDimensionOverrides,

    // second label applier new stuff
    WrPPPWakeup,
    WrPPPConfig,
    WrPPPWriteConfig,               // 90
    WrPPPDist,                      // printer plate distance from home
    WrPPPCalibration,               // printer calibration info
    WrReqCalibration,               // start the calibration
    WrPPPSensorVersion,
    WrWrapperPause,                 // 95 wrapper was paused, probably due to printer plates not in place
    WrPPPStatus,
    WrPPPTest,
    WrPPPResume,
    WrPPPBootVersion,
    WrPPPBootError,                 // 100 Yea I know this is 100 too, I dont know I didnt do it.
    WrReqKind                       = 100,


    // end second label applier new stuff

    WrKind,
    WrSetWrapMode,
    WrEvolTest = 108,
    WrEvolCycle = 112,
    WrEvolSensors = 115,            // 115
    WrEvolPackageDesc = 125,        // 125
    WrEvolRequestPackageDesc,
    WrLabelReady = 134,             // new NGW
    WrLabelSetup,                   // 135 new NGW
    WrLabelTaken = 138,             // new NGW
    WrCameraScan = 139,
    WrCameraHeightScan,             // 140
    WrCameraCalibration,
    WrReqEvolSystemInfo	= 143,

    WrEvolSystemInfo,
    WrEvolSystemInfoErase,
    WrEvolSystemInfoWrite,
    WrEvolSetMinTrayDims,		//	Set min tray dimensions using last EvolWrapperPackageDesc
    WrEvolSetMaxTrayDims,		//	Set max tray dimensions using last EvolWrapperPackageDesc
    WrEvolSetMinMaxTrayDims,	//	Set min & max tray dimensions using last EvolWrapperPackageDesc

    WrRequestWrapBoxAll = 150,
    WrRequestWrapBox,
    WrWrapBox,
    WrWrapBoxComplete,
    WrRequestHeightBox,

    WrNgwReqVersion = 155,
    WrNgwVersion,
    WrNgwCycleCount,
    WrNgwError,
    WrHeightBox,

    WrLoaderStatus = 160,  //for wrapper bootloader status msg.
    WrLoaderUpdate,
    WrHeightBoxComplete,
    WrChangeParam,
    WrChangeComplete,
    WrEvolTestComplete = 165,
    WrTimingsSave,
    WrTimingsSaveComplete,
    WrEvolPackageDischarge = 168,

    WrEvolReqCycleCount = 170,
    WrEvolCycleCount,
    WrEvolSaveCycleCount,
    WrEvolPkgSensorStatus,
    WrEvolRequestCameraThreshold,
    WrEvolCameraThreshold,          // 175
    WrEvolWriteCameraThreshold,
    WrEvolUpdateConfigLabelWidth,
    WrWrapBoxSize,
    WrWrapBoxAck,

    WrEvolEventCapture = 180,       // 180
    WrClearInstallKnifeTime,
    WrRequestKnifeTimeHistory,
    WrKnifeTimeHistory,
    WrReqUIWrapperModel,
    WrUIWrapperModel,               // 185
    WrEvolStartMfgRunin,
    WrWrapBoxSizeSave,

    WrEvolDiagnosticsTest           = 188,
    WrEvolDiagnosticsTestResult,

    WrEvolReqSystemReset = 190,     // 190
    WrEvolManufacturingRunInNotice,
    WrEvolMotorProfile,

    WrReqEvolHarwareAdditions,

    WrEvolCancelAttributeChanges = 195,
    WrEvolDoneAttributeChanges,
    WrEvolReadAllForAttribute,
    WrEvolWriteAllForAttribute,
    WrEvolSaveAttributeChanges,
    WrReqSettings = 200,            // 200
    WrSettings,
    WrTestTracking,
    WrTestSense,
    WrReqErrorTotals,
    WrErrorTotals,                  // 205
    WrResetErrorTotals,
    WrReqPackageTotals,
    WrPackageTotals,
    WrResetPackageTotals,
    WrSavePermanentTotals,          // 210
    WrRestorePermanentTotals,
    WrAllPackCodeNames,
    WrReadPackCode,
    WrWritePackCode,
    WrSavePackCode,                 // 215
    WrCycleMode,
    WrIntakePkgSize,
    WrResetFactoryDefaults,
    WrProductLoaded,
    WrSetGripperPWMTimings,         // 220
    WrReqPackageHistory,
    WrPackageHistory,
    WrEvolSavePackageTimings,			//	Was WrReqPackageTimings,
    WrEvolSavePackageTimingsComplete,	//	Was WrPackageTimings,
    WrWriteAllTrayTypes,            // 225

    WrEvolLoadPackageTimings = 226,
    WrEvolRequestPackageTimings,
    WrEvolPackageTimings,
    WrEvolResetDefaultPackageTimings,
    WrEvolDonePackageTimings,           // 130
    WrEvolPackageTimingsWrite,

    WrEvolAutoDetectTrayType = 232,
    WrEvolAutoConfigTrayType,
    WrEvolCreateTrayType,
    WrEvolChangeTrayTypePrecedence,     // 235
    WrEvolWriteTrayType,
    WrEvolWriteAllTrayTypes,
    WrEvolDeleteTrayType,
    WrWrapBoxTransferComplete,

    WrEvolReadWrapBox = 240,            // 240
    WrEvolWriteWrapBox,
    WrEvolPrintWrapBox,
    WrEvolCancelWrapBoxChanges,
    WrEvolDoneWrapBox,

    WrEvolReadHeightBox = 246,
    WrEvolWriteHeightBox,
    WrEvolRenameHeightBox,
    WrEvolCreateHeightBox,
    WrEvolVerifyInHeightBox,			// 250
    WrEvolDeleteHeightBox,

    WrEvolPackageHistory,
    WrEvolRestorePackageTimings,
    WrEvolBackupPackageTimings,
    WrNGWWrapperDetached,				// 255

    WrEvolSavePackageTimingsToDisc,
    WrEvolUnlockPackageTimings

} WrEvent_et;

// 1O,# - WrapperSystem
typedef enum WrSysEvent
{
    WrSysStatus                     = 4,
    WrSysReqStatus                  = 13,       // System Event
    SysEvolWrSysStatus              = 21        // could not find in SystemEvent
} WrSysEvent_et;

// 11,# - SystemSystem
typedef enum SystemSys
{
    SysPeripheralList               = 100,
    SysReqPeripheralList
} SystemSys_et;

// 14,# - Touchscreen
typedef enum TouchScreenEvent
{
    TsStartCalibrate                = 1,
    TsCalibrateNextStep             = 2,
    screenCoordinates               = 5
} TouchScreenEvent_et;

// 15,# - RFID
typedef enum RFIDEvent
{
    RFIDCurrentID                   = 20
} RFIDEvent_et;

// 16,# - Self Service Stacked GS1 Barcode Scanner
typedef enum BarcodeScanner
{
    ScannerRead                     = 0,            // DtEvent
    ScannerReadAll                  = 1,            // DtEvent
    ScannerWrite                    = 2,            // DtEvent
    ScannerWriteAll                 = 3,            // DtEvent
    ScannerDelete                   = 4,            // DtEvent
    ScannerSave                     = 6,            // DtEvent
    ScannerCreateNew                = 12,           // DtEvent
    ScannerEnabled                  = 20,
    ScannerDisabled                 = 21,
    ScannerReadProduct              = 22,
    ScannerReadPlu                  = 23
} BarcodeScanner_et;

// 17,# - Next Generation Wrapper
typedef enum NextGenWrapperEvent
{
    NGWUpgradeBegin                 = 0,
    NGWUpgradeStatus                = 1,
    NGWUpgradeProgress              = 2,
    NGWUpgradeEnd                   = 3,
    NGWUpgradeRequired              = 4,
    NGWUpgradeStart                 = 5,
    NGWUpgradeCancel                = 7,
    NGWReqUpgradeInfo               = 8,
    NGWUpgradeInfo                  = 9
} NextGenWrapper_et;

// 20,# KPC Messaging and upgrading
typedef enum KPCBootEvent
{
    KpcBootGeneric,			//0
    KpcBootStatus,
    KpcBootVersion,
    KpcBootAppVersion,
    KpcBootAck,
    KpcBootReady,			//5
    KpcBootReqStatus,
    KpcBootCmd,
    KpcBootFrame,
    KpcBootFrame1,
    KpcBootFrame2,			//10
    KpcBootFrame3,
    KpcBootFrame4,
    KpcBootFrame5,
    KpcBootFrame6,
    KpcBootFrame7,			//15
    KpcBootFrame8,
    KpcBootFlush,
    KpcBootReqVersion,
    KpcBootReqAppVersion,
    KpcBootDone,			//20
    KpcBootRestart,
    KpcNoEvent,
    KpcBootBulkMsg,

    KpcBootLoaderStartUpgrade,                // 24 number of nodes for UI to present
    KpcBootLoaderUpgradeStatus,               // 25 tell UI the status (nodeId,state,version,error) dont know what triggers it
    KpcBootLoaderUpgradeProgress,             // 26 tell the UI the current progress. Prolly response to KpcStartUpgrade
    KpcBootLoaderUpgradeFinished,             // 27 close UI upgrade page
    KpcBootLoaderUpgradeRequired,             // 28 ask UI to open upgrade page
    KpcBootLoaderUpgradeUIReady,              // 29 UI has signaled the upgrade page is ready
    KpcBootLoaderReqUpgradeInfo,              // 30 UI requests upgrade information
    KpcBootLoaderUpgradeInfo,                 // 31 upgrade information

    // SOF-3478
    KpcBootLoaderUpgradeCancel,               // 32 cancel kpc upgrade (not sure if this one is needed)
    KpcBootLoaderUpgradeRequest,              // 33 Request to proceed with upgrade
    KpcBootLoaderUpgradeUpdate                // 34 Backend has updated the UI it is ready to upgrade
    // SOF-3478 END

} KPCBootEvent_et;

// 21,# KPC Messaging and upgrading
typedef enum KPCBootUpgraderEvent
{
    KpcBootUpgraderGeneric,			//0
        KpcBootUpgraderStatus,
        KpcBootUpgraderVersion,
        KpcBootUpgraderAppVersion,
        KpcBootUpgraderAck,
        KpcBootUpgraderReady,			//5
        KpcBootUpgraderReqStatus,
        KpcBootUpgraderCmd,
        KpcBootUpgraderFrame,
        KpcBootUpgraderFrame1,
        KpcBootUpgraderFrame2,			//10
        KpcBootUpgraderFrame3,
        KpcBootUpgraderFrame4,
        KpcBootUpgraderFrame5,
        KpcBootUpgraderFrame6,
        KpcBootUpgraderFrame7,			//15
        KpcBootUpgraderFrame8,
        KpcBootUpgraderFlush,
        KpcBootUpgraderReqVersion,
        KpcBootUpgraderReqAppVersion,
        KpcBootUpgraderDone,			//20
        KpcBootUpgraderRestart,
        KpcBootUpgraderBulkMsg,
        KpcNoUpgraderEvent,

        KpcBootUpgraderStartUpgrade,                // 24 number of nodes for UI to present
        KpcBootUpgraderUpgradeStatus,               // 25 tell UI the status (nodeId,state,version,error) dont know what triggers it
        KpcBootUpgraderUpgradeProgress,             // 26 tell the UI the current progress. Prolly response to KpcStartUpgrade
        KpcBootUpgraderUpgradeFinished,             // 27 close UI upgrade page
        KpcBootUpgraderUpgradeRequired,             // 28 ask UI to open upgrade page
        KpcBootUpgraderUpgradeUIReady,              // 29 UI has signaled the upgrade page is ready
        KpcBootUpgraderReqUpgradeInfo,              // 30 UI requests upgrade information
        KpcBootUpgraderUpgradeInfo,                 // 31 upgrade information

        KpcBootUpgaderUpgradeCancel                 // 32 cancel the kpc upgrade - SOF-3478
}  KPCBootUpgrader_et;

// SOF-3449
// 23,# Boardsite Messaging
typedef enum BoardSiteMessagingEvent
{
        BrdSiteReqTestMode,         // 0
        BrdSiteTestStatus,
        BrdSiteBoardTest,
        BrdSiteBoardTestComplete,
        BrdSiteBoardTestCompleteUI,
        BrdSiteTestReqStatus,       // 5
        BrdSitePeripheralAttached,
        BrdSiteReqCompleteResults,
        BrdSitePercentCompete

}  BrdSiteMessaging_et;

// 100,# - Session
typedef enum SessionEvent
{
    SnUnknown                       = 0,
    SnRegister                      = 1,
    SnVersionInfo                   = 2,
    SnTimedOut                      = 3,
    SnTerminate                     = 4,
    SnReadDateTime                  = 5,
    SnWriteDateTime                 = 6,
    SnContinuousDateTime            = 7,
    SnStopDateTime                  = 8,
    SnReadWeighModeTransactionType  = 9,
    SnWriteWeighModeTransactionType = 10,
    SnSyncDOM                       = 16,
    SnWriteCurrentWebPage           = 20,
    SnTouchTypeSetting              = 23,
    SnPrintKeyPressed               = 25,
    SnClearKeyPressed               = 26,
    SnSelectCurrentLanguage         = 27,
    SnStartStreamingGraphic         = 30,
    SnStopStreamingGraphic          = 31,
    SnWriteStreamingGraphic         = 33,
    SnCreateDirectory               = 34,
    SnPlatterTareOn                 = 36,
    SnPlatterTareOff                = 37,
    SnRequestPlatterTare            = 38,
    SnDesktopAccess                 = 39,
    SnCompactFlashMakeBootrom       = 45,
    SnCompactFlashCopyInternal      = 46,
    SnReadStatistics                = 50,
    SnWriteStatistics               = 51,
    SnContinuousStatistics          = 52,
    SnStopStatistics                = 53,
    SnEnableInputEvent              = 56,
    SnShutdownTSInput               = 57,
    SnSoundBeep                     = 59,
	SnKeyActivation               	= 60,
	SnKeyDeactivation             	= 61,
	SnRequestKeyActivation        	= 62,
    SnKeyActivationStates           = 63,
    SnRequestDBCompress             = 65,
    SnPrinterMetrics                = 68,
    SnUsbKbdAttachChange            = 69,
    SnSelectActiveWebPageFrame      = 70,
    SnDismissVirtualNumberpad       = 72,
    SnBrowserMenuChanging           = 73,
    SnPopupVirtualKeyboard          = 75,
    SnDismissVirtualKeyboard        = 76,
    SnPopupIntranetBrowserWindow    = 79,
    SnPopupScaleBrowserWindow       = 80,
    SnSetFlashkeyInternetWebPage    = 82,
    SnRequestIntranetAddress        = 84,
    SnUsingMirrorDatabase           = 85,
    SnStopDialogs                   = 90,
    SnRestartDialogs                = 91,
    SnShutdownRequest               = 97,
    SnReboot                        = 99,
    SnTestMessage                   = 100,
    SnDBRestoreFinished             = 113,
    SnDBRestoreInProgress           = 114,
    SnReadNetworkInfo               = 132,
    SnWriteNetworkInfo              = 133,
    SnReadLicensedPackages          = 134,
    SnWriteLicensedPackages         = 135,
    SnManufacturingTestSummaryReport= 136,
    SnPlatterTareUnstable           = 145,
    SnNowServingQueueManagement     = 146,
    SnUpgradeFirmwareStatus         = 147,
    SnUpgradeSFTPBadAuthentication  = 148,
    SnBackendRemotePrint            = 160,
    SnReplyForRemotePrint           = 161,
    SnRdpActive                     = 185,
    SnRdpInactive                   = 186,
    SnSelfServReimageLabels         = 187,          // used for BigY print to screen and label
    snMergeNow                      = 188,          // SOF-4769
	snMergeNowAfterComm             = 189,
    SnExecuteTask                   = 200,
    SnInvalidModeDetected           = 201
} SessionEvent_et;

// 101,# - ProductEvent
typedef enum ProductEvent
{
    ProductUnknown,
    ProductChange,
    ProductUpdate,
    ProductComplete,
    ProductPrint,
    ProductRead,                                    // 5
    ProductReadAll,
    ProductWrite,
    ProductDelete,
    ProductReadSupportData,
    ProductWriteSupportData,                        // 10
    ProductChangeSupportData,
    ProductDeleteSupportData,
    ProductReadLifeData,
    ProductReadCurrentTotalValues,
    ProductWriteCurrentTotalValues,                 // 15
    ProductWriteSingleField,
    ProductError,
    ProductReadDefault,
    ProductWriteDefault,
    ProductDeleteDefault,                           // 20
    ProductSaveDefault,
    ProductSave,
    ProductShowLinksToSupportData,
    ProductCreate,
    ProductDeleteAll,                               // 25
    ProductRestore,
    ProductReadNutriLabel,
    ProductReadSupportRecord,
    ProductGetTree,
    ProductFindByDescription,                       // 30
    ProductFindSoftKeysByDescription,
    ProductTransactionTypeChanged,
    ProductReadLabelFormSize,
    ProductWriteLabelFormSize,
    ProductPackageComplete,                         // 35
    ProductResetUnitPrice,
    ProductApplyDiscountCentsOff,
    ProductApplyDiscountPercentOff,
    ProductApplyDiscountPerPackage,
    ProductStatusEvent,                             // 40
    ProductGetBackDateDays,

    ProductStartPrintAllLabels,
    ProductStopPrintAllLabels,
    ProductFinishedPrintingAllLabels,
    ProductStartPrintForTransaction,                // 45
    ProductDontUpdateProductionPlanning,
    ProductClearUI,
    ProductPrintMultipleLabels,
    ProductDeleteAllCommodityClass                  = 49,
    ProductReadSoftKeyList                          = 50,
    ProductWriteSoftKeyList,
    ProductReadAllSoftKeyGraphicNames,
    ProductWriteAllSoftKeyGraphicNames,
    ProductToggleLabelType,                         //	54
    ProductPrintNewExpirationLabel                  = 58,
    ProductPrintExistingExpirationLabel             = 59,
    ProductReadExpirationLabel,                     // 60
    ProductReadAllExpirationLabels,
    ProductWriteExpirationLabel,
    ProductWriteAllExpirationLabels,
    ProductDeleteExpirationLabel,
    ProductDeleteAllExpirationLabels,               // 65
    ProductSaveExpirationLabel,
    ProductRestoreExpirationLabel,

    ProductBatchApplyDelete,
    ProductIncomplete,

    ProductGetBackDateDate,                         //	70
    ProductChangeRotationPlacement,                 //	Was 44 in vxWorks

    ProductChangeSmart,
    ProductAddItemToAccumulatedItems,
    ProductRemoveLastItemFromAccumulatedItems,
    ProductShowAccumulatedItems,                    // 75
    ProductRemoveAllAccumulatedItems,
    ProductPrintAccumulatedItems,
    ProductRequestReadAllAccumulatedItems,
    ProductReadAllAccumulatedItems,
    ProductWriteAllAccumulatedItems,                // 80
    ProductRemoveAccumulatedItems,
    ProductRemoveSingleAccumulatedItem,
    ProductReadAndAddSideItem,
    ProductSetMealWeight,
    ProductLeaveBuildAMealMode,                     // 85
    ProductReadAccumulatedMasterTrans,
    ProductAddItemToAccumOrMeal,
    ProductReadToo,
    ProductClearPrintingProductsList,
    ProductPrintProductSupportText                  = 90,
    ProductSaveSupervisorTransaction,
    ProductRemotePrintStatus                        = 92,   // THIS MUST ALWAYS BE 92 FOR REMOTE PRINT TO WORK
    ProductRemoteRequestRWItem                      = 93,   // THIS MUST ALWAYS BE 93 FOR REMOTE PRINT TO WORK
    ProductAcceptSelfServiceBarcode,
    ProductPausePrintAllLabels,                  // 95
	Product2ndLabelApplyToggle,
    ProductReadOnlineOrderingProduct,
    ProductWriteOnlineOrderingProductTotals
} ProductEvent_et;


// 102,# - Operator Events
// DtEvent represents DataTypeEvent, used across HFC
typedef enum OperatorEvent
{
    DtRead                                  = 0,            // DtEvent
    OpDtReadAll                             = 1,            // DtEvent
    OpDtWrite                               = 2,            // DtEvent
    OpDtWriteAll                            = 3,            // DtEvent
    OpDtDelete                              = 4,            // DtEvent
    OpDtSave                                = 6,            // DtEvent
    OpDtCreateNew                           = 12,           // DtEvent
    OpLogin                                 = 20,
    OpLogout                                = 21,
    OpNotifyUIofRFIDInputStart              = 24,
    OpNotifyUIofRFIDInputEnd                = 25,
    OpDoesRFIDExist                         = 26,
    OpRFIDExists                            = 27,
    OpRFIDDoesntExist                       = 28,
    OpPromptForOperatorIDOrRFID             = 29,
    OpRenumberOperator                      = 30,
    OpRemoteAccessCredentialsReq            = 31,
    OpRemoteAccessCredentials               = 32,
    OpOperatorChanges                       = 31,           // not in OperatorEvent -- Larry says this should not be here!!! maybe 131??
    OpPromptForOperatorIDOrRFIDFieldAccess  = 39,
    OpDoesIDExist                           = 46,
    OpIDExists                              = 47,
    OpIDDoesntExist                         = 48,
    OpReadNote                              = 50,
    OpReadNoteForEditing                    = 51,
    OpWriteNote                             = 52,
    OpDeleteNote                            = 54,
    OpSaveNote                              = 56,
    OpRestoreNote                           = 57
} OperatorEvent_et;

// 105,# - Graphic Generated Events
typedef enum GraphicEvent
{
    GrDtWrite                               = 2,
    GrDtDelete                              = 4,
    GrDtFirst                               = 8,
    GrDtNext                                = 10,
    GrDtPrevious                            = 11,
    GraphicReadAllNames                     = 20,
    ReadAllProductGraphicNames              = 55,
    ReadAllCategoryGraphicNames             = 75,
    ReadAllSpecialsGraphicNames             = 95,
    ReadAllSelfServeGraphicNames            = 135,
    ReadAllSoftkeyGraphicNames              = 155,
    ReadPlanogramGraphic                    = 160,
    ReadAllPlanogramGraphics                = 161,
    WritePlanogramGraphic                   = 162,
    WriteAllPlanogramGraphics               = 163,
    DeletePlanogramGraphic                  = 164,
    DeleteAllPlanogramGraphics              = 165,
    SavePlanogramGraphic                    = 166,
    RestorePlanogramGraphic                 = 167,
    ReadAllPlanogramGraphicNames            = 175,
    BatchApplyDeletePlanogramGraphic        = 176,
    NewPlanogramReceived                    = 177,
    ReadMostRecentPlanogramGraphic          = 178,
    WriteMostRecentPlanogramGraphic         = 179,
    AcknowledgeMostRecentPlanogramGraphic   = 180,
    ReadAllVideoNames                       = 195
} GraphicEvent_et;

// 106,# - ExpandedTextDev Events
typedef enum ExpandedTextEvent
{
    ExpandedDtWrite             = 2             // DtEvent
} ExpandedTextEvent_et;

// 107,# - SpecialMessageDev Events
typedef enum SpecialMessageEvent
{
    SpecialDtWrite              = 2             // DtEvent
} SpecialMessageEvent_et;

// 108,# - ProductNotesDev Events
typedef enum PrductNoteEvent
{
    ProductDtWrite              = 2             // DtEvent
} PrductNoteEvent_et;

// 109,# - Backup Restore Events
typedef enum BackupRestoreEvent
{
    UnknownBackRestore          = 0,
    Backup                      = 1,		// tell session to perform a backup
    Restore                     = 2,        // tell session to perform a restore
    BadRecordScan               = 3,		// tell session to perform a BadRecordScan of the database
    Intranet                    = 4,        // not used on the UI side so far
    BRCancel                    = 5,
    Terminate                   = 6,        // used to indicate its done updating
    Progress                    = 7,
    ProgressBadRecords          = 8
} BackupRestoreEvent_et;

// 110,# - ClassDev Events
typedef enum ClassEvent
{
    ClassDtRead                 = 0,
    ClassDtWrite                = 2,
    ClassDtDelete               = 4,
    ClassDtSave                 = 6,
    ClassDtCreateNew            = 12,
    ClassGetTree                = 21,
    ClassGetOrphanTree          = 22
} ClassEvent_et;

// 111,# - LabelTypeEvent Events
typedef enum LabelTypeEvent
{
    LabelTypeDtRead                 = 0,
    LabelTypeDtReadAll              = 1,
    LabelTypeDtWrite                = 2,
    LabelTypeDtWriteAll             = 3,
    LabelTypeDtDelete               = 4,
    LabelTypeDtDeleteAll            = 5,
    LabelTypeDtSave                 = 6,
    LabelTypeDtRestore              = 7,
    LabelTypeDtFirst                = 8,
    LabelTypeDtLast                 = 9,
    LabelTypeDtNext                 = 10,
    LabelTypeDtPrevious             = 11,
    LabelTypeDtCreateNew            = 12,
    LabelTypeDtBatchApplyDelete     = 13,
    LabelTypeDtBatchApplyDeleteAll  = 14,
    LabelTypeDtBatchApplyRestore    = 15
} LabelTypeEvent_et;

// 112,# - ConfigurationDev Events
typedef enum ConfigurationEvent
{
    CfgReadSingleField          = 16,
    CfgWriteSingleField         = 17,
    CfgSaveSingleField          = 18,
    CfgScaleRead                = 20,
    CfgScaleWrite               = 21,
    CfgScaleSave                = 22,
    CfgCountrySettingsRead      = 30,
    CfgClassGetOrphanTree       = 31,
    CfgCountrySettingsSave      = 32,
    CfgFrequentShopperRead      = 40,
    CfgFrequentShopperWrite     = 41,
    CfgFrequentShopperSave      = 42,
    CfgPrintedFieldsRead        = 50,
    CfgPrintedFieldsWrite       = 51,
    CfgPrintedFieldsSave        = 52,
    CfgSupportedLabelsRead      = 55,
    CfgSupportedLabelsWrite     = 56,
    CfgSupportedLabelsSave      = 57,
    CfgOperatorAccessRead       = 60,
    CfgOperatorAccessWrite      = 61,
    CfgOperatorAccessSave       = 62,
    CfgScaleOperationRead       = 70,
    CfgScaleOperationWrite      = 71,
    CfgScaleOperationSave       = 72,
    CfgGenericTextReadAll       = 80,
    CfgGenericTextWriteAll      = 81,
    CfgGenericTextUpdated       = 82,
    CfgStoreInfoRead            = 90,
    CfgStoreInfoWrite           = 91,
    CfgStoreInfoSave            = 92,
    CfgStoreInfoRestore         = 93,
    CfgCurrencyRead             = 100,
    CfgCurrencyWrite            = 101,
    CfgCurrencySave             = 102,
    CfgSelfServiceRead          = 110,
    CfgSelfServiceWrite         = 111,
    CfgSelfServiceSave          = 112,
    CfgScaleUpgradeRead         = 120,
    CfgScaleUpgradeWrite        = 121,
    CfgScaleUpgradeSave         = 122,
    CfgEmailRead                = 130,
    CfgEmailWrite               = 131,
    CfgEmailSave                = 132,
    CfgPrepackRead              = 140,
    CfgPrepackWrite             = 141,
    CfgPrepackSave              = 142,
    CfgExpirationLabelNoteRead  = 150,
    CfgExpirationLabelNoteWrite = 151,
    CfgExpirationLabelNoteSave  = 152,
    CfgMarqueeRead              = 160,
    CfgMarqueeWrite             = 161,
    CfgMarqueeSave              = 162,
    CfgFTPRead                  = 170,
    CfgFTPWrite                 = 171,
    CfgFTPSave                  = 172,
    CfgFTPOptionsRead           = 173,
    CfgFTPOptionsWrite          = 174,
    CfgFTPOptionsSave           = 175,
    CfgFTPResponseWrite         = 177,
    CfgSynchronizationRead      = 180,
    CfgSynchronizationWrite     = 181,
    CfgSynchronizationSave      = 182,
    CfgSynchronizationRestore   = 183,
    CfgSyncListRead             = 184,
    CfgSyncListWrite            = 185,
    CfgSyncListSave             = 186,
    CfgSyncListRestore          = 187,
    CfgCommInterfaceRead        = 190,
    CfgCommInterfaceWrite       = 191,
    CfgCommInterfaceSave        = 192,
    CfgScaleManagementRead      = 200,
    CfgScaleManagementWrite     = 201,
    CfgScaleManagementSave      = 202,
    CfgCommIntfaceScheduledRead = 205,
    CfgCommIntfaceScheduledWrite= 206,
    CfgCommIntfaceScheduledSave = 207,
    CfgRotationOverrideRead     = 210,
    CfgRotationOverrideWrite    = 211,
    CfgRotationOverrideSave     = 212,
    CfgReportsSetupRead         = 220,
    CfgReportsSetupWrite        = 221,
    CfgReportsSetupSave         = 222,
    CfgGCDRead                  = 230,
    CfgGCDWrite                 = 231,
    CfgGCDSave                  = 232,
    CfgGCDReload                = 233,
	CfgAccumulateItemsRead 		= 240,
	CfgAccumulateItemsWrite		= 241,
	CfgAccumulateItemsSave		= 242,
    CfgDeleteAllFiles           = 245,
    CfgBatchApplyDeleteAllFiles = 246,
    CfgSaved                    = 253,
    CfgNetworkSaved             = 254
} ConfigurationEvent_et;

// 113,# - FontDev Events
typedef enum FontEvent
{
    FontWriteAllNames           = 21
} FontEvent_et;

// 116,# - ReportManagerDev Events
typedef enum ReportEvent
{
    StartReport                 = 0,
    CancelReport                = 1,
    TerminateReport             = 2
} ReportEvent_et;

// 117,# - NutrifactsDev Events
typedef enum NutrifactsEvent
{
    NutrifactsNew               = 20,
    NutrifactsUse               = 21
} NutrifactsEvent_et;

// 119,# - GenericTextDev Events
typedef enum GenericTextEvent
{
    GenericDtRead               = 0,
    GenericDtReadAll            = 1,
    GenericDtWrite              = 2,
    GenericDtWriteAll           = 3,
    GenericDtSave               = 6,
    GenericDtCreateNew          = 12,
    AppletViewerReadAll         = 20,
    AppletViewerWriteAll        = 21

} GenericTextEvent_et;

// 120,# - VersionManagerDev Events
typedef enum VersionManagerEvent
{
    UpgradeManual               = 1,
    WriteFileToSCEEP            = 2,
    InitiateUpgradeProcess      = 13,
    UpgradeComplete       	    = 14,
    UsbWrapperUpgradeStatus     = 15,
    UpgradeXferStatus           = 16,
    PeripheralUpgradeList       = 17,
    CancelUpgrade               = 18,
    BeginUpgrade                = 19,
    RemoveWrapperSWComponents   = 20,
    RemoveCLASWComponents       = 24,
    UpdateNow2016               = 25,
    SendToDept2016              = 26,
} VersionManagerEvent_et;

// 121,# - CategoryDev Events
typedef enum CategoryEvent
{
    CategoryDtRead                      = 0,
    CategoryDtReadAll                   = 1,
    CategoryDtWrite                     = 2,
    CategoryDtWriteAll                  = 3,
	CategoryDtDeleteSingle				= 4,
	CategoryDtWriteSingle				= 6,
	CategoryDtCreateNewSingle			= 12,
    CategoryReadAllCatGraphicNames      = 20,
    CategoryWriteAllCatGraphicNames     = 21,
    CategoryReadAllMainCategories       = 22,
    CategoryWriteAllMainCategories      = 23,
    CategoryWriteSoftKeyList            = 51
} CategoryEvent_et;

// 122,# - NetworkDev Events
typedef enum NetworkEvent
{
    NetworkPingResponse         = 21,
    CfgTcpIpWrite               = 31,
    CfgDnsWrite                 = 41,
    CfgWirelessCardWrite        = 51,
    NetworkSignalStrength       = 62,
    NetworkConnectionStatus     = 65,
    NetworkTftpUpgradeWrite     = 71,
    cfgLEAPWrite                = 81,
    cfgLEAPStatusWrite          = 84
} NetworkEvent_et;

// 123,# - TotalDev Events
// I think these have been removed
// I could not find this in the current scale application
// I think this enum needs further researched in other branches
typedef enum TotalEvent
{
    productRunTotal02           = 2,
    ReadProductTotal            = 20,
    totalRecordData22           = 22,
    productTotalList37          = 37,
    ReadOperatorTotal           = 40,
    totalRecordData42           = 42,
    operatorTotalList43         = 43,
    ReadHourlyTotal             = 60,
    totalRecordData62           = 62,
    hourlyTotalList63           = 63,
    totalRecordData82           = 82,
    productTotalList97          = 97
} TotalEvent_et;

// 124,# - SpecialsDev
typedef enum SpecialsEvent
{
    SpecialsDtReadSingle		= 0, // added
    SpecialsDtRead              = 1,
    SpecialsDtWrite             = 2,
    SpecialsDtWriteAll          = 3,
	SpecialsDtDelete			= 4,
    SpecialsDtSave              = 6,
    SpecialsDtCreateNew			= 12,
    ReadAllGraphics             = 20
} SpecialsEvent_et;

// 126,# - ProductStatisticsDev
typedef enum ProductStatisticsEvent
{
    ProdStatsDtReadSingle		= 0, // added
    ProdStatsDtRead             = 1,
    ProdStatsDtWrite            = 2,
    ProdStatsDtWriteAll         = 3,
	ProdStatsDtDelete			= 4,
    ProdStatsDtDeleteAll	    = 5,
	ProdStatsDtCreateNew		= 12
} ProductStatisticsEvent_et;

// 129,# - StreamingGraphicDev Events
typedef enum StreamingGraphicEvent
{
    StreamingGraphicDtReadAll               = 1,
    StreamingGraphicDtWrite                 = 2,
    StreamingGraphicDtWriteAll              = 3,
    StreamingGraphicReadAllGraphicNames     = 20,
    StreamingGraphicWriteAllGraphicNames    = 21,
    StreamingGraphicSelectSlide             = 22,
    StreamingGraphicMoveSlideUp             = 23,
    StreamingGraphicMoveSlideDown           = 24,
    StreamingGraphicInsertNewSlide          = 25,
    StreamingGraphicDeleteSelectedSlide     = 26
} StreamingGraphicEvent_et;

// 131,# - OperatorChangesDev Events
typedef enum OperatorChangeEvent
{
    ReadOperatorChangeSummary       = 20,
    WriteOperatorChangeSummary      = 21,
    ReadOperatorChangesGroup        = 30,
    WriteOpertorChangesGroup        = 31
} OperatorChangeEvent_et;

// 134,# - EmailAlertsDev Events
typedef enum emailAlertEvent
{
    emailAlertDtReadAll         = 1,
    emailAlertDtWrite           = 2,
    emailAlertDtWriteAll        = 3,
	emailAlertDtSave			= 6,
    emailAlertDtRestore         = 7
} emailAlertEvent_et;

// 135,# - COOLDev Events
typedef enum COOLDevEvent
{
    COOLDevReadCountry                              = 20,
    COOLDevReadAllCountries                         = 21,
    COOLDevWriteCountry                             = 22,
    COOLDevWriteAllCountries                        = 23,
    COOLDevDeleteCountry                            = 24,
	COOLDevSaveCountry								= 26,
    COOLDevCreateNewCountry                         = 32,
	COOLDevReadByCategoryOnlyCountries				= 33,
    COOLDevWriteCountriesByCategory                 = 34,
    COOLDevAddCountryToCategory                     = 35,
    COOLDevDeleteCountryFromCategory                = 36,
	COOLDevCreateNewCountryAndSave					= 37,
    COOLDevReadByAllAndByCategoryCountries          = 38,
    COOLDevReadProductionStep                       = 40,
    COOLDevReadAllProductionSteps                   = 41,
    COOLDevWriteProductionStep                      = 42,
    COOLDevWriteAllProductionSteps                  = 43,
    COOLDevDeleteProductionStep                     = 44,
	COOLDevSaveProductionStep						= 46,
    COOLDevCreateNewProductionStep                  = 52,
	COOLDevReadByCategoryOnlyProductionSteps        = 53,
    COOLDevWriteProductionStepsByCategory           = 54,
    COOLDevAddProductionStepToCategory              = 55,
    COOLDevDeleteProductionStepFromCategory         = 56,
	COOLDevCreateNewProductionStepAndSave			= 57,
    COOLDevReadByAllAndByCategoryProductionSteps    = 58,
    COOLDevReadPretext                              = 60,
    COOLDevReadAllPretexts                          = 61,
    COOLDevWritePretext                             = 62,
    COOLDevWriteAllPretexts                         = 63,
    COOLDevDeletePretext                            = 64,
	COOLDevSavePretext								= 66,
    COOLDevCreateNewPretext                         = 72,
	COOLDevReadByCategoryOnlyPretexts				= 73,
    COOLDevWritePretextsByCategory                  = 74,
    COOLDevAddPretextToCategory                     = 75,
    COOLDevDeletePretextFromCategory                = 76,
    COOLDevReadByAllAndByCategoryPretexts           = 78,
    COOLDevReadCategory                             = 80,
    COOLDevReadAllCategories                        = 81,
    COOLDevWriteCategory                            = 82,
    COOLDevWriteAllCategories                       = 83,
    COOLDevDeleteCategory                           = 84,
	COOLDevSaveCategory								= 86,
    COOLDevCreateNewCategory                        = 92,
    COOLDevWriteCoolText                            = 102,
    COOLDevDeleteCoolText                           = 104,
    COOLDevSaveCoolText                             = 106,
    COOLDevCreateNewCoolText                        = 112,
    COOLDevShortlistReadAllCoolText                 = 113,
    COOLDevShortlistWriteAllCoolText                = 114,
    COOLDevShortlistAddText                         = 115,
    COOLDevShortlistDeleteText                      = 116
} COOLDevEvent_et;

// 136,# - Softkey Generated Events
typedef enum SoftkeyDevEvent
{
    ReadSoftkey                 = 20,
    WriteSoftkey                = 22,
    DeleteSoftkey               = 24,
    SaveSoftkey                 = 26,
    CreateNewSoftkey            = 32,
    ExecuteSoftkey              = 34,
    ReadSoftkeyScripts          = 36,
    ListSoftkeyScripts          = 37,
    SoftkeyScriptResult         = 38,
    ReadSoftkeyFeature          = 40,
    WriteSoftkeyFeature         = 42,
    SaveSoftkeyFeature          = 46,
    ReadSoftkeyStyle            = 60,
    WriteSoftkeyStyle           = 62,
    SaveSoftkeyStyle            = 66,
    WriteAllSoftkeyStyleNumbers = 74,
    ReadSoftkeyGroup            = 80,
    ReadAllSoftkeyGroups        = 81,
    WriteSoftkeyGroup           = 82,
    WriteAllSoftkeyGroups       = 83,
    DeleteSoftkeyGroup          = 84,
    SaveSoftkeyGroup            = 86,
    CreateNewSoftkeyGroup       = 92,
    LoadSoftkeyGroup            = 93,
    LoadSoftkeyGroupForEditor   = 95,
    LoadAllSoftkeyGroups        = 255
} SoftkeyDevEvent_et;

// 137,# - MarqueeTextDev Events
typedef enum MarqueeTextEvent
{
    MarqueeTextDtWrite          = 2
} MarqueeTextEvent_et;

// 138,# - PeripheralDev
typedef enum PeripheralEvent
{
   PeripheralDtWrite            = 2,
   PeripheralDtSave				= 6,
   WriteLostPeripheralsList     = 20,
   RemoveCheckingLostPeripheral = 21,
   ReadCreatePeripheral         = 22
} PeripheralEvent_et;

// 142,# - NumericDev Events
typedef enum NmEvent
{
    NmStatus                    = 4,
    NmSysReqStatus              = 13

} NmEvent_et;

// 143,# - Marquee Device Events
typedef enum MqEvent
{
    MqStatus                    = 4,
    MqSysReqStatus              = 13
} MqEvent_et;

// 144,# - Import events
typedef enum ImportEvent
{
    UnknownImportEvent          = 0,
    ImportStart                 = 1,                                //tell session to perform a import
    ImportTerminate             = 2                                 //tell session to perform a restore
 } ImportEvent_et;

// 145,# - NetworkConnectivityManager Events
typedef enum NetworkConnectivityEvent
{
    GoodComm                        = 20,
	BadComm 						= 21,
    RequestCurrentStatus            = 22,
    CurrentStatus                   = 23,
	BadCommRepeat                   = 24,
    NetworkAvailable                = 25,
    NetworkUnavailable              = 26,
    NetworkSyncStarted              = 27,
    ActivityNormal                  = 28,
	ActivityBusy                    = 29,
	NetworkActivity                 = 30,
    SnWriteNetworkStatus            = 31,
    StartContinuousNetworkStatus    = 32,
    StopContinuousNetworkStatus     = 33,
    RequestNetworkDeviceType        = 34,
    NetworkDeviceType               = 35                            // responses: 0=wire 1=wifi
} NetworkConnectivityEvent_et;

// 150,# - CGICallManager Events
typedef enum CGICallManagerEvent
{
    CGICallPerformed            = 2
} CGICallManagerEvent_et;

// 151,# - Transaction events
typedef enum TransactionEvent
{
    TransactionEventReadNonVoidGroup    = 20,
    TransactionWriteNonVoidGroup        = 21,
    TransactionEventVoidTransactions    = 22,
    TransactionReadDates                = 30,
    TransactionWriteDates               = 31,
    TransactionReadAndClear             = 40,
    ClearProductAndClassTotals,
	ClearOperatorTotals,
	ClearHourlyTotals,
	ClearCOOLTotals,
	ClearOperatorChanges,
	TransactionLastEvent
} TransactionEvent_et;

// 152,# - TransactionType events
typedef enum TransactionTypeEvent
{
    TransactionTypeDtRead			= 0,
    TransactionTypeDtWrite          = 2,
    TransactionTypeDtDelete         = 4,            // DtEvent
	TransactionTypeDtSave			= 6,
	TransactionTypeDtCreateNew		= 12,
    TransactionReadNonVoidGroup     = 20,
    TransactionTypeWriteAllNames    = 21,
    TransactionVoidTransactions     = 22,
    TransactionTypeReadDates        = 30,
    TransactionTypeWriteCurrentType = 31
} TransactionTypeEvent_et;

// 153,# - HourlyTotal events
typedef enum HourlyTotalEvent
{
    ReadFilteredHourlyTotals        = 20,
    WriteFilteredHourlyTotals       = 21
} HourlyTotalEvent_et;

// 154,# - OperatorTotal events
typedef enum OperatorTotalEvent
{
    ReadFilteredOperatorTotals      = 20,
    WriteFilteredOperatorTotals     = 21
} OperatorTotalEvent_et;

// 156,# - ProductTotal events
typedef enum ProductTotalEvent
{
    ReadFilteredProductTotals       = 20,
    WriteFilteredProductTotals      = 21
} ProductTotalEvent_et;

// 159,# - CutTest Events
typedef enum CutTestDevEvent
{
    CuttingTestDtRead               = 0,
    CuttingTestDtWrite              = 2,        ///< Write the data of a Cut Test
    CuttingTestDtSave               = 6,
    BeginCuttingTest                = 20,       ///< Tell the application to begin a cut test
    CancelCuttingTest               = 21,       ///< The the application to cancel a cut test
    EndCuttingTest                  = 22,       ///< Tell the application to end cut tests
    CuttingTestDetailsRead          = 23,       ///< Ask to see the details for Cutting Test Option 1
    CuttingTestDetailsWrite         = 24,       ///< Show the detail results of cut tests Default and Option1
    CuttingTestHistoryRead          = 25,       ///< Request the history of cutting tests
    CuttingTestHistoryWrite         = 26,       ///< The cutting test history results
    SuspendCuttingTest              = 27,       ///< Suspend a cutting test
    DeleteCuttingTest               = 28,       ///< Delete this cut test
    ResumeCuttingTest               = 29,       ///< Resume this cut test
    PrintCuttingTest                = 30,       ///< Print the current cut test
    SaveCuttingTestToUsb            = 31,       ///< Save the current cut test
    CuttingTestHistoryLogin         = 32,       ///< Login for Cutting Test History
    CuttingTestComplete             = 33,       ///< When we are all done with the Current Cutting Test
    CuttingTestSuspendSuccess       = 34,       ///< Lets the UI know when a request to suspend a test was good to go
    CuttingTestClear                = 35,       ///< Used when the user click Finish on Details Screen to clear the C.T. in the app
    CuttingTestSaveToUSBFinished    = 36
} CutTestDevEvent_et;

// 161,# - GCD Events
typedef enum GCDSessionEvent
{
    GcdSessionRestart               = 0,
    GcdSessionShuttingDown          = 1,
    GcdSessionColorTestOn           = 2,
    GcdSessionColorTestOff          = 3,
    GcdSessionStatus                = 4,			// also SysStatus
    GcdSessionEnglish               = 5,
    GcdSessionFrench                = 6,
    GcdSessionSpanish               = 7,
    GcdSessionAvoir                 = 8,
    GcdSessionMetric                = 9,
    GcdSessionKind                  = 10,
	GcdSessionColorTestWhiteOn      = 11,
    GcdSysReqStatus                 = 13,           // System Event
    GcdSessionSetVolume             = 32,
    GcdSessionUpdateViaFtpNow       = 33,
    GcdSessionUpdateViaUsbNow       = 34,
    GcdSessionSetPlaybackWaitTime   = 35,
    GcdSessionUpdateLayout			= 36,
    GcdSessionUpdatePlaylist		= 37
} GCDSessionEvent_et;

// 163,# - HIDDev (USB) Events
typedef enum HIDDevEvent
{
    HIDDevUnknownEvent              = 0,
    HIDDevMouseAttached             = 1,
    HIDDevMouseRemoved              = 2,
    HIDDevKeyboardAttached          = 3,
    HIDDevKeyboardRemoved           = 4,
    HIDDevBarcodeScannerAttached    = 5,
    HIDDevBarcodeScannerRemoved     = 6,
    HIDDevStorageDeviceAttached     = 7,
    HIDDevStorageDeviceRemoved      = 8
} HIDDevEvent_et;

// 164,# - MfgTestDev Events
typedef enum MfgTestDevEvent
{
    WriteMfgData = 0,
    WriteOfMfgDataPassed,
    WriteOfMfgDataFailed,
    RequestMfgData,
    ReadMfgData,
    WriteScaleHwConfig,             // 5
    WriteOfScaleHwConfigPassed,
    WriteOfScaleHwConfigFailed,
    RequestScaleHwConfig,
    ReadScaleHwConfig,
    WriteScaleSWConfig,             // 10

    WriteOfScaleSWConfigPassed,
    WriteOfScaleSWConfigFailed,
    RequestScaleSWConfig,
    ReadScaleSWConfig,
    RequestLoaderVersion,           // 15 - u-boot for linux bios for windows
    ReadLoaderVersion,
    WriteOfScaleSWConfigMetrologicalVersion,
    RequestScaleSWMetrologicalVersion,
    WriteOfScaleSWConfigPoundsFor,
    RequestScaleSWPoundsFor,        // 20

    WriteOfScaleSWConfigFractionalPrice,
    RequestScaleSWFractionalPrice,
    RequestPrinterLabelCount,
    PrinterLabelCount,
    ClearPrinterLabelCount,         // 25
    RequestWrapperCycleCount,
    WrapperCycleCount,
    RequestInstallDate,
    WrapperInstallDate,
    SetInstallDate                  // 30
} MfgTestDevEvent_et;

// 165,# - ExpandedTextDev Events
typedef enum CookingInstructionsEvent
{
    CookingInstructionsDtWrite             = 2             // DtEvent
} CookingInstructionsEvent_et;

// 166,# - RecipeDev Events
typedef enum RecipeEvent
{
    RecipeDtWrite             = 2             // DtEvent
} RecipeEvent_et;

// 167,# - ExpandedText2Dev Events
typedef enum ExpandedText2Event
{
    ExpandedText2DtWrite             = 2             // DtEvent
} ExpandedText2Event_et;

// 168,# - SuggestiveSellingDev Events
typedef enum SuggestiveSellingEvent
{
    SuggestiveSellingDtWrite             = 2             // DtEvent
} SuggestiveSellingEvent_et;

// 169,# - Allergens Events
typedef enum AllergensEvent
{
    AllergensDtWrite             = 2             // DtEvent
} AllergensEvent_et;

// 170,# - Recalls Dev Events
typedef enum RecallsDevEvent
{
    RecallsDevReadAll               = 1,
    RecallsDevWriteAll              = 3,
    RecallsDevDelete                = 4,
    RecallsDevNewRecallsExist       = 20,
    RecallsDevMarkRead              = 21
} RecallsDevEvent_et;

// 173,# - Diagnostics events
typedef enum DiagEvent
{
    // configuration messages
    DiagConfigurationReq = 32,
    DiagConfiguration,

    // status messages
    DiagStatusReq,
    DiagStatus,

    // retrieve audit log records
    DiagAuditLogReq = 48,
    DiagAuditLog,

    // retrieve audit record
    DiagAuditLogRecordReq,
    DiagAuditLogRecord,

    // record service visit to audit log
    DiagAuditLogServiceVisitStart,
    DiagAuditLogServiceVisitStop,
    DiagAuditLogServiceVisitStatus,

    // check for diagnostics error handler
    DiagCheckErrorHandlerReq = 64,
    DiagCheckErrorHandler,

    // start diagnostics
    DiagStartErrorHandler,
    DiagStartErrorHandlerFromMaintRecord,

    DiagErrorHandlerFailed,

    // show diagnostic wizard
    DiagShowWizard = 80,

    // initialize diagnostic wizard
    DiagInitWizardReq,
    DiagInitWizard,

    // perform diagnostics
    DiagPerformWizardTestingReq,
    DiagPerformWizardTesting,

    // exit diagnostic wizard
    DiagExitWizard,

    // retrieve service maintenance records
    DiagServiceMaintLogReq = 96,
    DiagServiceMaintLog,

    // clear maintenance log record
    DiagClearServiceMaintRecord,

    // print service maintenance log
    DiagPrintServiceMaintLog,

    // retrieve service actions
    DiagServiceActionsReq,
    DiagServiceActions,

    // export options for diagnostics
    DiagExportDatabase = 112,
    DiagExportDatabaseComplete,

    // send options for diagnostics
    DiagSendDatabaseEmail,
    DiagSendDatabaseGatewayDevice,
    DiagSendDatabaseComplete

} DiagEvent_et;

// 175,# - Department Messages
typedef enum DeptDevEvent
{
    DeptDevDtRead                   = 0,
    DeptDevDtReadAll                = 1,
    DeptDevDtWrite                  = 2,
    DeptDevDtWriteAll               = 3,
    DeptDevDtDelete                 = 4,
    DeptDevDtDeleteAll              = 5,
    DeptDevDtSave                   = 6,
    DeptDevDtRestore                = 7,
    DeptDevDtCreateNew		        = 12,
    DeptDevDtReadCurrentDept        = 20,
    DeptDevDtSetCurrentDept         = 21
} DeptDevEvent_et;

// 250,# - TestMessages Events
typedef enum TestMessageEvent
{
    TestNetworkReport               = 12,
    TestOperatorNames               = 21,
    TestOperatorDataEntry           = 41,
    TestCreateCopiedPLU             = 97,
    TestTestMessage                 = 98,
    TestTestWeight                  = 99,
    TestTotals                      = 118
} TestMessageEvent_et;

///////////////////////////////////////////////////////////////////////////////////////////////////

// 103, # - Error Messages
typedef enum ErrorEvent
{
	ErrUnknownDevice				= 0,
	ErrWeigherDevice				= 1,
	ErrEventDevice					= 3,
	ErrOperationModeDevice			= 4,
    ErrLabelApplierDevice           = 5,
	ErrPrinterDevice				= 7,
    ErrWrapperDevice				= 9,
    ErrWrapperStatusDevice			= 10,
	ErrTouchScreenDevice			= 14,
    ErrRFIDReader                   = 15,
    ErrBarcodeScanner               = 16,
    ErrSessionDevice				= 100,
	ErrProductDevice				= 101,
	ErrOperatorDevice				= 102,
	ErrTotalTypeDevice				= 104,
	ErrGraphicDevice				= 105,
	ErrExpandedTextDevice			= 106,
	ErrBackupRestoreDevice			= 109,
	ErrClassDevice 					= 110,
	ErrConfigurationDevice 			= 112,
	ErrVersionManagerDevice			= 120,
	ErrCategoryDevice 	   			= 121,
	ErrNetworkDevice 	   			= 122,
	ErrTotalsDevice 	   			= 123,
	ErrSpecialsDevice 	   			= 124,
	ErrOperatorChangesDevice		= 131,
	ErrCOOLDevice 					= 135,
	ErrSoftkeyDevice 				= 136,
	ErrImportDevice 				= 144,
	ErrNetworkConnectivityDevice	= 145,
	ErrSynchronizationDevice 		= 146,
	ErrTransactionDevice			= 151,
	ErrHourlyTotalDevice			= 153,
	ErrOperatorTotalDevice			= 154,
	ErrProductTotalDevice			= 156,
	ErrWindowsConfigurationDevice	= 158,
    ErrCutTestDev                   = 159,      ///< Cut Test Development Dialogs
    ErrCutTestTotalDev              = 160,      ///< Cut Test Totals Dialogs
	ErrGCDSessionDevice			 	= 161,
    ErrGCDDev                       = 162,
    ErrHIDDev                       = 163,
    ErrMfgTestDev                   = 164,
    ErrCookingInstructionsDev       = 165,
    ErrRecipeDev                    = 166,
    ErrExpandedText2Dev             = 167,
    ErrSuggestiveSellingDev         = 168,
    ErrAllergensDev                 = 169,
    ErrRecallsDev                   = 170,
    ErrDeviceDev                    = 171,
    ErrDiagnostics                  = 173,
	ErrDepartments					= 175,
	ErrEOFDevice					= 254,
    ErrRemoveAnyModalDialogDevice	= 255

} ErrorEvent_et;

/// Label applier tests.
typedef enum LaTest
{
    LaTestCyclePrinterMotor = 1,
    LaTestCycleWeigherMotor = 2,
    LaTestCompressorOn      = 3,
    LaTestCompressorOff     = 4,
    LaTestGateUp            = 5,
    LaTestGateDown          = 6,
    LaTestBeltsUp           = 7,
    LaTestBeltsDown         = 8,
    LaTestWandDown          = 9,
    LaTestWandUp            = 10,
    LaTestApplierDown       = 11,
    LaTestApplierUp         = 12,
    LaTestWandVacuumOn      = 13,
    LaTestWandVacuumOff     = 14,
    LaTestRunInOff          = 15,
    LaTestRunInOn           = 16
} LaTestEvent_et;

/// Wrapper tests.
typedef enum WrTest
{
    WrNoServiceTest          = 0,
    WrPrimaryIntakeTest      = 1,
    WrSecondaryIntakeTest    = 2,
    WrCameraTest             = 3,
    WrElevatorTest           = 4,
    WrElevatorWidePkgTest    = 5,
    WrFilmSelectTest         = 6,
    WrKnifeTest              = 7,
    WrUpperFilmBrakeTest     = 8,
    WrLowerFilmBrakeTest     = 9,
    WrFilmSensorTest         = 10,
    WrGripperNarrowTest      = 11,
    WrGripperWideTest        = 12,
    WrGripperTest            = 13,
    WrSideClampsTest         = 14,
    WrRearFolderTest         = 15,
    WrFrontFolderTest        = 16,
    WrPusherTest             = 17,
    WrSealerTest             = 18,
    WrCycleIntakeTest        = 19,
    WrPrimaryApplierTest     = 20,
    WrShifterTest            = 21,
    WrAirCompressorTest      = 22,
    WrSideFolderTest         = 23,
    WrCycleWrapper           = 24,
    WrWrapperMechResetTest   = 25,
    WrSecondLabelApplierTest = 26,
    WrSecondaryApplierTest   = 27,
    WrReservedForFirmware_1  = 28,		// DO NOT USE INTERNAL TEST ONLY = CYCLE_ACTUATOR_TEST              
    WrReservedForFirmware_2  = 29,      // DO NOT USE INTERNAL TEST ONLY = CYCLE_ACTUATOR_STARTUP_TEST  
    WrReservedForFirmware_3  = 30,      // DO NOT USE INTERNAL TEST ONLY = Verify 24v and 170v powered up properly
    WrWrapperCleanTest       = 31,		// Raise the Elevator so it can be cleaned around

    endWrEvolTestCode
} WrTestEvent_et;

///////////////////////////////////////////////////////////////////////////////////////////////////


/** @} */

#endif // EVENTS_H
