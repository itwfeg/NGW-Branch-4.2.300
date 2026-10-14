#ifndef DEPTEVENT_H
#define DEPTEVENT_H

	// This file contains all the events that occur for Department related events
	#include "Event.h"
	#include "DataTypeEvent.h"

	typedef enum DeptEvent
	{
		ReadCurrentDept = 20,
		SetCurrentDept
	}   DeptEvent_t;

	typedef enum DeptDevError
	{
		DeptErrUnknownError = DeptDev * DEVICE_ERROR_OFFSET,
		DeptErrDefaultNotEditable,
		DeptErrRecordNotFound,
		DeptErrDeleteActiveDept,
		DeptErrDeletingRelatedRecords
	}   DeptDevError_t;

#endif // DEPTEVENT_H
