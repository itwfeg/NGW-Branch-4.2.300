/**
 * @file ScaleClasses.h
 * @brief Allows for defining the enums of an item matching the Application 1:1.
 *
 * @copyright Copyright (c) 2018, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */
#ifndef SCALECLASSES_H
#define SCALECLASSES_H

#include "Devices.h"
#include "Events.h"

class ScaleClasses
{
public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================
    /// Constructor.
    /**
     * @param[in] None
     * @returns None
     */
    /// Constructor.
    ScaleClasses();

    /// Destructor.
    ~ScaleClasses();
};

class PrinterType
{
public:
    // from Printer.h - PrinterType::FirstPrinter
    enum PrinterTypeOffsets
    {
        FirstPrinter,
        SecondPrinter,
        ThirdPrinter,
        MaxPrinterTypes
    };
};

class PrinterKind
{
public:
    // from Printer.h - usage: PrinterKind::EppPrinter
    enum PrinterKindOffsets
    {
        fieldOffsetNoPrinter = 0,   //  0
        fieldOffsetEppPrinter,		//	  Standalone EPP Printer, but we are running out of parts for it, moving to EPP2. Supports the EPP2 printer (without the LSMN board)
        fieldOffsetEssPrinter,		//	  Not supported.
        fieldOffsetEscPrinter,		//	  Not supported.
        fieldOffsetApcPrinter,		//	  The APC Printer is the normal everyday printer for the HT scales as a primary printer.Service counter printer
        fieldOffsetEpp2Printer,	    //	5 Supports the EPP3 printer (Atmel-based SAM-BA device) Newest Atmel EPP Printer which is becoming the newer standard.
        fieldOffsetUSBPrinter,
        fieldOffsetNetworkPrinter,
        fieldOffsetFakePrinter,
        fieldOffsetKPCPrinter,		//    Support new CAN KPC (Weigher / Printer) New Kinetis Printer is the normal everyday printer for the HT scales as a primary printer.
        fieldOffsetARKPrinterOnly,  // 10 Supports new CAN printer only
        fieldOffsetKPPPrinter,		//    Supports new CAN prepack printer
        fieldOffsetSSKPrinter       // New freestanding self service printer
    };
};

