/**
 * @file NGWBulkTransfer.cpp
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message containing source image
 * Created on: Jan 12, 2016
 *
 **/

#include "NGWBulkTransfer.h"
#include "WrapperEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"

NGWBulkTransfer::NGWBulkTransfer( Hobart::CEvolMsgReceiver *pHobThread, const unsigned char *pImage, UINT8 cmd, int frameNum, int size, UINT8 id ) : EvolMessage( Wrapper, WrNGWBulkFrame, pHobThread )
{
	numFrames = frameNum;
	length = size;
	nodeId = id;
	sourceId = 255;
	this->cmd = cmd;


	Debug::trace( "NGWBulkTransfer", "Constructor()  length: " + Integer::toString(length)  + "number of frames:" + Integer::toString(frameNum));
	int payloadSize = 0;
	if( length != 0 )
	{
		payloadSize = ( ( numFrames -1 ) * 8 ) + length;
	}
	else
	{
		payloadSize = numFrames * 8;
	}

	for( int i = 0; i < payloadSize; i++ )
	{
		data[i] = *pImage++;
	}
}

NGWBulkTransfer::NGWBulkTransfer(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &numFrames;
	for (unsigned int i = 0; i < sizeof(numFrames); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &length;
	for (unsigned int i = 0; i < sizeof(length); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &cmd;
	for (unsigned int i = 0; i < sizeof(cmd); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sourceId;
	for (unsigned int i = 0; i < sizeof(sourceId); i++)
	{
		*pChar++ = buffer[j++];
	}

	int payloadSize = ( numFrames * 8 ) + length;
	for( int i = 0; i < payloadSize; i++ )
	{
		data[i] = buffer[j++];
	}



}

NGWBulkTransfer::~NGWBulkTransfer()
{

}

const char *NGWBulkTransfer::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &numFrames;
	for (unsigned int i = 0; i < sizeof(numFrames); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &length;
	for (unsigned int i = 0; i < sizeof(length); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &cmd;
	for (unsigned int i = 0; i < sizeof(cmd); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sourceId;
	for (unsigned int i = 0; i < sizeof(sourceId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	int payloadSize = ( numFrames * 8 ) + length;
	for( int i = 0; i < payloadSize; i++ )
	{
		serializedData.append(1, data[i]);
	}
	std::string s;
	for( int i = 0; i < payloadSize; i++ )
	{
		s += Hex::toString(data[i]);
	}
	Debug::trace("NGWBulkTransfer", "serializedData: " + s);

	return serializedData.data();


}

