/**
 * @file NGWAck.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message
 * Created on: Jan 5, 2016
 *
 **/
#include "NGWAck.h"

NGWAck::NGWAck( Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Wrapper, WrNGWAck, pHobThread )
{
	nodeId = 0;
	sourceId = 0;
	pad = 0;
	pad1 = 0;
}

NGWAck::NGWAck(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sourceId;
	for (unsigned int i = 0; i < sizeof(sourceId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pad;
	for (unsigned int i = 0; i < sizeof(pad); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pad1;
	for (unsigned int i = 0; i < sizeof(pad1); i++)
	{
		*pChar++ = buffer[j++];
	}

}

NGWAck::~NGWAck()
{

}

const char *NGWAck::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &sourceId;
	for (unsigned int i = 0; i < sizeof(sourceId); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &pad;
	for (unsigned int i = 0; i < sizeof(pad); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &pad1;
	for (unsigned int i = 0; i < sizeof(pad1); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}






