/**
 * @file NGWAck.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper ACK
 * Created on: Sept 5, 2017
 *
 **/

#ifndef NGWACK_H_
#define NGWACK_H_
#include "EvolMessage.h"
#include "WrapperEvent.h"

class NGWAck : public EvolMessage
{
	public:

	NGWAck(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWAck(const char *buffer);
	~NGWAck();

	UINT8 getNodeId() const 		{ return nodeId; }
	void setNodeId( UINT8 id )		{ nodeId = id; }

	UINT8 getSourceId() const		{ return sourceId; }
	void setSourceId( UINT8 id )	{ sourceId = id; }

	const char *serialize()const;

	private:
	UINT8 nodeId;
	UINT8 sourceId;
	UINT8 pad;
	UINT8 pad1;
};




#endif /* NGWACK_H_ */
