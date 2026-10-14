/**
 * @file NGWBootFrame.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message containing source image
 * Created on: Dec 9, 2015
 *
 **/

#include "NGWBootFrame.h"
#include "WrapperEvent.h"
#include "Debug.h"
#include "Integer.h"

NGWBootFrame::NGWBootFrame( Hobart::CEvolMsgReceiver *pHobThread, FrameID frameId, UINT8 *pFrameData, int length  ) : EvolMessage( Wrapper, WrNGWBootFrame, pHobThread )
{
	frameNumber = frameId;
	dataLength = length;
	str = "";
	//Debug::trace( "NGWBootFrame", "Constructor()  length: " + Integer::toString(length) );

	for( int i = 0; i < 8; i++ )
	{
		data[i] = 0;
	}

	if( frameId == _CanHeaderFrame )
	{
		if( pFrameData != NULL )
		{
			for( int i = 0; i < length; i++ )
			{
				data[i] = *pFrameData++;
			}
		}
	}
	else
	{
		if( pFrameData != NULL )
		{
			for( int i = 0; i < length; i++ )
			{
				str.append( 1, *pFrameData++ );
			}
		}
	}
}

NGWBootFrame::NGWBootFrame(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &frameNumber;
	for (unsigned int i = 0; i < sizeof(frameNumber); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &dataLength;
	for (unsigned int i = 0; i < sizeof(dataLength); i++)
	{
		*pChar++ = buffer[j++];
	}

	if( frameNumber == _CanHeaderFrame )
	{
		for( int i = 0; i < dataLength; i++ )
		{
			data[i] = buffer[j++];
		}
	}
	else
	{
		for( int i = 0; i < 8; i++ )
			str.append(1, buffer[j++]);
		for( int i = 0; i < 8; i++ )
			data[i] = str[i];
	}
	//Debug::trace( "NGWBootFrame", "Const char* constructor data:  " + str );
}

NGWBootFrame::~NGWBootFrame()
{

}

const char *NGWBootFrame::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &frameNumber;
	for (unsigned int i = 0; i < sizeof(frameNumber); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &dataLength;
	for (unsigned int i = 0; i < sizeof(dataLength); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	if( frameNumber == _CanHeaderFrame )
	{
		for (unsigned int i = 0; i < dataLength; i++)
		{
			serializedData.append(1, data[i]);
		}
	}
	else
	{
		serializedData += str;
	}
	//Debug::trace( "NGWBootFrame", "Serialized str: " + str);
	//Debug::trace( "NGWBootFrame", "Serialized data:  " + serializedData );
	return serializedData.data();
}

UINT8 NGWBootFrame::getFrameData( UINT8 id ) const
{
	if( frameNumber == _CanHeaderFrame )
		return data[id];
	else
		return str.at(id);
}

void NGWBootFrame::show()
{
	/*
	Debug::trace( "NGWBootFrame", "Frame number: " + Integer::toString(frameNumber));
	Debug::trace( "NGWBootFrame", "Data length: " + Integer::toString(dataLength));
	*/
	if( frameNumber != _CanHeaderFrame)
	{
		Debug::trace( "NGWBootFrame", "Frame data : " + str );
	}
	else
	{
		/*
		Debug::trace( "NGWBootFrame", "Frame data 0: " + Integer::toString(data[0]));
		Debug::trace( "NGWBootFrame", "Frame data 1: " + Integer::toString(data[1]));
		Debug::trace( "NGWBootFrame", "Frame data 2: " + Integer::toString(data[2]));
		Debug::trace( "NGWBootFrame", "Frame data 3: " + Integer::toString(data[3]));
		Debug::trace( "NGWBootFrame", "Frame data 4: " + Integer::toString(data[4]));
		Debug::trace( "NGWBootFrame", "Frame data 5: " + Integer::toString(data[5]));
		Debug::trace( "NGWBootFrame", "Frame data 6: " + Integer::toString(data[6]));
		Debug::trace( "NGWBootFrame", "Frame data 7: " + Integer::toString(data[7]));
		*/
	}
}

