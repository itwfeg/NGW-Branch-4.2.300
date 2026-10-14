#ifndef CLASSEVENT_H
#define CLASSEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// Class Generated Events - ClassDev
	typedef enum ClassEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		ClassUse = 20,
		ClassGetTree,
		ClassGetOrphanTree
	}   ClassEvent_t;

	typedef enum ClassDevError
	{
		ClassErrUnknownError = ClassDev * DEVICE_ERROR_OFFSET,
		ClassErr,
		ClassErrRecordNotFound,
		ClassErrReportingClassNotFound
	}   ClassDevError_t;

#endif // CLASSEVENT_H
