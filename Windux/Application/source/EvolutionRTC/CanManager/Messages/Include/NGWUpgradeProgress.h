/**
 * @file NGWUpgradeProgress.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief
 * Created on: April 4, 2016
 *
 **/
#ifndef NGWUPGRADEPROGRESS_H_
#define NGWUPGRADEPROGRESS_H_
#include "EvolMessage.h"
#include "NGWEvent.h"

class NGWUpgradeProgress : public EvolMessage
{
	public:

	NGWUpgradeProgress( Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT8 id = 1, UINT8 percent = 0 );
	NGWUpgradeProgress( const char *buffer );
	~NGWUpgradeProgress();

	UINT8 getNodeId() const							{ return nodeId; }
	void setNodeId( UINT8 id )						{ nodeId = id; }

	UINT8 getPercentComplete() const				{ return percentComplete; }
	void setPercentComplete( UINT8 percent )		{ percentComplete = percent; }

	const char *serialize()const;

	private:
	UINT8 nodeId;
	UINT8 percentComplete;
};

#endif
