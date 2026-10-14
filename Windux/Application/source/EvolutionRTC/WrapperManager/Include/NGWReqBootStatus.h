/**
 * @file NGWReqBootStatus.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Message requesting the CAN wrapper node boot status
 * Created on: Dec 9, 2015
 *
 **/
#ifndef NGWREQBOOTSTATUS_H_
#define NGWREQBOOTSTATUS_H_
#include "EvolMessage.h"

class NGWRequestBootStatus : public EvolMessage
{
	public:
	NGWRequestBootStatus(Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT8 id = 0);
	NGWRequestBootStatus(const char *buffer);
	~NGWRequestBootStatus();

	const char *serialize()const;
	UINT8 getNodeId() const { return nodeId; }

	private:
	UINT8 nodeId;

};



#endif
