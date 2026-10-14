#ifndef RFIDEVENT_H
#define RFIDEVENT_H



#include "Event.h"
#include "DataTypeEvent.h"



// RFIDManager Handled Events - RFIDDev
typedef enum RFIDEvent
{
	RFIDCurrentID = 20,
	RFIDRequestCurrentID,
	RFIDCurrentStatus,
	RFIDRequestCurrentStatus
	
} RFIDEvent_t;


// for ErrNetworkConnectivityDev
typedef enum RFIDDevError
{
	RFIDManagerErrUnknownError = RFIDReader * DEVICE_ERROR_OFFSET,
	RFIDManagerErrLostComm
} RFIDDevError_t;




#endif // RFIDEVENT_H

