#ifndef TRANSACTIONTYPEEVENT_H
#define TRANSACTIONTYPEEVENT_H



	#include "Event.h"
	#include "DataTypeEvent.h"



	// TransactionType events
	typedef enum TransactionTypeEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		TransactionTypeReadAllNames = 20,
		TransactionTypeWriteAllNames,
//		TransactionTypeSetCurrentType,
//		TransactionTypeSetCurrentRunModeType,

		TransactionTypeReadCurrentType = 30,
		TransactionTypeWriteCurrentType,
		TransactionTypeRefreshCurrentType,

		TransactionTypeLastEvent
	} TransactionTypeEvent_t;



	typedef enum TransactionTypeError
	{
		TransactionTypeErrUnknownError = TransactionTypeDev * DEVICE_ERROR_OFFSET,

		TransactionTypeErr,

		TransactionTypeErrLast
	} TransactionTypeError_t;



#endif // TRANSACTIONTYPEEVENT_H
