#ifndef GENERICTEXTEVENT_H
#define GENERICTEXTEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// GenericText Generated Events - GenericTextDev
	typedef enum GenericTextEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		AppletViewerReadAll = 20,
		AppletViewerWriteAll
	}   GenericTextEvent_t;

	typedef enum GenericTextDevError
	{
		GenericTextDevUnknownError = GenericTextDev * DEVICE_ERROR_OFFSET,
		GenericTextErr
	}   GenericTextDevError_t;

#endif // GENERICTEXTEVENT_H
