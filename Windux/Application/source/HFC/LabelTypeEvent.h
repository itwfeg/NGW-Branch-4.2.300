#ifndef LABELTYPEEVENT_H
#define LABELTYPEEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	typedef enum LabelTypeDevError
	{
		LabelTypeDevUnknownError = LabelTypeDev * DEVICE_ERROR_OFFSET,
		LabelTypeErr					//	Label won't fit on label stock.
	}   LabelTypeDevError_t;

#endif // LABELTYPEEVENT_H
