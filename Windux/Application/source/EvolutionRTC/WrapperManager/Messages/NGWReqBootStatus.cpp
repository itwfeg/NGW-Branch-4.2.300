/**
 * @file NGWRequestBootStatus.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Message requesting the CAN wrapper node boot status
 * Created on: Dec 9, 2015
 *
 **/
#include "NGWReqBootStatus.h"
#include "WrapperEvent.h"

NGWRequestBootStatus::NGWRequestBootStatus(Hobart::CEvolMsgReceiver *pHobThread, UINT8 id) : EvolMessage( Wrapper, WrNGWReqBootStatus, pHobThread )
{
	nodeId = id;
}

NGWRequestBootStatus::NGWRequestBootStatus(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		*pChar++ = buffer[j++];
	}
}

NGWRequestBootStatus::~NGWRequestBootStatus()
{

}

const char *NGWRequestBootStatus::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}


