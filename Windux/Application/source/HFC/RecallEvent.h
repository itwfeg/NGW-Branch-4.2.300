#ifndef RECALLEVENT_H
#define RECALLEVENT_H

	#include "Event.h"
	#include "DataTypeEvent.h"

	// Recall Generated Events - RecallDev
	typedef enum RecallEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		NewRecallsExist = 20,
		AcknowledgeRecall,
		CheckForRecalls

	}   RecallEvent_t;

	typedef enum RecallDevError
	{
		RecallErrUnknownError = RecallDev * DEVICE_ERROR_OFFSET,
		RecallErrRecordNotFound
	}   RecallDevError_t;

#endif // RECALLEVENT_H