// 100,# - Fields of a product that make up a product
class Product
{
public:
    // from Product.h
    enum ProductFieldOffsets
    {
        fieldOffsetProductNum = 1,
        fieldOffsetProductType,
        fieldOffsetDescription,
        fieldOffsetBarcodeType,
        fieldOffsetBarcodeFields,		// 5
        fieldOffsetBarcodePrefix,
        fieldOffsetBarcodeProductNum,
        fieldOffsetBarcodeVendorNum,
        fieldOffsetBarcodeEANCode,
        fieldOffsetByCount,				// 10
        fieldOffsetExceptionPrice,
        fieldOffsetUnitPrice,
        fieldOffsetDiscountType,
        fieldOffsetDiscountValue1,
        fieldOffsetDiscountValue2,		// 15
        fieldOffsetDiscountValue3,
        fieldOffsetDiscountLabel1,
        fieldOffsetDiscountLabel2,
        fieldOffsetDiscountLabel3,
        fieldOffsetTareAValue,			// 20
        fieldOffsetProportionalTare,
        fieldOffsetFixedWeight,
        fieldOffsetShelfLifeDays,
        fieldOffsetShelfLifeHours,
        fieldOffsetProductLifeDays,		// 25
        fieldOffsetProductLifeHours,
        fieldOffsetPriceModifier,
        fieldOffsetProductReportingClassNum,
        fieldOffsetLabelTypeNum1,       // 29
        fieldOffsetLabelRotation1,		// 30
        fieldOffsetLabelTypeNum2,
        fieldOffsetLabelRotation2,
        fieldOffsetLabelTypeNum3,
        fieldOffsetLabelRotation3,
        fieldOffsetBackDateDays,		// 35
        fieldOffsetPoints,
        fieldOffsetGraphicName1,
        fieldOffsetGraphicName2,
        fieldOffsetGraphicName3,
        fieldOffsetGraphicName4,		// 40
        fieldOffsetLabelTypeWidth1,				//	The width in inches or mm
        fieldOffsetLabelTypeLength1,			//	The length in inches or mm
        fieldOffsetLabelTypeWidth2,				//	The width in inches or mm
        fieldOffsetLabelTypeLength2,			//	The length in inches or mm
        fieldOffsetLabelTypeWidth3,		// 45	//	The width in inches or mm
        fieldOffsetLabelTypeLength3,			//	The length in inches or mm
        fieldOffsetByCountForced,
        fieldOffsetTareAForced,
        fieldOffsetFractionalPriceUnit,
        fieldOffsetPoundsForAmount,		// 50
        fieldOffsetUnitPriceForced,
        fieldOffsetTareType,
        fieldOffsetApplySecurity,
        fieldOffsetDontPrintPackedOn,
        fieldOffsetDontPrintSellBy,		// 55
        fieldOffsetDontPrintBestBefore,
        fieldOffsetLabelPlacement1,
        fieldOffsetLabelPlacement2,
        fieldOffsetLabelPlacement3,
        fieldOffsetFractionalPrice,		// 60
        fieldOffsetForcedFractionalPrice,
        fieldOffsetPoundsForPrice,
        fieldOffsetForcedPoundsFor,
        fieldOffsetSellByDate,
        fieldOffsetSellByTime,			// 65
        fieldOffsetBestBeforeDate,
        fieldOffsetBestBeforeTime,
        fieldOffsetHasNutrifactsFlag,
        fieldOffsetIntakeMode,
        fieldOffsetLabelHasFreqShopper,	// 70		//	One of the labels for this product has Frequent shopper fields
        fieldOffsetProductGraphicName,
        fieldOffsetProductName1,
        fieldOffsetProductName2,
        fieldOffsetProdCategoryNum,
        fieldOffsetProdSpecialNum,		// 75
        fieldOffsetProdCoolCategoryNum,
        fieldOffsetProdCoolPretextNum,
        fieldOffsetProdCoolDefaultTextNum,
        fieldOffsetProdCoolTextNum,
        fieldOffsetProdCoolShortlistNum,		// 80
        fieldOffsetProdCoolVerifyForced,
        fieldOffsetProdCOOLTrackingText,
        fieldOffsetProdCoolRequired,
        fieldOffsetKeyboardNetWeight,
        fieldOffsetAcceptKbdNetWeight,		   // 85		//	One of the labels for this product has the FORCED_KB_WEIGHT field.
        fieldOffsetNumOfLabelsForced,
        fieldOffsetNumOfLabels,
        fieldOffsetProductionPlanningType,
        fieldOffsetForcedBackDateDays,
        fieldOffsetExpirationDaysForced,       // 90
        fieldOffsetExpirationDays,
        fieldOffsetEANFormatString,
        fieldOffsetEAN128CFormatString,							// Product Defaults only
        fieldOffsetGS1DatabarGTIN,
        fieldOffsetGS1DatabarAI,               // 95
        fieldOffsetBackDateDate,
        fieldOffsetPortions,
        fieldOffsetShowPortions,
        fieldOffsetBuildAMealSideItemsGroupNum,
        fieldOffsetBuildAMealSideItemsAreFree,  // 100
        fieldOffsetBuildAMealNumberOfSidesIncluded,
        fieldOffsetItemIsBuildAMealSideItem,     // Application internal flag only, no external access
        fieldOffsetCutTestMeatSpecSheetGroupNum, // numbers start matching here now. were offset by 1 before now
        fieldOffsetTareBValue,
        fieldOffsetQRCodeFormatString,			// 105
        fieldOffsetDataMatrixFormatString,
        fieldOffsetMinWeight,
        fieldOffsetMaxWeight,
        fieldOffsetMinPrice,
        fieldOffsetMaxPrice,                    // 110

        fieldOffsetDefaultLabelTypeNum1,
        fieldOffsetDefaultLabelTypeNum2,
        fieldOffsetDefaultLabelTypeNum3,

