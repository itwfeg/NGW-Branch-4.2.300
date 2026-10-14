
/**
 * @file NGWUpgradeUIReady.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief NGW Wrapper Events
 * Created on: july 18, 2016
 *
 **/
#ifndef NGWUPGRADEUIREADY_H_
#define NGWUPGRADEUIREADY_H_
#include "EvolMessage.h"
#include "NGWEvent.h"

class NGWUpgradeUIReady : public EvolMessage
{
	public:

	NGWUpgradeUIReady( Hobart::CEvolMsgReceiver *pHobThread = NULL );
	NGWUpgradeUIReady( const char *buffer );
	~NGWUpgradeUIReady();

	const char *serialize()const;

};





#endif
