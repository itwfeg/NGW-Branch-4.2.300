/**
 * @file NGWBootNodeCMD.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper node command message
 * Created on: Dec 10, 2015
 *
 **/

#ifndef NGWBOOTNODECMD_H_
#define NGWBOOTNODECMD_H_
#include "EvolMessage.h"
#include "WrapperEvent.h"

class NGWBootNodeCMD : public EvolMessage
{

	public:

	typedef enum
	{
		FlashUpdateNode = 124
	}NodeCMD;

	typedef enum
	{
		NoAction,
		UpdateImage = 2,
		JumpToApp,
		ResetNode,
		VerifyApplication
	}NodeAction;

	NGWBootNodeCMD(Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT8 id = 0, NodeAction action = NoAction);
	NGWBootNodeCMD(const char *buffer);
	~NGWBootNodeCMD();

	UINT8 getNodeId() const { return nodeId; }
	UINT8 getCommand() const { return cmd; }
	UINT8 getAction() const { return action; }

	const char *serialize()const;

	private:
	UINT8 nodeId;
	UINT8 cmd;
	UINT8 action;
};



#endif
