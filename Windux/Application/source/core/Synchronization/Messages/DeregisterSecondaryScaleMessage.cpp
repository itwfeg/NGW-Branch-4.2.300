
#if defined( __linux__ )
    #include <string.h>
#endif

#include "DeregisterSecondaryScaleMessage.h"


#include "SynchronizationDevEvent.h"



namespace synchronization
{


	DeregisterSecondaryScaleMessage::DeregisterSecondaryScaleMessage(int scaleId)
		: EvolMessage(SynchronizationDev, DeregisterSecondaryScaleEvent), scaleId_(scaleId)
	{
	}


	DeregisterSecondaryScaleMessage::DeregisterSecondaryScaleMessage(const char* buffer)
		: EvolMessage(buffer)
	{
		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&scaleId_, buffer, sizeof(scaleId_));
	}


	DeregisterSecondaryScaleMessage::~DeregisterSecondaryScaleMessage()
	{
	}


	const char* DeregisterSecondaryScaleMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&scaleId_, sizeof(scaleId_));

		return serializedData.data();
	}


}