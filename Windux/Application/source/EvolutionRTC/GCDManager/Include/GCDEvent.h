#ifndef GCDEVENT_H
#define GCDEVENT_H

#include "Event.h"

// GCD Session Generated Events

enum GCDSessionEvent
{ 
	GcdSessionRestart,						// 0
	GcdSessionShuttingDown,					// 1
	GcdSessionColorTestOn,					// 2
	GcdSessionColorTestOff,					// 3
	GcdSessionStatus,						// 4 - also SysStatus
	GcdSessionEnglish,						// 5
	GcdSessionFrench,						// 6
	GcdSessionSpanish,						// 7
	GcdSessionAvoir,						// 8
	GcdSessionMetric,						// 9
	GcdSessionKind,							// 10
	GcdSessionColorTestWhiteOn,				// 11
	GcdSessionNowServingCustomer,   		// 12
	GcdSessionReqStatus,					// 13 - also SysReqStatus
	GcdSessionDismissNowServingCustomer,	// 14
	GcdSessionDisplayItemTransImage,		// 15
	GcdSessionDisplayItemTransText,			// 16
	GcdSessionDisplayItemTransPlaylist,		// 17
	GcdSessionColorTestBlackOn,				// 18
	GcdSessionColorTestBlankScreenOn,		// 19
	GcdSessionColorTestBlankScreenOff,		// 20

	GcdSessionSetVolume = 32,				// 32
	GcdSessionUpdateViaFtpNow,				// 33
	GcdSessionUpdateViaUsbNow,				// 34
	GcdSessionSetPlaybackWaitTime,			// 35
	GcdSessionUpdateLayout,					// 36
	GcdSessionUpdatePlaylist,				// 37
	GcdSessionRNDISSettingsChanged			// 38
};

enum ErrGCDSessionDev
{
	GCDSessionLostConnectionToGCD = 161002,
	GCDSessionRestartingGCD = 161003,
	GCDSessionUpdatingGCDFirmware = 161004,
	GCDSessionUpdatingGCDMedia = 161005,
	GCDSessionMediaUpdateInProgressGCD = 161006,
	GCDSessionMediaUSBUpdateFailure = 161008,
	GCDSessionMediaUSBUpdateSuccess = 161009,
	GCDSessionMediaFTPUpdateFailure = 161010,
	GCDSessionMediaFTPUpdateSuccess = 161011,
	GCDSessionFirmwareFTPUpdateFailed = 161012,
	GCDSessionFirmwareFTPUpdateSuccess = 161013,
	GCDSessionFirmwareUSBUpdateFailed = 161014,
	GCDSessionFirmwareUSBUpdateSuccess = 161015,
	GCDSessionNothing = 161999
};

enum GCDEvent
{
    GcdData = 11,
    GcdLoadDisplayObjectXML,
    GcdTestColor,
    GcdShutdownDevice,
    GcdLanguageAndWeighMode,
    GcdSetVolume,
    GcdHeartbeat,
    GcdGetStatus,
    GcdGetFileInfo,
    GcdFileUpload,
    GcdLoadPlaylist,
    GcdStartFirmwareUpdate,
	GcdStartPlaylistUpdate,
	GcdSetPlaybackWaitTime,
	GcdSetTimeInfo,
	GCDTransferStart,
	GCDTransferDone,
	GCDTransferError,
	GcdSetUnitSeparator,
	GcdSetCurrencySymbol,
	GcdNowServingCustomerNumber,
	GcdDismissNowServingCustomerNumber,
	GcdDisplayItemTransImage,
	GcdDisplayItemTransText,
	GcdDisplayItemTransPlaylist,

	// Response from GCD
    GcdError = 111,
    GcdHeartbeatResponse,
    GcdLoadDisplayObjectXMLResponse,
    GcdLoadPlaylistResponse,
    GcdStatus,
	GcdFileInfo,
	GcdRequestFileUpload,
	GcdFirmwareUpdateStaged,
	GcdFirmwareUpdateCompleted,
	GcdPlaylistUpdateStaged,
	GcdPlaylistUpdateCompleted
};

#endif // GCDEVENT_H
