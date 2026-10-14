#ifndef PRODUCTTEXTEVENT_H
#define PRODUCTTEXTEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// ProductText Generated Events - ProductText5/6/7/8/9Dev
	typedef enum ProductTextEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		ProductTextNew = 20,
		ProductTextUse
	}   ProductTextEvent_t;

	typedef enum ProductText5DevError
	{
		ProductText5ErrUnknownError = ProductText5Dev * DEVICE_ERROR_OFFSET,
		ProductText5Err
	}   ProductText5DevError_t;

	typedef enum ProductText6DevError
	{
		ProductText6ErrUnknownError = ProductText6Dev * DEVICE_ERROR_OFFSET,
		ProductText6Err
	}   ProductText6DevError_t;

	typedef enum ProductText7DevError
	{
		ProductText7ErrUnknownError = ProductText7Dev * DEVICE_ERROR_OFFSET,
		ProductText7Err
	}   ProductText7DevError_t;

	typedef enum ProductText8DevError
	{
		ProductText8ErrUnknownError = ProductText8Dev * DEVICE_ERROR_OFFSET,
		ProductText8Err
	}   ProductText8DevError_t;

	typedef enum ProductText9DevError
	{
		ProductText9ErrUnknownError = ProductText9Dev * DEVICE_ERROR_OFFSET,
		ProductText9Err
	}   ProductText9DevError_t;


#endif // PRODUCTTEXTEVENT_H
