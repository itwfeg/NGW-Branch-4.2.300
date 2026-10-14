#ifndef HOURLYTOTALEVENT_H
#define HOURLYTOTALEVENT_H



#include "Event.h"
#include "DataTypeEvent.h"



namespace hfc
{


	// HourlyTotal events
	typedef enum HourlyTotalEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		ReadFilteredHourlyTotals = 20,
		WriteFilteredHourlyTotals,

		HourlyTotalLastEvent
	} HourlyTotalEvent_t;


	// HourlyTotal errors
	typedef enum HourlyTotalDevError
	{
		HourlyTotalErrUnknownError = HourlyTotalDev * DEVICE_ERROR_OFFSET,

		HourlyTotalErrLast
	} HourlyTotalDevError_t;


}



#endif // HOURLYTOTALEVENT_H
