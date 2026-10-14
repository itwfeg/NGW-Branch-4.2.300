#ifndef EXPANDEDTEXTEVENT_H
#define EXPANDEDTEXTEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	typedef enum ExpandedTextDevError
	{
		ExpandedTextErrUnknownError = ExpandedTextDev * DEVICE_ERROR_OFFSET,
		ExpandedTextErr
	}   ExpandedTextDevError_t;

#endif // EXPANDEDTEXTEVENT_H
