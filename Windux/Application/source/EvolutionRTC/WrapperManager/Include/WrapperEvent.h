#ifndef WRAPPEREVENT_H
#define WRAPPEREVENT_H

	#include "Event.h"

	//Wrapper Device Generated Events
	typedef enum WrEvent
	{
		WrWakeup,
		WrConfig,
		WrInquire,
		WrReset,
		WrStatus,
		WrEnable,
		WrDisable,
		WrMode,
		WrTest,
		WrPause,
		WrResume,
		WrWrap,
		WrCycle,
		WrJog,
		WrControl,
		WrSensors,
		WrAbort,
		WrOpen,
		WrErrorHistory,
		WrLoad,
		WrReqWakeup,		// Request Wakeup message from Peripheral
		WrReqInquire,
		WrReqStatus,
		WrReqSensors,
		WrReqErrorHistory,
		WrSensorsTracking,
		WrErrorHistoryTracking,
		WrStatusTracking,
		WrIntakeMode,

		WrNGWReqBootStatus = 30,
		WrNGWBootStatus,
		WrNGWBootWriteAppFlash,
		WrNGWBootFrame,
		WrNGWBootFrame1,
		WrNGWBootFrame2,
		WrNGWBootFrame3,
		WrNGWBootFrame4,
		WrNGWBootFrame5,
		WrNGWBootFrame6,
		WrNGWBootFrame7,
		WrNGWBootFrame8,
		WrNGWAck,
		WrNGWFlush,
		WrNGWBootCMD,
		WrNGWBootVersion,
		WrNGWRequestBootVersion,
		WrNGWBulkFrame,


		// EvolWrapperMessages
		WrEvolWakeup = 50,
		WrEvolStatus,
		WrEvolControl,
		WrEvolLoad,
		WrEvolWrap,
		WrEvolSettings,
		WrEvolInquire,
		WrEvolConfig,
		WrEvolCamerasTracking,
		WrEvolCameraHeightScanTracking,
		WrEvolReqErrorHistory,		//	60
		WrEvolErrorHistory,
		WrEvolDoneErrorHistory,
		WrEvolSendDateTime,
		WrEvolRemoveUpgradeFiles,
		WrEvolUpgradeInProgress,
		WrEvolErrorHistoryList,
		WrEvolErrorHistoryClear,
		WrEvolErrorHistoryListPrevious,
		WrEvolErrorHistoryListNext,
		WrEvolErrorHistoryListDone,	//	70
		WrEvolSaveCycleCountToCF,
		WrEvolReqElevatorPkgDetectorStatus,
		WrEvolReqSealerPkgDetectorStatus,
		WrEvolPkgDetectorStatus,
		WrNGWWrapperReStart,
		WrNGWWrapperRecover,

		WrNGWEngInfoN1 = 80,
		WrNGWEngInfoN2,
		WrNGWEngInfoN3,
		WrNGWEngInfoN4,
		WrNGWEngInfoN5,
		WrNGWEngInfoN6,
		WrNGWENGException,
		WrNGWDimensionOverrides,

		WrPPPWakeup,
		WrPPPConfig,
		WrPPPWriteConfig,
		WrPPPDist,
		WrPPPCalibration,
		WrPPPRequest,
		WrPPPVersion,
		WrWrapperPause,							// not using this anymore to pause for original printer plate positioning screen
		WrPPPStatus,
		WrPPPTest,
		WrPPPResume,	
		WrPPPBootVersion,
		WrPPPBootError,
		WrReqKind = 100,
		WrKind,
		WrSetWrapMode,
		WrEvolTest = 108,
		WrEvolCycle = 112,
		WrEvolSensors = 115,
		WrEvolPackageDesc = 125,
		WrEvolRequestPackageDesc,
		WrLabelReady = 134,			//new NGW
		WrLabelSetup,				//new NGW
		WrLabelTaken = 138,			//new NGW
		WrCameraScan = 139,
		WrCameraHeightScan,
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
		WrEvolCameraThreshold,
		WrEvolWriteCameraThreshold,
		WrEvolUpdateConfigLabelWidth,
		WrWrapBoxSize,
		WrWrapBoxAck,

		WrEvolEventCapture = 180,
		WrClearInstallKnifeTime,
		WrRequestKnifeTimeHistory,
		WrKnifeTimeHistory,
		WrReqUIWrapperModel,
		WrUIWrapperModel,
		WrEvolStartMfgRunin,
		WrWrapBoxSizeSave,
		
		WrEvolDiagnosticsTest = 188,
		WrEvolDiagnosticsTestResult,

		WrEvolReqSystemReset = 190,
		WrEvolManufacturingRunInNotice,
		WrEvolMotorProfile,

		WrReqEvolHarwareAdditions,

		WrEvolCancelAttributeChanges = 195,
		WrEvolDoneAttributeChanges,
		WrEvolReadAllForAttribute,
		WrEvolWriteAllForAttribute,
		WrEvolSaveAttributeChanges,
		WrReqSettings = 200,
		WrSettings,
		WrTestTracking,
		WrTestSense,
		WrReqErrorTotals,
		WrErrorTotals,               // 205
		WrResetErrorTotals,
		WrReqPackageTotals,
		WrPackageTotals,
		WrResetPackageTotals,
		WrSavePermanentTotals,       // 210
		WrRestorePermanentTotals,
		WrAllPackCodeNames,
		WrReadPackCode,
		WrWritePackCode,
		WrSavePackCode,              // 215
		WrCycleMode,
		WrIntakePkgSize,
		WrResetFactoryDefaults,
		WrProductLoaded,
		WrSetGripperPWMTimings,      // 220
		WrReqPackageHistory,
		WrPackageHistory,
		WrEvolSavePackageTimings,			//	Was WrReqPackageTimings,
		WrEvolSavePackageTimingsComplete,	//	Was WrPackageTimings,
		WrWriteAllTrayTypes,         // 225

		WrEvolLoadPackageTimings = 226,
		WrEvolRequestPackageTimings,
		WrEvolPackageTimings,
		WrEvolResetDefaultPackageTimings,
		WrEvolDonePackageTimings,
		WrEvolPackageTimingsWrite,

		WrEvolAutoDetectTrayType = 232,
		WrEvolAutoConfigTrayType,
		WrEvolCreateTrayType,
		WrEvolChangeTrayTypePrecedence,
		WrEvolWriteTrayType,
		WrEvolWriteAllTrayTypes,
		WrEvolDeleteTrayType,
		WrWrapBoxTransferComplete,

		WrEvolReadWrapBox = 240,
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
	} WrEvent_t;

#endif // WRAPPEREVENT_H
