#ifndef DATATYPEEVENT_H
#define DATATYPEEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"


	// DataType Generated Events
	// These are to be consistent for all the HFC classes used in the application.
	// Making all the hfc datatypes consistent with these will happen over time.
	typedef enum DtEvent
	{
		DtRead,                // 0
		DtReadAll,
		DtWrite,               // 2
		DtWriteAll,
		DtDelete,              // 4
		DtDeleteAll,
		DtSave,                // 6
		DtRestore,
		DtFirst,               // 8
		DtLast,
		DtNext,                // 10
		DtPrevious,
		DtCreateNew,           // 12
		DtBatchApplyDelete,
		DtBatchApplyDeleteAll, // 14
		DtBatchApplyRestore,
		DtReadSingleField,     // 16
		DtWriteSingleField,
		DtSaveSingleField,     // 18
		DtUpdateStatus,
	}   DtEvent_t;

#endif // DATATYPEEVENT_H
