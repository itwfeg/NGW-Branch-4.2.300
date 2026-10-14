/**
 * @file NGWUpgradeRequired.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the UI to start the NGW upgrade page
 * Created on: July 18, 2016
 */
#include "NGWUpgradeRequired.h"
NGWUpgradeRequired::NGWUpgradeRequired( Hobart::CEvolMsgReceiver *pHobThread): EvolMessage( NGWrapper, _UpgradeRequired, pHobThread )
{

}

NGWUpgradeRequired::NGWUpgradeRequired( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

}

NGWUpgradeRequired::~NGWUpgradeRequired()
{

}

const char *NGWUpgradeRequired::serialize()const
{
	EvolMessage::serialize();

	return serializedData.data();
}


