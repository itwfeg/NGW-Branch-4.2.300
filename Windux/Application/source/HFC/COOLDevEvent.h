#ifndef COOLDEVEVENT_H
#define COOLDEVEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// COOLDev Generated Events
	typedef enum COOLDevEvent
	{
		//	Since we don't use any of the DtEvents for COOL, I need to reuse the first set of 20
		COOLDevReadAllCategoryCountriesForUltima,	//	0 - To support the old Quantum format via Comm
		COOLDevWriteAllCategoryCountriesForUltima,	//		To support the old Quantum format via Comm
		COOLDevReadAllCategoryProdStepsForUltima,	//		To support the old Quantum format via Comm
		COOLDevWriteAllCategoryProdStepsForUltima,	//		To support the old Quantum format via Comm
		COOLDevReadAllShortlistsForUltima,			//		To support the old Quantum format via Comm
		COOLDevWriteAllShortlistsForUltima,			//	5 - To support the old Quantum format via Comm
		COOLDevReadAllCOOLTotalsForUltima,			//		To support the old Quantum format via Comm
		COOLDevWriteAllCOOLTotalsForUltima,			//		To support the old Quantum format via Comm
		COOLDevRestoreShortlistForUltima,			//		To support the old Quantum format via Comm
		COOLDevDeleteAllCountryTextForUltima,		//		To support the old Quantum format via Comm

		//	Country text
		COOLDevReadCountry						= 20,
		COOLDevReadAllCountries,
		COOLDevWriteCountry,
		COOLDevWriteAllCountries,
		COOLDevDeleteCountry,
		COOLDevDeleteAllCountries,
		COOLDevSaveCountry,						// 26
		COOLDevRestoreCountry,
		COOLDevFirstCountry,					// 28
		COOLDevLastCountry,
		COOLDevNextCountry,						// 30
		COOLDevPreviousCountry,
		COOLDevCreateNewCountry,				// 32
		COOLDevReadByCategoryOnlyCountries,
		COOLDevWriteCountriesByCategory,				// 34
		COOLDevAddCountryToCategory,
		COOLDevDeleteCountryFromCategory,		// 36
		COOLDevCreateNewCountryAndSave,
		COOLDevReadByAllAndByCategoryCountries,			// 38
		COOLDevCountryBatchApplyDelete,

		//	Production Step text
		COOLDevReadProductionStep				= 40,
		COOLDevReadAllProductionSteps,
		COOLDevWriteProductionStep,
		COOLDevWriteAllProductionSteps,
		COOLDevDeleteProductionStep,
		COOLDevDeleteAllProductionSteps,
		COOLDevSaveProductionStep,				// 46
		COOLDevRestoreProductionStep,
		COOLDevFirstProductionStep,				// 48
		COOLDevLastProductionStep,
		COOLDevNextProductionStep,				// 50
		COOLDevPreviousProductionStep,
		COOLDevCreateNewProductionStep,			// 52
		COOLDevReadByCategoryOnlyProductionSteps,
		COOLDevWriteProductionStepsByCategory,			// 54
		COOLDevAddProductionStepToCategory,
		COOLDevDeleteProductionStepFromCategory,// 56
		COOLDevCreateNewProductionStepAndSave,
		COOLDevReadByAllAndByCategoryProductionSteps,	// 58
		COOLDevProductionStepBatchApplyDelete,

		//	Pretext text
		COOLDevReadPretext						= 60,
		COOLDevReadAllPretexts,
		COOLDevWritePretext,
		COOLDevWriteAllPretexts,
		COOLDevDeletePretext,
		COOLDevDeleteAllPretexts,
		COOLDevSavePretext,						// 66
		COOLDevRestorePretext,
		COOLDevFirstPretext,					// 68
		COOLDevLastPretext,
		COOLDevNextPretext,						// 70
		COOLDevPreviousPretext,
		COOLDevCreateNewPretext,				// 72
		COOLDevReadByCategoryOnlyPretexts,
		COOLDevWritePretextsByCategory,					// 74
		COOLDevAddPretextToCategory,
		COOLDevDeletePretextFromCategory,		// 76
		COOLDevUnusedPretextEvent,
		COOLDevReadByAllAndByCategoryPretexts,			// 78
		COOLDevPretextBatchApplyDelete,

		//	COOL Category
		COOLDevReadCategory						= 80,
		COOLDevReadAllCategories,
		COOLDevWriteCategory,
		COOLDevWriteAllCategories,
		COOLDevDeleteCategory,
		COOLDevDeleteAllCategories,
		COOLDevSaveCategory,					// 86
		COOLDevRestoreCategory,
		COOLDevFirstCategory,					// 88
		COOLDevLastCategory,
		COOLDevNextCategory,					// 90
		COOLDevPreviousCategory,
		COOLDevCreateNewCategory,				// 92
		COOLDevReadAllCategoriesByCountry,
		COOLDevWriteAllCategoriesByCountry,		// 94
		COOLDevReadAllCategoriesByProdStep,
		COOLDevWriteAllCategoriesByProdStep,	// 96
		COOLDevReadAllCategoriesByPretext,
		COOLDevWriteAllCategoriesByPretext,		// 98
		COOLDevCategoryBatchApplyDelete,

		//	COOL Text
		COOLDevReadCoolText						= 100,
		COOLDevReadAllCoolText,
		COOLDevWriteCoolText,
		COOLDevWriteAllCoolText,
		COOLDevDeleteCoolText,
		COOLDevDeleteAllCoolText,
		COOLDevSaveCoolText,					// 106
		COOLDevRestoreCoolText,
		COOLDevFirstCoolText,					// 108
		COOLDevLastCoolText,
		COOLDevNextCoolText,					// 110
		COOLDevPreviousCoolText,
		COOLDevCreateNewCoolText,				// 112

		COOLDevShortlistReadAllCoolText,
		COOLDevShortlistWriteAllCoolText,		// 114
		COOLDevShortlistAddText,
		COOLDevShortlistDeleteText,		// 116
		COOLDevReadAllCoolTextForUltima,				// 117	- To support the old Quantum format via Comm
		COOLDevWriteAllCoolTextForUltima,				// 118	- To support the old Quantum format via Comm
		COOLDevCoolTextBatchApplyDelete,

		//	COOL Category Country
		COOLDevReadCategoryCountry = 140,
		COOLDevReadAllCategoryCountries,
		COOLDevWriteCategoryCountry,
		COOLDevWriteAllCategoryCountries,
		COOLDevDeleteCategoryCountry,
		COOLDevDeleteAllCategoryCountries,
		COOLDevSaveCategoryCountry,
		COOLDevRestoreCategoryCountry,
		COOLDevFirstCategoryCountry,
		COOLDevLastCategoryCountry,
		COOLDevNextCategoryCountry,
		COOLDevPreviousCategoryCountry,
		COOLDevCreateNewCategoryCountry,
		COOLDevBatchApplyDeleteCategoryCountry,

		//	COOL Category ProductionStep
		COOLDevReadCategoryProductionStep = 160,
		COOLDevReadAllCategoryProductionSteps,
		COOLDevWriteCategoryProductionStep,
		COOLDevWriteAllCategoryProductionSteps,
		COOLDevDeleteCategoryProductionStep,
		COOLDevDeleteAllCategoryProductionSteps,
		COOLDevSaveCategoryProductionStep,
		COOLDevRestoreCategoryProductionStep,
		COOLDevFirstCategoryProductionStep,
		COOLDevLastCategoryProductionStep,
		COOLDevNextCategoryProductionStep,
		COOLDevPreviousCategoryProductionStep,
		COOLDevCreateNewCategoryProductionStep,
		COOLDevBatchApplyDeleteCategoryProductionStep,

		//	COOL Category Pretext
		COOLDevReadCategoryPretext = 180,
		COOLDevReadAllCategoryPretexts,
		COOLDevWriteCategoryPretext,
		COOLDevWriteAllCategoryPretexts,
		COOLDevDeleteCategoryPretext,
		COOLDevDeleteAllCategoryPretexts,
		COOLDevSaveCategoryPretext,
		COOLDevRestoreCategoryPretext,
		COOLDevFirstCategoryPretext,
		COOLDevLastCategoryPretext,
		COOLDevNextCategoryPretext,
		COOLDevPreviousCategoryPretext,
		COOLDevCreateNewCategoryPretext,
		COOLDevBatchApplyDeleteCategoryPretext,

		//	COOL Shortlist
		COOLDevReadShortlist = 200,
		COOLDevReadAllShortlists,
		COOLDevWriteShortlist,
		COOLDevWriteAllShortlists,
		COOLDevDeleteShortlist,
		COOLDevDeleteAllShortlists,
		COOLDevSaveShortlist,
		COOLDevRestoreShortlist,
		COOLDevFirstShortlist,
		COOLDevLastShortlist,
		COOLDevNextShortlist,
		COOLDevPreviousShortlist,
		COOLDevCreateNewShortlist,

		COOLDevReadShortlistText = 220,
		COOLDevReadAllShortlistTexts,
		COOLDevWriteShortlistText,
		COOLDevWriteAllShortlistTexts,
		COOLDevDeleteShortlistText,
		COOLDevDeleteAllShortlistTexts,
		COOLDevSaveShortlistText,
		COOLDevRestoreShortlistText,
		COOLDevFirstShortlistText,
		COOLDevLastShortlistText,
		COOLDevNextShortlistText,
		COOLDevPreviousShortlistText,
		COOLDevCreateNewShortlistText
	} COOLDevEvent_t;


	typedef enum COOLDevError
	{
		COOLDevErrUnknownError = COOLDev * DEVICE_ERROR_OFFSET,
		COOLDevErrCategoryNotFound,
		COOLDevErrCountryNotFound,
		COOLDevErrProductionStepNotFound,
		COOLDevErrPretextNotFound,
	} COOLDevError_t;

#endif // COOLDEVEVENT_H