        fieldOffsetManualLabelTypeNum1,
        fieldOffsetManualLabelTypeNum2,         // 115
        fieldOffsetManualLabelTypeNum3,

        fieldOffsetPrepackLabelTypeNum1,
        fieldOffsetPrepackLabelTypeNum2,
        fieldOffsetPrepackLabelTypeNum3,

        fieldOffsetProdEntryLabelTypeNum1,      // 120
        fieldOffsetProdEntryLabelTypeNum2,
        fieldOffsetProdEntryLabelTypeNum3,

        fieldOffsetSelfServLabelTypeNum1,
        fieldOffsetSelfServLabelTypeNum2,
        fieldOffsetSelfServLabelTypeNum3,       // 125

        fieldOffsetPick5LabelTypeNum1,
        fieldOffsetPick5LabelTypeNum2,
        fieldOffsetPick5LabelTypeNum3,

        fieldOffsetOnlineOrderingLabelTypeNum1,
        fieldOffsetOnlineOrderingLabelTypeNum2, // 130 
        fieldOffsetOnlineOrderingLabelTypeNum3,

        fieldOffsetShelfLifeMinutes,
        fieldOffsetProductLifeMinutes,

        fieldOffsetLabelStockRotation1,
        fieldOffsetLabelStockRotation2,        // 135
        fieldOffsetLabelStockRotation3,

        ProductFieldOffsetsMax
    };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////// Rando Enum lists not tied to devices and events //////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class ProductType
{
public:
    // from Product.h - usage: ProductType::PRODUCTTYPE_RANDOMWEIGHT
    enum ProductTypeOffsets
    {
        PRODUCTTYPE_UNKNOWN,
        PRODUCTTYPE_RANDOMWEIGHT,
        PRODUCTTYPE_FIXEDWEIGHT,
        PRODUCTTYPE_BYCOUNT,
        PRODUCTTYPE_FLUIDOUNCE,
        PRODUCTTYPE_INVALID
    };
};

class ScaleModel
{
public:
    // from scaleModel.h
    enum ScaleModelOffsets
    {
        ScaleModel_Unknown = 0,
        ScaleModel_ESS,             // self serv
        ScaleModel_EPP,             // prepack wrapper
        ScaleModel_ESC,             // counter scale
        ScaleModel_endOfKnownModels
    };
};

class SelfServicePrintOption
{

    // All of these values (unless I made a mistake) represent the combo boxes on the self service tab in Normal Scale Ops

public:
    enum SelfServicePrintOptionOffsets
    {
        SelfServOption_PrintToLabel,                    // immediately print label - standard default
        SelfServOption_PrintToScreen,                   // print entire label on 10071 to the screen for scan (Amazon GO)
        SelfServOption_PrintToScreenAndLabel,           // print entire label on 10071 to the screen and add a print button to print
        SelfServOption_PrintToScreenAndBarcode,         // print barcode ONLY on 10072 to the screen with a print button (Hannaford)
        SelfServOption_PrintQRCodeOnlyToScreen,         // print QR Code ONLY on 10072 to screen (Walmart CA and Walmart US Weigher Only)
        MaxSelfServPrintOptions
    };

    enum SelfServiceStartOptionOffsets
    {
        SelfServOption_StartGreenGO,                    // Big green button (in combo its 2nd item in the list, index 1)
        SelfServOption_StartNone,                       // goes straight to item entry keypad or flashkeys (in combo its 1st item in the list, index 0)
        SelfServOption_StartWeightRequired,             // 3 images add weight to continue (unless clickable)
        SelfServOption_StartPlaylists,                  // display playlists wait for weight (unless clickable)
        SelfServOption_StartOperLogin,                  // force operators to log in if you want to use self service (pickers)
        MaxSelfServStartOptions
    };

    enum SelfServiceSearchOptionOffsets
    {
        SelfServOption_CharByChar,                      // character by character
        SelfServOption_AtoZ,                            // A-Z categories with best sellers (until you search)
        SelfServOption_SearchNone,                      // This hides the Search button/menu option
        MaxSelfServSearchOptions
    };

