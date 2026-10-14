/* Evolution Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef BACKUPRESTORE_H
#define BACKUPRESTORE_H

//active function states
typedef enum BRState
{
	BRIdleState,
	BRBackupState,
	BRRestoreState,
	BRProcessFileState,
	BRBadRecordScanState,
	BRIntranetState,
	BRCancelState,
	BRDoneState
} BRState_t;

//what we are currently working on "active function subStates"
typedef enum BRSubStates
{
	BRIdleSubState,                 	//	0

	BRProductSubState,
	BRProductDefaultSubState,
	BRClassSubState,
	BRNutriDataSubState,
	BRExpandedTextSubState,				//	5
	BRSpecialMessageSubState,
	BRProductNoteSubState,
	BRMarqueeTextSubState,
	BRProductText5SubState,
	BRProductText6SubState,				//	10
	BRProductText7SubState,
	BRProductText8SubState,
	BRProductText9SubState,

	BRGraphicsSubState,
	BRFontsSubState,					//	15
	BRLabelTypesSubState,
	BRLabelTextNamesSubState,

	BRStoreDataSubState,
	BRScaleConfigSubState,

	BROperatorDataSubState,				//	20
	BROperatorNoteSubState,
	BROperatorChangesSubState,

	BRWrapperSettingsSubState,

	BRProductTotalsSubState,
	BRClassTotalsSubState,					//	25
	BROperatorTotalsSubState,
	BRHourlyTotalsSubState,
	BRCOOLTotalsSubState,
	BRTransactionsSubState,
	BRReadClearTransactionsSubState,		//	30
	BRTransactionTypeSubState,

	BRCategoriesSubState,
	BRPresentationMethodsSubState,
	BRSpecialsSubState,
	BRStreamingGraphicsSubState,    		//	35
	BRProductStatisticsSubState,

	BRCategoryGraphicsSubState,
	BRProductGraphicsSubState,
	BRSpecialsGraphicsSubState,
	BRStreamingGraphicsGraphicsSubState,	//	40
	BRSoftkeyGraphicsSubState,

	BREmailAlertsSubState,

	BRCOOLCategorySubState,
	BRCOOLCountrySubState,
	BRCOOLProductionStepSubState,   		//	45
	BRCOOLPretextSubState,
	BRCOOLTextSubState,
	BRCOOLShortlistSubState,

	BRCOOLCategoryCountrySubState,
	BRCOOLCategoryProductionStepSubState,	//	50
	BRCOOLCategoryPretextSubState,

	BRCOOLPretextUltimaSubState,
	BRCOOLTextUltimaSubState,
	BRCOOLShortlistUltimaSubState,
	BRCOOLCategoryCountryUltimaSubState,    //	55
	BRCOOLCategoryProductionStepUltimaSubState,
	BRCOOLTotalUltimaSubState,

	BRSoftkeyGroupSubState,
	BRSoftkeyStyleSubState,
	BRSoftkeyFeatureSubState,				//	60
	BRSoftkeySubState,

	BRExpirationLabelSubState,

	BRScalesSubState,
	BRBatchesSubState,
	BRSynchronizationChangesSubState,		//	65

	BRSynchronizeConfigSubState,

	BRRecallSubState,

	BRCutTestTotalSubState,
	BRCuttingTestSubState,

	BRUltimaHeaderSubState,					//	70
	BRUltimaFooterSubState,

	BRRestoreSubState,

	BRCompressingDB,
	BRAfterCompressingDB,

	BRWrapperSettingsRequestSubState,		//	75

	BRDevicesSubState,

	BRProductTextCfgSubState,
	BRBarcodeScannerSubState,

	BRCancelSubState,

	BRScaleStatusSubState,
	BRScaleStatisticsSubState,				//	80

	BRPrinterSettingsSubState,

	BRProcessFileSubState,
	BRExecuteProcessFileSubState,

	BRWeigherSettingsSubState,
	BRDeptSubState,

	BRDoneSubState							//	84
} BRSubStates_t;

//tells the BackupRestoreManager what to backup or restore
typedef enum BRSelections
{
	BRPluData,
	BRResources,
	BROperatorData,
	BRStoreData,
	BRScaleConfig,
	BRWrapperSettings,
	BRSoftkeyData,
	BRCOOLData,
	BRSelfServeData,
	BRSynchronizeConfig
} BRSelections_t;

// Tells the UI what file we are working on
// The UI depends on these enums being in the same order listed below.  If you want to change the order,
// or insert one in the list, be sure to consult shared/SessionHandler/screens/BackupRestore.cpp
typedef enum BRGroupIndicator
{
	BRGIPlu = 1,
	BRGIResources,
	BRGIOperatorData,
	BRGIStoreData,
	BRGIScaleConfig,			// 5
	BRGIWrapperSettings,
	BRGISoftkey,
	BRGICOOL,
	BRGISelfServe,
	BRGITotals,					// 10
	BRGISchronizeConfig
} BRGroupIndicator_t;

// Tells the UI what file we are working on
// The UI depends on these enums being in the same order listed below.  If you want to change the order,
// or insert one in the list, be sure to consult shared/SessionHandler/screens/BackupRestore.cpp
typedef enum BRFileIndicator
{
	BRFIUnknown = 0,

//	BRFIPlu = 1,
	BRFIProducts = 1,
	BRFIClasses,
	BRFINutriFacts,
	BRFIExpandedText,
	BRFISpecialMessages,		// 5
	BRFIProductNotes,
	BRFIMarqueeText,
	BRFIProductText5,
	BRFIProductText6,
	BRFIProductText7,			// 10
	BRFIProductText8,
	BRFIProductText9,
	BRFITransactionTypeNames,

//	BRFIResources,
	BRFIFonts,
	BRFIUserLabelGraphics,		// 15
	BRFILabelTypes,
	BRFILabelTextNames,

//	BRFIStoreData,
	BRFIStData,

//	BRFIOperatorData,
	BRFIOpData,
	
// BRFIScaleConfig,
	BRFIConfigData,				// 20

//	BRFIWrapperSettings,
	BRFIWrSettings,

	BRFICategories,
	BRFIPresentationMethods,
	BRFISpecials,
	BRFIStreamingGraphics,		// 25
	BRFIProductStatistics,

	BRFIOpNote,
	BRFIOpChanges,
	BRFIProductDefaults,

	BRFIProductTotals,			// 30
	BRFIClassTotals,
	BRFIOperatorTotals,
	BRFIHourlyTotals,
	BRFITransactions,

	BRFIProductGraphics,		// 35
	BRFICategoryGraphics,
	BRFISpecialsGraphics,
	BRFIStreamingGraphicsGraphics,
	BRFISoftkeyGraphics,

	BRFIEmailAlerts,			// 40

	BRFICOOLCategory,
	BRFICOOLCountry,
	BRFICOOLProductionStep,
	BRFICOOLPretext,
	BRFICOOLText,				// 45
	BRFICOOLShortlist,
	BRFICOOLTotal,

	BRFISoftkeyStyle,
	BRFISoftkeyFeature,
	BRFISoftkey,				// 50
	BRFISoftkeyGroup,

	BRFICOOLCategoryCountry,
	BRFICOOLCategoryProductionStep,
	BRFICOOLCategoryPretext,

	BRFIExpirationLabels,		// 55

	BRFISynchronizeConfigData,

	BRFIRecall,

	BRFIDevice,
	BRFIProductTextCfg,
	BRFIBarcodeScanner,			// 60

	BRFICutTests,
	BRFIDepts

} BRFileIndicator_t;

//tell the BackupRestoreManager how to handle the restore
typedef enum BRMode
{
	BRIdle,
	BRMerge,
	BROverWrite
} BRMode_t;

typedef enum BRFormat
{
	BRNULL,
	BREvolution,		//Ultima's scale comm and Intranet format
	BRUltima,			//Ultima's disk format
	BRVersionIndependent
} BRFormat_t;
					   
#endif // BACKUPRESTORE_H
