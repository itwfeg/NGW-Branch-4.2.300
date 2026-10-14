#ifndef DEVICEEVENT_H
#define DEVICEEVENT_H

	// This file contains all the events that occur in the Hobart Scales Projects
	#include "Event.h"
	#include "DataTypeEvent.h"

	// Device Generated Events - DeviceDev
/*
	typedef enum DeviceEvent
	{
	}   DeviceEvent_t;
*/

	typedef enum DeviceDevError
	{
		DeviceErrUnknownError = DeviceDev * DEVICE_ERROR_OFFSET,
		DeviceErrRecordNotFound
	}   DeviceDevError_t;

#endif // DEVICEEVENT_H
