/**
 * @file WeigherCat3PageRequest.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for requesting CAT3 records from the weigher
 * Created on: July 1, 2016
 */
#include "WeigherCat3PageRequest.h"
#include "WeigherEvent.h"
#include "Debug.h"
#include "Integer.h"

WeigherCat3PageRequest::WeigherCat3PageRequest(Hobart::CEvolMsgReceiver *pHobThread, UINT8 number, UINT8 index) : EvolMessage(Weigher, WgCat3ReqPage, pHobThread)
{
	pageNumber = number;
	pageIndex = index;
}

WeigherCat3PageRequest::WeigherCat3PageRequest(const char *buffer) : EvolMessage(buffer)
{
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &pageNumber;
	for (unsigned int i = 0; i < sizeof(pageNumber); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &pageIndex;
	for (unsigned int i = 0; i < sizeof(pageIndex); i++)
	{
		*pChar++ = buffer[j++];
	}

}

WeigherCat3PageRequest::~WeigherCat3PageRequest()
{}

const char *WeigherCat3PageRequest::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &pageNumber;
	for (unsigned int i = 0; i < sizeof(pageNumber); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &pageIndex;
	for (unsigned int i = 0; i < sizeof(pageIndex); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}
void WeigherCat3PageRequest::show()
{
	Debug::trace("WeigherCat3PageRequest", "WeigherCat3PageRequest: ");
	Debug::trace("WeigherCat3PageRequest", "Page number: " + Integer::toString(pageNumber) );
	Debug::trace("WeigherCat3PageRequest", "Page index: " + Integer::toString(pageIndex) );
}

