/**
 * @file NGWRequestBootVersion.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message boot message requesting version
 * Created on: Dec 15, 2015
 *
 **/
#include "NGWRequestBootVersion.h"

NGWRequestBootVersion::NGWRequestBootVersion(Hobart::CEvolMsgReceiver *pHobThread, UINT8 id) : EvolMessage( Wrapper, WrNGWRequestBootVersion, pHobThread )
{
	nodeId = id;
	sourceId =  255;
	cmd = REQUESTBOOTVERSION;
}

NGWRequestBootVersion::NGWRequestBootVersion(const char *buffer) : EvolMessage(buffer)
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
	pChar = (char *) &cmd;
	for (unsigned int i = 0; i < sizeof(cmd); i++)
	{
		*pChar++ = buffer[j++];
	}

}

NGWRequestBootVersion::~NGWRequestBootVersion()
{

}

const char *NGWRequestBootVersion::serialize() const
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
	pChar = (char *) &cmd;
	for (unsigned int i = 0; i < sizeof(cmd); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}



