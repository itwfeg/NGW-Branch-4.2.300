/**
 * @file WeigherValueMaxAllGains.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max current x,y and z gain factor
 * Created on: Sept 25, 2017
 *
 **/
#include "WeigherValueMaxAllGains.h"
#include "WeigherEvent.h"

WeigherValueMaxAllGains::WeigherValueMaxAllGains( float x, float y, float z, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxAllGains, pHobThread )
{
	xGain = x;
	yGain = y;
	zGain = z;
}

WeigherValueMaxAllGains::WeigherValueMaxAllGains( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &xGain;
	for (unsigned int i = 0; i < sizeof(xGain); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &yGain;
	for (unsigned int i = 0; i < sizeof(yGain); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &zGain;
	for (unsigned int i = 0; i < sizeof(zGain); i++)
	{
		*pChar++ = buffer[j++];
	}

}

WeigherValueMaxAllGains::~WeigherValueMaxAllGains()
{
}

const char *WeigherValueMaxAllGains::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &xGain;
	for (unsigned int i = 0; i < sizeof(xGain); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &yGain;
	for (unsigned int i = 0; i < sizeof(yGain); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &zGain;
	for (unsigned int i = 0; i < sizeof(zGain); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}