    enum SelfServiceItemEntryOptionOffsets
    {
        SelfServOption_LookupBestSellers,               // Best Sellers (unless including keypad)
        SelfServOption_LookupCategories,                // Categories (unless including keypad)
        SelfServOption_LookupCategoriesShowKeypad,      // Categories show Keypad button (unless including keypad)
        SelfServOption_LookupFixedFlashkeys,            // Fixed Row and Columns
        SelfServOption_LookupKeypadOnly,                // keypad only (red apple)
        MaxSelfServItemEntryOptions
    };

    enum SelfServicePrintFinishedOptionOffsets
    {
        SelfServOption_AfterPrintVideo,                // display a video after printing indicating where to pull the label
        SelfServOption_AfterPrintImage,                // display a static image after printing indicating where to pull the label
    };

    enum SelfServiceEnterKey
    {
        Action_Enter,                                   // Enter key can look up a PLU or enter a ByCount
        Action_Print,                                   // Enter key acts like a Print key, still the Enter key though
        Action_LookupOperator,                          // Enter key looks like a login key, still the Enter key though
        Max_Actions
    };

};

class FixedRowAndCol
{
public:
    // from Product.h - usage: ProductType::PRODUCTTYPE_RANDOMWEIGHT
    enum ProductTypeOffsets
    {
        ROWCOL_UNKNOWN,
        ROWCOL_TOP_ROW,
        ROWCOL_LEFT_COL,
        ROWCOL_RIGHT_COL,
        ROWCOL_BOT_ROW,
    };
};

class OriginalScreen
{
public:
    // from scaleModel.h
    enum OriginalScreenOffsets
    {
        SCREEN_UNKNOWN = 0,
        CUT_TEST_HISTORY_SCREEN,            // cut test history
        CUT_TEST_COMMENT_SCREEN,            // cut test comments screen
        TEXT_EDITOR_GCD_SCREEN,             // Supervisor Customer Display
        TEXT_EDITOR_SELF_SERV_SCREEN,       // Supervisor Normal Scale Ops Self Serv Screen
        endOfOriginalScreens                // fin
    };
};

class CutTestOption
{
public:
    // from scaleModel.h
    enum CutTestOptionOffsets
    {
        CutTestOption_None = 0,
        CutTestOption_Default,             // Original
        CutTestOption_Option1,             // New and improved with validation,history,suspend,delete
        CutTestOption_endOfCutTestOptions
    };
};

class CutTestState
{
public:
    // from scaleModel.h
    enum CutTestStateOffsets
    {
        CutTestState_None = 0,
        CutTestState_Begin,
        CutTestState_End,
        CutTestState_endOfCutTestStates
    };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
////////////////////////////////// ERRORS  ////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//ClassNameError::Enum
// Weigher - 1000
class WeigherDevError
{
public:
    enum WeigherDevErrorOffsets
    {
        WeigherErrUnknownError = Weigher * DEVICE_ERROR_OFFSET,
        WeigherErrCommLoss,
        WeigherErrNegativeGrossWeight,
        WeigherErrOverGrossWeightLimit,
        WeigherErrScaleWarmup,
        WeigherErrRezeroing,                    // 1005
        WeigherErrZeroOutOfMaintZone,
        WeigherErrZeroOutOfMaintZoneStatus,
        WeigherErrRemoveWeight,
        WeigherErrSelectWeightsType,
        WeigherErrPressCalibrateSwitch,         // 1010
        WeigherErrDeltaChangeTooBig,
        WeigherErrCalibrationSuccessful,
        WeigherErrNotAttached,
        WeigherErrScaleNotZero,
        WeigherErrInvalidAddress,               // 1015
        WeigherErrManagerDeleted,
        WeigherErrTareDisabled,
        WeigherErrConfigOutOfRange,
        WeigherErrCantRezeroNegativeGrossWeight,
        WeigherErrRemoteConfigurationStarted,   // 1020
        WeigherErrOutOfCalibrateTiltRange,
        WeigherErrValMaxEepCommFailure,
        WeigherErrValMaxEepChecksumFailure,
        WeigherErrValMaxEepAcclCommFailure,
        WeigherErrValMaxTiltReached,            // 1025
        WeigherErrUnderMinimumWeight,

