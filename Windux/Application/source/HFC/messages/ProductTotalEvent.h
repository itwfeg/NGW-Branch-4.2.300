#ifndef PRODUCTTOTALEVENT_H
#define PRODUCTTOTALEVENT_H



#include "Event.h"
#include "DataTypeEvent.h"



namespace hfc
{


	// ProductTotal events
	typedef enum ProductTotalEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		ReadFilteredProductTotals = 20,
		WriteFilteredProductTotals,

		ProductTotalLastEvent
	} ProductTotalEvent_t;


	// ProductTotal errors
	typedef enum ProductTotalDevError
	{
		ProductTotalErrUnknownError = ProductTotalDev * DEVICE_ERROR_OFFSET,

		ProductTotalErrLast
	} ProductTotalDevError_t;


}



#endif // PRODUCTTOTALEVENT_H
