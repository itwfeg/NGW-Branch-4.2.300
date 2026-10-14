/**
 * @file NGWFlush.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper node command message
 * Created on: Jan 5, 2016
 *
 **/

#ifndef NGWFLUSH_H_
#define NGWFLUSH_H_
#include "EvolMessage.h"
#include "WrapperEvent.h"

class NGWFlush : public EvolMessage
{
	public:

	NGWFlush(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWFlush(const char *buffer);
	~NGWFlush();

	UINT8 getNodeId() const 		{ return nodeId; }
	void setNodeId( UINT8 id )		{ nodeId = id; }

	UINT8 getSourceId() const		{ return sourceId; }
	void setSourceId( UINT8 id )	{ sourceId = id; }

	const char *serialize()const;

	private:
	UINT8 nodeId;
	UINT8 sourceId;
};

#endif /* NGWFLUSH_H_ */
