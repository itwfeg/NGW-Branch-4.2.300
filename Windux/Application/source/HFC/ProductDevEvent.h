#ifndef PRODUCTDEVEVENT_H
#define PRODUCTDEVEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"

	// Product Generated Events - ProductDev
	typedef enum ProductEvent
	{
		ProductUnknown,
		ProductChange,
		ProductUpdate,
		ProductComplete,
		ProductPrint,
		ProductRead,					// 5
		ProductReadAll,
		ProductWrite,
		ProductDelete,
		ProductReadSupportData,
		ProductWriteSupportData,		// 10
		ProductChangeSupportData,
		ProductDeleteSupportData,
		ProductReadLifeData,
		ProductReadCurrentTotalValues,
		ProductWriteCurrentTotalValues,	// 15
		ProductWriteSingleField,
		ProductError,
		ProductReadDefault,
		ProductWriteDefault,
		ProductDeleteDefault,			// 20
		ProductSaveDefault,
		ProductSave,
		ProductShowLinksToSupportData,
		ProductCreate,
		ProductDeleteAll,				// 25
		ProductRestore,
		ProductReadNutriLabel,
		ProductReadSupportRecord,
		ProductGetTree,
		ProductFindByDescription,		// 30
		ProductFindSoftKeysByDescription,
		ProductTransactionTypeChanged,
		ProductReadLabelFormSize,
		ProductWriteLabelFormSize,
		ProductPackageComplete,		    // 35
		ProductResetUnitPrice,
		ProductApplyDiscountCentsOff,
		ProductApplyDiscountPercentOff,
		ProductApplyDiscountPerPackage,
		ProductStatusEvent,				// 40
		ProductGetBackDateDays,

		// Related to Production Entry Mode
		ProductStartPrintAllLabels,
		ProductStopPrintAllLabels,
		ProductFinishedPrintingAllLabels,
		ProductStartPrintForTransaction,
		ProductDontUpdateProductionPlanning,

		ProductClearUI,

		ProductPrintMultipleLabels,

		ProductDeleteAllCommodityClass = 49,
		ProductReadSoftKeyList = 50,
		ProductWriteSoftKeyList,
		ProductReadAllSoftKeyGraphicNames,
		ProductWriteAllSoftKeyGraphicNames,

		ProductToggleLabelType,			//	54

		ProductPrintNewExpirationLabel = 58,
		ProductPrintExistingExpirationLabel = 59,
		ProductReadExpirationLabel,		// 60
		ProductReadAllExpirationLabels,
		ProductWriteExpirationLabel,
		ProductWriteAllExpirationLabels,
		ProductDeleteExpirationLabel,
		ProductDeleteAllExpirationLabels,	// 65
		ProductSaveExpirationLabel,
		ProductRestoreExpirationLabel,

		ProductBatchApplyDelete,
		ProductIncomplete,

		ProductGetBackDateDate,				//	70
		ProductChangeRotationPlacement,		//	Was 44 in vxWorks

		ProductChangeSmart,

		ProductAddItemToAccumulatedItems,
		ProductRemoveLastItemFromAccumulatedItems,
		ProductShowAccumulatedItems,		// 75  deprecated
		ProductRemoveAllAccumulatedItems,
		ProductPrintAccumulatedItems,
		ProductRequestReadAllAccumulatedItems,
		ProductReadAllAccumulatedItems,
		ProductWriteAllAccumulatedItems,	//80
		ProductRemoveAccumulatedItems,
		ProductRemoveSingleAccumulatedItem,
		ProductReadAndAddSideItem,
		ProductSetMealWeight,
		ProductLeaveBuildAMealMode,			// 85
		ProductReadAccumulatedMasterTrans,

		ProductAddItemToAccumOrMeal,

		ProductReadToo,
		ProductClearPrintingProductsList,

		ProductPrintProductSupportText = 90,
		ProductSaveSupervisorTransaction,

        ProductRemotePrintStatus,
		ProductRemoteRequestRWItem,

			
		// Used in conjuction with the SelfServePrintToScreenFunctionality
		ProductCompletePrintToScreen,

		ProductPausePrintAllLabels,
		Product2ndLabelApplyToggle,
		
		ProductReadOnlineOrderingProduct,
		ProductWriteOnlineOrderingProductTotals,
		ProductRemovePreviousTransaction,

	}   ProductEvent_t;

	typedef enum ProductRemotePrintEvent
	{
		RemotePrintRedoMax     = 25,
		RemotePrintErrorsMax   = 100,		    // what status codes can close the socket
		RemotePrintUnknown = 989,
		RemotePrintTooFewArgs,					// 990 smBackend only
		RemotePrintTryAgainLater,				// 991 no longer used - single instance P2 restriction
		RemotePrintJobStatus,					// 992 smBackend only
		RemotePrintQueueStatusError,			// 993 smBackend only
		RemotePrintQueueEmptied,				// 994 smBackend only
		RemotePrintQueueClearingError,			// 995 smBackend only
		RemotePrintRegistrationError,			// 996 could not register so close a socket if opened
		RemotePrintRegistrationSuccess,			// 997 leave open
		RemotePrintScaleIsAvailable,			// 998 is available success so close
		RemotePrintSuccessfulPrint,				// 999 printing successful so close

		RemotePrintLastError
	}   ProductRemotePrintEvent_t;

	typedef enum ProductDevError
	{
		ProductErrUnknownError = ProductDev * DEVICE_ERROR_OFFSET,
		ProductErr,							//	Was ProductErrRecordNotFound
		ProductErrCannotChangeProduct,
		ProductErrCannotPrintLabel,
		ProductErrReportingClass,
		ProductErrLabelType,								//	101005
		ProductErrGraphic,

		// The following 3 are for Joe's use, to prompt the operator during support record maintenance.
		ProductErrUse,
		ProductErrCopyUse,
		ProductErrRenumber,
		ProductErrPrintWrongOpMode,							//	101010
		ProductErrPrintNoProduct,
		ProductErrPrintNoTransaction,
		ProductErrPrintNoWeight,
		ProductErrPrintBelowMinWeight,
		ProductErrScaleNotPresent,							//	101015
		ProductErrPriLabelNotFound,
		ProductErrSecLabelNotFound,
		ProductErrThirdLabelNotFound,
		ProductErrPriLabelWontFit,
		ProductErrSecLabelWontFit,							//	101020
		ProductErrThirdLabelWontFit,
		ProductErrPriLabelTypeChanged,
		ProductErrSecLabelTypeChanged,
		ProductErrThirdLabelTypeChanged,
		ProductErrPrintBadByCount,							//	101025
		ProductErrDeleteAreYouSure,
		ProductErrInvalidWeightToPrint,
		ProductErrRenumberDuplicateRecord,
		ProductErrExpandedTextNotFound,
		ProductErrSpecialMessageNotFound,					//	101030
		ProductErrProductNoteNotFound,
		ProductErrNutrifactsNotFound,
		ProductErrPriLabelNotSized,
		ProductErrSecLabelNotSized,
		ProductErrThirdLabelNotSized,						//	101035
		ProductErrPrintingLabel,
		ProductErrInvalidInSelfServeMode,
		ProductErrSearchByPluInvalidChar,
		ProductErrPriceEntryRequired,
		ProductErrByCountEntryRequired,						//	101040
		ProductErrTareEntryRequired,
		ProductErrNoLabelTypesSelected,
		ProductErrPlatterTareMode,
		ProductErrMarqueeTextNotFound,
		ProductErrTareInvalid,								// 101045
		ProductErrForcedFieldNotEntered,
		ProductErrProductNumberForFlashkeyInvalid,
		ProductErrEANMaskNotSetup,
		ProductErrCannotSelectRandomWeightProduct,
		ProductErrOnlyForManualWeighMode,					// 101050
		ProductErrLabelOnlyForRandomWeight,
		ProductErrFPPSinglePLUUpdate,
		ProductErrFPPMultiplePLUUpdate,
		ProductErrFPPFailedToUpdate,
		ProductErrFPPIngredientNotAllowedInProdEntry,		// 101055
		ProductErrCannotPrintExpLabelInProdEntry,
		ProductErrNoProductForPrintMultLabel,
		ProductErrCannotPrintMultLabelInProdEntry,
		ProductErrCannotPrintMultLabelsForRandomWeight,
		ProductErrFPPSinglePLUUpdateMultipleLabels,         // 101060
		ProductErrCannotPrintExpLabelInPrintMultiple,
		ProductErrCannotPrintMultLabelsForFPPType2,
		ProductErrNoExistingExpirationRecordForPrintExisting,
		ProductErrExistingExpirationRecordIsExpired,
		ProductErrNewExpirationRecordAlreadyExists,         // 101065
		ProductErrTotalPriceOutOfRange,
		ProductErrRotationPlacementChanged,					// Was 101046 in vxWorks
		//ProductErrCannotPrintWhenGCDNotConnected = 101071,   // 101071
		ProductErrCannotPrintWhenCustomerDisplayNotConnected = 101071,   // 101071

		ProductErrCannotSelectRandomWeightForAccumulateItems,
		ProductErrFPPIngredientNotAllowedInAccumulateItems,
		ProductErrItemAddedToAccumulateItemsList,
		ProductErrNoProductForAccumulateItems,				// 101075
		ProductErrAccumulateItemsRemoveAllAreYouSure,
		ProductErrNoWeightForAccumulateItems,
		ProductErrOnlyForPrepackMode,
		ProductErrForcedFieldEntryInvalidInSelfService,

		ProductErrTareGreaterThanMaxTare,					// 101080
        ProductErrPrepackPrintWrongOpMode,

		ProductErrMaxSidesAlreadyAddedToMeal,
		ProductErrNeedMoreSidesAddedToMeal,
		ProductErrWeightNeededForMeal,
		ProductErrInvalidSideProductType,					// 101085

		ProductErrProductNotValidInThisMode,

		ProductErrFixedWeightVerificationNoProductionEntry,
		ProductErrFixedWeightVerificationNoPrintMultiple,
		ProductErrFixedWeightVerificationNoWeigher,
		ProductErrFixedWeightVerificationNeedWeight,		// 101090
		ProductErrFixedWeightVerificationInsufficientWeight,

		ProductErrProductNotValidForThisCutTest,
		ProductErrProductNoFixedWeightInCutTest,

		// adding a bunch of error conditions for remote printing
		ProductErrCannotPrintInSupervisorMode,
		ProductErrCannotPrintInServiceMode,					// 101095
		ProductErrCannotPrintInSelfServiceMode,
		ProductErrCannotPrintInKeyboardNetWeightMode,
		ProductErrCannotPrintInFixedWeightVerificationMode,
		ProductErrCannotPrintInRandomWeightItemMode,
		ProductErrCannotPrintInStreamingLabelsMode,			// 101100
		ProductErrCannotPrintInProdEntryMode,
		ProductErrCannotPrintAccumulateMode,
		ProductErrPrintBadUnitPrice,
		ProductErrPrintBadTare,
		ProductErrPrintBadCool,								// 101105	
		ProductErrPrintByCountAboveNineNine,
		ProductErrPrintWeigherIssue,
		ProductErrPrintCantFindMainSession,
		ProductErrPrintCantFindConfiguration,
		ProductErrPrintCheckScaleForPrinterRelatedError,	// 101110
		ProductErrPrintCantPrintProductionPlanningLabel,
		ProductErrPrintTransactionMissingToPrint,
		ProductErrCannotPrintBuildAMeal,
		ProductErrPrintInvalidPLU,
		ProductErrPrintInvalidByCount,						// 101115
		ProductErrPrintInvalidUnitPrice,
		ProductErrPrintInvalidTare,
		ProductErrPrintHeadDisengaged,
		ProductErrPrintLabelSizing,
		ProductErrPrintMediaJammed,							// 101120
		ProductErrPrintOutOfMedia,
		ProductErrPrintCassetteMissing,
		ProductErrPrintThermalOverload,
		ProductErrPrintThermalShutdown,
		ProductErrPrintHardwareError,						// 101125
		ProductErrPrintMediaShutdown,
		ProductErrPrintManagerDeleted,
		ProductErrPrintLabelTakenError,
		ProductErrPrintTakeLabelError,
		ProductErrPrintScaleNotLicensed,					// 101130
		ProductErrPrintCantCreateProductUpdate,
		ProductErrPrintPrintOnDefaultLabel,
		ProductErrPrintPrintOnCorrectedLabel,
		ProductErrPrintInvalidDefaultAndOverride,
		ProductErrPrintLabelDoesNotFit,						// 101135
		ProductErrPrintUnknownLabelError,
		ProductErrPrintPrintingRemoteLabel,
		ProductErrPrintPluLoadedOnScale,
		ProductErrPrintNoForcedByCounts,
		ProductErrPrintLabelMissing,						// 101140
		ProductErrPrintPeripheralComm,
		ProductErrPrintRandomWeightPluReturned,
		ProductErrPrintWhiteListOnScaleRule,
		ProductErrPrintExceedsMaxPrice,
		ProductErrPrintNoPluPassedIn,						// 101145
		ProductErrNumberOfStreamingLabelisZeroOrInvalid,
		ProductErrCantFindProduct,
		ProductErrPrintSocketTimedOutWaitingForLabels,
		ProductErrPrintSocketTimedOutWaitingForMsgs,
		ProductErrPrintUnknownModeError,					// 101150
		ProductErrPrintUnknownPrinterError,
		ProductErrPrintRemoteLabelsStarted,

		ProductErrBelowMinWeight,							// 101153 - was 52 Start PICK 5 messages
		ProductErrAboveMaxWeight,                           // 101154 - was 53
		ProductErrBelowMinTotalPrice,						// 101155 - was 54
		ProductErrAboveMaxTotalPrice,						// 101156 - was 55
		ProductErrMaxPriceBelowMinPrice,                    // 101157
		ProductErrMaxBelowUnitPrice,                        // 101158
		ProductErrMinBelowUnitPrice,                        // 101159
		ProductErrMaxWeightBelowMinWeight,					// 101160 - END PICK 5 messages

		ProductErrPrintPrintJobCancelViaUI,					// 101161 - START Remote Print errors
		ProductErrPrintInvalidBarcodeForTemplate,
		ProductErrPrintInvalidTemplateNumber,
		ProductErrPrintItemAddedToPrintQueue,
		ProductErrPrintPrintQueueIsFull,					// 101165
		ProductErrPrintInvalidPrintOperation,
		ProductErrPrintQueueIsEmpty,
        ProductErrNumberOfSellByDaysisZeroOrInvalid,        // 101168 - new
		ProductErrZeroPriceDigitsWarning,                   // 101169 - was 68
		ProductErrInvalidBarcodePrefix,                     // 101170 - was 69
		ProductErrInvalidBarcodeUPCPrefix,                  // 101171 - was 70
        ProductErrInvalidBarcodeUPCPrefixOnFW,
        ProductErrDoNotPrintEmptyNutrifactsLabel,           // reserved spot for when SMART is merged over
	    ProductErrCannotStopCutTestNotStarted,
		ProductErrCannotSuspendCutTestNotStarted,			// 101175
		ProductErrExtraDayTimeIsZero,
		ProductErrExtraDayAlreadyEnabled,
		ProductErrSaveWithDoneButtonOnly,
		ProductDevWaitForLabelToPrintOn2P,					// 101179	

		ProductErrRFIDWriteFail = 101200,
		ProductNoSupportRecord,
		ProductErrRFIDBoardFail,
		ProductCOOLTextNone,

        ProductErrPrintLastError
	}   ProductDevError_t;

#endif // PRODUCTDEVEVENT_H
