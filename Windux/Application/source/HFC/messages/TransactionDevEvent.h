#ifndef TRANSACTIONDEVEVENT_H
#define TRANSACTIONDEVEVENT_H



	#include "Event.h"
	#include "DataTypeEvent.h"


	// Transaction events
	typedef enum TransactionEvent
	{
		// This data type uses the common data type events defined in DtEvent above.
		TransactionReadNonVoidGroup  = 20,
		TransactionWriteNonVoidGroup,
		TransactionVoidTransactions,

		TransactionReadDates         = 30,
		TransactionWriteDates,

		TransactionReadAndClear      = 40,
		ClearProductAndClassTotals,
		ClearOperatorTotals,
		ClearHourlyTotals,
		ClearCOOLTotals,
		ClearOperatorChanges,
		ClearAllTotals,           // This one doesn't take any configuration flags into consideration when clearing totals.
        ClearSupervisorChanges,
		TransactionLastEvent
	} TransactionEvent_t;


	// Transaction errors
	typedef enum TransactionDevError
	{
		TransactionErrUnknownError = TransactionDev * DEVICE_ERROR_OFFSET,

		TransactionErrLast
	} TransactionDevError_t;



#endif // TRANSACTIONDEVEVENT_H
