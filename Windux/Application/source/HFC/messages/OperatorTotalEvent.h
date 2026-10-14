#ifndef OPERATORTOTALEVENT_H
#define OPERATORTOTALEVENT_H



#include "Event.h"
#include "DataTypeEvent.h"



namespace hfc
{


	// OperatorTotal events
	typedef enum OperatorTotalEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		ReadFilteredOperatorTotals = 20,
		WriteFilteredOperatorTotals,

		OperatorTotalLastEvent
	} OperatorTotalEvent_t;


	// OperatorTotal errors
	typedef enum OperatorTotalDevError
	{
		OperatorTotalErrUnknownError = OperatorTotalDev * DEVICE_ERROR_OFFSET,

		OperatorTotalErrLast
	} OperatorTotalDevError_t;


}



#endif // OPERATORTOTALEVENT_H
