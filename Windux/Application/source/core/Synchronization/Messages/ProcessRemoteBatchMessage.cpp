
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ProcessRemoteBatchMessage.h"


#include "SynchronizationDevEvent.h"



namespace synchronization
{


	ProcessRemoteBatchMessage::ProcessRemoteBatchMessage(int id)
		: EvolMessage(SynchronizationDev, ProcessRemoteBatchEvent), batchId_(id)
	{
	}


	ProcessRemoteBatchMessage::ProcessRemoteBatchMessage(const char* buffer)
		: EvolMessage(buffer)
	{
		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&batchId_, buffer, sizeof(batchId_));
	}


	ProcessRemoteBatchMessage::~ProcessRemoteBatchMessage()
	{
	}


	const char* ProcessRemoteBatchMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&batchId_, sizeof(batchId_));

		return serializedData.data();
	}


}