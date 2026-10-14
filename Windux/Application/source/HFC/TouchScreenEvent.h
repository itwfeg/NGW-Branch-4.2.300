#ifndef TOUCHSCREENEVENT_H
#define TOUCHSCREENEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"

	// touch screen Events
	typedef enum TouchScreenEvent
	{
		TsUnknown,
		TsStartCalibrate,
		TsCalibrateNextStep
	}   TouchScreenEvent_t;

	typedef enum TouchScreenError
	{
		TouchScreenErrorUnknownError = TouchScreen * DEVICE_ERROR_OFFSET,
		TouchScreenErrorNoStartCalibration
	}   TouchScreenError_t;

#endif // TOUCHSCREENEVENT_H
