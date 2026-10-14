/*
 * SAM7xUpdaterMsg.cpp
 *
 *  Created on: May 18, 2015
 *      Author: aaron
 */
#include "Sam7xUpdaterMsg.h"
#include <string.h>
#include "Debug.h"
#include "Hex.h"

Sam7xUpdaterMsg::Sam7xUpdaterMsg( Sam7xMsgType msg, int id)
{
#if defined ( __linux__ )
	memset( ( void * )&frame, 0, sizeof( can_frame ) );
	type = msg;

	if( type != _Empty)
		buildFrame(id);
#endif // __linux__
}

Sam7xUpdaterMsg::Sam7xUpdaterMsg( Sam7xMsgType msg, int id, unsigned char *pFrameData = NULL, int numbytes = 8 )
{
#if defined ( __linux__ )
	memset( ( void * )&frame, 0, sizeof( can_frame ) );
	type = msg;
	bytesToTransfer = numbytes;

	if( pFrameData != NULL )
		memcpy( (void *)&frameData[0],  (void *)pFrameData, FRAME_DATA_SIZE  );

	if( type != _Empty)
		buildFrame(id);
#endif // __linux__
}

Sam7xUpdaterMsg::~Sam7xUpdaterMsg()
{

}

#if defined ( __linux__ )
can_frame *Sam7xUpdaterMsg::getMessage()
{
	return &frame;
}

bool Sam7xUpdaterMsg::isDeviceInterfaceOpen()
{
	if (  frame.can_id == 0 && frame.data[0] == 0x01 && frame.data[1] == 0x01 )
		return true;
	else
		return false;
}
bool Sam7xUpdaterMsg::isHobartDevice()
{
if( ( frame.data[1] == 0x12 ) && ( frame.data[0] == 0x8c ) )
	return true;
else
	return false;
}

int Sam7xUpdaterMsg::getProductId()
{
	int id = 0;
	id = frame.data[1];
	id <<= 8;
	id |= frame.data[0];

	return id;
}

int Sam7xUpdaterMsg::getCommandResponse()
{
	return (int)frame.data[0];
}

void Sam7xUpdaterMsg::buildFrame( int id )
{
	switch(type)
	{
		case _OpenInterface:
		{
			/* set the message id */
			frame.can_id = GlobalNodeID;
			/* set the message size */
			frame.can_dlc = 1;
			/*command to open interface*/
			frame.data[0] = OpenInterfaceCMD;
			break;
		}
		case _ReadVendorId:
		{
			frame.can_id = ReadWordCMD;			//0x004
			frame.can_dlc = 5;
			frame.data[0] = 0x06;
			frame.data[1] = 0x00;
			frame.data[2] = 0x17;
			frame.data[3] = 0xff;
			frame.data[4] = 0xf0;
			break;
		}
		case _ReadProductId:
		{
			frame.can_id = ReadWordCMD;			//0x004
			frame.can_dlc = 5;
			frame.data[0] = 0x06;
			frame.data[1] = 0x00;
			frame.data[2] = 0x17;
			frame.data[3] = 0xff;
			frame.data[4] = 0xf2;
			break;
		}
		case _ReadMajVersion:
		{

			frame.can_id = ReadWordCMD;			//0x004
			frame.can_dlc = 5;
			frame.data[0] = 0x06;
			frame.data[1] = 0x00;
			frame.data[2] = 0x17;
			frame.data[3] = 0xff;
			frame.data[4] = 0xf4;
			break;
		}
		case _ReadMinVersion:
		{
			frame.can_id = ReadWordCMD;			//0x004
			frame.can_dlc = 5;
			frame.data[0] = 0x06;
			frame.data[1] = 0x00;
			frame.data[2] = 0x17;
			frame.data[3] = 0xff;
			frame.data[4] = 0xf6;
			break;
		}
		case _ReadNodeId:
		{
			frame.can_id = SpecialCMD;			//0x006
			frame.can_dlc = 2;
			frame.data[0] = 0x00;
			frame.data[1] = 0x00;
			break;
		}
		case _WriteNodeId:
		{
			frame.can_id = SpecialCMD;			//0x006
			frame.can_dlc = 3;
			frame.data[0] = 0x01;
			frame.data[1] = 0x00;
			frame.data[2] = ( unsigned char )id;
			break;
		}
		case _WriteBuffer:
		{
			frame.can_id = WriteBufferCMD; 		//0x001
			frame.can_dlc = 8;
			memcpy( (void *)&frame.data[0],  (void *)&frameData[0], FRAME_DATA_SIZE  );
			break;
		}
		case _WriteData:
		{
			frame.can_id = WriteDataBufferCMD;	//0x002
			frame.can_dlc = bytesToTransfer;
			memcpy( (void *)&frame.data[0],  (void *)&frameData[0], bytesToTransfer  );
			break;
		}
		case _JumpToApplication:
		{
			frame.can_id = SpecialCMD;			//0x006
			frame.can_dlc = 5;
			frame.data[0] = 0x02;
			frame.data[1] = 0x10;
			frame.data[2] = 0x82;
			frame.data[3] = 0x00;
			frame.data[4] = 0x00;
			break;
		}
		case _Empty:
			break;
	}
}
void Sam7xUpdaterMsg::show()
{
	Debug::trace( "Sam7xUpdaterMsg", "Frame Id: " + Hex::toString(frame.can_id) );
	Debug::trace( "Sam7xUpdaterMsg", "Frame Length: " + Hex::toString(frame.can_dlc) );
	for( int i = 0; i < frame.can_dlc; i++ )
	{
		Debug::trace( "Sam7xUpdaterMsg", "Frame Data[" + Hex::toString(i) + "]" + " = " + Hex::toString(frame.data[i]) );
	}
}
#endif // __linux__
