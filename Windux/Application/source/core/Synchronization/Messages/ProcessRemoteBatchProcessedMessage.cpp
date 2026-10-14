
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ProcessRemoteBatchProcessedMessage.h"


#include "SynchronizationDevEvent.h"



namespace synchronization
{


	ProcessRemoteBatchProcessedMessage::ProcessRemoteBatchProcessedMessage(int status, int batchId)
		: ProcessedMessage(SynchronizationDev, ProcessRemoteBatchProcessedEvent, status), batchId_(batchId)
	{
	}


	ProcessRemoteBatchProcessedMessage::ProcessRemoteBatchProcessedMessage(const char* buffer)
		: ProcessedMessage(buffer)
	{
		ProcessedMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&batchId_, buffer, sizeof(batchId_));
	}


	ProcessRemoteBatchProcessedMessage::~ProcessRemoteBatchProcessedMessage()
	{
	}


	const char* ProcessRemoteBatchProcessedMessage::serialize() const
	{
		ProcessedMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}