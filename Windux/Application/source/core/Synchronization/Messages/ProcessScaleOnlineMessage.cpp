
#if defined( __linux__ )
    #include <string.h>
#endif

#include "ProcessScaleOnlineMessage.h"


#include "SynchronizationDevEvent.h"



namespace synchronization
{


	ProcessScaleOnlineMessage::ProcessScaleOnlineMessage(int scaleId)
		: EvolMessage(SynchronizationDev, ProcessScaleOnlineEvent), scaleId_(scaleId)
	{
	}


	ProcessScaleOnlineMessage::ProcessScaleOnlineMessage(const char* buffer)
		: EvolMessage(buffer)
	{
		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&scaleId_, buffer, sizeof(scaleId_));
	}


	ProcessScaleOnlineMessage::~ProcessScaleOnlineMessage()
	{
	}


	const char* ProcessScaleOnlineMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&scaleId_, sizeof(scaleId_));

		return serializedData.data();
	}


}