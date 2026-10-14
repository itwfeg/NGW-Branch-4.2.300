#ifndef WEIGHEREVENT_H
#define WEIGHEREVENT_H

#include "Event.h"

// Weigher Device Generated Events

typedef enum WgEvent
{ 
	WgWakeup, 
	WgConfig, 
	WgInquire, 
	WgReset, 
	WgStatus,
	WgEnable, 
	WgDisable, 
	WgMode, 
	WgRezero,
	WgControl,
	WgColdReset,		//	Sent to the Atmel Sam-ba weigher. [PTR 387 & 388]
	WgCat3ReqStatistics,
	WgCat3AuditStatistics,
	WgCat3DateTime,
	WgCat3ReqPage,
	WgCat3Records,
	WgCat3ReqNextRecords,
	WgCat3WriteRecord,
	WgCat3RecordStatus,
	WgCat3ReqEraseLog,
    WgCat3LogEraseComplete,
	WgCat3Error,
	WgResetFactoryDefaults,
	WgResetFactoryDefaultsComplete,
	WgKPCDetached,
	WgReqSystemInfo,
	WgSystemInfo,

	WgNormal = 32,		// this is a command from rtc
	WgLock,				// this is a command from rtc
	WgReqWakeup,		// Request Wakeup message from Peripheral
	WgReqStatus, 		// Request Status Message From Peripheral
	WgReqInquire,		// Request Inquire Message From Peripheral
	WgReqKind,
	WgKind,
	WgCachedStatus,     // old, cached status reported to RTC

	WgReqValueMax,

	WgValueMaxAssembly,
	WgValueMaxWriteAssembly,
	WgValueMaxHwVersion,
	WgValueMaxWriteHwVersion,
	WgValueMaxSerialNumber,
	WgValueMaxWriteSerialNumber,
	WgValueMaxDateOfManufacture,
	WgValueMaxWriteDateOfManufacture,
	WgValueMaxXGain,
	WgValueMaxWriteXGain,
	WgValueMaxYGain,
	WgValueMaxWriteYGain,
	WgValueMaxZGain,
	WgValueMaxWriteZGain,
	WgValueMaxAllGains,
	
	WgKPCError		 = 56,

	WgStatusAndPrint = 99,

	WgCalibrationStep = 100, // Used for messages between the browser and Session to calibrate weigher

	// the data within this message indicates if the calibration ends the steps portion of the calibration (0),
	// or if the whole calibration is ending (after the gain factor is checked) (1),
	// or if the user is cancelling the whole thing (2).
	WgCalibrationEnd,

	WgReqInfo,			//	Request data for the Weigher Info page.
	WgInfo,				//	The data for the Weigher Info page.
	WgStatusCounts,	// Used to send the raw counts to the Browser for the Weigher Info page.
	WgSaveInfo,			//	The modified data from the Weigher Info page.
	WgAttach,			// Set the weigher attached flag.
	WgDoneInfo,			//	The user is leaving the Weigher Info page.

	WgConfigurationBegin = 110,
	WgConfigurationEnd,

	WgBackupWeigherSettings,
	WgRestoreWeigherSettings,
	WgBeginRemoteWgConfig,
	WgEndRemoteWgConfig,
	WgReadAuditTrail,
	WgWriteAuditTrail,
	WgPrintAuditTrail,
	WgSaveAuditTrailToDisk,
	WgSaveAuditTrailToDiskFinished,
	WgReadAllAuditTrailForHTe,
	WgReadAllAuditTrailForComm,
	WgReadWeigherConfigForHTe,

	WgReqSerialNum,
	WgSerialNumber,
	WgReqVersion,
	WgVersion,
	WgValueMaxFitParams,
	WgReqConfig, 			//Same as WgReqInquire except with a retransmit timer


	// DO NOT ADD NEW WEIGHER EVENTS AFTER 200. THEY WILL NOT BE PROCESSED IN Session::handleWeigherMessage()
	WgTestInit = 200,
	WgTestRezero,
	WgTestReadConfig,
	WgTestUp1 = 205,
	WgTestUp10th,
	WgTestUp100th,
	WgTestUp1000th,
	WgTestDown1 = 210,
	WgTestDown10th,
	WgTestDown100th,
	WgTestDown1000th,

	WgTestSwitch = 215,

	WgTestSimulateWgStatus,
	WgEndTestSimulateWgStatus,

	WgEventMax

} WgEvent_t;

typedef enum WeigherErrors
{
	WeigherErrUnknownError = Weigher * DEVICE_ERROR_OFFSET,
	WeigherErrCommLoss,
	WeigherErrNegativeGrossWeight,
	WeigherErrOverGrossWeightLimit,
	WeigherErrScaleWarmup,
	WeigherErrRezeroing,
	WeigherErrZeroOutOfMaintZone,
	WeigherErrZeroOutOfMaintZoneStatus,
	WeigherErrRemoveWeight,
	WeigherErrSelectWeightsType,
	WeigherErrPressCalibrateSwitch,
	WeigherErrDeltaChangeTooBig,
	WeigherErrCalibrationSuccessful,
	WeigherErrNotAttached,
	WeigherErrScaleNotZero,
	WeigherErrInvalidAddress,
	WeigherErrManagerDeleted,
	WeigherErrTareDisabled,
	WeigherErrConfigOutOfRange,
	WeigherErrCantRezeroNegativeGrossWeight,
	WeigherErrRemoteConfigurationStarted,
	WeigherErrOutOfCalibrateTiltRange,
	WeigherErrValMaxEepCommFailure,
	WeigherErrValMaxEepChecksumFailure,
	WeigherErrValMaxEepAcclCommFailure,
	WeigherErrValMaxTiltReached,
	WeigherErrUnderMinimumWeight
	
} WeigherErrors_t;

#endif // WEIGHEREVENT_H
