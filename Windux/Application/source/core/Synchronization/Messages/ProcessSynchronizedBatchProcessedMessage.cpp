
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ProcessSynchronizedBatchProcessedMessage.h"


#include "SynchronizationDevEvent.h"



namespace synchronization
{


	ProcessSynchronizedBatchProcessedMessage::ProcessSynchronizedBatchProcessedMessage(int status, int batchId)
		: ProcessedMessage(SynchronizationDev, ProcessSynchronizedBatchProcessedEvent, status), batchId_(batchId)
	{
	}


	ProcessSynchronizedBatchProcessedMessage::ProcessSynchronizedBatchProcessedMessage(const char* buffer)
		: ProcessedMessage(buffer)
	{
		ProcessedMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&batchId_, buffer, sizeof(batchId_));
	}


	ProcessSynchronizedBatchProcessedMessage::~ProcessSynchronizedBatchProcessedMessage()
	{
	}


	const char* ProcessSynchronizedBatchProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}