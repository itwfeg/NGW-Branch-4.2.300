#ifndef MARQUEETEXTEVENT_H
#define MARQUEETEXTEVENT_H

	// This file contains all the events that occur in the Access Scale Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	typedef enum MarqueeTextError
	{
		MarqueeTextErrUnknownError = MarqueeTextDev * DEVICE_ERROR_OFFSET,
		MarqueeTextErr
	}   MarqueeTextError_t;

#endif // MARQUEETEXTEVENT_H