        // Self Serve Errors
        ssWeigherErrCommLoss                      =   WeigherErrCommLoss  + SELF_SERVE_ERROR_OFFSET,                       // = 1501,
        ssWeigherErrNegativeGrossWeight           =   WeigherErrNegativeGrossWeight  + SELF_SERVE_ERROR_OFFSET,            // = 1502     REZERO then OK
        ssWeigherErrOverGrossWeightLimit          =   WeigherErrOverGrossWeightLimit  + SELF_SERVE_ERROR_OFFSET,           // = 1503
        ssWeigherErrZeroOutOfMaintZoneStatus      =   WeigherErrZeroOutOfMaintZoneStatus  + SELF_SERVE_ERROR_OFFSET,       // = 1507,
        ssWeigherErrDeltaChangeTooBig             =   WeigherErrDeltaChangeTooBig   + SELF_SERVE_ERROR_OFFSET,             // = 1511,
        ssWeigherErrNotAttached                   =   WeigherErrNotAttached   + SELF_SERVE_ERROR_OFFSET,                   // = 1513,
        ssWeigherErrCantRezeroNegativeGrossWeight =   WeigherErrCantRezeroNegativeGrossWeight + SELF_SERVE_ERROR_OFFSET,   // = 1519,
        ssWeigherErrUnderMinimumWeight            =   WeigherErrUnderMinimumWeight + SELF_SERVE_ERROR_OFFSET,              // = 1526,

        WeigherDevErrLastError
    };
};

// Operate Mode 4 - 4000
class OperationModeError
{
public:
    enum OperationModeErrorOffsets
    {
        NoErrorSource = OperationMode * DEVICE_ERROR_OFFSET,
        WrapperPressStart,
        WrapperIdleError,
        WrapperGhostPackage,
        WrapperOverWeight,
        PrinterOverflowFirstPrinter,
        PrinterOverflowSecondPrinter,
        PrinterOverflowThirdPrinter,
        InsertServiceKey,
        LaPackageOverrun,
        WrapperTestEncoderWindow_1_Failed,
        WrapperTestEncoderWindow_2_Failed,
        WrapperTestEncoderWindow_3_Failed,
        EvolWrapperHoming,
        ProductionEntryProductsEntered,
        EvolWrapperCalPlacePackage,
        PkgDischargeErrorFirstPrinter,
        PkgDischargeErrorSecondPrinter,
        PkgDischargeErrorThirdPrinter,
        ProductRequiredToWrap,			// HA-65
        BuildingAirPressure
    };
};

// Printer 7 - 7000
class PrinterErrors
{
public:
    enum PrinterErrorsOffsets
    {
        PrinterErrUnknownError = Printer * DEVICE_ERROR_OFFSET,
        PrinterErrCommLoss,
        PrinterErrManagerDeleted,
        PrinterErrHeadUp,
        PrinterErrLabelSizing,
        PrinterErrTakeLabel,                // 7005
        PrinterErrTakeLabelBeep,
        PrinterErrLabelJammed,
        PrinterErrLabelMissing,
        PrinterErrOutOfMedia,
        PrinterErrCassetteMissing,          // 7010
        PrinterErrHardwareError,
        PrinterErrThermalOverload,
        PrinterErrThermalShutdown,
        PrinterErrInvalidTypeSetup,
        PrinterErrMSDisengage,              // 7015
        PrinterErrMSSituateLabelStock,
        PrinterErrMSEngage,
        PrinterErrMSLabelStockTaut,
        PrinterErrMSCalibrating,
        PrinterErrMSComplete,               // 7020
        PrinterErrLTClearSensor,
        PrinterErrLTCalibrating,
        PrinterErrLTComplete,
        PrinterErrSLAIsOff,
        PrinterErrSLAOutOfLabels,           // 7025
        PrinterErrHeadUpForEpp,
        PrinterErrNoSpaceAllowedInName,
        PrinterErrInvalidCharInName,
        PrinterErrNoSizingDueToContinuous,
        PrinterErrSizedContinuousOK,        // 7030
        PrinterErrLabelCutterJammed,
        PrinterErrLabelCutterInterlockOpen,
        PritnerErrLabelCutterTimedOut,
        PrinterErrProductionEntryInterlockOpen,
        PrinterErrLowStock,                 // 7035
        PrinterErrOutOfStock,               // 7036
        PrinterErrFSSLabelCutterInterlockOpen,
        PrinterErrFSSLabelDoorInterlockOpen,

