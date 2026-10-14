
#if defined( __linux__ )
    #include <string.h>
#endif

#include "RegisterSecondaryScaleMessage.h"


#include "SynchronizationDevEvent.h"



namespace synchronization
{


	RegisterSecondaryScaleMessage::RegisterSecondaryScaleMessage(int scaleId)
		: EvolMessage(SynchronizationDev, RegisterSecondaryScaleEvent), scaleId_(scaleId)
	{
	}


	RegisterSecondaryScaleMessage::RegisterSecondaryScaleMessage(const char* buffer)
		: EvolMessage(buffer)
	{
		EvolMessage::serialize();
		buffer += getSerializedLength();

		// Now, for the derived class items
		memcpy(&scaleId_, buffer, sizeof(scaleId_));
	}


	RegisterSecondaryScaleMessage::~RegisterSecondaryScaleMessage()
	{
	}


	const char* RegisterSecondaryScaleMessage::serialize() const
	{
		EvolMessage::serialize();

		serializedData.append((char*)&scaleId_, sizeof(scaleId_));

		return serializedData.data();
	}


}