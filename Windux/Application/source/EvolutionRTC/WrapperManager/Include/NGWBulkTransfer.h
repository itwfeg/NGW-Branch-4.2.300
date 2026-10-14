/**
 * @file NGWBulkTransfer.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Responsible for retrieving information from the CAN messages
 * Created on: Jan 12, 2016
 * @{
 */

#ifndef NGWBULKTRANSFER_H_
#define NGWBULKTRANSFER_H_
#include "EvolMessage.h"
#include "WrapperEvent.h"
class NGWBulkTransfer : public EvolMessage
{
public:

	static const size_t MAX_PAYLOAD_SIZE = 64;


	NGWBulkTransfer( Hobart::CEvolMsgReceiver *pHobThread = NULL, const unsigned char *pImage = NULL, UINT8 cmd = 0, int frameNum = 0, int size = 0, UINT8 id = 0 );
	NGWBulkTransfer( const char *buffer );
	~NGWBulkTransfer();

	int getNumFrames() const { return numFrames; }
	void setNumFrame( int num ) { numFrames = num; }

	int getLength() const { return length; }
	void setLength( int x )	{ length = x; }

	UINT8 getCommand() const { return cmd; }
	void setCommand( UINT8 x )	{ cmd = x; }

	UINT8 getNodeId() const	{ return nodeId; }
	void setNodeId( UINT8 id) { nodeId = id; }

	UINT8 getSourceId() const { return sourceId; }
	void setSourceId( UINT8 id ) { sourceId = id; }

	UINT8 getData(int index) const { return data[index]; }
	const char *serialize()const;

private:

	int numFrames;
	int length;
	UINT8 cmd;
	UINT8 nodeId;
	UINT8 sourceId;
	unsigned char data[MAX_PAYLOAD_SIZE];


};



#endif /* NGWBULKTRANSFER_H_ */
