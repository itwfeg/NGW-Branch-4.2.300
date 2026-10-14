#ifndef ULTIMADISKEVENT_H
#define ULTIMADISKEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"

	typedef enum UltimaDiskEvent
	{
		UltimaDiskUnknown,
		HeaderWrite,
		FooterWrite
	} UltimaDiskEvent_t;

#endif // ULTIMADISKEVENT_H
