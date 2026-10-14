#ifndef SPECIALSEVENT_H
#define SPECIALSEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	typedef enum SpecialsEvent
	{
		ReadAllGraphics = 20
	}   SpecialsEvent_t;

	typedef enum SpecialsDevError
	{
		SpecialsErrUnknownError = SpecialsDev * DEVICE_ERROR_OFFSET,
		SpecialsErrRecordNotFound
	}   SpecialsDevError_t;

#endif // SPECIALSEVENT_H
