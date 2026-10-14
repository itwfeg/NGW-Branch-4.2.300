#ifndef CUTTESTDEVEVENT_H
#define CUTTESTDEVEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	#include "DataTypeEvent.h"

	// CutTest Generated Events - CutTestDevEvent
	typedef enum CutTestDevEvent
	{
		// This data type uses the common data type events defined in DtEvent above.

		BeginCuttingTest		= 20,
		CancelCuttingTest,
		EndCuttingTest,				 // 
        CuttingTestDetailsRead,      // UI sends cutTestNum as first param     // UI sends this after it sends the CuttingTestComplete message
        CuttingTestDetailsWrite,     // chobart_d's response to the CuttingTestReadReport message
		CuttingTestHistoryRead,	     // UI doesn't send any data, chobart_d to use currently logged in operator to get data 
		CuttingTestHistoryWrite,     // chobart_d's response to the CuttingTestHistoryRead message
        SuspendCuttingTest,          // UI sends CutTest data
        DeleteCuttingTest,           // UI sends cutTestNum as first param.
        ResumeCuttingTest,           // UI sends cutTestNum as first param.
        PrintCuttingTest,            // UI sends ReportManagerDev 116, ReportEventType 0, RunTotalsReportType 12, FirstPrinter 0 
        SaveCuttingTestToUsb,        // UI sends cutTestNumber, filePath
        CuttingTestHistoryLogin, 
		CompleteCuttingTest,         // UI sends CutTest data      // UI sends after final comments entered
		CuttingTestSuspendSuccess,   // chobart_d sends to UI if successful suspension
		CuttingTestClear,            // sent to app to clear the current cut test. See cancelCuttingTest() in CutTestManager
		CuttingTestSaveToUSBFinished,

        CutTestDevLastEvent
	} CutTestDevEvent_t;

	typedef enum CutTestDevError
	{
		CutTestErrUnknownError = CutTestDev * DEVICE_ERROR_OFFSET,
		CutTestErrCantStartBecauseTooManyOperatorSuspended,
		CutTestErrCantStartBecauseTooManyOverallSuspended,
		CutTestErrSuspendedDueToInactivity,
		CutTestErrPrimalPLUNumberDoesntExist,
		CutTestErrCantDeleteCompletedCutTest,
		CutTestErrCantDeleteAlreadyDeletedCutTest,
		CutTestErrOperatorAccessLevelInsufficientForDelete,
		CutTestErrCanOnlyDeleteYourOwnCutTests,
		CutTestErrCanOnlyResumeYourOwnCutTests,
		CutTestErrFinishCutTestFirst,

		CutTestMaxErrors
	} CutTestDevError_t;

#endif // CUTTESTDEVEVENT_H
