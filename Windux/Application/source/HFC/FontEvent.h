#ifndef FONTEVENT_H
#define FONTEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// Font Generated Events - FontDev
	typedef enum FontEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		FontReadAllNames = 20,
		FontWriteAllNames
	}   FontEvent_t;

	typedef enum FontDevError
	{
		FontErrUnknownError = FontDev * DEVICE_ERROR_OFFSET,
		FontErr
	}   FontDevError_t;

#endif // FONTEVENT_H