        ssPrinterErrCommLoss            = PrinterErrCommLoss + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrManagerDeleted      = PrinterErrManagerDeleted + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrHeadUp              = PrinterErrHeadUp + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrLabelSizing         = PrinterErrLabelSizing + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrTakeLabel           = PrinterErrTakeLabel + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrTakeLabelBeep       = PrinterErrTakeLabelBeep + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrLabelJammed         = PrinterErrLabelJammed + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrLabelMissing        = PrinterErrLabelMissing + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrOutOfMedia          = PrinterErrOutOfMedia + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrCassetteMissing     = PrinterErrCassetteMissing + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrHardwareError       = PrinterErrHardwareError + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrThermalOverload     = PrinterErrThermalOverload + SELF_SERVE_ERROR_OFFSET,
        ssPrinterErrThermalShutdown     = PrinterErrThermalShutdown + SELF_SERVE_ERROR_OFFSET,

        PrinterErrorsLastError

    };
};

// Product 100 - 100000
class SessionDevError
{
public:
    enum SessionDevErrorOffsets
    {
        SessionDevErrUnknownError = SessionDev * DEVICE_ERROR_OFFSET,
        SessionDevErrNotPrimary,
        SessionDevErrCommLoss,
        SessionDevAllFilesDeleted,
        SessionDevBootableCFInProgress,
        SessionDevBootableCFComplete,               // 5
        SessionDevBootableCFError,
        SessionDevReqNewDirName,
        SessionDevErrDuplicateDirectory,
        SessionDevDeleteAllFilesInProgress,
        SessionDevChangingLanguages,                // 10
        SessionDevDBCompressStarting,
        SessionDevEnteringMonitorMode,
        SessionDevRemoveThumbDrive,
        SessionDevDateTimeReadError,
        SessionDevDeleteAllFilesFailed,             // 15
        SessionDevBootableCFErrorSuperKeyPresent,
        SessionDevBrowserBootupStarted,
        SessionDevBrowserBootupCompleted,           //	The always famous 100018
        SessionDevTimedWrSealerBeltOff,
        SessionDevTimedWrSealerBeltOn,              // 20
        SessionDevCommExistingBeforeShutdown,
        SessionDevShuttingdown,
        SessionDevShutdownComplete,
        SessionDevInactivityAutoLogout,
        SessionDevShutdownAreYouSure,				// 25
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
        SessionDevAccumulateItemsNotEnabled,        // 40
        SessionDevAccumulateItemsEmptyList,
        SessionDevAccumulateItemsItemRemoved,
        SessionDevAccumulateItemsUndefinedLabelType,
        SessionDevAccumulateItemsNoValidLabelType,
        SessionDevItemAddedToAccumulateItemsList,	// 45
        SessionDevAccumulateItemsMaxItemsExceeded,

        SessionDevRebootImmediate,
        SessionDevRemoveUSBDrive,
        SessionDevRemoteAccessProhibited,

        SessionBackingUpDatabase,					// 50 - confirmed 10-23-23

        SessionDevDBIsCorrupt,

        SessionDevDeleteAllFilesAlreadyInProgress,

        SessionDevAccumulateItemsFirstItemBarcodeFitError,
        SessionDevAccumulateItemsFirstItemBarcodeFitWarning,

        SessionDevBeginSelfService,                 // 55
        SessionDevSelfServInitAndLoading,
        SessionDevSearceFeatureDisabled,

        SessionDevToggleModeNotEnabled = (SessionDev * DEVICE_ERROR_OFFSET) + 58,
        SessionDevRemotePrintWarning,
        SessionDevPick5ActiveStatus,                // 60
        SessionDevRemoveUSBServiceKey,
        SessionDevOnlineOrderingActive,
        SessionDevOnlineOrderingEnabled,
        SessionDevWillNotDisableExtraDay,
        SessionDevSecLabelEnableToggleStatus,       // 65
        
