/**
 * @file WeigherCat3DateTime.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief message for setting epoch date and time to weigher
 * Created on: July 1, 2016
 */
#include "WeigherCat3DateTime.h"
#include "WeigherEvent.h"
#include "Debug.h"
#include "Integer.h"

WeigherCat3DateTime::WeigherCat3DateTime(Hobart::CEvolMsgReceiver *pHobThread, UINT32 time ) : EvolMessage(Weigher, WgCat3DateTime, pHobThread)
{
	epochTime = time;
}

WeigherCat3DateTime::WeigherCat3DateTime(const char *buffer) : EvolMessage(buffer)
{
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &epochTime;
	for (unsigned int i = 0; i < sizeof(epochTime); i++)
	{
		*pChar++ = buffer[j++];
	}

}

WeigherCat3DateTime::~WeigherCat3DateTime()
{}

const char *WeigherCat3DateTime::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &epochTime;
	for (unsigned int i = 0; i < sizeof(epochTime); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}

void WeigherCat3DateTime::show()
{
	Debug::trace("WeigherCat3DateTime", "WeigherCat3DateTime: ");
	Debug::trace("WeigherCat3DateTime", "Epoch Time: " + Integer::toString(epochTime) );
}




