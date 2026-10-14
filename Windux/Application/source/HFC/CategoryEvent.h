#ifndef CATEGORYEVENT_H
#define CATEGORYEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// Category Generated Events - CategoryDev
	typedef enum CategoryEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		CategoryReadAllCatGraphicNames = 20,
		CategoryWriteAllCatGraphicNames,
		CategoryReadAllMainCategories,
		CategoryWriteAllMainCategories,
		CategoryChangeFeaturedCategory,

		CategoryReadSoftKeyList = 50,
		CategoryWriteSoftKeyList,
		CategoryReadSubCategoriesList,
		CategoryWriteSubCategoriesList
	}   CategoryEvent_t;

	typedef enum CategoryDevError
	{
		CategoryErrUnknownError = CategoryDev * DEVICE_ERROR_OFFSET,
		CategoryErrRecordNotFound,
		CategoryErrDeleteAreYouSure
	}   CategoryDevError_t;

#endif // CATEGORYEVENT_H
