/**
 * @file NGWBootStatus.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN wrapper node boot status message
 * Created on: Dec 10, 2015
 *
 **/
#include "NGWBootStatus.h"

NGWBootStatus::NGWBootStatus(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage( Wrapper, WrNGWBootStatus, pHobThread )
{

}

NGWBootStatus::NGWBootStatus(const char *buffer)  : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &faultAddress;
	for (unsigned int i = 0; i < sizeof(faultAddress); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &fault;
	for (unsigned int i = 0; i < sizeof(fault); i++)
	{
		*pChar++ = buffer[j++];
	}

}

NGWBootStatus::~NGWBootStatus()
{

}

const char *NGWBootStatus::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &faultAddress;
	for (unsigned int i = 0; i < sizeof(faultAddress); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &fault;
	for (unsigned int i = 0; i < sizeof(fault); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}


