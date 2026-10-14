/**
 * @file NGWUpgradeFinished.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Notifies the UI to close the upgrade page
 * Created on: April 4, 2016
 *
 **/
#include "NGWUpgradeFinished.h"

NGWUpgradeFinished::NGWUpgradeFinished( Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( NGWrapper, _UpgradeFinished, pHobThread )
{

}

NGWUpgradeFinished::NGWUpgradeFinished( const char *buffer )
{
	EvolMessage::serialize();
}

NGWUpgradeFinished::~NGWUpgradeFinished()
{

}

const char *NGWUpgradeFinished::serialize()const
{
	EvolMessage::serialize();
	return serializedData.data();
}




