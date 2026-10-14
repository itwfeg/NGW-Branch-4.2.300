#ifndef COOLTOTALEVENT_H
#define COOLTOTALEVENT_H



#include "Event.h"
#include "DataTypeEvent.h"



namespace hfc
{


	// COOLTotal events
	typedef enum COOLTotalEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		ReadAllCOOLTotalsForUltima = 20,
		WriteAllCOOLTotalsForUltima,

		COOLTotalLastEvent
	} COOLTotalEvent_t;


	// COOLTotal errors
	typedef enum COOLTotalDevError
	{
		COOLTotalErrUnknownError = COOLTotalDev * DEVICE_ERROR_OFFSET,

		COOLTotalErrLast
	} COOLTotalDevError_t;


}



#endif // COOLTOTALEVENT_H
