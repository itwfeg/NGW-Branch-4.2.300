#ifndef PRODUCTTEXTCFGEVENT_H
#define PRODUCTTEXTCFGEVENT_H

	// This file contains all the events that occur in the Hobart Scales Projects
	#include "Event.h"
	#include "DataTypeEvent.h"

	// Device Generated Events - ProductTextCfgDev
/*
	typedef enum ProductTextCfgEvent
	{
	}   ProductTextCfgEvent_t;
*/

	typedef enum ProductTextCfgDevError
	{
		ProductTextCfgErrUnknownError = ProductTextCfgDev * DEVICE_ERROR_OFFSET,
		ProductTextCfgErrRecordNotFound
	}   ProductTextCfgDevError_t;

#endif // PRODUCTTEXTCFGEVENT_H
