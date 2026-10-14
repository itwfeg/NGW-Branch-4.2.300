#ifndef OPERATORCHANGEEVENT_H
#define OPERATORCHANGEEVENT_H



#include "Event.h"
#include "DataTypeEvent.h"



namespace hfc
{


	// OperatorChange events
	typedef enum OperatorChangeEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		ReadOperatorChangeSummary = 20,
		WriteOperatorChangeSummary,

		ReadOperatorChangesGroup = 30,
		WriteOpertorChangesGroup,

		OperatorChangeLastEvent
	} OperatorChangeEvent_t;


	// OperatorChange errors
	typedef enum OperatorChangeDevError
	{
		OperatorChangeErrUnknownError = OperatorChangesDev * DEVICE_ERROR_OFFSET,

		OperatorChangeErrLast
	} OperatorChangeDevError_t;


}



#endif // OPERATORCHANGEEVENT_H
