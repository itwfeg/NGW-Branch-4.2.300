#ifndef CUTTESTTOTALDEVEVENT_H
#define CUTTESTTOTALDEVEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// CutTestTotals Generated Events - CutTestTotalDevEvent
	typedef enum CutTestTotalDevEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		ReadFilteredCutTestTotals = 20,
		WriteFilteredCutTestTotals,
		ReadCutTestReport,
		WriteCutTestReport,

		CutTestTotalLastEvent
	};

	typedef enum CutTestTotalDevError
	{
		CutTestTotalDevErrUnknownError = CutTestTotalDev * DEVICE_ERROR_OFFSET,
	};

#endif // CUTTESTTOTALDEVEVENT_H
