#ifndef CUTTESTTOTALDEVEVENT_H
#define CUTTESTTOTALDEVEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

namespace hfc
{

	// CutTestTotals Generated Events - CutTestTotalEvent
	typedef enum CutTestTotalEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		ReadCutTestReport		= 20,
		WriteCutTestReport,

		CutTestTotalLastEvent
	} CutTestTotalEvent_t;

	typedef enum CutTestTotalDevError
	{
		CutTestTotalDevErrUnknownError = CutTestTotalDev * DEVICE_ERROR_OFFSET,
	} CutTestTotalDevError_t;
}

#endif // CUTTESTTOTALDEVEVENT_H
