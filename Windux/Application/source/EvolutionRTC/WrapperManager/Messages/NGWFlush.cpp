/**
 * @file NGWFlush.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message
 * Created on: Jan 5, 2016
 *
 **/
#include "NGWFlush.h"

NGWFlush::NGWFlush( Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Wrapper, WrNGWFlush, pHobThread )
{
	nodeId = 0;
	sourceId = 0;
}

NGWFlush::NGWFlush(const char *buffer) : EvolMessage(buffer)
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
}

NGWFlush::~NGWFlush()
{

}

const char *NGWFlush::serialize()const
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
	return serializedData.data();
}