        SessionDevErrorLastEror
    };
};

// Product 101 - 101000
class ProductDevError
{
public:
    enum ProductDevErrorOffsets
    {
        ProductErrUnknownError = ProductDev * DEVICE_ERROR_OFFSET,          // 101000
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
        ProductErrForcedFieldNotEntered,                    // self serv bycount dialog - you must first enter a ....
        ProductErrProductNumberForFlashkeyInvalid,
        ProductErrEANMaskNotSetup,
        ProductErrCannotSelectRandomWeightProduct,
        ProductErrOnlyForManualWeighMode,					// 101050 - confirmed 10-23-23
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
        ProductErrCannotPrintInStreamingLabelsMode,			// 101100 - confirmed 10-23-23
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

        ProductErrPrintPrintJobCancelViaUI,                 // 101161 - START Remote Print errors
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
        ProductErrDoNotPrintEmptyNutrifactsLabel,           // wont be used until SMART replacement merge. Just reserving spot since I was here
        ProductErrCannotStopCutTestNotStarted,
        ProductErrCannotSuspendCutTestNotStarted,           // 101175
        ProductErrExtraDayTimeIsZero,
        ProductErrExtraDayAlreadyEnabled,
        ProductErrSaveWithDoneButtonOnly,
        ProductDevWaitForLabelToPrintOn2P,					// 101179


        // Self Service Product Errors
        ProductErrSelfServUnknownError = 101500,            // 101500
        ProductErrSelfServ,
        ProductErrSelfServCannotChangeProduct,
        ProductErrSelfServCannotPrintLabel,
        ProductErrSelfServReportingClass,
        ProductErrSelfServLabelType,                        // 101505

        ssProductErr                            = ProductErr + SELF_SERVE_ERROR_OFFSET,					    //	Was ProductErrRecordNotFound
        ssProductErrCannotChangeProduct         = ProductErrCannotChangeProduct + SELF_SERVE_ERROR_OFFSET,
        ssProductErrCannotPrintLabel            = ProductErrCannotPrintLabel + SELF_SERVE_ERROR_OFFSET,
        ssProductErrReportingClass              = ProductErrReportingClass + SELF_SERVE_ERROR_OFFSET,
        ssProductErrLabelType                   = ProductErrLabelType + SELF_SERVE_ERROR_OFFSET,			//	101005
        ssProductErrGraphic                     = ProductErrGraphic + SELF_SERVE_ERROR_OFFSET,

        ProductErrPrintLastError
    };
};

class BackupRestoreDevError
{
public:
    enum BackupRestoreDevErrorOffsets
    {
        BackupRestoreUnknownError = BackupRestoreDev * DEVICE_ERROR_OFFSET,     // 109000

        BackupRestoreNoFileName,
        BackupRestoreInvalidFileName,
        BackupRestoreNoSlashAllowed,
        BackupRestoreNoSemicolonAllowed,
        BackupRestoreUndefined,                             // 109005
        BackupRestoreNoAsteriskAllowed,
        BackupRestoreNoQuestionMarkAllowed,
        BackupRestoreNoQuoteAllowed,
        BackupRestoreNoGreaterThanAllowed,
        BackupRestoreNoPlusMinusAllowed,                    // 109010
        BackupRestoreNoSeparatorAllowed,
        BackupRestoreNoSpaceAllowed,
        BackupRestoreNoDataSelected,
        BackupRestoreDeletingProductData,
        BackupRestoreDeletingResources,                     // 109015
        BackupRestoreDeletingOperators,
        BackupRestoreDeletingSelfServeData,
        BackupRestoreDeletingCOOLData,
        BackupRestoreDeletingSoftkeyData,
        BackupRestoreFileWriteError,                        // 109020
        BackupRestoreCountingRecords,
        BackupRestoreSelfServeModified                      // 109022
    };
};


#endif // SCALECLASSES_H
