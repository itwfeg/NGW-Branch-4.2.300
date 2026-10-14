#ifndef EVENTDEVEVENT_H
#define EVENTDEVEVENT_H



#include "Event.h"



namespace Hobart
{


	typedef enum EventEvent
	{
		EventEventUnknown,

		ProxyEvent,                     

		EventEventLast
	} EventEvent_t;


	typedef enum EventDevError
	{
		EventErrUnknownError = Event * DEVICE_ERROR_OFFSET,
		EventErrNoError,                    //  146001

		EventErrLast
	} EventDevError_t;


}



#endif /* EVENTDEVEVENT_H */
