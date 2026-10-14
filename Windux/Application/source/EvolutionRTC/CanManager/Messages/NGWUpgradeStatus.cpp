/**
 * @file NGWUpgradeStatus.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to the state of the NGWUpgradeManager
 * Created on: April 4, 2016
 */
#include "NGWUpgradeStatus.h"
#include "Debug.h"
#include "Integer.h"

NGWUpgradeStatus::NGWUpgradeStatus( Hobart::CEvolMsgReceiver *pHobThread, UINT8 id ) : EvolMessage( NGWrapper, _UpgradeStatus, pHobThread )
{
	nodeId = id;
	state = _Version;
	version = "";
	_error = _None;
}

NGWUpgradeStatus::NGWUpgradeStatus( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &state;
	for (unsigned int i = 0; i < sizeof(state); i++)
	{
		*pChar++ = buffer[j++];
	}

	for (unsigned int i = 0; i < 4; i++)
	{
		version.append(1, buffer[j++]);
	}

	pChar = (char *) &_error;
	for (unsigned int i = 0; i < sizeof(_error); i++)
	{
		*pChar++ = buffer[j++];
	}

}

NGWUpgradeStatus::~NGWUpgradeStatus()
{

}

const char *NGWUpgradeStatus::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &state;
	for (unsigned int i = 0; i < sizeof(state); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)version.data();
	for (unsigned int i = 0; i < version.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_error;
	for (unsigned int i = 0; i < sizeof(_error); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void NGWUpgradeStatus::show()
{
	Debug::trace( "NGWUpgradeStatus", "nodeId: " + Integer::toString(nodeId) );
	Debug::trace( "NGWUpgradeStatus", "state: " + Integer::toString(state) );
	Debug::trace( "NGWUpgradeStatus", "version: " + version );
	Debug::trace( "NGWUpgradeStatus", "error: " + Integer::toString(_error) );
}



