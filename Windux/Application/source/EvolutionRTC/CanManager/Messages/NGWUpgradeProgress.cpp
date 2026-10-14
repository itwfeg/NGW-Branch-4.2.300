/**
 * @file NGWUpgradeProgress.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Updates the UI on the progress of the node upgrade
 * Created on: April 4, 2016
 *
 **/
#include "NGWUpgradeProgress.h"
#include "Debug.h"
#include "Integer.h"

NGWUpgradeProgress::NGWUpgradeProgress( Hobart::CEvolMsgReceiver *pHobThread, UINT8 id, UINT8 percent ) : EvolMessage( NGWrapper, _UpgradeProgress, pHobThread )
{
	nodeId = id;
	percentComplete = percent;
}

NGWUpgradeProgress::NGWUpgradeProgress( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &percentComplete;
	for (unsigned int i = 0; i < sizeof(percentComplete); i++)
	{
		*pChar++ = buffer[j++];
	}
}

NGWUpgradeProgress::~NGWUpgradeProgress()
{

}

const char *NGWUpgradeProgress::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &percentComplete;
	for (unsigned int i = 0; i < sizeof(percentComplete); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}



