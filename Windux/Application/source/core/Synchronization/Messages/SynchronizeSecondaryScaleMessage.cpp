
#if defined( __linux__ )
    #include <string.h>
#endif

#include "SynchronizeSecondaryScaleMessage.h"


#include "SynchronizationDevEvent.h"



namespace synchronization
{


	SynchronizeSecondaryScaleMessage::SynchronizeSecondaryScaleMessage(int scaleId)
		: EvolMessage(SynchronizationDev, SynchronizeSecondaryScaleEvent), scaleId_(scaleId)
	{
	}


	SynchronizeSecondaryScaleMessage::SynchronizeSecondaryScaleMessage(const char* buffer)
		: EvolMessage(buffer)
	{
		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&scaleId_, buffer, sizeof(scaleId_));
	}


	SynchronizeSecondaryScaleMessage::~SynchronizeSecondaryScaleMessage()
	{
	}


	const char* SynchronizeSecondaryScaleMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&scaleId_, sizeof(scaleId_));

		return serializedData.data();
	}


}