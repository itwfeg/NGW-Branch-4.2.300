/**
 * @file NGWRequestBootVersion.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message boot message requesting version
 * Created on: Dec 15, 2015
 *
 **/
#ifndef NGWREQUESTBOOTVERSION_H_
#define NGWREQUESTBOOTVERSION_H_
#include "EvolMessage.h"
#include "WrapperEvent.h"

class NGWRequestBootVersion : public EvolMessage
{
	public:

	NGWRequestBootVersion(Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT8 id = 0);
	NGWRequestBootVersion(const char *buffer);
	~NGWRequestBootVersion();

	const char *serialize()const;



	UINT8 getNodeId() const { return nodeId; }
	UINT8 getSourceId() const { return sourceId; }
	UINT8 getCommand() const { return cmd; }

	private:
	#define REQUESTBOOTVERSION	34

	UINT8 nodeId;
	UINT8 sourceId;
	UINT8 cmd;

};





#endif /* NGWREQUESTBOOTVERSION_H_ */
