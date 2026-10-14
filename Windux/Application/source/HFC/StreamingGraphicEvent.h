#ifndef STREAMINGGRAPHICEVENT_H
#define STREAMINGGRAPHICEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// StreamingGraphic Generated Events - StreamingGraphicDev
	typedef enum StreamingGraphicEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		StreamingGraphicReadAllGraphicNames = 20,
		StreamingGraphicWriteAllGraphicNames,
		StreamingGraphicSelectSlide,
		StreamingGraphicMoveSlideUp,
		StreamingGraphicMoveSlideDown,
		StreamingGraphicInsertNewSlide,				//	25
		StreamingGraphicDeleteSelectedSlide
	} StreamingGraphicEvent_t;

	typedef enum StreamingGraphicDevError
	{
		StreamingGraphicErrUnknownError = StreamingGraphicDev * DEVICE_ERROR_OFFSET,
		StreamingGraphicErrRecordNotFound
	} StreamingGraphicDevError_t;

#endif // STREAMINGGRAPHICEVENT_H
