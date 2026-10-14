#ifndef IMPORTEVENT_H
#define IMPORTEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"
	typedef enum ImportEvent
	{
		UnknownImportEvent,
		ImportStart,				//tell session to perform a import
		ImportTerminate			//tell session to perform a restore
	} ImportEvent_t;

	typedef enum ImportError
	{
		ImportUnknownError = ImportDev * DEVICE_ERROR_OFFSET,

		ImportStartFonts,
		ImportStartGraphics,
		ImportComplete,
		ImportCompleteRquestReboot,
		ImportSrcDirDoesNotExist,
		ImportOkToRemoveTD
	} ImportError_t;

#endif // IMPORTEVENT_H
