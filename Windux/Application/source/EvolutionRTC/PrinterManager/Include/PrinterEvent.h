#ifndef PRINTEREVENT_H
#define PRINTEREVENT_H

#include "Event.h"

//Printer Device Generated Events
typedef enum PrEvent
{
	PrWakeup,				// 0
	PrConfig,
	PrInquire,
	PrReset,
	PrStatus,
	PrEnable,				// 5
	PrDisable,
	PrMode,
	
	PrSensors,
	PrPrintheadPower,
	PrMask,					// 10
	PrTeach,
	PrCommand,
	PrCalibrate,
	PrRam,
	PrTest,					// 15
	PrFlush,
	PrNomSize,
	PrEnableTakeup = 19,
	PrReqWakeup = 21,		// Request Wakeup message from Peripheral
	PrReqInquire,
	PrReqStatus,
	PrReqSensors,
	PrReqLabelTransfer,	// 25
	PrReqCalibrate,
	PrSensorsTracking = 28,
	PrLabelTransferComplete,
	PrLabelTransferFailed,		// 30
	PrTestLabel,
	PrReqLabelWidth,
	PrLabelWidth,
	PrNewOrder,
	PrTakeLabelTime,			// 35
	PrLabelImageSize,
	PrReqPrinterType = 37,
	PrPrinterType,
	PrPrinterTypeTracking,
	PrReqStockMetrics,			// 40
	PrStockMetrics,
	PrReqAdvLabelStock,
	PrAdvLabelStock,
	PrNewAddress,
	PrSizing,					// 45
	PrReqPrintheadPower,
	PrPreImage,
	PrPostImage,
	PrReqKind,
	PrKind,						// 50
	PrLabelComplete,
	PrLabelReady,
	PrLabelTaken,
	PrDeletePackage,
	VpShutdown,					// 55
	VpIOCfg,
	PrResetFactoryDefaults = 57,
	PrResetFactoryDefaultsComplete,
	PrReqHeadType,
	PrHeadType,					// 60
	PrCutterCut,
	PrCutterHome,
	PrReqCutterStatus,
	PrCutterStatus,
	PrCutterEnabled,			// 65
	PrCutterDisabled,			// 66
	PrEjectCut,
	PrResetTeach,				//SOF-3847 3848
	PrCutterEnableSave,
	PrUseContinuous,

	PrReqInfo = 100, 	//	Request data for the Printer Info page.
	PrInfo,				//	The data for the Printer Info page.
	PrSaveInfo,			//	The modified data from the Printer Info page.
	PrSetPrinters = 104,
	PrResetImmediate,			// 105
	PrReqCurrentStatus,
	PrClearCurrentStatus,
	PrStatusClear,  	// There aren't any status bits set that would be reported to the UI, thus the UI can clear the icon that indicates printer statuses exist. (Fastrack)
	PrStatusNotClear,  	// There is at least one status bit set that we want to let the operator know about by changing the color of the printer icon.  See HfcPrinterStatus::notificationReasonExists() method.
	PrReadAllUsbPrinterNames = 111,
	PrWriteAllUsbPrinterNames = 112,
	PrReadAllNetworkPrinterNames = 113,
	PrWriteAllNetworkPrinterNames = 114,
	PrReadExternalPrinter = 120,	// 120
	PrReadAllExternalPrinters,
	PrWriteExternalPrinter,
	PrWriteAllExternalPrinters,
	PrSaveExternalPrinter = 126,
	PrReadAllProductTextCfgs = 131,
	PrWriteProductTextCfg = 132,
	PrWriteAllProductTextCfgs = 133,
	PrSaveProductTextCfg = 136,
	PrPrintPdfDocument = 140,
	PrPrintWebDocument,
	PrReqVirtualLabel = 150,
	PrVirtualLabelComplete,
	PrAgentAttached,
	PrAgentDetached,
	PrBackupPrinterSettings,
	PrRestorePrinterSettings,
	PrReqLabelSizing = 160,
	PrUseContinuousStock,
	PrSetupForNormalSizing,
	PrReqAreWeSizingLabels,
	PrAreWeSizingLabels,
	PrReqVersion = 170,
	PrVersion,
	PrKPCDetached,
	PrLabelTransferReady,
	PrReqHeadDotWear,
	PrHeadDotWear,
	PrRS485FlowControl,
	PrKPCError,
	PrFlushCmplt,
	PrHeadDotStatus,
	PrReqHeadDotStatus = 180,
	PrHeadDotAck,
	PrReqTestCassette,
	PrTestCassette,
	PrCutterBitDflt,					//SOF-3887
	PrPreImageBarcode,
	PrPostImageBarcode,
	PrDonePostImagingSelfServeBarcodeLabelImage,
	PrStartGapCalibration,				//SOF-4727 freestanding scale
	PrGapCalStatus,						//SOF-4727 freestanding scale
	PrGapCalNext						//SOF-4727 freestanding scale

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
} PrEvent_t;

typedef enum PrinterErrors
{
	PrinterErrUnknownError = Printer * DEVICE_ERROR_OFFSET,
	PrinterErrCommLoss,
	PrinterErrManagerDeleted,
	PrinterErrHeadUp,
	PrinterErrLabelSizing,
	PrinterErrTakeLabel,
	PrinterErrTakeLabelBeep,
	PrinterErrLabelJammed,
	PrinterErrLabelMissing,
	PrinterErrOutOfMedia,
	PrinterErrCassetteMissing,
	PrinterErrHardwareError,
	PrinterErrThermalOverload,
	PrinterErrThermalShutdown,
	PrinterErrInvalidTypeSetup,
	PrinterErrMSDisengage,
	PrinterErrMSSituateLabelStock,
	PrinterErrMSEngage,
	PrinterErrMSLabelStockTaut,
	PrinterErrMSCalibrating,
	PrinterErrMSComplete,
	PrinterErrLTClearSensor,
	PrinterErrLTCalibrating,
	PrinterErrLTComplete,
	PrinterErrSLAIsOff,
	PrinterErrSLAOutOfLabels,
	PrinterErrHeadUpForEpp,
    PrinterErrNoSpaceAllowedInName,
	PrinterErrInvalidCharInName,
	PrinterErrNoSizingDueToContinuous,
	PrinterErrSizedContinuousOK,
	PrinterErrLabelCutterJammed,
	PrinterErrLabelCutterInterlockOpen,
	PritnerErrLabelCutterTimedOut,
	PrinterErrProductionEntryInterlockOpen,
	PrinterErrLowStock,
	PrinterErrOutOfStock,
	PrinterErrFSSLabelCutterInterlockOpen,
	PrinterErrFSSLabelDoorInterlockOpen
	
} PrinterErrors_t;

#endif // PRINTEREVENT_H
