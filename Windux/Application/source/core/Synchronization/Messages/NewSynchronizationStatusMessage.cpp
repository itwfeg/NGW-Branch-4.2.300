
#if defined( __linux__ )
    #include <string.h>
#endif

#include "NewSynchronizationStatusMessage.h"


#include "SynchronizationDevEvent.h"



namespace synchronization
{


	NewSynchronizationStatusMessage::NewSynchronizationStatusMessage(SynchronizationDevError status)
		: EvolMessage(SynchronizationDev, NewSynchronizationStatusEvent), status_(status)
	{
	}


	NewSynchronizationStatusMessage::NewSynchronizationStatusMessage(const char* buffer)
		: EvolMessage(buffer)
	{
		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&status_, buffer, sizeof(status_));
	}


	NewSynchronizationStatusMessage::~NewSynchronizationStatusMessage()
	{
	}


	const char* NewSynchronizationStatusMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&status_, sizeof(status_));

		return serializedData.data();
	}


}