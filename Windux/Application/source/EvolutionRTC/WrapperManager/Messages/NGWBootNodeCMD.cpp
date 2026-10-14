/**
 * @file NGWBootNodeCMD.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper node command message
 * Created on: Dec 10, 2015
 *
 **/
#include "NGWBootNodeCMD.h"

NGWBootNodeCMD::NGWBootNodeCMD(Hobart::CEvolMsgReceiver *pHobThread, UINT8 id, NodeAction action) : EvolMessage( Wrapper, WrNGWBootCMD, pHobThread )
{
	nodeId = id;
	cmd = FlashUpdateNode;
	this->action = action;
}

NGWBootNodeCMD::NGWBootNodeCMD(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &cmd;
	for (unsigned int i = 0; i < sizeof(cmd); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &action;
	for (unsigned int i = 0; i < sizeof(action); i++)
	{
		*pChar++ = buffer[j++];
	}
}

NGWBootNodeCMD::~NGWBootNodeCMD()
{

}

const char *NGWBootNodeCMD::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &cmd;
	for (unsigned int i = 0; i < sizeof(cmd); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &action;
	for (unsigned int i = 0; i < sizeof(action); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}


