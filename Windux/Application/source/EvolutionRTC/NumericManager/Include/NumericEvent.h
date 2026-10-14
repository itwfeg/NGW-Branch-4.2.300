#ifndef NUMERICEVENT_H
#define NUMERICEVENT_H

#include "Event.h"

// Numeric Device Generated Events

typedef enum NmEvent
{ 
	NmWakeup, 
	NmConfig, 
	NmInquire, 
	NmReset, 
	NmStatus,
	NmEnable, 
	NmDisable, 
	NmMode, 
	NmTest,
	NmField,
	NmAnnuncicator,
	NmSysReqStatus = 13,
	NmReqWakeup = 32,
	NmReqInquire,
	NmKind,
	NmOperateMode
} NmEvent_t;

typedef enum NumericErrors
{
	NumericErrUnknownError = Numeric * DEVICE_ERROR_OFFSET,
} NumericErrors_t;

#endif // NUMERICEVENT_H
