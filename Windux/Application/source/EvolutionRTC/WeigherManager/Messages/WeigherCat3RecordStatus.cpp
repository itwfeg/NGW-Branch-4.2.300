/**
 * @file WeigherCat3RecordStatus.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Message provides status on record write
 * Created on: July 1, 2016
 */
#include "WeigherCat3RecordStatus.h"
#include "WeigherEvent.h"

WeigherCat3RecordStatus::WeigherCat3RecordStatus(Hobart::CEvolMsgReceiver *pHobThread, bool result) : EvolMessage(Weigher, WgCat3RecordStatus, pHobThread)
{
	status = result;
}

WeigherCat3RecordStatus::WeigherCat3RecordStatus(const char *buffer) : EvolMessage(buffer)
{
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherCat3RecordStatus::~WeigherCat3RecordStatus()
{}

const char *WeigherCat3RecordStatus::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}


