#ifndef CGICALLMANAGEREVENTS_H
#define CGICALLMANAGEREVENTS_H



#include "Event.h"



namespace comm
{


	// CGICallManager Handled Events - CGICallManagerDev
	typedef enum
	{
		InvalidCGIEvent,

		PerformCGICall,
		CGICallPerformed,
		ShutdownCGI,

		LastEvent
	} CGICallManagerEvent;


	// for ErrCGICallManagerDev
	typedef enum
	{
		ErrUnknownError = CGICallManagerDev * DEVICE_ERROR_OFFSET,

		UnableToSendCGICall,

		LastError
	} CGICallManagerDevError;


}



#endif // CGICALLMANAGEREVENTS_H
