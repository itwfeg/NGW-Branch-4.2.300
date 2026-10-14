#ifndef NUTRIFACTSEVENT_H
#define NUTRIFACTSEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// Nutrifacts Generated Events - NutrifactsDev
	typedef enum NutrifactsEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		NutrifactsNew = 20,
		NutrifactsUse
	}   NutrifactsEvent_t;

	typedef enum NutrifactsDevError
	{
		NutrifactsErrUnknownError = NutrifactsDev * DEVICE_ERROR_OFFSET,
		NutrifactsErr
	}   NutrifactsDevError_t;

#endif // NUTRIFACTSEVENT_H
