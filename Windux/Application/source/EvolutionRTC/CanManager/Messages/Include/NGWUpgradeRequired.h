
/**
 * @file NGWUpgradeRequired.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief NGW Wrapper Events
 * Created on: July 18, 2016
 *
 **/
#ifndef NGWUPGRADEREQUIRED_H_
#define NGWUPGRADEREQUIRED_H_
#include "EvolMessage.h"
#include "NGWEvent.h"

class NGWUpgradeRequired : public EvolMessage
{
	public:

	NGWUpgradeRequired( Hobart::CEvolMsgReceiver *pHobThread = NULL );
	NGWUpgradeRequired( const char *buffer );
	~NGWUpgradeRequired();


	const char *serialize()const;
};
#endif
