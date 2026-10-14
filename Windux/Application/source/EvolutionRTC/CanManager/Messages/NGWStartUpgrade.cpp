/**
 * @file NGWStartUpgrade.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI the number of nodes for the upgrade page.
 * Created on: April 4, 2016
 */
#include "NGWStartUpgrade.h"

NGWStartUpgrade::NGWStartUpgrade( Hobart::CEvolMsgReceiver *pHobThread, UINT8 numOfNodes ): EvolMessage( NGWrapper, _StartUpgrade, pHobThread )
{
	numberOfNodes = numOfNodes;
}

NGWStartUpgrade::NGWStartUpgrade( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &numberOfNodes;
	for (unsigned int i = 0; i < sizeof(numberOfNodes); i++)
	{
		*pChar++ = buffer[j++];
	}

}

NGWStartUpgrade::~NGWStartUpgrade()
{

}

const char *NGWStartUpgrade::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &numberOfNodes;
	for (unsigned int i = 0; i < sizeof(numberOfNodes); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}





