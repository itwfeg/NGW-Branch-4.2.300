/**
 * @file NGWUpgradeUIReady.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief notifies the upgrade manager that the UI upgrade page is ready.
 * Created on: July 18, 2016
 */
#include "NGWUpgradeUIReady.h"

NGWUpgradeUIReady::NGWUpgradeUIReady( Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( NGWrapper, _UpgradeUIReady, pHobThread )
{

}

NGWUpgradeUIReady::NGWUpgradeUIReady( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
}

NGWUpgradeUIReady::~NGWUpgradeUIReady()
{

}

const char *NGWUpgradeUIReady::serialize()const
{
	EvolMessage::serialize();
	return serializedData.data();
}






